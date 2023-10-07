/*
    Copyright (c) 2017 Jean THOMAS.

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the Software
    is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <string.h>
#include <CoreFoundation/CoreFoundation.h>
#include <libkern/OSByteOrder.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/usb/USBSpec.h>
#include "main.h"

#if (MAC_OS_X_VERSION_MAX_ALLOWED < 101700) // Before macOS 12 Monterey
#define kIOMainPortDefault kIOMasterPortDefault
#endif

UInt8 disableplugin_value= 0xba;

int main(int argc, char *argv[]) {
  int i, ret;
  enum TrinityAvailablePower availablePower;
  IOUSBDeviceInterface300** deviceInterface;

  /* Reading power delivery capacity */
  availablePower = POWER_NULL;
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--power-500") == 0) {
      printf("Audio device set to 500mA\n");
      availablePower = POWER_500MA;
    } else if (strcmp(argv[i], "--power-1500") == 0) {
      printf("Audio device set to 1500mA\n");
      availablePower = POWER_1500MA;
    } else if (strcmp(argv[i], "--power-3000") == 0) {
      printf("Audio device set to 3000mA\n");
      availablePower = POWER_3000MA;
    } else if (strcmp(argv[i], "--power-4000") == 0) {
      printf("Audio device set to 4000mA\n");
      availablePower = POWER_4000MA;
    }
  }

  if (availablePower == POWER_NULL) {
    printf("Available power settings :\n");
    printf("\t--power-500\t500mA\n");
    printf("\t--power-1500\t1500mA\n");
    printf("\t--power-3000\t3000mA\n");
    printf("\t--power-4000\t4000mA\n");
    
    return EXIT_SUCCESS;
  }

  /* Getting USB device interface */
  deviceInterface = usbDeviceInterfaceFromVIDPID(0x05AC,0x1101);
  if (deviceInterface == NULL) {
    return EXIT_FAILURE;
  }

  /* Opening USB device interface */
  ret = (*deviceInterface)->USBDeviceOpen(deviceInterface);
  if (ret == kIOReturnSuccess) {
    
  } else if (ret == kIOReturnExclusiveAccess) {
    printf("HID manager has already taken care of this device. Let's try anyway.\n");
  } else {
    printf("Could not open device. Quitting…\n");
    return EXIT_FAILURE;
  }
  
  if (disablePlugin(deviceInterface) != kIOReturnSuccess) {
    printf("Error while disabling plugin.\n");
    return EXIT_FAILURE;
  }
  if (downloadEQ(deviceInterface, availablePower) != kIOReturnSuccess) {
    printf("Error while downloading EQ to Trinity audio device.\n");
    return EXIT_FAILURE;
  }
  if (downloadPlugin(deviceInterface) != kIOReturnSuccess) {
    printf("Error while downloading plugin to Trinity audio device.\n");
    return EXIT_FAILURE;
  }
  if (enablePlugin(deviceInterface) != kIOReturnSuccess) {
    printf("Error while enabling plugin.\n");
    return EXIT_FAILURE;
  }

  /* Closing the USB device */
  (*deviceInterface)->USBDeviceClose(deviceInterface);

  return EXIT_SUCCESS;
}

