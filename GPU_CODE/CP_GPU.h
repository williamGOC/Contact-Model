#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/reduce.h>
#include <thrust/for_each.h>
#include <thrust/copy.h>
#include <thrust/fill.h>
#include <thrust/tuple.h>
#include <thrust/iterator/counting_iterator.h>
#include <thrust/iterator/transform_iterator.h>
#include <thrust/iterator/permutation_iterator.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/functional.h>

/* counter-based random numbers */
// http://www.thesalmons.org/john/random123/releases/1.06/docs/
#include "Random123/philox.h" 	// philox hearders
#include "Random123/u01.h"		// to get uniform deviates [0,1]
typedef r123::Philox2x32 RNG;   // particular counter-based RNG

#define WHITE	0
#define BLACK   1

__device__
float uniform(int n, int seed, int t) {

	// keys and counters
	RNG philox;
	RNG::ctr_type c = {{}};
	RNG::key_type k = {{}};
	RNG::ctr_type r;
	// Garantiza una secuencia random "unica" para cada thread
	k[0] = n;
	c[1] = seed;
	c[0] = t;
	r = philox(c, k);
	return (u01_closed_closed_32_53(r[0]));
}


struct evolution {

	int *spins;
	int L, N, shift, t, seed;
	float x;

	evolution(int* _spins, float _x, int _L, int _shift, int _t, int _seed):spins(_spins), x(_x), L(_L), shift(_shift), t(_t), seed(_seed) {

		N = L * L;
	}

	__device__
	void operator()(int index) {

		int n = (2 * index) + (int((2 * index) / L) + shift) % 2;

		int nx = n%L;
		int ny = int(n/L)%L;

		int left = (nx - 1 + L) % L + ny * L;
		int right = (nx + 1) % L + ny * L;
		int up= nx + ((ny - 1 + L) % L) * L;
		int down=nx +((ny + 1) % L) * L;

		float rate = 0.25 * (x / (x + 1.0)) * (1 - spins[n]) * (spins[left] + spins[right] + spins[up] + spins[down]) + (1.0 / (x + 1.0)) * spins[n];
		

		// random uniform number in [0,1]
		float rn = uniform(n, seed, t);

		if (rate >= rn)
			spins[n] = 1 - spins[n]; 
	}
	
};


class ContactProcess {

private:
	thrust::host_vector<int> host_spin;
	thrust::device_vector<int> spins;

	int * SpPtr;
	int L, N;
	float x;
	int seed, time;

public:
	ContactProcess(int _L, float _x, int _seed);

	void MonteCarloStep();
	void Dynamics(int trun);
	void Reinitialize(int sign);
	void SetX(float NewX);
	void addsquare(int x, int y, int z, int w, int sign);


	float GetX();
	float Magnetization();

	int GetTotalTime();

	int *dptr;
	
	~ContactProcess() {}
	
};


ContactProcess::ContactProcess(int _L, float _x, int _seed):L(_L), x(_x), seed(_seed) {

	N = L*L;

	host_spin.resize(N);
	thrust::fill(host_spin.begin(), host_spin.end(),0);

	spins.resize(N);
	thrust::copy(host_spin.begin(), host_spin.end(), spins.begin());

	SpPtr = thrust::raw_pointer_cast(&spins[0]);
	dptr = SpPtr;

	time = 0;
}

void ContactProcess::MonteCarloStep() {

	int t = time;
	int nwhite = N / 2;
	int nblack = N / 2;

	evolution owhite(SpPtr, x, L, WHITE, t, seed);
	evolution oblack(SpPtr, x, L, BLACK, t, seed);

	thrust::for_each(thrust::make_counting_iterator(0),
					 thrust::make_counting_iterator(nwhite),
					 owhite);

	thrust::for_each(thrust::make_counting_iterator(0),
					 thrust::make_counting_iterator(nblack),
					 oblack);

	time++;
}


void ContactProcess::Dynamics(int trun) {

	for (int t = 0; t < trun; t++){
		MonteCarloStep();

	
	}
}




void ContactProcess::SetX(float NewX) {

	x = NewX;
}


float ContactProcess::GetX() {

	return x;
}

float ContactProcess::Magnetization() {

	return float(thrust::reduce(spins.begin(), spins.end())) / N;
}

int ContactProcess::GetTotalTime() {

	return time;
}

void ContactProcess::Reinitialize(int sign){
	time = 0;
	thrust::fill(spins.begin(), spins.end(), sign);
}

void ContactProcess::addsquare(int x, int y, int z, int w, int sign){
	int ww = (w > L)?(L):(w);
	for(int j=y-ww/2;j<y+ww/2;j++){
		for(int i=x-ww/2;i<x+ww/2;i++){
			spins[(i+L)%L+((j+L)%L)*L]=sign;
	 	}   
	}
}