# Zephyr application for MEC1723 eSPI debugger

Copyright (c) 2026 [Antmicro](https://www.antmicro.com)

## Overview

This repository contains a Zephyr application targeting Antmicro’s open hardware [MEC1723 eSPI Debugger](https://github.com/antmicro/mec1723-espi-debugger).

## Features

* Routes `UART` data received over eSPI to MEC1723's `UART1` interface.
* Captures `POST` codes written by the host to the 0x80 I/O port over eSPI.
* Displays `POST` codes onto a dual seven-segment LED indicator.

## Project structure

The main directory contains LICENSE and a README.

Zephyr project files are stored in the `mec1723-espi-debugger` directory.

## Getting started

### Initialize the workspace

Before getting started, make sure you have a proper Zephyr development environment.
Follow the official [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html).

### Prerequisites

The application was built and tested using the following versions:

* Zephyr version: `4.4.0`
* Toolchain version: `Zephyr 1.0.1`

### Microchip SPI image generator

[Microchip MEC172x SPI Image Gen 5 ](https://github.com/MicrochipTech/CPGZephyrDocs/tree/main) is required to generate an SPI image for the `MEC1723`.

Clone the MEC172x SPI Image Gen 5 repository:

```
git clone --depth 1 https://github.com/MicrochipTech/CPGZephyrDocs.git
```
Image generator will be in `MEC172x/SPI_image_gen` directory.

Make it available for Zephyr by making the image generator searchable by path, or by setting an `MEC172X_SPI_GEN` environment variable:

```
export MEC172X_SPI_GEN=<cloned repository path>/MEC172x/SPI_image_gen/mec172x_spi_gen_lin_x86_64
```

### West workspace initialization

Initialize a west workspace within the project directory:

```
west init -l mec1723-espi-debugger
west update
west zephyr-export
```

## Building

From the repository root directory, run:

```
west build -p always -b mec172xevb_assy6906 mec1723-espi-debugger
```

## Flashing

To program the app into the MCU's flash, use an external SPI programmer.

We suggest using Antmicro open hardware [Debug Toolkit](https://github.com/antmicro/ftdi-toolkit) connected to `J13` port located on the MEC1723 eSPI Debugger with [Tag-Connect TC2050-IDC-NL-050](https://www.tag-connect.com/wp-content/uploads/bsk-pdf-manager/TC2050-IDC-NL_Datasheet_8.pdf).

VCC and logic signals used for flashing the MCU should be set to `1.8V`.

### Flashrom setup

To flash the image into the SPI flash, use the `flashrom` tool. 

Make sure to build it from source, as the builds available in the package manager may be outdated and not support `ft2232_spi`:

```
sudo apt-get update
sudo apt-get install build-essential libftdi-dev libusb-1.0-0-dev libpci-dev zlib1g-dev
git clone https://review.coreboot.org/flashrom.git
cd flashrom
meson setup builddir
meson compile -C builddir
meson test -C builddir
meson install -C builddir
```

Verify the installation:
```
flashrom -L | grep ft2232_spi
```

### Padding the SPI image

For the flashrom to properly flash the image, the unused space must be filled with zeroes:

```
cp <build/zephyr/>spi_image.bin full_image.bin
truncate -s 64M full_image.bin
```

### Writing the SPI image

Run:

```
sudo flashrom -p ft2232_spi:type=4232H,port=A,divisor=16 -w full_image.bin
```

Expected output:
```
Found GigaDevice flash chip "GD25LB512MF/GD25LR512MF" (65536 kB, SPI) on ft2232_spi.
Reading old flash chip contents... done.
Updating flash chip contents... Erase/write done from 0 to 3ffffff
Verifying flash... VERIFIED.
```

**Note:**
Flashing could take up to 5 minutes. If the flash memory verification fails, increase the divisor value (e.g. divisor=32).

## Usage
### eSPI connection

Connect MEC1723 eSPI Debugger's `J1` eSPI connector to your eSPI (1V8) controller. For pinout information, see the [project repository](https://github.com/antmicro/mec1723-espi-debugger).

### eSPI tunneled UART

The UART tunneled over eSPI is available on `UART1` through the `J15` USB-C port. 

To access it:

```bash
sudo picocom -b 115200 /dev/ttyUSB1
```

### Zephyr console

The Zephyr console is available on `UART0` through the `J15` USB-C port. 

To access it:

```bash
sudo picocom -b 115200 /dev/ttyUSB0
```

### POST codes

Port 80 POST codes received over eSPI are automatically displayed in hexadecimal on the seven-segment LED indicator.

## License

This project is licensed under the [Apache-2.0](LICENSE) license.
