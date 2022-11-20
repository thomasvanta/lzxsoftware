# Building firmware using PlatformIO

**NOTE: this is a work in progress, the firmware built from this fork is not yet fully functional. Documentation is provided for anyone interested in investigating this toolchain. Proceed at your own risk!**

### Requirements:

* Visual Studio Code or another IDE supported by PlatformIO
* Windows, Linux, or macOS
* A USB-A to Mini-B cable

## Install Prerequisites

1. Install [Visual Studio Code](https://code.visualstudio.com/).
2. Run Visual Studio Code and [follow these instructions](https://platformio.org/install/ide?install=vscode) to install the PlatformIO extension.
   * If you'd like to use a different IDE, read the [PlatformIO integration instructions here](https://platformio.org/install/integration). The rest of this document will assume Visual Studio Code, but the process should be similar in any IDE that PlatformIO integrates with.
3. Clone the fork of this repository at `https://github.com/Dewb/lzxsoftware` to your local computer using one of these methods:
   * Open Visual Studio Code and and choose "Clone Git repository" on the start screen and enter the above URL.
   * (Advanced) Use the [Git command-line tools](https://git-scm.com/download/win).
4. Switch to the `dewb/platformio` branch using the Visual Studio source control toolbar, or the `git switch` command.

## Build the solution

1. Choose *Terminal > Run Task...* and select *PlatformIO: Build*, or click the checkmark icon in the lower left PlatformIO toolbar.

## Load the firmware onto the hardware

**IMPORTANT NOTE**: Do not connect your module to a computer and Eurorack power at the same time. Misconfiguring electronics could damage your module, your Eurorack case, and/or your computer. This is an unofficial guide from internet randos, proceed completely at your own risk.

1. Disconnect your module from Eurorack power, and connect a USB A to Mini-B cable between your computer and the USB port on the back of the module.
2. Choose *Terminal > Run Task...* and select *PlatformIO: Upload*, or click the arrow icon in the lower left PlatformIO toolbar. 
3. Disconnect the USB cable and reconnect the module to Eurorack power.

## Debugging the firmware 

TBD. Debugging with PlatformIO should work with one or more of the following devices:
* [STLink v2 clone](https://www.adafruit.com/product/2548)
* [Official STLink v2 or v3](https://www.st.com/en/development-tools/st-link-v2.html)
* [Segger J-Link](https://www.segger.com/products/debug-probes/j-link/)
