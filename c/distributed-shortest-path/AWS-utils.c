#include "AWS-utils.h"

/** Err if the AWS prog doesn't have the correct number of
 *  inputs.
 */
void
check_number_of_args (int argc) {

  if (argc > 1) {
    fprintf(stderr,"The AWS prog doesn't take input!\n");
    exit(1);
  }
}

/** Error handling function for the library function
 * getaddrinfo.
 */
void
check_if_getaddrinfo_failed (int getaddrinfo_result,
			     char *s) {

  if ( getaddrinfo_result ) {
    fprintf(stderr,
	    "getaddrinfo (%s): %s\n",
	    s,
	    gai_strerror(getaddrinfo_result));
    exit(1);
  }
}

/** Set the hints variable.
 */
void
set_tcp_sock_preferences (struct addrinfo *sock_preferences) {

  memset(sock_preferences, 0, sizeof *sock_preferences);
  sock_preferences->ai_family = AF_UNSPEC; // IPv4 or IPv6
  sock_preferences->ai_socktype = SOCK_STREAM; // TCP
  sock_preferences->ai_flags = AI_PASSIVE; // use my IP
}

/** Set the hints variable.
 */
void
set_udp_sock_preferences (struct addrinfo *sock_preferences) {

  memset(sock_preferences, 0, sizeof *sock_preferences);
  sock_preferences->ai_family = AF_UNSPEC; // IPv4 or IPv6
  sock_preferences->ai_socktype = SOCK_DGRAM; // UDP
}

/** Try to create a socket() for each possible connection
 * returned from getaddrinfo, and keep the pointer to the
 * first one that succeeds!
 */
void
get_available_socket (struct addrinfo **cnntn, // O
		      struct addrinfo *poss_cnntns) {

  int socket_status;
  for (*cnntn = poss_cnntns;
       *cnntn != NULL;
       *cnntn = (*cnntn)->ai_next) {
    socket_status = socket((*cnntn)->ai_family,
			   (*cnntn)->ai_socktype,
			   0);
    if ( socket_status == -1 ) {
      perror("Failed to create socket for this cnntn");
      continue;
    }
    break;
  }
}

/** Try to create a socket() for each possible connection
 * returned from getaddrinfo, set the socket level tcp option
 * to reusable, and bind() to the first one
 * that is doesn't fail. Remeber we are listening with this
 * Server so we don't connect we just bind and wait for a
 * connection from a client.
 */
void
create_tcp_sock_and_bind (int *sock_fd,                   // O
			  struct addrinfo *poss_cnntns) { // O

  int yes = 1; // non-zero signals a sock option will change
  int bind_status;
  int setsockopt_status;
  struct addrinfo *cnntn;
  for (cnntn = poss_cnntns;
       cnntn != NULL;
       cnntn = cnntn->ai_next) {
    *sock_fd = socket(cnntn->ai_family,
		      cnntn->ai_socktype,
		      cnntn->ai_protocol);
    if ( *sock_fd == -1) {
      perror("tcp socket creation failed");
      continue;
    }
    setsockopt_status = setsockopt(*sock_fd,
				   SOL_SOCKET,
				   SO_REUSEADDR,
				   &yes,
				   sizeof(yes));
    if ( setsockopt_status == -1 ) {
      perror("tcp setsockopt failed ");
      exit(1);
    }
    bind_status = bind(*sock_fd,
		       cnntn->ai_addr,
		       cnntn->ai_addrlen);
    if ( bind_status == -1 ) {
      close(*sock_fd);
      perror("tcp bind failed");
      continue;
    }
    break;
  }
  if ( cnntn == NULL ) {
    fprintf(stderr, "awstcp: failed to connect\n");
    exit(2);
  }
}

/** Try to create a socket() for each possible connection
 * returned from getaddrinfo, and bind() to the first one
 * that is doesn't fail. Remeber we are listening with this
 * Server so we don't connect we just bind and wait for a
 * connection from a client.
 */
void
create_udp_sock_and_bind (int *sock_fd,                   // O
			  struct addrinfo *poss_cnntns) { // O

  int bind_status;
  struct addrinfo *cnntn;
  for ( cnntn = poss_cnntns;
	cnntn != NULL;
	cnntn = cnntn->ai_next ) {

    *sock_fd = socket(cnntn->ai_family,
			    cnntn->ai_socktype,
			    cnntn->ai_protocol);
    if ( *sock_fd == -1 ) {
      perror("aws udp: socket creation failed.");
      continue;
    }
    // Bind to 1st poss result from poss_cnntns.
    bind_status = bind(*sock_fd,
		       cnntn->ai_addr,
		       cnntn->ai_addrlen);
    if ( bind_status == -1 ) {
      close(*sock_fd);
      perror("aws udp: connection through socket failed.");
      continue;
    }
    break;
  }
  if ( cnntn == NULL ) {
    fprintf(stderr, "aws udp: failed to connect\n");
    exit(2);
  }
}

