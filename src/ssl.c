/* A simple HTTPS client

   It connects to the server, makes an HTTP
   request and waits for the response
*/
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <openssl/ssl.h>

#define CA_LIST "/etc/ssl/certs"
#define HOST	"localhost"
#define RANDOM  "random.pem"
#define PORT	4433
#define BUFSIZZ 1024


static char *host;
static int port;
BIO *bio_err=0;
static void sigpipe_handle(int x);



/* A simple error and exit routine*/
int err_exit(string)
  char *string;
  {
    fprintf(stderr,"%s\n",string);
    exit(0);
  }

/* Print SSL errors and exit*/
int berr_exit(string)
  char *string;
  {
    BIO_printf(bio_err,"%s\n",string);
    ERR_print_errors(bio_err);
    exit(0);
  }

/* Check that the common name matches the
   host name*/
int check_cert(int status, X509_STORE_CTX *x509_store_ctx){
	char buffer[256];
	X509* x509;

	if(status == 1)
		printf("Certificate OK: yes\n");
	else
		printf("Certificate did not verify.\n");

	x509 = X509_STORE_CTX_get_current_cert(x509_store_ctx);
		printf("Certificate subject:\n");

	X509_NAME_get_text_by_NID(X509_get_subject_name(x509), NID_commonName, buffer, 256);
	printf(" -Common name: %s\n",buffer);
	X509_NAME_get_text_by_NID(X509_get_subject_name(x509), NID_organizationName, buffer, 256);
	printf(" -Organization name: %s\n",buffer);

	printf("Certificate issuer:\n");	
	X509_NAME_get_text_by_NID(X509_get_issuer_name(x509), NID_commonName, buffer, 256);
	printf(" -Common name: %s\n",buffer);
	X509_NAME_get_text_by_NID(X509_get_issuer_name(x509), NID_organizationName, buffer, 256);
	printf(" -Organization name: %s\n",buffer);
	printf("===\n");

	return status;
	
	
  }	

static void sigpipe_handle(int x){
}

SSL_CTX *initialize_ctx()
  {
    SSL_METHOD *meth;
    SSL_CTX *ctx;
    
    if(!bio_err){
      /* Global system initialization*/
      SSL_library_init();
      SSL_load_error_strings();
      
      /* An error write context */
      bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);
    }

    /* Set up a SIGPIPE handler */
    signal(SIGPIPE,sigpipe_handle);
    
    /* Create our context*/
    meth=SSLv23_method();
    ctx=SSL_CTX_new(meth);
	SSL_CTX_set_verify(ctx,SSL_VERIFY_NONE, check_cert);


    /* Load the CAs we trust*/
    if(!(SSL_CTX_load_verify_locations(ctx,
      "/etc/ssl/certs/ca-certificates.crt",NULL)))
      berr_exit("Can't read CA list");
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
    SSL_CTX_set_verify_depth(ctx,3);
#endif
    
    return ctx;
  }
     
void destroy_ctx(ctx)
  SSL_CTX *ctx;
  {
    SSL_CTX_free(ctx);
  }

int tcp_connect(host,port)
  char *host;
  int port;
  {
    struct hostent *hp;
    struct sockaddr_in addr;
    int sock;
    
    if(!(hp=gethostbyname(host)))
      berr_exit("Couldn't resolve host");
    memset(&addr,0,sizeof(addr));
    addr.sin_addr=*(struct in_addr*)
      hp->h_addr_list[0];
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);

    if((sock=socket(AF_INET,SOCK_STREAM,
      IPPROTO_TCP))<0)
      err_exit("Couldn't create socket");
    if(connect(sock,(struct sockaddr *)&addr,
      sizeof(addr))<0)
      err_exit("Couldn't connect socket");
    
    return sock;
  }




static int http_request(ssl)
  SSL *ssl;
  {
    char *request = "GET / HTTP/1.1 \r\n\r\n";
    char buf[BUFSIZZ];
    int r;
    int len, request_len;
    
    /* Find the exact request_len */
    request_len=strlen(request);

	printf("%s.\n", request);
    r=SSL_write(ssl,request,request_len);

    switch(SSL_get_error(ssl,r)){      
      case SSL_ERROR_NONE:
        if(request_len!=r)
          err_exit("Incomplete write!");
        break;
        default:
          berr_exit("SSL write problem");
    }

    /* Now read the server's response, assuming
       that it's terminated by a close */
    while(1){									
      r=SSL_read(ssl,buf,BUFSIZZ);
      switch(SSL_get_error(ssl,r)){
        case SSL_ERROR_NONE:
          len=r;
          break;
        case SSL_ERROR_ZERO_RETURN:
          goto shutdown;
        case SSL_ERROR_SYSCALL:
          fprintf(stderr,
            "SSL Error: Premature close\n");
          goto done;
        default:
          berr_exit("SSL read problem");
      }

      fwrite(buf,1,len,stdout);
    }
    
  shutdown:
    r=SSL_shutdown(ssl);
    switch(r){
      case 1:
        break; /* Success */
      case 0:
      case -1:
      default:
        berr_exit("Shutdown failed");
    }
    
  done:
    SSL_free(ssl);
    free(request);
    return(0);
  }
    
int main(argc,argv)
  int argc;
  char **argv;
  {
    SSL_CTX *ctx;
    SSL *ssl;
    BIO *sbio;
    int sock;



   	host = argv[1];
	port = atoi(argv[2]);
	printf("Trying to connect to: %s, on port: %d\n", host, port);

	
    /* Build our SSL context*/
	printf("---Building SSL context---\n");
    ctx=initialize_ctx();
	printf("---Context Built---\n");

    /* Connect the TCP socket*/
	printf("---Creating socket to host---\n");
    sock=tcp_connect(host,port);
	if(sock != 0)
	printf("---Socket Created---\n");

    /* Connect the SSL socket */
	printf("---Connect the SSL socket---\n");
    ssl=SSL_new(ctx);
    sbio=BIO_new_socket(sock,BIO_NOCLOSE);
    SSL_set_bio(ssl,sbio,sbio);

    if(SSL_connect(ssl)<=0)
      berr_exit("SSL connect error");
	printf("---SSL Socket Connected---\n");

 
    /* Now make our HTTP request */
	printf("---Sending HTTP request---\n");
    http_request(ssl);
	printf("---HTTP request sent---\n");

    /* Shutdown the socket */
    destroy_ctx(ctx);
    close(sock);

    exit(0);
  }

