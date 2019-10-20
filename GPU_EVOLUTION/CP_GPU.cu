#include "CP_GPU.h"

int main(int argc, char **argv){
	
	int args = 1;
	// size of the system (default 128)
	int L = (argc > args) ? (atoi(argv[args])):(100);
	if (L%2 == 1 || L < 0) {
		std::cout << "ERROR: L must be even and positive" << std::endl;
		exit(1);
	}
	args++;

	// iterations (default 1000)
	int trun = (argc > args) ? (atoi(argv[args])):(100000);
	args++;

	// global seed (default 0)
	int seed = (argc > args) ? (atoi(argv[args])):(0);
	srand(seed);

	for (float x = 0.0; x <= 6.0 ; x+=0.01) {

		ContactProcess S(L, x, seed);
		
		S.SetX(x);
		S.SetCounterM(0.0);
		S.SetCounterMM(0.0);

		S.Dynamics(trun);

		double m = S.GetCounterM() / (0.8 * trun);
		double mm = S.GetCounterMM() / (0.8 * trun);

		printf("%f\t%lf\t%lf\n", x, m, mm);

	}

	return 0;
}