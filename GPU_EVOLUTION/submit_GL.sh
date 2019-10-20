#!/bin/bash
export PATH=/usr/local/cuda-8.0/bin${PATH:+:${PATH}}
export LD_LIBRARY_PATH=/usr/local/cuda-8.0/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}

nvcc CP_GPU.cu -arch=sm_20 -o CP_GPU.out -lglut -lGLU -lGL -lcufft -DIM
./CP_GPU.out
