# Building firmware using PlatformIO

**NOTE: this is a work in progress; documentation is provided for anyone interested in investigating this toolchain. Proceed at your own risk!**

### Requirements:

* A supported LZX Eurorack module (either Diver or Escher Sketch)
* Visual Studio Code or another IDE supported by PlatformIO
* Windows, Linux, or macOS
* A USB-A to Mini-B cable, or a STLink compatible programmer

## Install Prerequisites

1. Install [Visual Studio Code](https://code.visualstudio.com/).
2. Run Visual Studio Code and [follow these instructions](https://platformio.org/install/ide?install=vscode) to install the PlatformIO extension.
   * If you'd like to use a different IDE, read the [PlatformIO integration instructions here](https://platformio.org/install/integration). The rest of this document will assume Visual Studio Code, but the process should be similar in any IDE that PlatformIO integrates with.
3. Clone the forked repository at `https://github.com/Dewb/lzxsoftware` to your local computer using one of these methods:
   * Open Visual Studio Code and and choose "Clone Git repository" on the start screen and enter the above URL.
   * (Advanced) Use the [Git command-line tools](https://git-scm.com/download/win).
4. If it's not already selected, switch to the `dewb/refactor` or `dewb/platformio` branch using the Visual Studio source control toolbar, or the `git switch` command.

## Build the firmware and upload it to the hardware with USB

***IMPORTANT NOTE**: Do not connect your module to a computer with USB at the same time it is connected to Eurorack power.*

*Misconfiguring electronics could damage your module, your Eurorack case, and/or your computer. Proceed completely at your own risk!*

1. Disconnect your module from Eurorack power, and connect a USB A to Mini-B cable between your computer and the USB port on the back of the module.
2. Diver should present as a USB device in Device Firmware Update mode. If it doesn't appear in your USB device tree, or you get DFU errors in later steps, you may need to install STM32 DFU drivers.
3. Choose *Terminal > Run Task...* and select *PlatformIO: Upload (diver-usb)*, or click the arrow icon in the lower left PlatformIO toolbar. 
4. Disconnect the USB cable and reconnect the module to Eurorack power.

## Uploading firmware and debugging with ST-Link

***IMPORTANT NOTE**: Misconfiguring electronics could damage your module, your Eurorack case, and/or your computer. Proceed completely at your own risk!*

Debugging with PlatformIO should work with one or more of the following devices: 

* [STLink V2 clone](https://www.adafruit.com/product/2548)
   | Diver pin | STLink v2 pin |
   | ----------|---------------|
   | 6 +3V3    | 8 3.3V        |
   | 5 NRST    |               |
   | 4 SWO     |               |
   | 3 SWCLK   | 3 SWCLK       |
   | 2 SWDIO   | 4 SWDIO       |
   | 1 GND     | 5 GND         |

* [Official STLink V2 or V2-ISOL](https://www.st.com/en/development-tools/st-link-v2.html)
   | Diver pin | STLink v2 pin |
   | ----------|---------------|
   | 6 +3V3    | 1 MCU VDD     |
   | 5 NRST    |               |
   | 4 SWO     |               |
   | 3 SWCLK   | 9 SWCLK       |
   | 2 SWDIO   | 7 SWDIO       |
   | 1 GND     | 12 GND        |

1. Connect the specified pins of the debugger to the 6-pin debugging connector on the back panel of the module. Pin 1 is the closest to the bottom edge of the board.
2. Turn on your Eurorack case power.
3. Choose *Terminal > Run Task...* and select *PlatformIO: Upload (diver-stlink)*, or click the *Switch PlatformIO Project Environment* control on the PlatformIO toolbar and select *env:diver-stlink*, then click the arrow icon in the lower left of the toolbar. 
4. To debug the firmware code, go to *Run > Start Debugging* or hit F5. The module will reboot and pause in the initialization routine.

## Other debugging options that should work

* [Official STLink V3](https://www.st.com/en/development-tools/stlink-v3set.html) or [V3-MINIE](https://www.st.com/en/development-tools/stlink-v3minie.html)
   * TBD

* [Segger J-Link](https://www.segger.com/products/debug-probes/j-link/)
   * TBD   
