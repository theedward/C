#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int fd;
struct hostent *hostptr;
struct sockaddr_in serveraddr, clientaddr;
int clientlen;
char buffer[128];
int port = 58000;
char *str = "client";
char *tosend = "REQ ";
int i;

int main(int argc, char *argv[]){

  fd=socket(AF_INET,SOCK_STREAM,0);

  if(argc >= 3 && strcmp(argv[2], "-p") == 0 )
	port = atoi(argv[3]);
 		

  if(argc < 3 || strcmp(argv[2], "-n") != 0){
    gethostname(buffer, 128);
    hostptr=gethostbyname(buffer);
  }
  else
    hostptr=gethostbyname(argv[3]);

    memset((void*)&serveraddr,(int)'\0', sizeof(serveraddr)); 
    serveraddr.sin_family=AF_INET; 
    serveraddr.sin_addr.s_addr=((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr; 
    serveraddr.sin_port=htons((u_short)port); 


    connect(fd,(struct sockaddr*)&serveraddr,
    sizeof(serveraddr));

	strcpy(buffer, tosend);
    strcat(buffer, argv[1]);
    strcat(buffer, "\n");
    write(fd,buffer,strlen(buffer));

    read(fd,buffer, sizeof(buffer));
 
    if(strncmp("Ok", buffer, 2)==0){
    i = 0;
    while(buffer[i+1]!='\n'){
      buffer[i] = buffer[i+3];
      i++;
	}
}
    printf("%s", buffer);
    fflush(stdout);

    close(fd);


}
