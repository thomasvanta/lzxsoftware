# lzxsoftware (community refactor fork)
LZX Industries Open Source Software Projects

This fork adds PlatformIO build configuration to support a cross-platform, open source build toolchain, and refactors the source to enable more modular community contributions. 

The Diver firmware is in active use by multiple people, but still has bugs, both old and new.

Escher Sketch is confirmed to build but has not been tested on hardware.

## Building 

* [Building with PlatformIO](BUILDING-PlatformIO.md) (Windows/macOS/Linux)
* [Building with VisualGDB](BUILDING-VisualGDB.md) (Windows only)

## Uploading firmware images on hardware

Using the PlatformIO toolchain described in the above link offers the overall smoothest experience for both building and repeatedly uploading firmware. If you don't want to get involved with building and just want to install an already-built binary, read on.

> ***IMPORTANT NOTE**: Do not connect your module to a computer with USB at the same time it is connected to Eurorack power.*
> 
> *Misconfiguring electronics could damage your module, your Eurorack case, and/or your computer. Proceed completely at your own risk!*

### Using DFuSeDemo to upload .dfu images (Windows only)

The original LZX [firmware update instructions are available here](https://community.lzxindustries.net/t/all-about-diver/1455#firmware-update-instructions-7). 

DFuSeDemo should still work for .dfu files, but it only works on Windows, doesn't work well on newer versions of Windows, and STM has replaced it with [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html).

### Using STM32CubeProgrammer to upload .bin images (Windows/macOS/Linux)

1. Download STM32CubeProgrammer [from STM here](https://www.st.com/en/development-tools/stm32cubeprog.html).
2. Disconnect the module you intend to program from Eurorack power or any other power souce. Plug in a USB-A to USB-MiniB cable into your computer, and into the back of the module, respectively.
3. Run STM32CubeProgrammer. Click the reload ♻️ icon on the far right; you should see a USB port name show up in the **Port** dropdown.
4. Click the **Connect** button.
5. On the left-side toolbar, click the second icon from the top to enter the **Erasing & Programming** page.
6. Click **Browse** and select the new firmware .bin file.
7. Check **Verify Programming** if it isn't already checked.
8. Make sure **Start Address** reads **0x8000000**. (That's an 8 followed by six zeros.)
9. Click **Start Programming**.  After a few seconds, you should see the messages "Download Verified Successfully" and "File Download Complete."
