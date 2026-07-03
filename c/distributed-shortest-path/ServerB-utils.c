#include "ServerB-utils.h"


/** Err if the server B prog doesn't have the correct number of
 *  inputs.
 */
void
check_number_of_args (int argc) {

  if (argc > 1) {
    fprintf(stderr,"The server B prog doesn't take input!\n");
    exit(1);
  }
}

/** Set the hints variable.
 */
void
set_sock_preferences (struct addrinfo *sock_preferences) {

  memset(sock_preferences, 0, sizeof *sock_preferences);
  sock_preferences->ai_family = AF_UNSPEC; // IPv4 or IPv6
  sock_preferences->ai_socktype = SOCK_DGRAM; // UDP
  sock_preferences->ai_flags = AI_PASSIVE; // use my IP
}

void
print_info6_about (struct sockaddr_storage *addr) {

  char ip6[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6,
	    &((struct sockaddr_in6 *)addr)->sin6_addr,
	    ip6,
	    INET6_ADDRSTRLEN);
  printf("family=%d, ", ((struct sockaddr_in6 *)addr)->sin6_family);
  printf("port=%d, ", ((struct sockaddr_in6 *)addr)->sin6_family);
  printf("address=%s\n", ip6);
}

/**
 *  get sockaddr, IPv4 or IPv6
 */
void *
get_in_addr (struct sockaddr *sa) {

  if (sa->sa_family == AF_INET) {
    return &( ((struct sockaddr_in*)sa)->sin_addr );
  }
  return &( ((struct sockaddr_in6*)sa)->sin6_addr );
}

/** Try to create a socket() for each possible connection
 * returned from getaddrinfo, and bind() to the first one
 * that is doesn't fail. Remeber we are listening with this
 * Server so we don't connect we just bind and wait for a
 * connection from a client.
 */
void
create_sock_and_bind (int *sock_fd,                   // O
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
      perror("Server B: socket creation failed.");
      continue;
    }
    // Bind to 1st poss result from poss_cnntns.
    bind_status = bind(*sock_fd,
		       cnntn->ai_addr,
		       cnntn->ai_addrlen);
    if ( bind_status == -1 ) {
      close(*sock_fd);
      perror("Server B: connection through socket failed.");
      continue;
    }
    break;
  }
  if ( cnntn == NULL ) {
    fprintf(stderr, "Server B: failed to connect\n");
    exit(2);
  }
}

/**
 * addr is an Output
 */
void
prep_buf_and_recv_udp (char *buf,                     // O
		       socklen_t *addr_len,           // O
		       struct sockaddr_storage *addr, // O
		       int buf_len,
		       int sock_fd) {

  memset(buf, 0, buf_len * (sizeof *buf));
  int num_recv_bytes = recvfrom(sock_fd,
				buf,
				buf_len * sizeof(*buf),
				MSG_WAITALL,
				(struct sockaddr *)addr,
				addr_len); // mutated here
  if (num_recv_bytes == -1) {
    perror("recvfrom (aws_addr not filled)");
    exit(1);
  }
  buf[num_recv_bytes] = '\0';
  char *s = (char *)calloc(INET6_ADDRSTRLEN, sizeof(*s));
  inet_ntop(addr->ss_family,
	    get_in_addr((struct sockaddr *)addr),
	    s,
	    sizeof s);
  free(s);
}

/**
 * the addr is sending the sock_fd is recv'ing
 */
void
recv_udp_speeds (int *distances_len,            // O
		 int *file_size,                // O
		 int *prop_speed,               // O
		 int *trans_speed,              // O
		 socklen_t *addr_len,           // O
		 struct sockaddr_storage *addr, // O
		 char *buf,
		 int buf_len,
		 int sock_fd) {

  prep_buf_and_recv_udp(buf, addr_len, addr, buf_len, sock_fd);
  sscanf(buf,
	 "%d %d %d %d",
	 prop_speed,
	 trans_speed,
	 distances_len,
	 file_size);
}

/**
 * Find whitespace locactions to aid in parsing recv'd info.
 */
void
find_whitespace_locs (int *whitespace_locs, // O
		      char *buf) {
  int wl_i = 0;
  for ( int i = 0; i < strlen(buf); ++i ) {
    if (buf[i] == ' ') {
      whitespace_locs[++wl_i] = i;
    } // pre-fix increment to account for string structure.
  }
}

void
recv_udp_distances (char *buf,                     // O
		    int *distances,		   // O
		    socklen_t *addr_len,           // O
		    struct sockaddr_storage *addr, // O
		    int buf_len,
		    int distances_len,
		    int sock_fd) {

  prep_buf_and_recv_udp(buf, addr_len, addr, buf_len, sock_fd);
  int *whitespc_locs = (int *)calloc(distances_len,
				     sizeof(*whitespc_locs));
  find_whitespace_locs(whitespc_locs, buf);
  for ( int i = 0;  i != distances_len; ++i ) {
    sscanf(buf + ( i == 0 ? i : whitespc_locs[i] ),
	   "%d ",
	   distances + i);
  }
  free(whitespc_locs);
}

void
calc_delays (double *prop_delays,  // O
	     double *trans_delays, // O
	     double *ttl_delays, // O
	     int *distances,
	     int distances_len,
	     int prop_speed,
	     int trans_speed,
	     int file_size) {

  for ( int i = 0; i < distances_len; ++i ) {
    prop_delays[i] = (double)distances[i] / prop_speed;
    trans_delays[i] = (double)file_size / ( 8*trans_speed );
    ttl_delays[i] = prop_delays[i] + trans_delays[i];
  }
}

void
print_formatting_dashes (int n) {

  for (int i = 0; i < n; ++i) {
    printf( (i != n-1) ? "-" : "-\n" );
  }
}

void
print_results (double *ttl_delays,
	       int distances_len) {

  print_formatting_dashes(25);
  printf("Destination     TtlDelay\n");
  print_formatting_dashes(25);
  for ( int i = 0; i < distances_len; ++i ) {
    printf("%-15d %.6f\n",
	   i,
	   ttl_delays[i]);
  }
  print_formatting_dashes(25);
}

void
prep_buf_to_send_delays (char *buf, // O
			 double *prop_delays,
			 double *trans_delays,
			 double *ttl_delays,
			 int buf_len,
			 int distances_len) {

  memset(buf, 0, buf_len * sizeof(*buf));
  for (int i = 0; i < distances_len; ++i) {
    sprintf(buf + strlen(buf),
	    "%.3f %.3f %.3f",
	    prop_delays[i],
	    trans_delays[i],
	    ttl_delays[i] );
    if (i != distances_len-1) {
      sprintf(buf + strlen(buf), "\n");
    }
  }
}

void
send_udp_delays (char *buf,                     // O
		 struct sockaddr_storage *addr, // O
		 int buf_len,
		 int sock_fd,
		 socklen_t addr_len) {

  int num_send_bytes = sendto(sock_fd,
			      buf,
			      strlen(buf),
			      0,
			      (struct sockaddr *)addr,
			      addr_len);
  if (num_send_bytes == -1) {
    perror("Server B: sendto has failed");
    exit(1);
  }
}
