# ORB-implementation-on-STM32U575ZI-Q
A from-scratch implementation of the ORB (Oriented FAST and Rotated BRIEF) computer vision algorithm for the STM32U575ZI-Q. The project started as a naive working implementation and is currently focused on optimizing performance.

- [What is it?](#what)
- [Why did you make it?](#why)
- [Progress](#progress)
- [Performance](#performance)
- [Roadmap](#roadmap)
- [Hardware](#hardware)

## What
This project implements the ORB feature detection and description algorithm in STM32CubeIDE, targeting 320×240 images.

The implementation uses the DWT cycle counter for cycle-accurate performance measurements. Serial Wire Viewer (SWV) is used to output profiling and benchmark results through STM32CubeIDE.

Each branch contains the current best implementation of a specific stage of the ORB pipeline. Commits include profiling information showing how each change affects performance.

## Why
I started this project after reading this [research article](https://www.mdpi.com/1424-8220/25/12/3796). I wanted to try implementing a similar approach myself and explore how a computer vision algorithm such as ORB could be optimized for a resource-constrained embedded system.

## How
I used DWT cycle counter to count the cycles between each function and then aggregates the data and displays it using (SWV) in a easy to read format. A known flaw is that the cycles to store the data is also counted towards the total number of cycles. This means that the more granular each cycle reading is the more skewed the whole result. This is also the reason for the "unknown" category in the commits, to give a rough idea how much was actually the steps and what is other factors.

## Progress
| ORB stage | Status |
|------|-------------|
| Gaussian blur | Not implemented |
| FAST | Optimized | 
| Harris corner | Optimized |
| Image Centroid | Optimized  |
| BRIEF | Work in progress | 

## Running

1. Open the project in STM32CubeIDE.
2. Connect an STM32U575ZI-Q development board.
3. Build the project.
4. Flash the firmware.
5. Open the serial output / debugger to view the results.

## Performance
The measurements are the averages.
Speedups= Naive/current execution time

| ORB stage | Naive | Current | Speedup|  Measurement| 
|------|-------------|--------|--------|--------|
| Gaussian blur | - | - | - |
| FAST | 4.75 us | 0.5 us | 9.5x |Per pixel|
| Harris corner | 210.56 us | 12.29 us | 17.13x | Per FAST keypoint|
| Image Centroid |  384.68 us | 26.24 us | 14.66x | Per keypoint |
| BRIEF | 399.81  | - | - | - |


## Roadmap

- [ ] Fix FAST all-pass bug
- [ ] Implement Gaussian blur
- [ ] Complete BRIEF implementation
- [ ] Profile and optimize FAST using assembly

## Hardware

- **MCU:** STM32U575ZI-Q
- **Image resolution:** 320×240
- **IDE:** STM32CubeIDE 2_0.0
- **Compiler:** ARM GCC
- **CPU frequency:** 160 MHz
- **Performance counter:** DWT
- **Output:** Serial Wire Viewer (SWV)
- **Board:** NUCLEO-U575ZI-Q
