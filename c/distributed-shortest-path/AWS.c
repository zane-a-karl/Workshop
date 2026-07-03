#include "AWS-utils.h"

#define AWSTCPPORT "4242" // port # aws tcp proc runs on.
#define AWSUDPPORT "4343" // port # aws udp proc runs on.
#define SRVRAPORT "6565"  // port # that ServerA proc runs on
#define SRVRBPORT "6666"  // port # that ServerB proc runs on

#define BACKLOG 10 // max len(pending cnntns queue) for listen


int
main (int argc,
      char **argv) {

  int aws_tcp_fd;
  int aws_udp_fd;
  int getaddrinfo_result;
  struct addrinfo tcp_sock_prefs, udp_sock_prefs;
  struct addrinfo *tcp_poss_cnntns, *udp_poss_cnntns;

  check_number_of_args(argc);

  int buf_len = 2048;
  char *buf = (char *)calloc(buf_len, sizeof *buf);
  int recv_status, send_status;

  int clnt_fd;
  struct sockaddr_storage clnt_addr;
  socklen_t clnt_addr_sz;
  int clnt_port;

  struct addrinfo sva_sock_prefs;
  struct addrinfo *sva_poss_cnntns;
  struct addrinfo *sva_cnntn;
  int sva_port;
  set_udp_sock_preferences(&sva_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   SRVRAPORT,
				   &sva_sock_prefs,
				   &sva_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result, "sva");
  get_available_socket(&sva_cnntn, sva_poss_cnntns);

  struct addrinfo svb_sock_prefs;
  struct addrinfo *svb_poss_cnntns;
  struct addrinfo *svb_cnntn;
  int svb_port;
  set_udp_sock_preferences(&svb_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   SRVRBPORT,
				   &svb_sock_prefs,
				   &svb_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result, "svb");
  get_available_socket(&svb_cnntn, svb_poss_cnntns);

  /* char map_id; */
  /* int start_node; */
  /* int file_sz; */

  /*START: get TCP addrinfo*/
  set_tcp_sock_preferences(&tcp_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   AWSTCPPORT,
				   &tcp_sock_prefs,
				   &tcp_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result, "awstcp");
  /*END: get TCP addrinfo*/

  /*START: get UDP addrinfo*/
  set_udp_sock_preferences(&udp_sock_prefs);
  getaddrinfo_result = getaddrinfo(NULL,
				   AWSUDPPORT,
				   &udp_sock_prefs,
				   &udp_poss_cnntns);
  check_if_getaddrinfo_failed(getaddrinfo_result, "awsudp");
  /*END: get UDP addrinfo*/

  /*START:create TCP socket and bind to AWSTCPPORT*/
  create_tcp_sock_and_bind(&aws_tcp_fd, tcp_poss_cnntns);
  freeaddrinfo(tcp_poss_cnntns);
  /*END:create TCP socket and bind to AWSTCPPORT*/

  /*START:create UDP socket*/
  create_udp_sock_and_bind(&aws_udp_fd, udp_poss_cnntns);
  freeaddrinfo(udp_poss_cnntns);
  /*END:create UDP socket*/

  begin_tcp_listening(&aws_tcp_fd, BACKLOG);

  struct sigaction sa;
  reap_zombie_processes(&sa);

  printf("The AWS is up and running");
  printf("...waiting for connections...\n");
  int state = CTOAWS;

  while (1) { // main accept() loop

    memset(buf, 0, buf_len * sizeof(*buf));
    switch (state) {

      char map_id;
      int start_node;
      int file_sz;

    case CTOAWS:
      clnt_fd = accept(aws_tcp_fd,
		       (struct sockaddr *)&clnt_addr, // O
		       &clnt_addr_sz);
      clnt_port = get_port( (struct sockaddr *)&clnt_addr );

      // Child process creation is wrecking the svA cmmntn.
      // Only create child process if a cnntn is accepted.
      if ( /*!fork() &&*/ clnt_fd != -1 ) {
	recv_status = recv_tcp_clnt_query(&buf,
					  buf_len,
					  clnt_fd);
	parse_clnt_query_and_print(&map_id,
				   &file_sz,
				   &start_node,
				   buf,
				   clnt_port);
      }
      state = (recv_status != -1 ? AWSTOSRVRA : state);
      break;

    case AWSTOSRVRA:
      sva_port = get_port(sva_cnntn->ai_addr);
      send_status = prep_buf_and_send_map_info (&buf,
						buf_len,
						map_id,
						sva_port,
						aws_udp_fd,
						start_node,
						sva_cnntn);
      int prop_speed;
      int trans_speed;
      recv_status = prep_buf_and_recv_speeds(&buf,
					     buf_len,
					     aws_udp_fd,
					     sva_cnntn);
      parse_buf_for_speeds(&prop_speed,
			   &trans_speed,
			   buf);
      recv_status = prep_buf_and_recv_paths(&buf,
					    buf_len,
					    aws_udp_fd,
					    sva_cnntn);
      printf("The AWS has received shortest ");
      printf("path from server A:\n");
      print_formatting_dashes(25);
      printf("Destination     MinLength\n");
      print_formatting_dashes(25);
      printf("%s\n", buf);
      print_formatting_dashes(25);

      int num_distances;
      int *distances;
      parse_buf_for_distances(&num_distances,
			      &distances,
			      buf);

      state = (recv_status != -1 ? AWSTOSRVRB : state);
      break;

    case AWSTOSRVRB:
      svb_port = get_port(svb_cnntn->ai_addr);
      send_status = prep_buf_and_send_speeds(&buf,
					     buf_len,
					     file_sz,
					     num_distances,
					     prop_speed,
					     trans_speed,
					     aws_udp_fd,
					     svb_cnntn);
      send_status = prep_buf_and_send_paths(&buf,
					    buf_len,
					    num_distances,
					    aws_udp_fd,
					    distances,
					    svb_cnntn);
      printf("The AWS has sent path length, propagation ");
      printf("speed and transmission speed to server B ");
      printf("using UDP over port %d.\n", svb_port);
      double *prop_delays;
      double *trans_delays;
      double *ttl_delays;
      recv_status = prep_buf_and_recv_delays(&buf,
					     buf_len,
					     aws_udp_fd,
					     svb_cnntn);
      parse_buf_for_delays(&prop_delays,
			   &trans_delays,
			   &ttl_delays,
			   buf,
			   buf_len,
			   num_distances);
      printf("The AWS has received delays from server B:\n");
      print_delays_results(num_distances,
			   prop_delays,
			   trans_delays,
			   ttl_delays);
      state = (recv_status != -1 ? AWSTOC : state);
      break;

    case AWSTOC:
      // Child process creation is wrecking the svA cmmntn.
      // Only create child process if a cnntn is accepted.
      if ( /*!fork() &&*/ clnt_fd != -1 ) {
	send_status = prep_buf_and_send_dists(&buf,
					      buf_len,
					      num_distances,
					      clnt_fd,
					      distances);
      }
      // Child process creation is wrecking the svA cmmntn.
      // Only create child process if a cnntn is accepted.
      if ( /*!fork() &&*/ clnt_fd != -1 ) {
	send_status = prep_buf_and_send_delays(&buf,
					       buf_len,
					       num_distances,
					       clnt_fd,
					       prop_delays,
					       trans_delays,
					       ttl_delays);
      }
      printf("The AWS has sent calculated delay to ");
      printf("client using TCP over port = %d\n", clnt_port);
      state = (send_status != -1 ? ENDTRANS : state);
      break;

    case ENDTRANS:
    default:
      break;

    } // switch
    if (state == ENDTRANS) {
      break;
    }
  } // while loop

  close(clnt_fd);

  free(buf);

  return 0;
}