void
begin_tcp_listening (int *sock_fd,
		     int max_queue_len) {

  int listen_status = listen(*sock_fd, max_queue_len);
  if ( listen_status == -1) {
    perror(" tcp listen failed");
    exit(1);
  }
}

/**  reaping zombie processes that appear as the fork()ed
 * child processes exit. This solves the problem of the error
 * messages “port already in use” or “address already in use”
 */
void
reap_zombie_processes (struct sigaction *sa) {

  int sigaction_status;
  sa->sa_handler = sigchld_handler;
  sigemptyset(&sa->sa_mask);
  sa->sa_flags = SA_RESTART;
  sigaction_status = sigaction(SIGCHLD, sa, NULL);
  if ( sigaction_status == -1 ) {
    perror("sigaction failed");
    exit(1);
  }
}

void
sigchld_handler (int s) {

  // waitpid() might overwrite errno so we save and restore it
  int saved_errno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
}

void *
get_in_addr (struct sockaddr *sa) {

  if (sa->sa_family == AF_INET) {
    return &( ((struct sockaddr_in  *)sa)->sin_addr );
  } else {
    return &( ((struct sockaddr_in6  *)sa)->sin6_addr );
  }
}

int
get_port (struct sockaddr *addr) {

  char *ip_addr;
  if (addr->sa_family == AF_INET) {
    ip_addr = (char *)calloc(INET_ADDRSTRLEN,
			     sizeof *ip_addr);
    inet_ntop(addr->sa_family,
	      get_in_addr( (struct sockaddr *)&addr ),
	      ip_addr,
	      INET_ADDRSTRLEN);
    free(ip_addr);
    return ( (struct sockaddr_in *)&addr )->sin_port;

  } else if (addr->sa_family == AF_INET6) {
    ip_addr = (char *)calloc(INET6_ADDRSTRLEN,
			     sizeof *ip_addr);
    inet_ntop(addr->sa_family,
	      get_in_addr( (struct sockaddr *)&addr ),
	      ip_addr,
	      INET6_ADDRSTRLEN);
    free(ip_addr);
    return ( (struct sockaddr_in6 *)&addr )->sin6_port;

  } else {
    perror("Client socket type is not IPv4 or IPv6\n");
    exit(1);
  }
}

/** Receive the query data from the client and return the
 * number of recv'd bytes to denote success.
 */
int
recv_tcp_clnt_query (char **buf, // O
		     int buf_len,
		     int sock_fd) {

  int num_recv_bytes = recv(sock_fd,
			    *buf,
			    buf_len,
			    0);
  if (num_recv_bytes == -1) {
    perror("Error: receiving initial Client Data\n");
    close(sock_fd);
    exit(0);
  }
  (*buf)[num_recv_bytes] = '\0';
  return num_recv_bytes;
}

/** Grab the important parts of the client query and print
 * them in a human readable format.
 */
void
parse_clnt_query_and_print (char *map_id,    // O
			    int *file_sz,    // O
			    int *start_node, // O
			    char *buf,
			    int port) {

  sscanf(buf,
	 "%c %d %d",
	 map_id,
	 start_node,
	 file_sz);
  printf("The AWS has received map ID %c, ", *map_id);
  printf("start vertex %d ", *start_node);
  printf("and file size %d from the client ", *file_sz);
  printf("using TCP over port %d\n", port);
}

int
prep_buf_and_send_map_info (char **buf, // O
			    int buf_len,
			    int map_id,
			    int port,
			    int sock_fd,
			    int start_node,
			    struct addrinfo *sva_cnntn) {

  memset(*buf, 0, buf_len * sizeof **buf);
  sprintf(*buf, "%c %d %d", map_id, start_node, port);
  int num_send_bytes = sendto(sock_fd,
			      *buf,
			      buf_len * sizeof **buf,
			      0,
			      sva_cnntn->ai_addr,
			      sva_cnntn->ai_addrlen);
  if (num_send_bytes == -1) {
    perror("srvrA sendto failed");
    exit(1);
  }
  printf("The AWS has sent map ID=%c and ", map_id);
  printf("starting vertex=%d to server A, ", start_node);
  printf("using port %d\n", port);
  return num_send_bytes;
}

/** Clear out the buffer and recv the speeds that server b
 * will use to calculate the delays.
 */
