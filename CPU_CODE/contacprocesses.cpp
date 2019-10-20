#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <fstream>
#include <numeric>
#include <vector>
#include <ctime>
#include <cmath>


#define D 1
#define Q 2

#define N_SWEEPS 100000


void MC_STEP(std::vector<unsigned short>& spins, std::vector<unsigned>& neighbors, float x, int L);

#ifdef MOVIE
	void printer(std::vector<unsigned short> spins, FILE *gnuplotpipe, int L);
#endif


int main(int argc, char const *argv[]) {
	
	int args = 1;

	int L = (argc > args)?(atoi(argv[args])):(500);
	args++;

	int N = pow(L, D);

	#ifdef SYSTEM_EVOLUTION
		char NameFolder[50];
		sprintf(NameFolder, "DensityVsLambda_L_%d.dat", L);
		std::ofstream folder(NameFolder);
	#endif


	#ifdef TIME_EVOLUTION
		float x = (argc > args)?(atof(argv[args])):(3.0);
		args++;

		char NameFolder[50];
		sprintf(NameFolder, "DensityVsTimeL_%d_Lambda_%f.dat", L, x);
		std::ofstream folder(NameFolder);
	#endif


	#ifdef MOVIE
		float x = (argc > args)?(atof(argv[args])):(3.0);
		args++;

		FILE *pipe = popen("gnuplot", "w");
		assert(pipe);
	#endif

	srand48(time(NULL));

	// each spin has value 0, 1, ..., Q-1.
	std::vector<unsigned short> spins(N);

	// calculate lattice volume elements
	std::vector<unsigned> volume(D);
	for (unsigned i = 0; i <= D; i++) {
		if (i == 0)
			volume[i] = 1;
		else
			volume[i] = volume[i-1] * L;
	}


	// neighborhood table:
	std::vector<unsigned> neighbors(2*D*N);

	// calculate neighborhood table:
	for(unsigned i = 0; i < N; i++) {
		unsigned short k=0;

		for(unsigned short dim = 0; dim < D; dim++) {	// dimension loop
			for(short dir =- 1; dir <= 1; dir += 2) {	// two directions in each dimension
		
				// neighbor's id in spin list:
				int npos = i + dir * volume[dim];

				// periodic boundary conditions:
				int test = (i % volume[dim+1]) + dir*volume[dim];

				if(test < 0)
					npos += volume[dim+1];
				else if(test >= volume[dim+1])
					npos -= volume[dim+1];
				
				neighbors[2*D*i + k] = npos;
				k++;
			}
		}

		spins[i] = 1;
	}

	int magnetization;

	#ifdef SYSTEM_EVOLUTION
		for (float x = 0.0; x <= 5.0; x += 0.001) {

			double sum_m = 0.0;
			double sum_mm = 0.0;
	#endif

		for (int i = 0; i < N_SWEEPS; i++) {
		
			// Monte Carlo step
			MC_STEP(spins, neighbors, x, L);
			// Is absorving state??
			#ifdef SYSTEM_EVOLUTION
				if (i > 0.2*N_SWEEPS) {
			#endif
					
					magnetization = std::accumulate(spins.begin(), spins.end(), 0);
		
					if (magnetization == 0.0)
						spins[floor(drand48() * N)] = 1;

			#ifdef SYSTEM_EVOLUTION
					double m = static_cast<double>(magnetization) / static_cast<double>(N);
					sum_m += m;
					sum_mm += m*m;
				}
			#endif

			#ifdef TIME_EVOLUTION
				folder << i << '\t' << static_cast<double>(magnetization) / static_cast<double>(N) << std::endl;
			#endif


			#ifdef MOVIE
				printer(spins, pipe, L);
			#endif
		}
		
		#ifdef SYSTEM_EVOLUTION
			double m = sum_m / (0.8 * N_SWEEPS);
			double mm = sum_mm / (0.8 * N_SWEEPS);

			folder << x << '\t' << m << '\t' << mm << std::endl;
		#endif

		#ifdef MOVIE
			pclose(pipe);
		#endif
	
	#ifdef SYSTEM_EVOLUTION
		}
	#endif

	#ifndef MOVIE
		folder.close();
	#endif

	return 0;
}



void MC_STEP(std::vector<unsigned short>& spins, std::vector<unsigned>& neighbors, float x, int L) {

	int N = pow(L, D);
	float lambda =  (x / (x + 1));

	for (unsigned i = 0; i < N; i++) {
		
		unsigned index = floor(drand48() * N);
		unsigned short spinstate = spins[index];

		float rate = 0.0;

		for(unsigned j = 0; j < 2*D; j++) 
			rate += (lambda / (2.0*D)) * (1 - spinstate) * spins[neighbors[2*D*index + j]];
		
		rate += (1.0 - lambda) * spinstate;
	
		if (rate >= drand48())
			spins[index] = 1-spinstate;
	}
} 



#ifdef MOVIE
	void printer(std::vector<unsigned short> spins, FILE *gnuplotpipe, int L) {      
    	
    	int N = pow(L, D);
  		// comandos sobre la terminal para ejecutar acciones en gnuolot.
  		fprintf(gnuplotpipe, "set title '{/=20 Sistema de Spines}'\n");
  		fprintf(gnuplotpipe, "unset xtics ; unset ytics\n");
  		fprintf(gnuplotpipe, "set xrange[-3:%d]; set yrange[-4:%d]\n",L+3,L+3);
  		fprintf(gnuplotpipe,  "plot '-' u 1:2:3  w p pt 5 ps 3 lc variable t ''\n");
   
  		for (int i = 0; i < N; i++)
  			fprintf(gnuplotpipe, "%d\t%d\t%d\n",i % L, i / L, spins[i]);
  
		fprintf(gnuplotpipe, "e\n" );
  		fflush(gnuplotpipe);
	}
#endif