IOUSBDeviceInterface300** usbDeviceInterfaceFromVIDPID(SInt32 vid, SInt32 pid) {
  CFMutableDictionaryRef matchingDict;
  io_iterator_t usbRefIterator;
  io_service_t usbRef;
  IOCFPlugInInterface** plugin;
  IOUSBDeviceInterface300** deviceInterface;
  SInt32 score;

  /* Creating a matching dictionary to match the device's PID and VID */
  matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
  CFDictionaryAddValue(matchingDict,
		       CFSTR(kUSBVendorID),
		       CFNumberCreate(kCFAllocatorDefault,
				      kCFNumberSInt32Type,
				      &vid));
  CFDictionaryAddValue(matchingDict,
		       CFSTR(kUSBProductID),
		       CFNumberCreate(kCFAllocatorDefault,
				      kCFNumberSInt32Type,
				      &pid));

  /* Getting all the devices that are matched by the matching dictionary */
  IOServiceGetMatchingServices(kIOMainPortDefault,
			       matchingDict,
			       &usbRefIterator);

  /* We only use the first USB device */
  usbRef = IOIteratorNext(usbRefIterator);
  IOObjectRelease(usbRefIterator);

  if (usbRef == 0) {
    printf("%s : Can't find suitable USB audio device\n", __PRETTY_FUNCTION__);
    return NULL;
  } else {
    IOCreatePlugInInterfaceForService(usbRef,
				      kIOUSBDeviceUserClientTypeID,
				      kIOCFPlugInInterfaceID,
				      &plugin,
				      &score);
    IOObjectRelease(usbRef);
    (*plugin)->QueryInterface(plugin,
			      CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID300),
			      (LPVOID)&deviceInterface);
    (*plugin)->Release(plugin);

    return deviceInterface;
  }
}

IOReturn xdfpSetMem(IOUSBDeviceInterface300** deviceInterface, UInt8 *buf, UInt16 length, UInt16 xdfpAddr) {
  IOUSBDevRequest devReq;

  devReq.bmRequestType = USBmakebmRequestType(kUSBOut, kUSBVendor, kUSBDevice);
  devReq.bRequest = kMicronasSetMemReq;
  devReq.wValue = 0;
  devReq.wIndex = xdfpAddr;
  devReq.wLength = length;
  devReq.pData = buf;

  return (*deviceInterface)->DeviceRequest(deviceInterface, &devReq);
}

IOReturn xdfpWrite(IOUSBDeviceInterface300** deviceInterface, UInt16 xdfpAddr, SInt32 value) {
  static UInt8 xdfpData[5];

    if (value < 0) value += 0x40000;
    xdfpData[0] = (value >> 10) & 0xff;
    xdfpData[1] = (value >> 2) & 0xff;
    xdfpData[2] = value & 0x03;
    xdfpData[3] = (xdfpAddr >> 8) & 0x03;
    xdfpData[4] = xdfpAddr & 0xff;

    return xdfpSetMem(deviceInterface, xdfpData, 5, V8_WRITE_START_ADDR);
}

IOReturn downloadEQ(IOUSBDeviceInterface300** deviceInterface, enum TrinityAvailablePower availablePower) {
  UInt16 xdfpAddr;
  UInt32 eqIndex;
  IOReturn ret;
  static SInt32 *eqSettings;

  switch (availablePower) {
  case POWER_4000MA:
    eqSettings = power4AEQSettings;
    break;
  case POWER_3000MA:
    eqSettings = power3AEQSettings;
    break;
  case POWER_1500MA:
    eqSettings = power1500mAEQSettings;
    break;
  default:
  case POWER_500MA:
    eqSettings = power500mAEQSettings;
    break;
  }
  
  for (eqIndex = 0, xdfpAddr = XDFP_STARTING_EQ_ADDR; eqIndex < EQ_TABLE_SIZE; eqIndex++, xdfpAddr++) {
    ret = xdfpWrite(deviceInterface, xdfpAddr, eqSettings[eqIndex]);
    if (ret != kIOReturnSuccess) {
      return ret;
    }
    
    nanosleep((const struct timespec[]){{0, 3000000L}}, NULL);
  }

  return ret;
}

IOReturn disablePlugin(IOUSBDeviceInterface300** deviceInterface) {
  return xdfpSetMem(deviceInterface, &disableplugin_value, 1, V8_PLUGIN_START_ADDR);
}

IOReturn enablePlugin(IOUSBDeviceInterface300** deviceInterface) {
  return xdfpSetMem(deviceInterface, pluginBinary, 1, V8_PLUGIN_START_ADDR);
}

IOReturn downloadPlugin(IOUSBDeviceInterface300** deviceInterface) {
  return xdfpSetMem(deviceInterface, &pluginBinary[1], sizeof(pluginBinary), V8_PLUGIN_START_ADDR+1);
}
