#ifndef TABU_H_
#define TABU_H_


// This represents the action of reversing all stops from stop1 to stop2
class swap_tabu_option
{
public:
	swap_tabu_option(int stop1, int stop2, city* c);
	~swap_tabu_option();
	
	double get_cost_along_path(solution* sol) const;
	double get_improvement(solution* sol) const;
	bool intersects(solution* sol) const;
	bool in_bounds(solution* sol) const;
	
	int stop1, int stop2;
	double dist;
	
	swap_tabu_option* next;
};

// This represents the action of rescheduling stop1 to come after stop2
class reschedule_tabu_option
{
public:
	reschedule_tabu_option(int stop1, int stop2, city* c);
	~reschedule_tabu_option();
	
	double get_improvement(solution* sol) const;
	bool in_bounds(solution* sol) const;
	
	int stop1, int stop2;
	double dist;
	
	swap_tabu_option* next;
};

// This represents all the actions that could be performed on a city (whether they be good, tabu, or bad).
class tabu_options
{
public:
	tabu_options();
	~tabu_options();
	
	swap_tabu_option* swap_options;
	reschedule_tabu_option* reschedule_options;
};

tabu_options* genereate_tabu_options(city* c);



#endif