#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main (int argc, char **argv) {

  struct addrinfo hints, *res, *p;
  int status;
  //  char ipstr[INET6_ADDRSTRLEN]; // You're not allowed to malloc this for some reason (something to do with inet_ntop).
  char *ipstr = (char *)calloc(INET6_ADDRSTRLEN, sizeof(*ipstr));

  if (argc != 2) {
    fprintf(stderr, "Please enter a hostname\n");
    return 1;
  }

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC; // Use AF_INET(6) to force a version of IP.
  hints.ai_socktype = SOCK_STREAM;

  if ( (status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0 ) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 2;
  }

  printf("IP addresses for %s:\n\n", argv[1]);

  for (p = res; p != NULL; p = p->ai_next) {
    void *addr;
    char *ipver;

    // Get the ptr to the addr itself, different fields in IPv4 and IPv6.
    if (p->ai_family == AF_INET) { // IPv4
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPV4";
    } else { // IPv6
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
      ipver = "IPV6";
    }

    // Convert the IP to a string and print it
    //    inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
    inet_ntop(p->ai_family, addr, ipstr, INET6_ADDRSTRLEN * sizeof(ipstr));
    printf("  %s: %s\n", ipver, ipstr);
  }

  freeaddrinfo(res); // free the linked list

  return 0;
}
