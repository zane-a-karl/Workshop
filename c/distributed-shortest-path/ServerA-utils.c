#include "ServerA-utils.h"


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
      perror("Server A: socket creation failed.");
      continue;
    }
    // Bind to 1st poss result from poss_cnntns.
    bind_status = bind(*sock_fd,
		       cnntn->ai_addr,
		       cnntn->ai_addrlen);
    if ( bind_status == -1 ) {
      close(*sock_fd);
      perror("Server A: connection through socket failed.");
      continue;
    }
    break;
  }
  if ( cnntn == NULL ) {
    fprintf(stderr, "Server A: failed to connect\n");
    exit(2);
  }
}

/**
 * Using the input file stream open the file specified by
 * fname.
 */
void
open_map_file(FILE **fin, // O
	      char *fname) {

  *fin = fopen("map.txt", "r");
  if ( *fin == NULL ) {
    perror(strcat("Failed to open ", fname));
    exit(1);
  }
}

/**
 *
 */
char *
able_to_read_line (char *buf, // O
		   FILE **fin, // O
		   int buf_len) {

  char *ret_val = fgets(buf, buf_len, *fin);
  if (ret_val == NULL &&  0 != ferror(*fin)) {
    perror("Failed to read line from file");
  }
  return ret_val; // handles feof() too.
}

/**
 * The obersavtion of a letter designates a new map is
 * beginning to be described. As such, we merely count the
 * number of times we see letters to get the number of maps.
 * Note that the file pointer is moved forward during this
 * function's execution.
 */
void
calculate_num_maps (int *num_maps, // O
		    char *buf,
		    FILE **fin,
		    int buf_len) {

  memset(buf, 0, buf_len * sizeof *buf);
  while ( able_to_read_line(buf, fin, buf_len) ) {
    if ( isalpha(buf[0]) ) {
      ++(*num_maps);
    }
  }
}

/**
 * fseek() should be preferred over rewind(). Because
 * the rewind function sets the file position indicator for
 * the stream pointed to by stream to the beginning of the
 * file, except that the error indicator for the stream is
 * also cleared. This function executes the equivalent of
 * rewind() without losing the error checking.
 * NOTE: the second input to fseek is a 'long' and the 'L'
 * makes the '0' a long int without us having to declare
 * a new variable.
 */
void
rewind_map_file (FILE **fin) { // O

  if ( 0 != fseek(*fin, 0L, SEEK_SET) ) {
    perror("Error occurred rewinding map file");
    exit(1);
  }
}

/**
 * Prints a line of 'n' '-'s
 */
void
print_formatting_dashes (int n) {

  for (int i = 0; i < n; ++i) {
    printf( (i != n-1) ? "-" : "-\n" );
  }
}

/**
 * This program is a glorified version of peek() which only
 * exists in C++, but exists here as the pari fgetc() followed
 * by ungetc().
 */
void
check_whether_current_map_is_finished (char *seen_vertices,// O
				       int *map_cursor,    // O
				       char *buf,
				       FILE **fin,
				       int buf_len,
				       int map_i,
				       int *num_edges,
				       int *num_vertices) {

  int c = fgetc(*fin);
  if ( isalpha(c) || feof(*fin) ) {
    sprintf(buf, "%d", num_vertices[map_i]);// num_verts -> str
    printf("%-16s", buf);
    memset(buf, 0, buf_len * sizeof(*buf)); // clear buf
    sprintf(buf, "%d", num_edges[map_i]);   // num_edges -> str
    printf("%-13s\n", buf);
    memset(buf, 0, buf_len * sizeof(*buf)); // clear buf
    *map_cursor = 0;                         // Reset cursor
    *seen_vertices = 0;
  }
  ungetc(c, *fin);
}

/**
 *
 */
