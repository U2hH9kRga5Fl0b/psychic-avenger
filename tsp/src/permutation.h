#ifndef PERMUTATION_H_
#define PERMUTATION_H_

#include "solution.h"
#include "item.h"

#include <algorithm>

class permutation
{
public:
	permutation(solution* sol) : permutation{sol, 0, sol->get_city()->num_stops} {};
		
	permutation(solution* sol, int start_, int stop) :
		start{start_},
		len{stop-start_},
		perm{new int[len]},
		stps{new int[len]}
	{
		for (int i=0; i<len; i++)
		{
			perm[i] = i;
			stps[i] = sol->get_stop(i);
		}
	}
	
	permutation(const permutation& other) :
		start{other.start},
		len{other.len},
		perm{new int[len]},
		stps{new int[len]}
	{
		(*this) = other;
	}
	
	~permutation() { delete[] perm; delete[] stps; }
	
	bool increment()
	{
		return std::next_permutation(perm, perm+len);
	}
	
	std::ostream& print(std::ostream& outstream) const
	{
		for (int i=0; i<len; i++)
		{
			outstream << i << " ";
		}
		return outstream;
	}
	
	void assign(solution* sol) const
	{
		for (int i=0; i<len; i++)
		{
			sol->service(start + i, stps[perm[i]]);
		}
		sol->assign_serviced_indices();
	}
	
	
	permutation& operator=(const permutation& other)
	{
		if (len != other.len)
		{
			std::cout << "unable to assign to a permutation with different n" << std::endl;
			exit(-1);
		}
		
		start = other.start;
		
		for (int i=0; i<len; i++)
		{
			perm[i] = other.perm[i];
			stps[i] = other.stps[i];
		}
		
		return (*this);
	}
	
	bool operator<(const permutation& other) const
	{
		if (len != other.len)
		{
			return false;
		}
		for (int i=0;i<len;i++)
		{
			if (stps[perm[i]] != other.stps[perm[i]])
			{
				if (stps[perm[i]] < other.stps[perm[i]])
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		return false;
	}
	bool operator==(const permutation& other) const
	{
		if (len != other.len)
		{
			return false;
		}
		for (int i=0; i<len; i++)
		{
			if (stps[perm[i]] != stps[perm[i]])
			{
				return false;
			}
		}
		return true;
	}
	
private:
	int start;
	int len;
	int *perm;
	int *stps;
};


#endif
