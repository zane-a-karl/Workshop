#include "ServerB-utils.h"

#define SRVRBPORT "6666" // port # that ServerB proc runs on


int main (int argc,
	  char *argv[]) {

  int srvrB_sock_fd;
  int getaddrinfo_failed;
  struct addrinfo sock_preferences;
  struct addrinfo *possible_cnntns;
  struct sockaddr_storage aws_addr;
  socklen_t aws_addr_len = sizeof(struct sockaddr);

  check_number_of_args(argc);
  set_sock_preferences(&sock_preferences);
  getaddrinfo_failed = getaddrinfo(NULL,
				   SRVRBPORT,
				   &sock_preferences,
				   &possible_cnntns);
  if ( getaddrinfo_failed ) {
    fprintf(stderr,
	    "getaddrinfo: %s\n",
	    gai_strerror(getaddrinfo_failed));
    return 1;
  }

  create_sock_and_bind(&srvrB_sock_fd, possible_cnntns);
  freeaddrinfo(possible_cnntns);
  printf("The ServerB is up and running ");
  printf("using UDP on port %s.\n", SRVRBPORT);

  int prop_speed;
  int trans_speed;
  int distances_len;
  int file_size;

  int buf_len = 2048;
  char *buf = (char *)calloc(buf_len, sizeof *buf);
  recv_udp_speeds(&distances_len,
		  &file_size,
		  &prop_speed,
		  &trans_speed,
		  &aws_addr_len,
		  &aws_addr,
		  buf,
		  buf_len,
		  srvrB_sock_fd);

  printf("Server B has received data for calculation:\n");
  printf("* Propagation speed: %dkm/s\n", prop_speed);
  printf("* Transmission speed: %dBytes/s\n", trans_speed);

  int *distances = (int *)calloc(distances_len,
				 sizeof *distances);
  recv_udp_distances(buf,
		     distances,
		     &aws_addr_len,
		     &aws_addr,
		     buf_len,
		     distances_len,
		     srvrB_sock_fd);

  for ( int i = 0; i < distances_len; ++i ) {
    printf("* Path length for destination %d: %d\n",
	   i,
	   distances[i]);
  }

  double *ttl_delays = (double *)calloc(distances_len,
					sizeof *ttl_delays);
  double *prop_delays = (double *)calloc(distances_len,
					 sizeof *prop_delays);
  double *trans_delays = (double *)calloc(distances_len,
					  sizeof *trans_delays);
  calc_delays(prop_delays,
	      trans_delays,
	      ttl_delays,
	      distances,
	      distances_len,
	      prop_speed,
	      trans_speed,
	      file_size);
  printf("\nServer B has finished the calculation\n");
  print_results(ttl_delays, distances_len);

  prep_buf_to_send_delays(buf,
			  prop_delays,
			  trans_delays,
			  ttl_delays,
			  buf_len,
			  distances_len);
  send_udp_delays(buf,
		  &aws_addr,
		  buf_len,
		  srvrB_sock_fd,
		  aws_addr_len);
  printf("Server B finished sending output to AWS\n");
  for (int i = 0; i < distances_len; ++i) {
    printf( "%-16d %-14.3f %-13.3f %-16.3f\n",
	    i,
	    prop_delays[i],
	    trans_delays[i],
	    ttl_delays[i]);
  }

  close(srvrB_sock_fd);

  free(buf);
  free(distances);
  free(prop_delays);
  free(trans_delays);
  free(ttl_delays);

  return 0;
}
