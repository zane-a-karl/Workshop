#include "shortest-path-tree.h"

bool there_exist_unvisited_v (bool *visited,
			      int n) {
  for ( int i = 0; i < n; ++i ) {
    if ( !visited[i] ) {
      return true;
    }
  }
  return false;
}

void except_source_initialize_with_int_max (int *arr,
					    int n,
					    int src) {
  for ( int i = 0; i < n; ++i ) {
    arr[i] = (i != src ? INT_MAX : 0);
  }
}

int pick_unvisited_with_min_distance (bool *visited,
				      int *distances,
				      int n) {
  int min_dist = INT_MAX;
  int choice = INT_MAX;
  for ( int i = 0; i < n; ++i ) {
    if ( !visited[i] ) {
      if ( distances[i] < min_dist ) {
	min_dist = distances[i];
	choice = i;
      }
    }
  }
  return choice;
}

int *find_shortest_path_tree (int **A,
			      int v,
			      int src_v) {

  bool *visited = (bool *)calloc(v, sizeof *visited);
  int *distances = (int *)calloc(v, sizeof *distances);
  int current_v;
  int old, new;
  except_source_initialize_with_int_max(distances, v, src_v);

  while ( there_exist_unvisited_v(visited, v) ) {
    current_v = pick_unvisited_with_min_distance(visited, distances, v);
    visited[ current_v ] = true;
    for ( int i = 0; i < v; ++i ) {
      if ( A[current_v][i] != 0 ) {
	old = distances[i];
	new = distances[current_v] + A[current_v][i];
	distances[i] = (old > new ? new : old);
      }
    }
  }
  free(distances);

  return distances;
}

/*
int main ()
{
  //  NOTE:
  //  A function that takes a (<type> **) as a param,
  //    CAN take a (<type> *[<size>]),
  //    but NOT a (<type> [<size>][<size>])
  int v = 6;
  int a0[6] = {0,  7,  0,  9, 0, 0};
  int a1[6] = {7,  0,  2, 10, 0, 0};
  int a2[6] = {0,  2,  0, 15, 0, 0};
  int a3[6] = {9, 10, 15,  0, 5, 7};
  int a4[6] = {0,  0,  0,  5, 0, 4};
  int a5[6] = {0,  0,  0,  7, 4, 0};
  int a6[0] = {};
  //int *a[6] = {a6};//{a0, a1, a2, a3, a4, a5};
  int *A[6] = {a0, a1, a2, a3, a4, a5};

  int *d = (int *)calloc(v, sizeof *d);
  d = find_shortest_path_tree(A, v, 5);
  for (int i = 0; i < v; ++i) {
    printf( (i != v-1) ? "%d " : "%d\n", d[i] );
  }
  free(d);
  return 0;
}
*/
