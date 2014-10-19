/*
 * neighbor_options.h
 *
 *  Created on: Oct 16, 2014
 *      Author: thallock
 */

#ifndef NEIGHBOR_OPTIONS_H_
#define NEIGHBOR_OPTIONS_H_

#include "solution.h"
#include "tabu_list.h"

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
public:
	virtual std::string get_name() const = 0;

	int stop1, stop2;
	double dist;
};

// This represents the action of reversing all stops from stop1 to stop2
class two_opt : public neighbor_option
{
public:
	two_opt(int stop1, int stop2, city* c);
	~two_opt();

	double get_improvement(const solution* sol) const;
	bool in_bounds(const solution* sol) const;
	void apply(solution* sol) const;

	double get_cost_along_path(const solution* sol) const;
	bool intersects(const solution* sol) const;

	hash get_hash(const solution* sol) const;
protected:
	std::string get_name() const;
};

class three_opt : public neighbor_option
{
public:
	three_opt(int stop1, int stop2, int stop3);
	~three_opt();

	double get_improvement(const solution* sol) const;
	bool in_bounds(const solution* sol) const;
	void apply(solution* sol) const;
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

class swap_option : public neighbor_option
{
public:
	swap_option(int stop1, int stop2, city* c);
	~swap_option();

	double get_improvement(const solution* sol) const;
	bool in_bounds(const solution* sol) const;
	void apply(solution* sol) const;

	hash get_hash(const solution* sol) const;
protected:
	std::string get_name() const;
};

class nothing_option : public neighbor_option
{
public:
	nothing_option() : neighbor_option{0, 0, nullptr} {}
	~nothing_option() {}

	double get_improvement(const solution* sol) const { return 0.0; };
	bool in_bounds(const solution* sol) const { return true; };
	void apply(solution* sol) const {};

	hash get_hash(const solution* sol) const { return get_hash(sol); }
protected:
	std::string get_name() const { return "nothing"; };
};


#endif /* NEIGHBOR_OPTIONS_H_ */
