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

/* 0xB042-0xB044 is XDFP (DSP interface) data in  */
/* 0xB045-0xB046 is XDFP address/command interface */
/* 0xB05C-0xB05E is XDFP data out (unused here) */
#define V8_WRITE_START_ADDR   0xb042

/* This memory address is mapped to the RAM */
#define V8_PLUGIN_START_ADDR  0x8120
#define XDFP_STARTING_EQ_ADDR 0x50

#define EQ_TABLE_SIZE				16

#define kMicronasSetMemReq			4
#define kMicronasGetMemReq			5136

enum TrinityAvailablePower {
  POWER_NULL, POWER_500MA, POWER_1500MA, POWER_3000MA, POWER_4000MA
};

/* These EQ come from AppleUSBTrinityAudioDevice.cpp */
static SInt32	power4AEQSettings[] = {
    228,	-129968,	130513,
    -279,	-125942,	128415,
    -1689,	-123355,	126686,
    -5136,	-95891,		109553,
    -18995,	-993,		6924,
    -45000
};

static SInt32    power3AEQSettings[] = {
    228,	-129968,	130513,
    -279,	-125942,	128415,
    -1689,	-123355,	126686,
    -5137,	-95891,		109553,
    -18995,	-993,		6924,
    -42000};

static SInt32 power1500mAEQSettings[] = {
	228,	-129968,	130513, 
	-279,	-125942,	128415, 
	-1689,	-123355,	126686, 
	-5137,	-95891,		109553, 
	-18995,	-993,		6924, 
	-20000};

static SInt32    power500mAEQSettings[] = {
    228,	-129968,	130513,
    -279,	-125942,	128415,
    -1689,	-123355,	126686,
    -5137,	-95891,		109553,
    -18995,	-993,		6924,
    -8000};

/* Plugins are firmware extensions. The plugin is necessary to enable the amplifier chip. */
static UInt8 pluginBinary[] = {
    0xBF, 0x35, 0x81, 0xBA, 0x85, 0xEA, 0x7B, 0x80, 0xE1, 0x13, 0xBF, 0xDE, 0x0B, 0x8D, 0xB9, 0x85,
    0xBF, 0x1A, 0x0C, 0x8D, 0xB9, 0xE9, 0xF3, 0x81, 0xE8, 0x80, 0x80, 0x79, 0x90, 0x03, 0xBC, 0xEC,
    0x81, 0xEA, 0xA2, 0xB0, 0xE4, 0x00, 0xE5, 0x02, 0x44, 0x99, 0x01, 0x45, 0x15, 0x71, 0x14, 0x19,
    0x90, 0xF6, 0xE0, 0xF0, 0xC8, 0x87, 0x80, 0xC8, 0x51, 0xB0, 0x12, 0x63, 0x90, 0x03, 0x28, 0x98,
    0x02, 0xE0, 0x40, 0xC8, 0x89, 0x80, 0xC8, 0xA0, 0xB0, 0xE1, 0xFB, 0x12, 0x21, 0xC8, 0x88, 0x80,
    0xE8, 0x80, 0x80, 0x90, 0x09, 0xE8, 0x01, 0xA0, 0xC8, 0x80, 0x80, 0xBF, 0x2F, 0x81, 0xE8, 0x80,
    0x80, 0xC8, 0xF3, 0x81, 0xE1, 0x0C, 0x12, 0x21, 0x90, 0x25, 0xE9, 0xED, 0x81, 0xE8, 0x7B, 0x80,
    0x59, 0x49, 0x74, 0xE9, 0xF0, 0x81, 0xE2, 0x80, 0x2A, 0x73, 0x11, 0x2A, 0x7B, 0x99, 0x0A, 0xE8,
    0xF1, 0x81, 0x00, 0xC8, 0xF1, 0x81, 0xCC, 0x7B, 0x80, 0xE8, 0xEE, 0x81, 0xBC, 0xDA, 0x81, 0xE8,
    0xF2, 0x81, 0x90, 0x29, 0xE9, 0x7B, 0x80, 0xE8, 0xED, 0x81, 0x51, 0x72, 0xE8, 0xF1, 0x81, 0x98,
    0x16, 0x40, 0xC8, 0xF1, 0x81, 0x12, 0xE1, 0x80, 0x29, 0xE1, 0xB0, 0x79, 0x99, 0x04, 0x12, 0xBC,
    0xD4, 0x81, 0xE0, 0x30, 0xC8, 0x7B, 0x80, 0xE8, 0xEF, 0x81, 0xC8, 0xF2, 0x81, 0xE8, 0xF2, 0x81,
    0x90, 0x03, 0xBC, 0x24, 0x81, 0x40, 0xC8, 0xF2, 0x81, 0xBC, 0x24, 0x81, 0xB9, 0x01, 0x08, 0x0F,
    0xD0, 0x01, 0x01, 0x01
};


int main(int argc, char *argv[]);
IOReturn xdfpSetMem(IOUSBDeviceInterface300** deviceInterface, UInt8 *buf, UInt16 length, UInt16 xdfpAddr);
IOReturn xdfpWrite(IOUSBDeviceInterface300** deviceInterface, UInt16 xdfpAddr, SInt32 value);
IOReturn downloadEQ(IOUSBDeviceInterface300** deviceInterface, enum TrinityAvailablePower availablePower);
IOReturn downloadPlugin(IOUSBDeviceInterface300** deviceInterface);
IOReturn disablePlugin(IOUSBDeviceInterface300** deviceInterface);
IOReturn enablePlugin(IOUSBDeviceInterface300** deviceInterface);
IOUSBDeviceInterface300** usbDeviceInterfaceFromVIDPID(SInt32 vid, SInt32 pid);
