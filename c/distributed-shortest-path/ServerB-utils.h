#ifndef _SERVERB_UTILS_H_
#define _SERVERB_UTILS_H_

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

void
set_sock_preferences (struct addrinfo *sock_preferences);

void
print_info6_about (struct sockaddr_storage *addr);

void
check_number_of_args (int argc);

void *
get_in_addr (struct sockaddr *sa);

void
create_sock_and_bind (int *srvrB_sock_fd,
		      struct addrinfo *poss_cnntns);

void
prep_buf_and_recv_udp (char *buf,                     // O
		       socklen_t *addr_len,           // O
		       struct sockaddr_storage *addr, // O
		       int buf_len,
		       int sock_fd);

void
recv_udp_speeds (int *distances_len,            // O
		 int *file_size,                // O
		 int *prop_speed,               // O
		 int *trans_speed,              // O
		 socklen_t *addr_len,           // O
		 struct sockaddr_storage *addr, // O
		 char *buf,
		 int buf_len,
		 int sock_fd);

void
find_whitespace_locs (int *whitespace_locs,
		      char *buf);

void
recv_udp_distances (char *buf,                     // O
		    int *distances,		   // O
		    socklen_t *addr_len,           // O
		    struct sockaddr_storage *addr, // O
		    int buf_len,
		    int distances_len,
		    int sock_fd);

void
calc_delays (double *prop_delays,  // O
	     double *trans_delays, // O
	     double *ttl_delays, // O
	     int *distances,
	     int distances_len,
	     int prop_speed,
	     int trans_speed,
	     int file_size);

void
print_formatting_dashes (int n);

void
print_results (double *ttl_delays,
	       int distances_len);

void
prep_buf_to_send_delays (char *buf, // O
			 double *prop_delays,
			 double *trans_delays,
			 double *ttl_delays,
			 int buf_len,
			 int distances_len);

void
send_udp_delays (char *buf,                     // O
		 struct sockaddr_storage *addr, // O
		 int buf_len,
		 int sock_fd,
		 socklen_t addr_len);

#endif //_SERVERB_UTILS_H_
