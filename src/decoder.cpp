#include "decoder.hpp"
#include "bottom_left.hpp"

double Decoder::decode(const std::vector< double >& chromosome) const {
	double myFitness = 0.0;

	std::vector<Config> items;
	items.reserve(instance.reff);

	int idx = 0;
	for (auto const& task : instance.tasks) {
		for (int r = 0; r < task.repeat; ++r) {
			size_t nconfig = task.configs.size();
			auto selected_config = task.configs[std::floor(chromosome[idx] * nconfig)];
			items.push_back(selected_config);
			++idx;
		}
	}

	assert(idx == instance.reff);

	std::sort(items.begin(), items.end(), 
             [](auto const& a, auto const& b) { 
                if ((a.h > b.h) || (a.h == b.h && a.w < b.w)) {
                    return true;
                }
                return false;
            }
    );

	myFitness = bottom_left_impl(items, instance.ub, instance.w);

	return myFitness;
}