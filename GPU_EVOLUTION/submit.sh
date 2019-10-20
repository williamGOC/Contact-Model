#!/bin/bash
export PATH=/usr/local/cuda-8.0/bin${PATH:+:${PATH}}
export LD_LIBRARY_PATH=/usr/local/cuda-8.0/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}

nvcc CP_GPU.cu -arch=sm_20 -o CP_GPU.out

for L in 100 200 400 800 1600; do
	./CP_GPU.out ${L} >> DensityVsLambda_L_${L}.dat
done

