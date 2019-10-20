#! /bin/bash

mkdir ./1D/Simulation_full

g++ -D SYSTEM_EVOLUTION contacprocesses.cpp -o contacprocesses.out

for L in 400 1600; do
	./contacprocesses.out ${L}
	cp DensityVsLambda_L_${L}.dat ./1D/Simulation_full
	rm DensityVsLambda_L_${L}.dat
done