int
prep_buf_and_recv_speeds (char **buf,
			  int buf_len,
			  int sock_fd,
			  struct addrinfo *sva_cnntn) {

  memset(*buf, 0, buf_len * sizeof **buf);
  int num_recv_bytes = recvfrom(sock_fd,
				*buf,
				buf_len * sizeof **buf,
				MSG_WAITALL,
				sva_cnntn->ai_addr,
				&sva_cnntn->ai_addrlen);
  if (num_recv_bytes == -1) {
    perror("srvrA speeds recvfrom failed");
    exit(1);
  }
  (*buf)[num_recv_bytes] = '\0';

  return num_recv_bytes;
}

/** look through the buffer and parse the prop and trans speeds
 * that server b will use to calculate the delays.
 */
void
parse_buf_for_speeds (int *prop_speed,  // O
		      int *trans_speed, // O
		      char *buf) {

  sscanf(buf, "%d %d", prop_speed, trans_speed);
  printf("The p_speed(km/s) = %d, ", *prop_speed);
  printf("the t_speed(B/s) = %d\n", *trans_speed);
}

/** Clear out the buffer and recv the shortest paths from
 * server a. Return number of recv'd bytes on success, -1 on
 * failure.
 */
int
prep_buf_and_recv_paths (char **buf,
			 int buf_len,
			 int sock_fd,
			 struct addrinfo *sva_cnntn) {

  memset(*buf, 0, buf_len * sizeof **buf);
  int num_recv_bytes = recvfrom(sock_fd,
				*buf,
				buf_len * sizeof **buf,
				MSG_WAITALL,
				sva_cnntn->ai_addr,
				&sva_cnntn->ai_addrlen);
  if (num_recv_bytes == -1) {
    perror("srvrA shortest paths recvfrom failed");
    exit(1);
  }
  printf("AWS packet is %d bytes long\n", num_recv_bytes);
  (*buf)[num_recv_bytes] = '\0';

  return num_recv_bytes;
}

/** Look at the buf and grab the distances that were calculated
 * at server a. Also use the string structure to save the
 * number of nodes/vertices (ie the length of the distances
 * array).
 */
void
parse_buf_for_distances (int *distances_len, // O
			 int **distances,    // O
			 char *buf) {

  *distances_len = 1; // The last line has no '\n'
  for ( int i = 0; i < strlen(buf); ++i ) {
    if (buf[i] == '\n') {
      ++(*distances_len);
    }
  }
  int *newline_locs = (int *)calloc(*distances_len,
				    sizeof *newline_locs);
  int nl_i = 0;
  for ( int i = 0; i < strlen(buf); ++i ) {
    if (buf[i] == '\n') {
      newline_locs[++nl_i] = i;
    } // pre-fix increment accounts for string structure.
  }

  *distances = (int *)calloc(*distances_len,
			     sizeof **distances);
  for ( int i = 0;  i != *distances_len; ++i ) {
    sscanf(buf + ( i == 0 ? i : newline_locs[i] ),
	   "%*d %d\n",
	   *distances + i);
  }
  free(newline_locs);
}


void
print_formatting_dashes (int n) {

  for (int i = 0; i < n; ++i) {
    printf( (i != n-1) ? "-" : "-\n" );
  }
}

/** Clear out the buffer, store into it info server b needs
 * for its delays calculation, and send that info to server b.
 * return the number of bytes sent on success and -1 on
 * failure.
 */
int
prep_buf_and_send_speeds (char **buf, // O
			  int buf_len,
			  int file_sz,
			  int num_distances,
			  int prop_speed,
			  int trans_speed,
			  int sock_fd,
			  struct addrinfo *svb_cnntn) {

  memset(*buf, 0, buf_len * sizeof **buf);
  sprintf(*buf,
	  "%d %d %d %d",
	  prop_speed,
	  trans_speed,
	  num_distances,
	  file_sz);
  int num_send_bytes = sendto(sock_fd,
			      *buf,
			      buf_len * sizeof **buf,
			      0,
			      svb_cnntn->ai_addr,
			      svb_cnntn->ai_addrlen);
  if (num_send_bytes == -1) {
    perror("srvrB speeds sendto failed");
    exit(1);
  }
  return num_send_bytes;
}

/** Clear the buffer, store the distances in it and send them.
 */
int
prep_buf_and_send_paths (char **buf, // O
			 int buf_len,
			 int num_distances,
			 int sock_fd,
			 int *distances,
			 struct addrinfo *svb_cnntn) {

  memset(*buf, 0, buf_len * sizeof **buf);
  for (int i = 0; i < num_distances; ++i) {
    sprintf(*buf + strlen(*buf),
	    (i != num_distances-1 ? "%d " : "%d"),
	    distances[i] );
  }
  int num_send_bytes = sendto(sock_fd,
			      *buf,
			      buf_len * sizeof **buf,
			      0,
			      svb_cnntn->ai_addr,
			      svb_cnntn->ai_addrlen);
  if (num_send_bytes == -1) {
    perror("srvrB sendto failed");
    exit(1);
  }
  return num_send_bytes;
}

