# MEC1723 eSPI Zephyr debugger

This repository contains software source codes for Zephyr application targeting the Microchip MEC1723 / MEC172xEVB and demonstrating eSPI peripheral functionality.

## Features

* eSPI peripheral using Zephyr `microchip,xec-espi-v2` driver
* eSPI tunneled UART exposed onto `UART1`
* Additionally in `mec1723-espi-debugger`
  * Port 80 POST code reception
  * POST code display on two 7-segment displays


## Building

### Initialize the workspace

Before getting started, make sure you have a proper Zephyr development environment. Follow the official [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html).

[Microchip's SPI image generator](https://github.com/MicrochipTech/CPGZephyrDocs/tree/main/MEC172x/SPI_image_gen) is required to generate SPI image for the `MEC1723`

Clone the [MEC172x SPI Image Gen 5](https://github.com/MicrochipTech/CPGZephyrDocs/tree/main) repository or download the files within that directory.

Make the image generation available for Zephyr, by making the tool searchable by path, or by setting an environment variable MEC172X_SPI_GEN, for example:

```
    export MEC172X_SPI_GEN=<path to tool>/mec172x_spi_gen_lin_x86_64
```

### Building

The repository contains two target variants:

* [mec1723-evb](https://www.microchip.com/en-us/development-tool/ev71e60a#Overview)

* mec1723-espi-debugger

Below example targets the `mec1723-evb`

From repository root directory

```
    west init -l ./mec1723-evb
    west update
    west zephyr-export
    west build -p always -b mec172xevb_assy6906 ./mec1723-evb

```

## Flashing

## Console output
