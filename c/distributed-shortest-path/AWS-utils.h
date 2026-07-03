#ifndef _AWS_UTILS_H_
#define _AWS_UTILS_H_

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
#include <sys/wait.h>
#include <signal.h>


typedef enum {
  CTOAWS,
  AWSTOSRVRA,
  AWSTOSRVRB,
  AWSTOC,
  ENDTRANS
} AWSSTATE;

void
check_number_of_args (int argc);

void
check_if_getaddrinfo_failed (int getaddrinfo_result,
			     char *s);

void
set_tcp_sock_preferences (struct addrinfo *sock_preferences);

void
set_udp_sock_preferences (struct addrinfo *sock_preferences);

void
get_available_socket (struct addrinfo **cnntn, // O
		      struct addrinfo *poss_cnntns);

void
create_tcp_sock_and_bind (int *sock_fd,
			  struct addrinfo *poss_cnntns);

void
create_udp_sock_and_bind (int *sock_fd,
			  struct addrinfo *poss_cnntns);

void
begin_tcp_listening (int *sock_fd,
		     int max_queue_len);

void
reap_zombie_processes (struct sigaction *sa);

void
sigchld_handler (int s);

void *
get_in_addr (struct sockaddr *sa);

int
get_port (struct sockaddr *addr);

int
recv_tcp_clnt_query (char **buf, // O
		     int buf_len,
		     int sock_fd);

void
parse_clnt_query_and_print (char *map_id,    // O
			    int *file_sz,    // O
			    int *start_node, // O
			    char *buf,
			    int port);

int
prep_buf_and_send_map_info (char **buf, // O
			    int buf_len,
			    int map_id,
			    int port,
			    int sock_fd,
			    int start_node,
			    struct addrinfo *sva_cnntn);

int
prep_buf_and_recv_speeds (char **buf, // O
			  int buf_len,
			  int sock_fd,
			  struct addrinfo *sva_cnntn);

void
parse_buf_for_speeds (int *prop_speed,  // O
		      int *trans_speed, // O
		      char *buf);

int
prep_buf_and_recv_paths (char **buf,
			 int buf_len,
			 int sock_fd,
			 struct addrinfo *sva_cnntn);

void
parse_buf_for_distances (int *distances_len, // O
			 int **distances,    // O
			 char *buf);

void
print_formatting_dashes (int n);

int
prep_buf_and_send_speeds (char **buf, // O
			  int buf_len,
			  int file_sz,
			  int num_distances,
			  int prop_speed,
			  int trans_speed,
			  int sock_fd,
			  struct addrinfo *svb_cnntn);

int
prep_buf_and_send_paths (char **buf, // O
			 int buf_len,
			 int num_distances,
			 int sock_fd,
			 int *distances,
			 struct addrinfo *svb_cnntn);

int
prep_buf_and_recv_delays (char **buf, // O
			  int buf_len,
			  int sock_fd,
			  struct addrinfo *svb_cnntn);

void
parse_buf_for_delays (double **p_delays,   // O
		      double **t_delays,   // O
		      double **ttl_delays, // O
		      char *buf,
		      int buf_len,
		      int num_distances);

void
print_delays_results (int num_distances,
		      double *prop_delays,
		      double *trans_delays,
		      double *ttl_delays);

int
prep_buf_and_send_dists (char **buf,
			 int buf_len,
			 int num_distances,
			 int sock_fd,
			 int *distances);

int
prep_buf_and_send_delays (char **buf, // O
			  int buf_len,
			  int num_distances,
			  int sock_fd,
			  double *prop_delays,
			  double *trans_delays,
			  double *ttl_delays);

#endif // _AWS_UTILS_H_
