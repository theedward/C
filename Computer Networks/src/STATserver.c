#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct serverList{
	char ip[128];
	char port[128];
	int intNr;
	struct serverList* next;
};

struct clientList{
	char name[128];
	int intNr;
	struct clientList* next;
};

struct serverList* searchServer(char *ip, char *port, struct serverList *sl){
	struct serverList *aux = sl;
	while(aux != NULL){
		if(strcmp(ip, aux->ip) == 0 && strcmp(port, aux->port) == 0)
			return aux;
		aux = aux->next;
	}
	return NULL;
}

struct clientList* searchClient(char *name, struct clientList *cl){
	struct clientList *aux = cl;
	while(aux != NULL){
		if(strcmp(name, aux->name) == 0)
			return aux;
		aux = aux->next;
	}
	return NULL;
}

struct serverList* insertServer(char *ip, char *port, struct serverList *sl){
	struct serverList *aux = (struct serverList *)malloc(sizeof(struct serverList));
	strcpy(aux->ip, ip);
	strcpy(aux->port, port);
	aux->intNr=0;
	aux->next = sl;
	return aux;
}

struct clientList* insertClient(char *name, struct clientList *cl){
	struct clientList *aux = (struct clientList *)malloc(sizeof(struct clientList));
	strcpy(aux->name, name);
	aux->intNr = 1;
	aux->next = cl;
	return aux;
}

int fd, i;
struct hostent *hostptr;
struct sockaddr_in serveraddr, clientaddr;
int addrlen;
char *ip, *portstr, *name;
char *str = "Ok\n";
int port = 59000;
char buffer[128];
char bufferid[128];
struct serverList *sl, *auxServer;
struct clientList *cl, *auxClient;

int main(int argc, char *argv[]){
 //Verificação de argumentos da main para o -t
 if(argc >= 2 && strcmp(argv[1], "-t") == 0 )
   port = atoi(argv[2]);

  fd=socket(AF_INET,SOCK_DGRAM,0);
  memset((void*)&serveraddr,(int)'\0',
  sizeof(serveraddr));
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
  serveraddr.sin_port=htons((u_short)port);
  if(bind(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))== -1){
    printf("deu erro no bind\n");
    exit(-1);
  }
  while(1){
    addrlen=sizeof(clientaddr);
    if(recvfrom(fd,buffer,sizeof(buffer),0, (struct sockaddr*)&clientaddr, &addrlen) == -1){
      printf("erro na recepção de mensagem do servidor\n");      
      exit(-1);
    }
    if(strncmp("REG", buffer, 3)==0){
		i = 0;
		while(buffer[i+3]!='\n'){
		  bufferid[i] = buffer[i+4];
		  i++;
		}
		ip = strtok(bufferid, " ");
		portstr = strtok(NULL, "\n");
		
		sl = insertServer(ip, portstr, sl);
		auxServer = searchServer(ip, portstr, sl);
		
		printf("Server: %s %s - #%d\n", 
			auxServer->ip, auxServer->port, auxServer->intNr++);
		
		addrlen=sizeof(serveraddr);

		if(sendto(fd,str,strlen(str)+1,0,(struct sockaddr*)&clientaddr,addrlen) == -1){
			printf("erro no envio de mensagem do servidor\n");
			exit(-1);
		}
	}
	if(strncmp("UPD", buffer, 3)==0){
		i = 0;
		while(buffer[i+3]!='\n'){
		  bufferid[i] = buffer[i+4];
		  i++;
		}
		
		ip = strtok(bufferid, " ");
		portstr = strtok(NULL, " ");
		name = strtok(NULL, "\n");
		if((auxClient = searchClient(name, cl)) == NULL){
			cl = insertClient(name, cl);
			auxClient = cl;
		}
		auxServer = searchServer(ip, portstr, sl);
		
		printf("Server: %s %s - #%d\nClient: %s - #%d\n", 
			auxServer->ip, auxServer->port, auxServer->intNr++, auxClient->name, auxClient->intNr++);
		
		addrlen=sizeof(serveraddr);

		if(sendto(fd,str,strlen(str)+1,0,(struct sockaddr*)&clientaddr,addrlen) == -1){
			printf("erro no envio de mensagem do servidor\n");
			exit(-1);
		}
	}
  }
  close(fd);

}
