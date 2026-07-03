#ifndef _SERVERA_UTILS_H_
#define _SERVERA_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h> // isalpha()

void
check_number_of_args (int argc);

void
set_sock_preferences (struct addrinfo *sock_preferences);

void
print_info6_about (struct sockaddr_storage *addr);

void *
get_in_addr (struct sockaddr *sa);

void
create_sock_and_bind (int *sock_fd,
		      struct addrinfo *poss_cnntns);

void
open_map_file(FILE **fin,
	      char *fname);

char *
able_to_read_line (char *buf,
		   FILE **fin,
		   int buf_len);

void
calculate_num_maps (int *num_maps,
		    char *buf,
		    FILE **fin,
		    int buf_len);

void
rewind_map_file (FILE **fin);

void
print_formatting_dashes (int n);

void
check_whether_current_map_is_finished (char *seen_vertices,
				       int *map_cursor,
				       char *buf,
				       FILE **fin,
				       int buf_len,
				       int map_i,
				       int *num_edges,
				       int *num_vertices);

void
parse_map_file_for_graph_info (int *prop_speed,
			       int *num_edges,
			       int *num_vertices,
			       int *map_i,
			       int *trans_speed,
			       char *buf,
			       FILE **fin,
			       int buf_len);

void
prep_buf_and_recv_udp (char *buf,
		       socklen_t *addr_len,
		       struct sockaddr_storage *addr,
		       int buf_len,
		       int sock_fd);

void
construct_chosen_graph (int **adj_mat,
			char *buf,
			FILE **fin,
			int buf_len,
			int map_id);

void
close_map_file(FILE **fin);

void
prep_buf_and_send_udp_speeds (char *buf,
			      int buf_len,
			      int map_i,
			      int sock_fd,
			      int *prop_speed,
			      int *trans_speed,
			      socklen_t addr_len,
			      struct sockaddr_storage *addr);

void
prep_buf_and_send_udp_dists (char *buf,
			     int buf_len,
			     int map_i,
			     int sock_fd,
			     int *distances,
			     int *num_vertices,
			     socklen_t addr_len,
			     struct sockaddr_storage *addr);

#endif //_SERVERA_UTILS_H_
