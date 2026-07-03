#ifndef _CLIENT_UTILS_H_
#define _CLIENT_UTILS_H_

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


void
set_sock_preferences (struct addrinfo *sock_preferences);

void
check_number_of_args (int argc);

void *
get_in_addr (struct sockaddr *sa);

void
create_sock_and_connect (int *sock_fd,
			 struct addrinfo *poss_cnntns);

void
send_tcp_msg (int sock_fd,
	      int msg_len,
	      char **argv);

void
calc_distances_len (int *distances_len, // O
		    char *buffer);

void
find_whitespace_locs (int *whitespace_locs, // O
		      char* buffer);

void
parse_buf_for_distances (int *distances,
			 char *buffer,
			 int distances_len);

void
parse_buf_for_distances (int *distances, // O
			 char *buffer,
			 int distances_len);

void
recv_tcp (char *buffer, // O
	  int buffer_len,
	  int sock_fd);

void
parse_buf_for_delays (double *p_delays,   // O
		      double *t_delays,   // O
		      double *ttl_delays, // O
		      char *buf,
		      int buf_len,
		      int sock_fd,
		      int distances_len);

void
print_formatting_dashes (int n);

void
print_results (double *p_delays,
	       double *t_delays,
	       double *ttl_delays,
	       int *distances,
	       int distances_len);

#endif //_CLIENT_UTILS_H_