void
parse_map_file_for_graph_info (int *prop_speed,   // O
			       int *num_edges,    // O
			       int *num_vertices, // O
			       int *map_i,        // O
			       int *trans_speed,  // O
			       char *buf,
			       FILE **fin,
			       int buf_len) {

  char seen_vertices = 0;
  int map_cursor = 0;
  int v1 = 0;
  int v2 = 0;

  memset(buf, 0, buf_len * sizeof(*buf));
  while ( able_to_read_line(buf, fin, buf_len) ) {
    switch (map_cursor) {
    case 0:
      ++(*map_i);
      printf("%-10c", buf[0]); // mapid + 9whitespaces
      ++map_cursor;
      break;
    case 1:
      sscanf(buf, "%d", prop_speed + *map_i);
      ++map_cursor;
      break;
    case 2:
      sscanf(buf, "%d", trans_speed + *map_i);
      ++map_cursor;
      break;
    default:
      sscanf(buf, "%d %d %*d", &v1, &v2);
      if ( !((seen_vertices >> v1) & 1) ) {
	seen_vertices ^= (1 << v1);
	++num_vertices[*map_i];
      }
      if ( !((seen_vertices >> v2) & 1) ) {
	seen_vertices ^= (1 << v2);
	++num_vertices[*map_i];
      }
      ++num_edges[*map_i];
      break;
    }

    check_whether_current_map_is_finished(&seen_vertices,
					  &map_cursor,
					  buf,
					  fin,
					  buf_len,
					  *map_i,
					  num_edges,
					  num_vertices);

  } // end while
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
 *
 */
void
construct_chosen_graph (int **adj_mat, // O
			char *buf,
			FILE **fin,
			int buf_len,
			int map_id) {

  int dist = 0;
  int map_cursor = 0;
  int v1 = 0;
  int v2 = 0;

  memset(buf, 0, buf_len * sizeof(*buf));
  while ( able_to_read_line(buf, fin, buf_len) ) {
    if ( buf[0] == map_id ) {
      ++map_cursor;
      while ( able_to_read_line(buf, fin, buf_len) ) {
	if ( isalpha(buf[0]) ) { break; }
	switch (map_cursor) {
	case 0:
	  break;
	case 1:
	  ++map_cursor;
	  break;
	case 2:
	  ++map_cursor;
	  break;
	default:
	  sscanf(buf, "%d %d %d", &v1, &v2, &dist);
	  adj_mat[v1][v2] = dist;
	  adj_mat[v2][v1] = dist;
	  break;
	} // end switch
      } // end while
      break;
    } //end if
  } // end while
}

/**
 * Using the input file stream close the file
 */
void
close_map_file(FILE **fin) { // O

  int ret_val = fclose(*fin);
  if ( ret_val != 0 ) {
    perror("Something weird happened on file closure");
    exit(1);
  }
}

/**
 *
 */
void
prep_buf_and_send_udp_speeds (char *buf,
			      int buf_len,
			      int map_i,
			      int sock_fd,
			      int *prop_speed,
			      int *trans_speed,
			      socklen_t addr_len,
			      struct sockaddr_storage *addr) {

  memset(buf, 0, buf_len * (sizeof *buf));
  sprintf(buf,
	  "%d %d",
	  prop_speed[map_i],
	  trans_speed[map_i]);
  int num_send_bytes = sendto(sock_fd,
			      buf,
			      buf_len * sizeof(*buf),
			      0,
			      (struct sockaddr *)addr,
			      addr_len);
  if (num_send_bytes == -1) {
    perror("sendto srvrA -> aws failed");
    exit(1);
  }
  buf[num_send_bytes] = '\0';
  char *s = (char *)calloc(INET6_ADDRSTRLEN, sizeof(*s));
  inet_ntop(addr->ss_family,
	    get_in_addr((struct sockaddr *)addr),
	    s,
	    sizeof s);
  free(s);
}

/**
 *
 */
void
prep_buf_and_send_udp_dists (char *buf,
			     int buf_len,
			     int map_i,
			     int sock_fd,
			     int *distances,
			     int *num_vertices,
			     socklen_t addr_len,
			     struct sockaddr_storage *addr) {

  memset(buf, 0, buf_len * (sizeof *buf));
  for (int i = 0; i < num_vertices[map_i]; ++i) {
    // You'd have to create a new (FILE *) but fmemopen(3) would append these strings w/o recalc'ing strlen.
    sprintf(buf + strlen(buf),
	    (i != num_vertices[map_i]-1) ? "%-15d %d\n" : "%-15d %d",
	    i,
	    distances[i]);
  }
  int num_send_bytes = sendto(sock_fd,
			      buf,
			      buf_len * sizeof(*buf),
			      0,
			      (struct sockaddr *)addr,
			      addr_len);
  if (num_send_bytes == -1) {
    perror("sendto srvrA -> aws failed");
    exit(1);
  }
  buf[num_send_bytes] = '\0';
  char *s = (char *)calloc(INET6_ADDRSTRLEN, sizeof(*s));
  inet_ntop(addr->ss_family,
	    get_in_addr((struct sockaddr *)addr),
	    s,
	    sizeof s);
  free(s);
}
