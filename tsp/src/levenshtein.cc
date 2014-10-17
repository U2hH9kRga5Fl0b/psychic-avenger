/*
 * levenshtein.cc
 *
 *  Created on: Oct 17, 2014
 *      Author: thallock
 */

#include "path_relinking.h"



class edit_search
{
public:
	edit_search(int i_, int j_, edit_search* n=nullptr) :
		prev{n},
		dist{0},
		i{i_}, j{j_} {}
	~edit_search() {}

	void print()
	{
		std::cout << i << ", " << j << "- " << dist << std::endl;
		if (prev != nullptr)
		{
			prev->print();
		}
	}

	edit_search* prev;
	int dist;
	int i,j;
};

void levenshtein(solution* s1, solution *s2,
		edit_search ***dyn,
		int r, int c)
{
	bool equal = s1->get_stop(r)==s2->get_stop(c);

	if (r==0 && c==0)
	{
		dyn[r][c]->dist =  equal ? 0 : 1;
		return;
	}
	if (r == 0)
	{
		dyn[r][c]->dist = dyn[r][c-1]->dist + 1;
		dyn[r][c]->prev = dyn[r][c-1];
		return;
	}
	if (c == 0)
	{
		dyn[r][c]->dist = dyn[r-1][c]->dist + 1;
		dyn[r][c]->prev = dyn[r-1][c];
		return;
	}

	int a1 = dyn[r-1][c]->dist + 1;
	int a2 = dyn[r][c-1]->dist + 1;
	int a3 = dyn[r-1][c-1]->dist + (equal?0:1);

	dyn[r][c]->dist = std::min(a1, std::min(a2, a3));

	if (dyn[r][c]->dist == a1)
	{
		dyn[r][c]->prev = dyn[r-1][c];
	}
	else if (dyn[r][c]->dist == a2)
	{
		dyn[r][c]->prev = dyn[r][c-1];
	}
	else // dyn[r][c]->dist == a3
	{
		dyn[r][c]->prev = dyn[r-1][c-1];
	}
}






edit_search* reverse(edit_search* start)
{
	edit_search* next_one = nullptr;

	while (start != nullptr)
	{
		next_one = new edit_search{start->i, start->j, next_one};
		next_one->dist = start->dist;
		start = start->prev;
	}

	return next_one;
}


operation_path* get_path(edit_search* start, solution* s1, solution* s2)
{
	operation_path* sentinal = new operation_path{nullptr, nullptr};
	operation_path* last  = sentinal;

	int s1idx = 0;
	int s2idx = 0;

	while (start != nullptr)
	{
		// ensure that s1 to s2 match

		int i, j;
		i=0; j=-1;// need to figure out what goes here...

		last->next = new operation_path{new resched_option{i, j, s1->get_city()}, nullptr};
		last = last->next;
		start = start->prev; // aka next


		s1idx = start->i;
		s2idx = start->j;
	}

	operation_path* begin = sentinal->next;
	sentinal->next = nullptr;
	delete sentinal;

	return begin;
}








operation_path* levenshtein(solution* s1, solution* s2)
{
	int n = s1->get_city()->num_stops;

	std::cout << "Getting the distances between " << (*s1) << "\nand\n" << (*s2) << std::endl;

	edit_search ***dyn = new edit_search**[n];
	for (int i=0; i<n; i++)
	{
		dyn[i] = new edit_search*[n];
		for (int j=0; j<n; j++)
		{
			dyn[i][j] = new edit_search{i, j};
		}
	}

	for (int i=0; i<n; i++)
	{
		levenshtein(s1, s2, dyn, i, i);
		for (int j=i+1; j<n; j++)
		{
			levenshtein(s1, s2, dyn, j, i);
			levenshtein(s1, s2, dyn, i, j);



			std::cout << "==========================================" << std::endl;
			std::cout << i << ", " << j << ":" << std::endl;

			std::cout << "      ";
			for (int y=0; y<n; y++)
			{
				std::cout << std::setw(5) << s2->get_stop(y) << " ";
			}
			std::cout << std::endl;
			for (int x=0;x<n;x++)
			{
				std::cout << std::setw(5) << s1->get_stop(x) << " ";
				for (int y=0; y<n; y++)
				{
					std::cout << std::setw(5) << dyn[x][y]->dist << " ";
				}
				std::cout << std::endl;
			}
		}
	}

	std::cout << "distance is " << dyn[n-1][n-1]->dist << std::endl;

	edit_search* path = reverse(dyn[n-1][n-1]);
	path->print();

	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
		{
			delete dyn[i][j];
		}
		delete[] dyn[i];
	}
	delete[] dyn;

	return nullptr;
}


