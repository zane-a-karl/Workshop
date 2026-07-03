#include <stdio.h>
#include <limits.h> // INT_MAX
#include <stdbool.h>
#include <stdlib.h>


#ifndef _SHORTEST_PATH_TREE_H_
#define _SHORTEST_PATH_TREE_H_

bool there_exist_unvisited_v (bool *visited,
			      int n);

void except_source_initialize_with_int_max (int *arr,
					    int n,
					    int src);

int pick_unvisited_with_min_distance (bool *visited,
				      int *distances,
				      int n);

int *find_shortest_path_tree (int **A,
			      int v,
			      int src_v);

#endif // _SHORTEST_PATH_TREE_H_