/** Clear the buffer to store the received delay calculations
 * sent from server b in it. Return the number of recv bytes on
 * success and -1 on failure.
 */
int
prep_buf_and_recv_delays (char **buf,          // O
			  int buf_len,
			  int sock_fd,
			  struct addrinfo *svb_cnntn) {

  memset(*buf, 0, buf_len * sizeof **buf);
  int num_recv_bytes = recvfrom(sock_fd,
				*buf,
				buf_len * sizeof **buf,
				MSG_WAITALL,
				svb_cnntn->ai_addr,
				&svb_cnntn->ai_addrlen);
  if (num_recv_bytes == -1) {
    perror("srvrB recvfrom failed");
    exit(1);
  }
  (*buf)[num_recv_bytes] = '\0';

  return num_recv_bytes;
}

/** Look throught the buffer for the results of the delays
 * calculations that server b gave aws.
 */
void
parse_buf_for_delays (double **p_delays,   // O
		      double **t_delays,   // O
		      double **ttl_delays, // O
		      char *buf,
		      int buf_len,
		      int num_distances) {

  *p_delays = (double *)calloc(num_distances,
			       sizeof **p_delays);
  *t_delays = (double *)calloc(num_distances,
			       sizeof **t_delays);
  *ttl_delays = (double *)calloc(num_distances,
				 sizeof **ttl_delays);

  int nl_i = 0;
  int *newline_locs = (int *)calloc(num_distances,
				    sizeof *newline_locs);
  for ( int i = 0; i < strlen(buf); ++i ) {
    if (buf[i] == '\n') {
      newline_locs[++nl_i] = i;
    } // pre-fix increment accounts for string structure.
  }
  for ( int i = 0;  i != num_distances; ++i ) {
    sscanf(buf + (i == 0 ? i : newline_locs[i] ),
	   "%lf %lf %lf",
	   *p_delays + i,
	   *t_delays + i,
	   *ttl_delays + i );
  }
  free(newline_locs);
}

/** Prints the table with the delays results
 */
void
print_delays_results (int num_distances,
		      double *prop_delays,
		      double *trans_delays,
		      double *ttl_delays) {

  print_formatting_dashes(54);
  printf("Destination      ");
  printf("TransDelay     ");
  printf("PropDelay     ");
  printf("TtlDelay\n");
  print_formatting_dashes(54);
  for (int i = 0; i < num_distances; ++i) {
    printf( "%-16d %-14.3f %-13.3f %-16.3f\n",
	    i,
	    prop_delays[i],
	    trans_delays[i],
	    ttl_delays[i]);
  }
  print_formatting_dashes(54);
}

/**
 */
int
prep_buf_and_send_dists (char **buf,
			 int buf_len,
			 int num_distances,
			 int sock_fd,
			 int *distances) {

  memset(*buf, 0, buf_len * sizeof **buf);
  for (int i = 0; i < num_distances; ++i) {
    sprintf(*buf + strlen(*buf),
	    (i != num_distances-1 ? "%d " : "%d"),
	    distances[i] );
  }
  int num_send_bytes = send(sock_fd,
			    *buf,
			    buf_len * sizeof **buf,
			    0);
  if (num_send_bytes == -1) {
    perror("Failed to send shortest paths to client!\n");
    close(sock_fd);
    exit(0);
  }

  return num_send_bytes;
}

/**
 */
int
prep_buf_and_send_delays (char **buf, // O
			  int buf_len,
			  int num_distances,
			  int sock_fd,
			  double *prop_delays,
			  double *trans_delays,
			  double *ttl_delays) {

  memset(*buf, 0, buf_len * sizeof **buf);
  for (int i = 0; i < num_distances; ++i) {
    sprintf(*buf + strlen(*buf),
	    "%.3f %.3f %.3f",
	    prop_delays[i],
	    trans_delays[i],
	    ttl_delays[i] );
    if (i != num_distances-1) {
      sprintf(*buf + strlen(*buf), "\n");
    }
  }
 int num_send_bytes = send(sock_fd,
			*buf,
			buf_len * sizeof **buf,
			0);
  if (num_send_bytes == -1) {
    perror("Failed to send delays to client!\n");
    close(sock_fd);
    exit(0);
  }

  return num_send_bytes;
}
