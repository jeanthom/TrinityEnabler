# Trinity Enabler

Apple Pro Speakers (codenamed "Trinity") feature a built-in Micronas UAC3552A USB DAC chip. This chip needs some firmware and some parameters (EQ settings) from the host computer to work properly. Apple's driver took care of this. Unfortunately, Apple Pro Speakers support was dropped from Mac OS X when Snow Leopard came out.

This utility brings Apple Pro Speakers support back in newer macOS versions. Are you running Linux? [Try this instead.](https://github.com/jeanthom/TrinityEnabler)

## Howto ##

### Using release builds ###

Download the utility from the [release page](https://github.com/jeanthom/trinityenabler/releases). Drag'n'drop it into a terminal, and type in the matching power rating for your USB port (in most cases it will be `--power-500` or `--power-1500`). Hit the enter key, and your Apple Pro Speakers should be working :)

### Compile it yourself ###

```
make
bin/trinityenabler --power-500
```
