#include "solution.h"




namespace
{
	class minima
	{
		minima(solution* sol) : n{sol->get_city()->num_cities}, perm{new int[n]}
		{
			for (int i=0; i<n; i++)
			{
				perm[i] = sol->get_stop(i);
			}
		}
		minima() { delete[] perm; }
		
		bool operator<(const minima& other) const
		{
			if (n != other.n)
			{
				return false;
			}
			for (int i=0;i<n;i++)
			{
				if (perm[i] != other.perm[i])
				{
					if (perm[i] < other.perm[i])
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
		bool operator==(const minima& other) const
		{
			if (n != other.n)
			{
				return false;
			}
			for (int i=0;i<n;i++)
			{
				if (perm[i] != other.perm[i])
				{
					return false;
				}
			}
			return true;
		}
	private:
		int n;
		int *perm;
	};
	
	
	class minima_collection
	{
	public:
		minima_collection() {}
		~minima_collection() {}
		
		void sample(solution* sol)
		{
			
		}
	private:
		int n;
		std::map<minima, std::vector<minima>> collection;
	};
	
	class anneal_statistics
	{
	public:
		anneal_statistics(){}
		~anneal_statistics(){}
	private:
		int num_sampled;
		int local_minima_found;
		double cost_of_best_local_minima;
	};
}



void sample_decision_space(city* c) {}
void complete_decision_space(city* c) {}

anneal_statistics* find_neighbors(solution* sol, int distance_to_anneal) {}


