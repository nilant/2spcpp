#include "heuresult.hpp"
#include "instance.hpp"
#include "cli.hpp"
#include "decoder.hpp"
#include "MTRand.h"
#include "BRKGA.h"

#include <chrono>
#include <fmt/core.h>

HeurResult heuristic(Instance const& inst, Args const& args) {

    auto t0 = std::chrono::high_resolution_clock::now();
    HeurResult res{args.exec_name};

	const unsigned n = 10;		// size of chromosomes
	const unsigned p = 100;		// size of population
	const double pe = 0.10;		// fraction of population to be the elite-set
	const double pm = 0.10;		// fraction of population to be replaced by mutants
	const double rhoe = 0.70;	// probability that offspring inherit an allele from elite parent
	const unsigned K = 3;		// number of independent populations
	const unsigned MAXT = 1;	// number of threads for parallel decoding
	
	Decoder decoder;				// initialize the decoder
	
	const long unsigned rngSeed = 0;	// seed to the random number generator
	MTRand rng(rngSeed);				// initialize the random number generator
	
	// initialize the BRKGA-based heuristic
	BRKGA< Decoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);
	
	unsigned generation = 0;		// current generation
	const unsigned X_INTVL = 100;	// exchange best individuals at every 100 generations
	const unsigned X_NUMBER = 2;	// exchange top 2 best
	const unsigned MAX_GENS = 1000;	// run for 1000 gens
	std::cout << "Running for " << MAX_GENS << " generations..." << std::endl;
	do {
		algorithm.evolve();	// evolve the population for one generation
		
		if((++generation) % X_INTVL == 0) {
			algorithm.exchangeElite(X_NUMBER);	// exchange top individuals
		}
	} while (generation < MAX_GENS);
	
	res.obj = algorithm.getBestFitness();
	
    auto t1 = std::chrono::high_resolution_clock::now();
    res.runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0;
    return res;
}
