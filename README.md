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
To see a movie of the evolution of the system in real time (in the case of a chain we can make a mapping to a square lattice) we can use the ```gnuplot``` software. The code in linux is as follow
```c++
// In te main fuction
...
FILE *pipe = popen("gnuplot", "w");
// body of main
pclose(pipe)
...

// external to the main function
void printer(std::vector<unsigned short> spins, FILE *gnuplotpipe, int L) {      
  
  int N = pow(L, D)
  fprintf(gnuplotpipe, "set title '{/=20 Sistema de Spines}'\n");
  fprintf(gnuplotpipe, "unset xtics ; unset ytics\n");
  fprintf(gnuplotpipe, "set xrange[-3:%d]; set yrange[-4:%d]\n",L+3,L+3);
  fprintf(gnuplotpipe,  "plot '-' u 1:2:3  w p pt 5 ps 3 lc variable t ''\n");
   
  for (int i = 0; i < N; i++)
  	fprintf(gnuplotpipe, "%d\t%d\t%d\n",i % L, i / L, spins[i]);
  
  fprintf(gnuplotpipe, "e\n" );
  fflush(gnuplotpipe);
}
```
