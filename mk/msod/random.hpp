#ifndef _random_hpp
#define _random_hpp
#include <gsl/gsl_rng.h>
#include <random>
#include <cmath>


class randomG {
	private:
		gsl_rng* r;
		std::mt19937 engine;

	public:
		randomG() 
		{ r=gsl_rng_alloc (gsl_rng_ranlux); }
		~randomG() { gsl_rng_free(r); }
//		void randomize();
		void randomize( long seed );
		std::mt19937& get_engine() {
			return engine;
		}
//		void randomize( double seed );
		double uniform01();
		double uniform( double a, double b );
		int    uniform( int a, int b );
		double exponential( double mu );
		bool   bernoulli( double p );
		bool   flip( double p ) { return bernoulli(p); }
		long   geometric( double p );
		double normal01();
		double normal( double mean, double variance );
};


#endif
