# Parallel-K-means

Repository contains code for k-means clustering used for Image Colour Quantization and Image Compression. 

## Overview

Image colour quantization is an important problem in Computer Graphics and Image Processing. This reduces the
number of colours in the image and facilitates the display, storage, and transmission of images. This is a necessary prepossessing to make images compatible for low-end hardware device which can display a limited number of colours. This work shows how this quantization can be achieved using of k-means clustering. The algorithm is implemented in parallel in various platforms such as OpenMP, CUDA, and MPI. We find that CUDA outperforms other platforms by a large margin due to fine-grained parallelism and higher degree of concurrency.

OpenCV Installation link can be found [here](http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html)

```bash 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
```

To compile : 
```bash 
g++ a.cpp `pkg-config --cflags --libs opencv`
```
To run kmeans_serial.cpp : 
```bash
./a.out test.jpg
```
Or just use serial.sh  :p
