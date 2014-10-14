
#ifndef TABU_LIST_H_
#define TABU_LIST_H_

#include "hash.h"

class tabu_list
{
public:
	tabu_list() { refresh_stats(); }
	virtual ~tabu_list() {}
	
	virtual bool is_tabu(const solution* sol, bool save=true) = 0;
	
	void refresh_stats()
	{
		count = 0;
		total = 0;
	}
	
	double get_stop_percent()
	{
		return count / (double) total;
	}
protected:
	bool stopped(bool success)
	{
		total++;
		if (success)
		{
			count++;
		}
		return success;
	}
private:
	int count;
	int total;
};

class bloom_list : public tabu_list
{
public:
	bloom_list(int init_size) :
		list{init_size} {}
	~bloom_list() {}
	
	bool is_tabu(const solution* sol, bool save=true)
	{
		bool ret = list.already_visited(sol, save);
		
		if (!(rand() % 1000) && list.get_saturation() > .5)
		{
			list.set_size(2 * list.get_size());
		}
		
		return stopped(ret);
	}
private:
	bloom_filter list;
};

class ch_list : public tabu_list
{
public:
	ch_list(int init_size) :
		list{init_size} {}
	~ch_list() {}
	
	bool is_tabu(const solution* sol, bool save=true)
	{
		return stopped(list.already_visited(sol, save));
	}
private:
	circular_hash_set list;
};

class empty_list : public tabu_list
{
public:
	empty_list() {}
	~empty_list() {}
	
	bool is_tabu(const solution* sol, bool save=true)
	{
		return false;
	}
};




#if 0
		
		
	size_t max_memory;
#endif

	
#endif