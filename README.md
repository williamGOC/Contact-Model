# Contact Model
The folder ```CPU_CODE``` contain a ```c++``` code for Monte Carlo simulation.
## Requirements
- g++ (c++ compiler)
## Compilation
If you use any version of ```linux``` you can see the time evolution of density system as follow:
```
g++ -D TIME_EVOLUTION contacprocesses.cpp -o contacprocesses.out
./contacprocesses.out <lambda value>
```
for calculations of mean value of density as function of lambda you can use:
```
g++ -D SYSTEM_EVOLUTION contacprocesses.cpp -o contacprocesses.out
./contacprocesses.out <system's size> <lambda value>
```
