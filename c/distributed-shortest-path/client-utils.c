#include "client-utils.h"


/** Set the hints variable.
 */
void
set_sock_preferences (struct addrinfo *sock_preferences) {

  memset(sock_preferences, 0, sizeof *sock_preferences);
  sock_preferences->ai_family = AF_UNSPEC; // IPv4 or IPv6
  sock_preferences->ai_socktype = SOCK_STREAM; // TCP
}

/** Err if the client prog doesn't have the correct number of
 *  inputs.
 */
void
check_number_of_args (int argc) {

  if (argc < 2) {
    fprintf(stderr,"The client program takes input!\n");
    exit(1);
  }
}

/** Get the sockaddr, IPv4 or IPv6:
 */
void *
get_in_addr (struct sockaddr *sa) {

  if ( sa->sa_family == AF_INET ) {
    return &( ((struct sockaddr_in*)sa)->sin_addr );
  }
  return &( ((struct sockaddr_in6*)sa)->sin6_addr );
}

/** Try to create a socket() for each possible connection
 * returned from getaddrinfo, and connect() to the first one
 * that is doesn't fail.
 */
void
create_sock_and_connect (int *sock_fd,
			 struct addrinfo *poss_cnntns) {

  int cnntn_status;
  struct addrinfo *cnntn;
  for ( cnntn = poss_cnntns;
	cnntn != NULL;
	cnntn = cnntn->ai_next ) {

    *sock_fd = socket(cnntn->ai_family,
			   cnntn->ai_socktype,
			   cnntn->ai_protocol);
    if ( *sock_fd == -1 ) {
      perror("client: socket creation failed.");
      continue;
    }
    // Connect to 1st poss result from poss_cnntns.
    cnntn_status = connect(*sock_fd,
			   cnntn->ai_addr,
			   cnntn->ai_addrlen);
    if ( cnntn_status == -1 ) {
      close(*sock_fd);
      perror("client: connection through socket failed.");
      continue;
    }
    break;
  }
  if ( cnntn == NULL ) {
    fprintf(stderr, "client: failed to connect\n");
    exit(2);
  }
}

/** Calls send() to push msg = argv[1] argv[2] argv[3]  of
 *  length msg_len out the client socket.
 */
void
send_tcp_msg (int sock_fd,
	      int msg_len,
	      char **argv) {

  char *msg = (char *)calloc(msg_len, sizeof(*msg));
  sprintf(msg, "%s %s %s", argv[1], argv[2], argv[3]);
  if ( send(sock_fd, msg, msg_len, 0) == -1 ) {
    perror("Failed to send intial client msg to AWS!\n");
    close(sock_fd);
    exit(1);
  }
  free(msg);
}

/** Calculate the length of the distances array, i.e. the
 *  numbers of nodes that we need to calc shortest paths
 *  for.
 */
void
calc_distances_len (int *distances_len, // O
		    char *buf) {

  *distances_len = 1; // The last line has no '\n'
  for ( int i = 0; i < strlen(buf); ++i ) {
    if (buf[i] == ' ') {
      ++(*distances_len);
    }
  }
}

/** Find whitespace locations to aid in parsing recv'd
 *  information.
 */
void
find_whitespace_locs (int *whitespace_locs, // O
		      char* buf) {

  int wl_i = 0;
  for ( int i = 0; i < strlen(buf); ++i ) {
    if (buf[i] == ' ') {
      whitespace_locs[++wl_i] = i;
    } // pre-fix increment to account for string structure.
  }
}

/** Parse the buf to grab the distances
 */
void
parse_buf_for_distances (int *distances, // O
			 char *buf,
			 int distances_len) {

  int *whitespace_locs = (int *)calloc(distances_len,
				       sizeof(*whitespace_locs));
  find_whitespace_locs(whitespace_locs, buf);
  for ( int i = 0;  i != distances_len; ++i ) {
    sscanf(buf + ( i == 0 ? i : whitespace_locs[i] ),
	   "%d ", distances + i);
  }
  free(whitespace_locs);
}

/** Populate the distances array after parsing them from the
 *  msg recv'd from the AWS.
 *  NOTE: don't forget to free() int *distances in the caller.
 */
void
recv_tcp (char *buf, // O
	  int buf_len,
	  int sock_fd) {

  memset(buf, 0, buf_len * sizeof(*buf));
  int num_recv_bytes = recv(sock_fd, buf, buf_len, 0);
  if ( num_recv_bytes == -1 ) {
    perror("Error: receiving Client Data\n");
    close(sock_fd);
    exit(1);
  }
  buf[num_recv_bytes] = '\0';
}

/** Populate the delays arrays after parsing them from the
 *  msg recv'd from the AWS.
 *  NOTE: don't forget to free() int *total_delays,
 *  int *prop_delays, and int* trans_delays in the caller.
 */
void
parse_buf_for_delays (double *p_delays,   // O
		      double *t_delays,   // O
		      double *ttl_delays, // O
		      char *buf,
		      int buf_len,
		      int sock_fd,
		      int distances_len) {

  int *newline_locs = (int *)calloc(distances_len,
				    sizeof(*newline_locs));
  int nl_i = 0;
  for ( int i = 0; i < strlen(buf); ++i ) {
    if (buf[i] == '\n') {
      // pre-fix increment to account for string structure.
      newline_locs[++nl_i] = i;
    }
  }
  for ( int i = 0;  i != distances_len; ++i ) {
    sscanf(buf + newline_locs[i],
           "%lf %lf %lf",
           p_delays + i,
           t_delays + i,
           ttl_delays + i );
  }
  free(newline_locs);
}

void
print_formatting_dashes (int n) {

  for (int i = 0; i < n; ++i) {
    printf( (i != n-1) ? "-" : "-\n" );
  }
}

void
print_results (double *p_delays,
	       double *t_delays,
	       double *ttl_delays,
	       int *distances,
	       int distances_len) {

  for (int i = 0; i < distances_len; ++i) {
    printf( "%-15d %-15d %-15.3f %-15.3f %-15.3f\n",
	    i,
	    distances[i],
	    t_delays[i],
	    p_delays[i],
	    ttl_delays[i]);
  }
}
