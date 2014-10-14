
#ifndef COUNT_MINIMA_H_
#define COUNT_MINIMA_H_

#include "permutation.h"
#include "viewer.h"



class minima_collection
{
public:
	minima_collection() {}
	~minima_collection() {}
	
	void sample(solution* sol)
	{
		const permutation start{sol};
		
		itemizer itms{sol->get_city()};
		itms.greedy_search(sol);
		const permutation local_opt{sol};
		
		auto it = collection.find(local_opt);
		if (it == collection.end())
		{
			collection.insert(
				std::pair<permutation, std::set<permutation> >
					{local_opt, std::set<permutation>{}});
			it = collection.find(local_opt);
		}
		
		it->second.insert(start);
		c = sol->get_city();
	}
	
	void view()
	{
		std::cout << "Found " << collection.size() << " local minima." << std::endl;
		
		double sum = 0.0;
		
		std::for_each (collection.begin(), collection.end(),
			       [this, &sum](const std::pair<permutation, std::set<permutation>>& pair)
		{
			
			solution* opt = new solution{this->c};
			pair.first.assign(opt);
			viewer local_opt{opt, "local minimum"};
			local_opt.update();
			
			std::cout << "For this local minimum there were " << pair.second.size() << " initial configurations." << std::endl;
			sum += pair.second.size();
			
			std::for_each (pair.second.begin(), pair.second.end(),
				       [this](const permutation& ipair)
			{
				solution* start = new solution{this->c};
				ipair.assign(start);
				viewer ini{start, "one starting point optimum"};
				ini.update();
				
				ini.pause(1000);
			});
		});
		
		std::cout << "That is an average of " << (sum / collection.size()) << " initial configurations per local minima." << std::endl;
	}
private:
	std::map<permutation, std::set<permutation>> collection;
	city* c;
};

class anneal_statistics
{
public:
	anneal_statistics(){}
	~anneal_statistics(){}

	int num_sampled;
	int local_minima_found;
	double cost_of_best_local_minima;
};


void complete_decision_space(city* c);


#endif