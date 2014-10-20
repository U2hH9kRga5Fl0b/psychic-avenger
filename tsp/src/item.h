#ifndef TABU_H_
#define TABU_H_

#include "solution.h"
#include "tabu_list.h"

#include "neighbor_options.h"

#include <algorithm>


// This represents all the actions that could be performed on a city (whether they be good, tabu, or bad).
class itemizer
{
public:
	itemizer(city* c);
	~itemizer();
	
	const neighbor_option* get_random_option(const solution* sol);
	const neighbor_option* get_best_option(const solution* sol);
	const neighbor_option* get_best_of_sample(const solution* sol, int nsamples);
	const neighbor_option* get_nth_best_option(const solution* sol, int n, bool hurt=false);

	int get_num_options() const { return num_options; };
	const neighbor_option* get_option(int ndx) const { return options[ndx]; };
	
	void print_improvements(const solution* sol) const;
	void sort(const solution* sol);
	





// search....
	void anneal(solution* sol, int num_steps, int range, std::function<void(void)> callback=[](){});

	void nth_anneal(solution* sol, int num_steps, int range, std::function<void(void)> callback=[](){});
	void sample_anneal(solution* sol, int num_steps, int nsamples, std::function<void(void)> callback=[](){});
	
	void hybrid_search(solution* sol, int nsamples, std::function<void(void)> callbac=[](){});

	// tabu
	void ordered_search(solution* sol, std::function<void(void)> callback=[](){});
	void greedy_search(solution* sol, std::function<void(void)> callback=[](){});

	// tabu
	
	void hybrid_search(solution* sol, double percent, std::function<void(void)> callback=[](){});
	void ordered_tabu_search(solution* sol, bloom_filter& tabs, int threshold, std::function<void(void)> callback=[](){});
private:
	void bubble_sort(const neighbor_option* first, const solution* sol);
	int num_options;
	neighbor_option** options;
};



#endif
