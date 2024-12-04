#pragma once

#include <vector>

#include "instance.hpp"

class Decoder {
	Instance const& instance;

public:
	Decoder(Instance const& inst) : instance{inst} {}


	// Decode a chromosome, returning its fitness as a double-precision floating point:
	double decode(const std::vector< double >& chromosome) const;
};