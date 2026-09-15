# MEC1723 eSPI Zephyr debugger

Copyright (c) 2026 [Antmicro](https://www.antmicro.com)

## Overview

This repository contains software source codes for Zephyr application targeting Antmicro’s open hardware [MEC1723 eSPI Debugger](https://github.com/antmicro/mec1723-espi-debugger).

## Features

* Routes `UART` data received over eSPI to the MEC1723's `UART1` interface.
* Captures `POST` codes written by the host to I/O port 0x80 over eSPI.
* Displays `POST` codes onto dual seven-segment LED indicator.

## Getting Started

### Initialize the workspace

Before getting started, make sure you have a proper Zephyr development environment. Follow the official [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html).

### Microchip's SPI image generator

[Microchip's SPI image generator](https://github.com/MicrochipTech/CPGZephyrDocs/tree/main/MEC172x/SPI_image_gen) is required to generate SPI image for the `MEC1723`.

Clone the [MEC172x SPI Image Gen 5](https://github.com/MicrochipTech/CPGZephyrDocs/tree/main) repository.

Make the image generation available for Zephyr, by making the tool searchable by path, or by setting an environment variable `MEC172X_SPI_GEN`:

```
export MEC172X_SPI_GEN=<path to tool>/mec172x_spi_gen_lin_x86_64
```

### West workspace initialization

Initialize a west workspace within the project directory

```
west init -l mec1723-espi-debugger
west update
west zephyr-export
```

## Building

From repository root directory

```
west build -p always -b mec172xevb_assy6906 mec1723-espi-debugger
```

## Flashing

To program the app into the MCU's flash use external SPI programmer.

We suggest using Antmicro's open hardware [Debug Toolkit](https://github.com/antmicro/ftdi-toolkit) connected to [MEC1723 eSPI Debugger's](https://github.com/antmicro/mec1723-espi-debugger) `J13` with [Tag-Connect TC2050-IDC-NL-050](https://www.tag-connect.com/wp-content/uploads/bsk-pdf-manager/TC2050-IDC-NL_Datasheet_8.pdf).

VCC and logic signals should be set to `1.8V`.

### Flashrom setup

To flash the image into SPI flash, use the `flashrom` tool. 

Make sure to build it from source, as the builds in package manager may be outdated and not support `ft2232_spi`:

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
./flashrom -L | grep ft2232_spi
```

### Padding the SPI Image

For the flashrom to properly flash the image, the unused space must be filled with zeroes:

```
cp <spi-image-name>.bin full_image.bin
truncate -s 64M full_image.bin
```

### Flashing

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
Flashing could take up to 5 minutes. In a case of failed flash verification, increase divisor value (e.g. divisor=32).

## Usage
### eSPI connection

Connect MEC1723 eSPI Debugger's `J1` eSPI connector to your eSPI (1V8) controller accordingly. For the pinout, reference [project repository](https://github.com/antmicro/mec1723-espi-debugger).

### eSPI tunneled UART

The UART tunneled over eSPI is available on `UART1` through the `J15` USB-C port. 

To access it:

```bash
sudo picocom -b 115200 /dev/ttyUSB1
```

### Zephyr console

Zephyr console is available on `UART0` through the `J15` USB-C port. 

To access it:

```bash
sudo picocom -b 115200 /dev/ttyUSB0
```

### POST codes

Port 80 POST codes received over eSPI are automatically displayed in hexadecimal on the D6 seven-segment display.