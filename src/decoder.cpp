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

	std::vector<std::pair<int, double>> idx_key;
	idx_key.reserve(instance.reff);
	for (int i = 0; i < instance.reff; ++i) {
		idx_key.push_back({i, chromosome[instance.reff + i]});
	}

	std::sort(idx_key.begin(), idx_key.end(), [] (auto const& a, auto const& b) { return a.second >= b.second; });

	std::vector<Config> sorted_items;
	sorted_items.reserve(items.size());

	for (int i = 0; i < items.size(); ++i) {
		sorted_items.push_back(items[idx_key[i].first]);
	}

	myFitness = bottom_left_impl(sorted_items, instance.ub, instance.w);

	return myFitness;
}