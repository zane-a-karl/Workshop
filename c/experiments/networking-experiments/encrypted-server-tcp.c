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

#include <openssl/bn.h>
#include <openssl/rand.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#define PORT "3490" // The port users will be connecting to.

#define BACKLOG 10

/*gcc server.c -o server.out -I /usr/local/opt/openssl@1.1/include -L /usr/local/opt/openssl@1.1/lib -lcrypto -Wall*/

void sigchld_handler (int s)
{
  // waitpid() might overwrite errno so we save and restore it.
  int saved_errno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
}

// get sockaddr IPv4 or IPv6
void *get_in_addr (struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &( ((struct sockaddr_in  *)sa)->sin_addr );
  } else {
    return &( ((struct sockaddr_in6  *)sa)->sin6_addr );
  }
}

int main (int argc, char **argv)
{
  int sockfd, new_fd; // listen on sockfd and new connection on new_fd.
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; //connector's addr info
  socklen_t sin_size;
  struct sigaction sa;
  int yes = 1;
  char *s = (char *)calloc(INET6_ADDRSTRLEN, sizeof(*s));
  int rv;

  /*START: initializing the libcrypto stuff*/
  /* Load the human readable error strings for libcrypto */
  ERR_load_crypto_strings();

  /* Load all digest and cipher algorithms */
  OpenSSL_add_all_algorithms();

  /* Load config file, and other important initialisation */
  OPENSSL_config(NULL);
  /*END: initializing the libcrypto stuff*/

  

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // Use my IP.
  
  if ( (rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0 ) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // Loop through all the results and bind to the first one we can.
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ( (sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1 ) {
      perror("setsockopt");
      exit(1);
    }

    if ( bind(sockfd, p->ai_addr, p->ai_addrlen) == -1 ) {
      close(sockfd);
      perror("server: bind");
      continue;
    }
    
    break;
  }

  freeaddrinfo(servinfo); // all done with this struct.

  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  if ( listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if ( sigaction(SIGCHLD, &sa, NULL) == -1 ) {
    perror("sigaction");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  /*START: encrypt a msg */
  /* A 256 bit key */
  unsigned char *key = (unsigned char *)"01234567890123456789012345678901";
  
  /* A 128 bit IV */
  unsigned char *iv = (unsigned char *)"0123456789012345";

  /* A 128 bit tag */
  unsigned char *tag = (unsigned char *)"0123456789012345";
  
  /* Message to be encrypted */
  unsigned char *plaintext = (unsigned char *)"The quick brown fox jumps over the lazy dog";

  /* AAD */
  unsigned char *add = (unsigned char *)"The quick brown fox jumps over the lazy dog";
  
  /*
   * Buffer for ciphertext. Ensure the buffer is long enough for the
   * ciphertext which may be longer than the plaintext, depending on the
   * algorithm and mode.
   */
  unsigned char ciphertext[128];
  
  int ciphertext_len;

  int howmany, dec_success, len;
  const EVP_CIPHER *cipher;
  switch(key_len)
  {
  case 128: cipher  = EVP_aes_128_gcm ();break;
  case 192: cipher  = EVP_aes_192_gcm ();break;
  case 256: cipher  = EVP_aes_256_gcm ();break;
  default:break;
  }
  // Encrypt
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  EVP_EncryptInit (ctx, cipher, KEY, IV);
  EVP_EncryptUpdate (ctx, NULL, &howmany, AAD, aad_len);
  len = 0;
  while(len <= in_len-128)
  {
     EVP_EncryptUpdate (ctx, CIPHERTEXT+len, &howmany, PLAINTEXT+len, 128);
     len+=128;
  }
  EVP_EncryptUpdate (ctx, CIPHERTEXT+len, &howmany, PLAINTEXT+len, in_len - len);
  EVP_EncryptFinal (ctx, TAG, &howmany);
  EVP_CIPHER_CTX_ctrl (ctx, EVP_CTRL_GCM_GET_TAG, 16, TAG);  
  EVP_CIPHER_CTX_free(ctx);
  
  /*END: encrypt a msg of "Hello client" */

  
  while(1) { // main accept() loop
    sin_size = sizeof (their_addr);
    if ( (new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size) ) == -1 ) {
      //      perror("accept");
      continue;
    }

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    printf("server: got connection from %s\n", s);

    if ( !fork() ) { // this is the child process
      close(sockfd); // child doesn't need the listener
      if (send(new_fd, ciphertext, ciphertext_len, 0) == -1) {
	perror("send");
	close(new_fd);
	exit(0);
      }
    }
    close(new_fd); // parent doesn't need this.
  }
  
  /*START: cleaning up the libcrypto stuff*/
  /* Removes all digests and ciphers */
  EVP_cleanup();

  /* if you omit the next, a small leak may be left when you make use of the BIO (low level API) for e.g. base64 transformations */
  CRYPTO_cleanup_all_ex_data();

  /* Remove error strings */
  ERR_free_strings();
  /*END: cleaning up the libcrypto stuff*/

  return 0;
}
