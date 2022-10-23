---
author: Kristoffer Plagborg Bak Sørensen
title: "Wireless Sensor Networks - Assignment 2"
date: 22-10-2022
geometry: "left=3cm,right=3cm,top=3cm,bottom=3cm"
output: pdf_document
---

<!-- https://dsp.stackexchange.com/questions/58326/what-do-you-mean-by-radio-sensing-a-channel?newreg=9dfbfee0c9e84512854071be332a5451 -->


<!-- **auid:** 649525 -->



## Introduction

> This report goes over how one can measure the interference level at different channels in the IEEE 802.15.4 2.4 GHz radio frequency band with the `contiki-ng` operating system.

## Motivation

The IEEE 802.15.4 uses ISM band 2.4 GHz radio frequencies (from channel 11 to channel 26) to transmit and receive data. This frequency spectrum is also used by other radio technologies, such as WiFi, Bluetooth. It is desirable to know which channels are the least interfered with, so that the sensor network can use these channels to transmit data. This is especially important for sensor networks that are deployed in urban areas, where there are many WiFi and Bluetooth devices. If there is a lot of interference on a channel, the sensor network will not be able to transmit data reliably, or have to use a more energy in the radio amplifier to transmit data.


## Methodology

Channel sensing is used to determine the channel with the lowest interference level. Channel sensing works by making the radio enter receive mode on a given channel, and then sample over a short period. The received signal strengh indicator (RSSI) is used as a measurement.

$$RSSI = \sum_{n = 0}^{N - 1}|x[n]|^2\ \mathrm{dBm} $$

where $N$ is the number of samples i.e. the radio receivers configured window length, which corresponds to the number of samples sampled during a sampling period.

In `contiki-ng` the following modules are used to measure the interference level on a given channel:

```c
#include "dev/radio.h"
#include "net/netstack.h"
```

To change the channel, the following function call is used:

```c
NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, 15);
```

To sample the RSSI, the following function call is used:
```c
radio_value_t rssi;
NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &rssi);
```

To determine the interference level, the RSSI is measured as an average of 100 measurements, measured in quick succession after each other. This is done to get a more accurate measurement of the RSSI, as the RSSI is a noisy value.


## Experiments and Results

The Telos B Mote microcontroller was used for the hardware. The Telos B Mote has a CC2420 radio transceiver, which is a IEEE 802.15.4 compliant radio transceiver. 


|rank|id|rssi|
|:-:|:-:|:-:|
|0|20|-93|
|1|21|-92|
|2|24|-92|
|3|11|-91|
|4|14|-91|
|5|15|-90|
|6|16|-90|
|7|18|-90|
|8|22|-90|
|9|25|-90|
|10|26|-90|
|11|19|-88|
|12|12|-87|
|13|23|-86|
|14|17|-85|
|15|13|-82|

In this experiment, channel `20`, with an average RSSI value of `-93 dBm` over 100 samples, was the best channel.


The dynamic range observed in this experiment is `~ 11 dBm`, which roughly corresponds to tenfold increase/decrease in signal strength, which would seem to be significant.

## Repository

[github.com/kpbs5/wireless_sensor_networks_assignment_2](https://github.com/kpbs5/wireless_sensor_networks_assignment_2)


## Intructions for how to replicate results

Build binary and upload to Telos B Mote:
```bash
make TARGET=sky MOTES=/dev/ttyUSB0 measure_interference.upload login
```

The `./Makefile` assumes that the repository is placed in a subdirectory of your `contiki-ng` installation. e.g. `contiki-ng/<subdir>/<repo>`

Clean build:
```bash
make distclean
```

Generate report:
```bash
pandoc ./report.md -o ./report.pdf
```
