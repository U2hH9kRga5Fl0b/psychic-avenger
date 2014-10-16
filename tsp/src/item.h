#ifndef TABU_H_
#define TABU_H_

#include "solution.h"
#include "tabu_list.h"

#include <algorithm>

class itemizer;

class neighbor_option
{
	friend itemizer;
public:
	neighbor_option(int stop1, int stop2, city* c);
	virtual ~neighbor_option();
	
	virtual double get_improvement(const solution* sol) const = 0;
	virtual bool in_bounds(const solution* sol) const = 0;
	virtual void apply(solution* sol) const = 0;

	virtual hash get_hash(const solution* sol) const = 0;
	
	friend std::ostream& operator<<(std::ostream& out, const neighbor_option& op);
protected:
	virtual std::string get_name() const = 0;
	
	int stop1, stop2;
	double dist;
};

// This represents the action of reversing all stops from stop1 to stop2
class swap_option : public neighbor_option
{
public:
	swap_option(int stop1, int stop2, city* c);
	~swap_option();
	
	double get_improvement(const solution* sol) const;
	bool in_bounds(const solution* sol) const;
	void apply(solution* sol) const;
	
	double get_cost_along_path(const solution* sol) const;
	bool intersects(const solution* sol) const;

	hash get_hash(const solution* sol) const;
protected:
	std::string get_name() const;
};

// This represents the action of rescheduling stop1 to come after stop2
class resched_option : public neighbor_option
{
public:
	resched_option(int stop1, int stop2, city* c);
	~resched_option();
	
	double get_improvement(const solution* sol) const;
	bool in_bounds(const solution* sol) const;
	void apply(solution* sol) const;
	
	hash get_hash(const solution* sol) const;
protected:
	std::string get_name() const;
	
};

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
	
	void print_improvements(const solution* sol) const;
	void sort(const solution* sol);
	
	void anneal(solution* sol, int num_steps, int range, std::function<void(void)> callback=[](){});

	void nth_anneal(solution* sol, int num_steps, int range, std::function<void(void)> callback=[](){});
	void sample_anneal(solution* sol, int num_steps, int nsamples, std::function<void(void)> callback=[](){});
	
	// tabu
	void hybrid_search(solution* sol, int nsamples, std::function<void(void)> callbac=[](){});

	// tabu
	void ordered_search(solution* sol, std::function<void(void)> callback=[](){});
	void greedy_search(solution* sol, std::function<void(void)> callback=[](){});
	
	
	void hybrid_search(solution* sol, double percent, std::function<void(void)> callback=[](){});
	void ordered_tabu_search(solution* sol, bloom_filter& tabs, int threshold, std::function<void(void)> callback=[](){});
private:
	void bubble_sort(const neighbor_option* first, const solution* sol);
	int num_options;
	neighbor_option** options;
};



#endif