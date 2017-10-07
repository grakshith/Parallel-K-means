# Parallel-K-means

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
