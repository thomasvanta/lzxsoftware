# lzxsoftware
LZX Industries Open Source Software Projects

An open archive of LZX Industries firmware project source files. 

## Building 

* [Building with PlatformIO](BUILDING-PlatformIO.md)
* [Building with VisualGDB](BUILDING-VisualGDB.md) (Windows only)

## Installing firmware images on hardware

Using PlatformIO above to both build and upload firmware offers the overall smoothest experience. If you don't want to get involved with building firmware and just want to install an already-built binary, read on.

### Using DFuSeDemo to install .dfu images

The original LZX [firmware update instructions are available here](https://community.lzxindustries.net/t/all-about-diver/1455#firmware-update-instructions-7). 

DFuSeDemo should still work for .dfu files, but it only works on Windows, doesn't work well on newer versions of Windows, and STM has replaced it with [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html).

## Using STM32CubeProgrammer to install .bin images

1. Download STM32CubeProgrammer [from STM here](https://www.st.com/en/development-tools/stm32cubeprog.html) (Win/Mac/OSX).
2. Disconnect the module you intend to program from Eurorack power or any other power souce. Plug in a USB-A to USB-MiniB cable into your computer, and into the back of the module, respectively.
3. Run STM32CubeProgrammer. Click the reload icon on the far right; you should see a USB port name show up in the **Port** dropdown.
4. Click the **Connect** button.
5. On the left-side toolbar, click the second icon from the top to enter the **Erasing & Programming** page.
6. Click **Browse** and select the new firmware .bin file.
7. Check **Verify Programming** if it isn't already checked.
8. Make sure **Start Address** reads **0x8000000**. (That's an 8 followed by six zeros.)
9. Click **Start Programming**.  After a few seconds, you should see the messages "Download Verified Successfully" and "File Download Complete."
