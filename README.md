# ORB-implementation-on-STM32U575ZI-Q
A from-scratch implementation of the ORB (Oriented FAST and Rotated BRIEF) computer vision algorithm for the STM32U575ZI-Q. The project started as a naive working implementation and is currently focused on optimizing performance.

- [What is it?](#what-is-it)
- [Why did you make it?](#why-did-you-make-it)
- [Progress](#progress)
- [Performance](#performance)
- [Roadmap](#roadmap)

## What is it
This project implements the ORB feature detection and description algorithm in STM32CubeIDE, targeting 320×240 images.

The implementation uses the DWT cycle counter for cycle-accurate performance measurements and SVD profiling to identify performance bottlenecks and guide optimization.

Each branch corresponds to the current best version of that specific stage in the ORB algorithm. Each commit has detailed profiling information detailing the performance of the system at that stage.

## Why did you make it
I started this project after reading this [research article](https://www.mdpi.com/1424-8220/25/12/3796). I wanted to try implementing a similar approach myself and explore how a computer vision algorithm such as ORB could be optimized for a resource-constrained embedded system.

## Progress
| ORB stage | Status |
|------|-------------|
| Gaussian blur | Not implemented |
| FAST | Optimized | 
| Harris corner | Optimized |
| Image Centroid | Optimized  |
| BRIEF | Work in progress | 


## Performance
The measurements are the averages.
| ORB stage | Naive | Current | Speedup|  Comment| 
|------|-------------|--------|--------| Comment|
| Gaussian blur | x | x | Not implemented |
| FAST | 4.75 | 0.5 us | 9.5x |Per pixel|
| Harris corner | 210.56 us | 12.29 us |17.13x | Per keypoint that pass FAST|
| Image Centroid |  384.68 us | 26.24us | 14.66x | Per keypoint |
| BRIEF | 399.81  | x |x | Not implemented |


## Roadmap

- [ ] Solve FAST all-pass bug
- [ ] Implement Gaussian blur
- [ ] Complete BRIEF implementation
- [ ] Profile FAST using assembly 
