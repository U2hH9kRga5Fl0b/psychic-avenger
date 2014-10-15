
#ifndef TABU_LIST_H_
#define TABU_LIST_H_

#include "hash.h"

class tabu_list
{
public:
	tabu_list() { refresh_stats(); }
	virtual ~tabu_list() {}
	
	virtual bool is_tabu(const solution* sol) = 0;
	virtual void mark_tabu(const solution* sol)  = 0;
	
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

class bloom_filter : public tabu_list
{
public:
	bloom_filter(int nbits);
	~bloom_filter();
	
	bool is_tabu(const solution* sol);
	void mark_tabu(const solution* sol);
	
	friend std::ostream& operator<<(std::ostream& out, const bloom_filter& filter);
private:
	double get_saturation() const;
	bool get_bit_at_index(int n) const;
	void set_bit_at_index(int n);
	// wolverine style...
	void erase_memory();
	
	int nbits;
	int nbytes;
	uint8_t *mem;
};


class circular_hash_set : public tabu_list
{
public:
	circular_hash_set(int size);
	~circular_hash_set();
	
	bool is_tabu(const solution* sol);
	void mark_tabu(const solution* sol);
private:
	
	hash* first;
	hash* last;
	
	int size;
	std::set<hash> hashes;
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
		
		if (!(rand() % list.get_size()) && list.get_saturation() > .5)
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