#include "client-utils.h"

#define CLNTPORT "4141" // port # clnt proc runs on
#define AWSPORT "4242"  // port # AWS proc runs on


int main (int argc,
	  char *argv[]) {

  int clnt_sock_fd;
  int getaddrinfo_failed;
  struct addrinfo sock_preferences;
  struct addrinfo *possible_cnntns;

  check_number_of_args(argc);
  set_sock_preferences(&sock_preferences);
  getaddrinfo_failed = getaddrinfo(NULL,
				   AWSPORT,
				   &sock_preferences,
				   &possible_cnntns);
  if ( getaddrinfo_failed ) {
    fprintf(stderr,
	    "getaddrinfo: %s\n",
	    gai_strerror(getaddrinfo_failed));
    return 1;
  }

  create_sock_and_connect(&clnt_sock_fd,
			  possible_cnntns);
  freeaddrinfo(possible_cnntns);
  printf("The client is up and running.\n");

  send_tcp_msg(clnt_sock_fd, 256, argv);
  printf("The client has sent query to AWS using TCP");
  printf("over port %s:\n", AWSPORT);
  printf("start vertex %s; ", argv[2]);
  printf("map %s; ", argv[1]);
  printf("file size %s.\n", argv[3]);

  int clnt_buf_len = 2048;
  char *clnt_buf = (char *)calloc(clnt_buf_len,
				  sizeof *clnt_buf);
  int distances_len;
  recv_tcp(clnt_buf,
	   clnt_buf_len,
	   clnt_sock_fd); // distances
  calc_distances_len(&distances_len, clnt_buf);
  int *distances = (int *)calloc(distances_len,
				 sizeof *distances);
  parse_buf_for_distances(distances,
			  clnt_buf,
			  distances_len);

  double *ttl_delays = (double *)calloc(distances_len,
					sizeof(*ttl_delays));
  double *prop_delays = (double *)calloc(distances_len,
					 sizeof(*prop_delays));
  double *trans_delays = (double *)calloc(distances_len,
					  sizeof(*trans_delays));
  recv_tcp(clnt_buf,
	   clnt_buf_len,
	   clnt_sock_fd); // delays
  parse_buf_for_delays(prop_delays,
		       trans_delays,
		       ttl_delays,
		       clnt_buf,
		       clnt_buf_len,
		       clnt_sock_fd,
		       distances_len);

  printf("The client has received results from AWS:\n");
  print_formatting_dashes(72);
  printf("Destination     ");
  printf("MinLength       ");
  printf("TransDelay      ");
  printf("PropDelay       ");
  printf("TtlDelay\n");
  print_formatting_dashes(72);
  print_results(prop_delays,
		trans_delays,
		ttl_delays,
		distances,
		distances_len);
  print_formatting_dashes(72);

  close(clnt_sock_fd);

  free(clnt_buf);
  free(distances);
  free(prop_delays);
  free(trans_delays);
  free(ttl_delays);

  return 0;
}
