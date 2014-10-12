#ifndef TABU_H_
#define TABU_H_


#include "solution.h"

#include <algorithm>

class tabu_options;

class tabu_option
{
	friend tabu_options;
public:
	tabu_option(int stop1, int stop2, city* c, const tabu_option *n=nullptr);
	virtual ~tabu_option();
	
	tabu_option(const tabu_option& other) = delete;
	tabu_option& operator=(const tabu_option& other) = delete;
	
	virtual double get_improvement(const solution* sol) const = 0;
	virtual bool in_bounds(const solution* sol) const = 0;
	virtual void apply(solution* sol) const = 0;
	
	friend std::ostream& operator<<(std::ostream& out, const tabu_option& op);
protected:
	virtual std::string get_name() const = 0;
	
	int stop1, stop2;
	double dist;
	
	const tabu_option* next;
};

// This represents the action of reversing all stops from stop1 to stop2
class swap_tabu_option : public tabu_option
{
public:
	swap_tabu_option(int stop1, int stop2, city* c, const tabu_option *n=nullptr);
	~swap_tabu_option();
	
	double get_improvement(const solution* sol) const;
	bool in_bounds(const solution* sol) const;
	void apply(solution* sol) const;
	
	double get_cost_along_path(const solution* sol) const;
	bool intersects(const solution* sol) const;
	
	swap_tabu_option(const swap_tabu_option& other) = delete;
	swap_tabu_option& operator=(const swap_tabu_option& other) = delete;
protected:
	std::string get_name() const;
};

// This represents the action of rescheduling stop1 to come after stop2
class reschedule_tabu_option : public tabu_option
{
public:
	reschedule_tabu_option(int stop1, int stop2, city* c, const tabu_option *n=nullptr);
	~reschedule_tabu_option();
	
	double get_improvement(const solution* sol) const;
	bool in_bounds(const solution* sol) const;
	void apply(solution* sol) const;
	
	reschedule_tabu_option(const reschedule_tabu_option& other) = delete;
	reschedule_tabu_option& operator=(const reschedule_tabu_option& other) = delete;
protected:
	std::string get_name() const;
	
};

// This represents all the actions that could be performed on a city (whether they be good, tabu, or bad).
class tabu_options
{
public:
	tabu_options(city* c);
	~tabu_options();
	
	const tabu_option* get_random_option(const solution* sol);
	const tabu_option* get_best_option(const solution* sol);
	void print_improvements(const solution* sol) const;
	
	const swap_tabu_option* swap_options;
	const reschedule_tabu_option* reschedule_options;
	
	void sort(const solution* sol);
	
	// first geniune tabu search
	void apply_all_improvements(solution* sol, std::function<void(void)> callback=[](){});
private:
	void bubble_sort(const tabu_option* first, const solution* sol);
};


void local_search(solution *sol, std::function<void(void)> callback=[](){});



#endif