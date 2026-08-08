# ORB-implementation-on-STM32U575ZI-Q
Self made implementation of ORB on a STM32U575ZI-Q. First made a naive working benchmark and is currently trying to optimize it.

## What is it
An implementation of the computer vision algorithm ORB in STM32CUBEIDE on a 320x240 image. It uses DWT cycle counting and SVD to profile and display the results.

## Why did you make it
I made it as a project after I saw this research article https://www.mdpi.com/1424-8220/25/12/3796 and wanted to try to do my own.

## Progress
| ORB stage | Status | Next stage |
|------|-------------|--------|
| Gaussian blur | X | Not implemented |
| FAST | Optimized | In progress |
| Harris corner | Optimized | Planned |
| Image Centroid | Optimized | Planned |
| BRIEF | Work in progress | Planned |
