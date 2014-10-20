/*
 * path_relinking.h
 *
 *  Created on: Oct 17, 2014
 *      Author: thallock
 */

#ifndef PATH_RELINKING_H_
#define PATH_RELINKING_H_

#include "solution.h"
#include "item.h"

class operation_path
{
public:
	operation_path(const neighbor_option *op_, operation_path *n_) :
		op {op_},
		next{n_},
		dist{0} {}

	~operation_path()
	{
		delete next;
	}

	void print()
	{




	}

	const neighbor_option* op;
	operation_path* next;
	int dist;
};


int distance_between_sequence_no_order(const solution* sol1, const solution *sol2);

int move_from_to(solution* source, solution *dest, std::function<void(const solution*s)> f);



//operation_path* levenshtein(solution* s1, solution* s2);
//void link_paths(solution* source, solution* dest, itemizer& items, std::function<void(void)> callback = [](){});
//void fastest_descent(solution* source, solution* dest, itemizer& items, std::function<void(void)> callback = [](){});


#endif /* PATH_RELINKING_H_ */
