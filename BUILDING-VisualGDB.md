# Building firmware using VisualGDB

### Requirements:

* Windows 10 or later
* A VisualGDB trial license
* A USB-A to Mini-B cable

The original build toolchain for the firmware in this repository requires VisualGDB, a commercial product. A 30-day trial of this tool is available until [an alternate toolchain](BUILDING-PlatformIO.md) is ready.

## Install Prerequisites

1. Install [Visual Studio Community 2022](https://visualstudio.microsoft.com/downloads/)
2. Install the [VisualGDB 30-day trial](https://visualgdb.com/download/).
3. Clone this repository to your local computer using one of these methods:
   * Click the 'Code' button on GitHub and click "Open in Visual Studio"
   * Open Visual Studio and and choose "Clone a repository" on the start screen
   * (Advanced) Use the [Git command-line tools](https://git-scm.com/download/win).

## Open and build the solution

1. If you cloned the repository with Visual Studio, you should see two solutions in the Solution Explorer, one for Escher Sketch, and one for Diver. Double-click the module you want to build.
2. Otherwise, open the solution file directly, either `lzxsoftware/hardware/diver/main/main.sln` or `lzxsoftware/hardware/eschersketch/main/main.sln`.
3. VisualGDB should now let you know about additional toolchain dependencies:
4. Click the top right *Resolve* button next to *Missing Toolchain*, select *Download the missing package*, and hit *OK*.
5. Click the second *Resolve* button, next to *Missing Board Support Package*, then click *Download BSP*.
6. Finally, hit the *Resolve* button in the lower right-hand corner. VisualGDB will download several additional components. 
7. When the downloads are complete, another Resolve Project Load dialog will appear, hit *Resolve* again.
8. If you get a dialog offering to upgrade your packages to the latest version, decline by hitting *Close* or *Cancel*.
9. Finally, from the *Build* menu, click *Build Solution*. You should see something like this:
   ```
   Linking VisualGDB/Debug/main...
   ------------------- Memory utilization report -------------------
   Used FLASH: 32KB out of 256KB (12%)
   Used SRAM: 52KB out of 128KB (41%)
   ========== Project Build Summary ==========
       main  built in 00:11
   ========== Build: 1 Succeeded, 0 Failed, 0 Skipped ==========
   ```

## Load the firmware onto the hardware

From here you have a few different options:

### Programming with DfuSeDemo

The [official instructions for updating Diver firmware](https://community.lzxindustries.net/t/all-about-diver/1455#firmware-update-instructions-7) use [DfuSeDemo](https://www.st.com/en/development-tools/stsw-stm32080.html). 
To follow these instructions, you'll need to convert the .hex or .bin file that VisualGDB built into a .dfu file. 
The *DFU File Manager* utility can perform this conversion and is part of the same download as DfuSeDemo.

### Programming with STM32CubeProgrammer

This is a newer tool from ST Microdevices that replaces DfuSeDemo. It can program .hex/.bin files directly, but it's 
more complex. If DfuSeDemo does not detect your attached Diver module as a programming target, then STM32CubeMX and 
its newer drivers might work. [Download it here](https://www.st.com/en/development-tools/stm32cubeprog.html), 
and you can follow the [official instructions](https://community.lzxindustries.net/t/all-about-diver/1455#firmware-update-instructions-7), except instead of using DfuSeDemo, do the following:
   * Run STM32CubeProgrammer and select the *Erasing & Programming* icon on the left side toolbar (second from the top.)
   * Change the dropdown in the upper-right-hand corner from *STLINK* to *USB*, then click *Connect*.
   * In the Download section, click *Browse* next to File Path and choose the `.hex` file that VisualGDB built, in `lzxsoftware\hardware\diver\main\VisualGDB\Debug\main.hex`.
   * Click *Start Programming*.
   * Click *Disconnect*.

### Programming with Segger J-Link directly in Visual Studio
   * TBD


## Debugging 

TBD. Debugging with VisualGDB currently requires a [Segger J-Link](https://www.segger.com/products/debug-probes/j-link/) programming device.
