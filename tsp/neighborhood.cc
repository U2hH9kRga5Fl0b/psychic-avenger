
#include "routines.h"

void find_neighbor_2pts(solution *s, solution *out, double locality)
{
	int n = s->c->num_cities;
	
	if (locality < 0)
	{
		locality = 0;
	}
	if (locality > 1)
	{
		locality = 1;
	}
	
	int size = 1 + (int) (locality * (rand() % (n - 1)));
	
	int idx1 = rand() % (n - size);
	int idx2 = idx1 + size;
	
	*out = *s;
	
	int tmp = out->path[idx2];
	out->path[idx2] = out->path[idx1];
	out->path[idx1] = tmp;
}


void find_neighbor_2op(solution *s, solution *out, double locality)
{
	int n = s->c->num_cities;
	
	if (locality < 0)
	{
		locality = 0;
	}
	if (locality > 1)
	{
		locality = 1;
	}
	
	int size = 1 + (int) (locality * (rand() % (n - 1)));
	
	int idx1 = rand() % (n - size);
	int idx2 = idx1 + size;
	
	*out = *s;
	
	for (int l=idx1, u=idx2; l < u; l++, u--)
	{
		int tmp = out->path[u];
		out->path[u] = out->path[l];
		out->path[l] = tmp;
	}
}


bool find_neighbor_2op_inter(solution *s, solution *out, int num_attempts)
{
	int n = s->c->num_cities;
	
	int idx1, idx2;
	if (!find_intersecting_paths(s, idx1, idx2, num_attempts))
	{
		return false;
	}
	
	*out = *s;
	
	for (int l=idx1, u=idx2; l < u; l++, u--)
	{
		int tmp = out->path[u];
		out->path[u] = out->path[l];
		out->path[l] = tmp;
	}
}


void find_neighbor_resched(solution *s, solution *tmp, double annealing_factor)
{
	*tmp = *s;
	
	int n = s->c->num_cities;
	city* c = s->c;
	
	int idx = 0;
	double mc = c->get_cost(s->path[idx], s->path[idx+1]);
	for (int i=0;i<20;i++)
	{
		int idx2;
		do
		{
			idx2 = rand() % (n-1);
		} while (s->path[idx2+1] < 0);
		
		double c2 = c->get_cost(s->path[idx2], s->path[idx2+1]);
		if (c2 > mc)
		{
			idx = idx2;
			mc = c2;
		}
	}
	
	std::vector<int> vec;
	for (int i=0;i<n;i++)
	{
		if (i == idx)
		{
			continue;
		}
		vec.push_back(i);
	}
	
	std::sort(vec.begin(), vec.end(), [c, idx](int idx1, int idx2) { return c->get_cost(idx, idx1) < c->get_cost(idx,idx2); });
	
	int closest = vec.at(0);
	
	int idx2 = -1;
	
	for (int i=0;i<n;i++)
	{
		if (s->path[i] == closest)
		{
			idx2 = i;
			break;
		}
	}
	
	if (idx2 < 0)
	{
		return;
	}
	
	int stop = tmp->remove_at(idx);
	
	double c1 = DBL_MAX;
	
	if (idx2 < n-1)
	{
		tmp->insert_at(stop, idx2+1);
		c1 = tmp->get_cost();
		tmp->remove_at(idx2+1);
	}
	
	tmp->insert_at(stop, idx2);
	if (tmp->get_cost() < c1)
	{
		return;
	}
	
	tmp->remove_at(idx2);
	tmp->insert_at(stop, idx2+1);
}

