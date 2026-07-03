#include "ServerA-utils.h"
#include "shortest-path-tree.h"

#define SRVRAPORT "6565" // port # that ServerA proc runs on


int main (int argc,
	  char **argv) {

  int srvrA_sock_fd;
  int getaddrinfo_failed;
  struct addrinfo sock_preferences;
  struct addrinfo *possible_cnntns;
  struct sockaddr_storage aws_addr;
  socklen_t aws_addr_len = sizeof(struct sockaddr);

  check_number_of_args(argc);
  set_sock_preferences(&sock_preferences);
  getaddrinfo_failed = getaddrinfo(NULL,
				   SRVRAPORT,
				   &sock_preferences,
				   &possible_cnntns);
  if ( getaddrinfo_failed ) {
    fprintf(stderr,
	    "getaddrinfo: %s\n",
	    gai_strerror(getaddrinfo_failed));
    return 1;
  }

  create_sock_and_bind(&srvrA_sock_fd, possible_cnntns);
  freeaddrinfo(possible_cnntns);
  printf("Server A is up and running ");
  printf("using UDP on port %s.\n", SRVRAPORT);

  int buf_len = 2048;
  char *buf = (char *)calloc(buf_len, sizeof *buf);
  int num_maps = 0;
  FILE *fin;

  open_map_file(&fin, "map.txt");
  calculate_num_maps(&num_maps, buf, &fin, buf_len);
  rewind_map_file(&fin);
  printf("Server A has constructed a list of ");
  printf("%d maps:\n", num_maps);

  int map_i = -1;
  int *num_vertices = (int *)calloc(num_maps,
				    sizeof *num_vertices);
  int *num_edges =  (int *)calloc(num_maps,
				  sizeof *num_edges);
  int *prop_speed = (int *)calloc(num_maps,
				  sizeof *prop_speed);
  int *trans_speed = (int *)calloc(num_maps,
				   sizeof *trans_speed);
  print_formatting_dashes(34);
  printf("MapID     NumVertices     NumEdges\n");
  print_formatting_dashes(34);

  parse_map_file_for_graph_info(prop_speed,
				num_edges,
				num_vertices,
				&map_i,
				trans_speed,
				buf,
				&fin,
				buf_len);

  print_formatting_dashes(34);

  prep_buf_and_recv_udp(buf,
			&aws_addr_len,
			&aws_addr,
			buf_len,
			srvrA_sock_fd);
  char map_id;
  int start_node;
  sscanf(buf, "%c %d %*d", &map_id, &start_node);
  printf("The Server A has received input");
  printf("for finding shortest paths:");
  printf("starting vertex %d", start_node);
  printf("of map %c.\n", map_id);

  /*START: constructing the chosen graph*/
  rewind_map_file(&fin);
  map_i = map_id - 65; // convert map ID to int.
  int **adj_mat = (int **)calloc(num_vertices[map_i],
				 sizeof *adj_mat);
  for (int i = 0; i < num_vertices[map_i]; ++i) {
    adj_mat[i] = (int *)calloc(num_vertices[map_i],
			       sizeof *adj_mat[i]);
  }

  construct_chosen_graph(adj_mat,
			 buf,
			 &fin,
			 buf_len,
			 map_id);

  close_map_file(&fin);
  /*END: constructing the chosen graph*/

  /* for (int i = 0; i < num_vertices[map_i]; ++i) { */
  /*   for (int j = 0; j < num_vertices[map_i]; ++j) { */
  /*     printf( (j==num_vertices[map_i]-1) ? "%-3d\n" : "%-3d ", */
  /* 	      adj_mat[i][j]); */
  /*   } */
  /* } */
  /* printf("\n"); */


  /*START: Perform find shortest path tree alg*/
  int *distances = calloc(num_vertices[map_i],
			  sizeof *distances);
  distances = find_shortest_path_tree(adj_mat,
				      num_vertices[map_i],
				      start_node);
  printf("The Server A has identified ");
  printf("the following shortest paths:\n");
  print_formatting_dashes(25);
  printf("Destination     MinLength\n");
  print_formatting_dashes(25);
  for (int i = 0; i < num_vertices[map_i]; ++i) {
    printf( "%-15d %d\n", i, distances[i] );
  }
  print_formatting_dashes(25);
  /*END: Perform djikstra's alg*/

  /*START: sending udp packets*/
  prep_buf_and_send_udp_speeds(buf,
			       buf_len,
			       map_i,
			       srvrA_sock_fd,
			       prop_speed,
			       trans_speed,
			       aws_addr_len,
			       &aws_addr);
  prep_buf_and_send_udp_dists(buf,
			      buf_len,
			      map_i,
			      srvrA_sock_fd,
			      distances,
			      num_vertices,
			      aws_addr_len,
			      &aws_addr);
  printf("The Server A has sent shortest paths to AWS.\n");
  /*END: sending udp packets*/

  close(srvrA_sock_fd);

  for (int i = 0; i < num_vertices[map_i]; ++i) {
    free(adj_mat[i]);
  }
  free(adj_mat);
  free(buf);
  free(distances);
  free(num_edges);
  free(num_vertices);
  free(prop_speed);
  free(trans_speed);

  return 0;
}
