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

	const unsigned n = inst.reff * 2;		// size of chromosomes
	const unsigned p = args.p;		// size of population
	const double pe = args.pe;		// fraction of population to be the elite-set
	const double pm = args.pm;		// fraction of population to be replaced by mutants
	const double rhoe = args.rhoe;	// probability that offspring inherit an allele from elite parent
	const unsigned K = args.K;		// number of independent populations
	const unsigned MAXT = args.threads;	// number of threads for parallel decoding
	
	Decoder decoder{inst};				// initialize the decoder
	
	const long unsigned rngSeed = args.seed;	// seed to the random number generator
	MTRand rng(rngSeed);				// initialize the random number generator
	
	// initialize the BRKGA-based heuristic
	BRKGA< Decoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);
	
	unsigned generation = 0;		// current generation
	const unsigned X_INTVL = 100;	// exchange best individuals at every 100 generations
	const unsigned X_NUMBER = 2;	// exchange top 2 best
	const unsigned MAX_GENS = args.iterlimit;	// run for 1000 gens
	std::cout << "Running for " << MAX_GENS << " generations..." << std::endl;
	double runtime = 0;
	do {
		algorithm.evolve();	// evolve the population for one generation
		
		if((++generation) % X_INTVL == 0) {
			algorithm.exchangeElite(X_NUMBER);	// exchange top individuals
		}
		auto t1 = std::chrono::high_resolution_clock::now();
		runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0;
	} while (generation < MAX_GENS && runtime < args.timelimit);
	
	res.obj = algorithm.getBestFitness();
	
    auto t2 = std::chrono::high_resolution_clock::now();
    res.runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t0).count() / 1000.0;
    return res;
}
