#pragma once

#include <vector>

class Decoder {
public:
	Decoder();	// Constructor
	~Decoder();	// Destructor

	// Decode a chromosome, returning its fitness as a double-precision floating point:
	double decode(const std::vector< double >& chromosome) const;
};