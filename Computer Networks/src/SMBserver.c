#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

//Parte do TCP server

int fdtcp, newfdtcp;
struct sockaddr_in serveraddrtcp, clientaddrtcp;
int clientlentcp;
char buffertcp[128];
char *str = "#";
int fdudp;
struct hostent *hostptrudp, *auxptr;
struct sockaddr_in serveraddrudp, clientaddrudp;
struct in_addr *a;
int addrlenudp;
char bufferudp[128];
int porttcp = 58000;
int portudp = 59000;
char simbolo[2];
char ip[128];
char port[128];
char *boas = "Ok Bem vindo ";
char *strconnect = " conectado";
char buffer2[128];
char bufferenvio[128];
char bufferreg[128];
char bufferregaux[128];
char buffername[128];
int i;


//Função para o Setup do UDPClient
void UDPClientSetup(int argc, char *argv[]){


gethostname(ip, 128);
strcpy(simbolo,str);
sprintf(port, "%d", portudp);
strcat(ip,simbolo);
strcat(ip,port);

if((fdudp=socket(AF_INET,SOCK_DGRAM,0)) == -1){
    printf("erro na criacao do socket no cliente\n");
    exit(-1);
  }
  
//Verificação de argumentos da main para o -n
  if(argc >= 3 && strcmp(argv[2], "-n") == 0){
		hostptrudp=gethostbyname(argv[3]);
  }
	else if (argc >= 5 && strcmp(argv[4], "-n") == 0){
    hostptrudp=gethostbyname(argv[5]);
  }
  else
		gethostname(bufferudp, 128);	
    hostptrudp=gethostbyname(bufferudp);
    
    
    //Verificação de argumentos da main para o -t
  if(argc >= 2 && strcmp(argv[1], "-t") == 0){
		portudp=atoi(argv[2]);
  }
	else if (argc >= 4 && strcmp(argv[3], "-t") == 0){
    portudp=atoi(argv[4]);
  }
    else if (argc >= 6 && strcmp(argv[5], "-t") == 0){
    portudp=atoi(argv[6]);
  }

    

  memset((void*)&serveraddrudp,(int)'\0',sizeof(serveraddrudp));
  serveraddrudp.sin_family=AF_INET;
  serveraddrudp.sin_addr.s_addr=((struct in_addr *)(hostptrudp->h_addr_list[0]))->s_addr;
  serveraddrudp.sin_port=htons((u_short)portudp);
  
}

//Função para o setup do TCPServer
void TCPServerSetup(int argc, char *argv[]){


    //Verificação de argumentos da main para o -p
if(argc >= 2 && strcmp(argv[1], "-p") == 0 )
	porttcp = atoi(argv[2]);


fdtcp=socket(AF_INET,SOCK_STREAM,0);

memset((void*)&serveraddrtcp,(int)'\0',
sizeof(serveraddrtcp));
serveraddrtcp.sin_family=AF_INET;
serveraddrtcp.sin_addr.s_addr=htonl(INADDR_ANY);
serveraddrtcp.sin_port=htons((u_short)porttcp);


if(bind(fdtcp,(struct sockaddr*)&serveraddrtcp,
sizeof(serveraddrtcp))== -1){
	printf("deu erro no bind\n");
	exit(-1);
	}


}

int read_line(FILE *in, char *buffer, size_t max)
{
  return fgets(buffer, max, in) == buffer;
}




int main(int argc, char *argv[]){

//inicialização do cliente e do servidor
  FILE *in;
    char line[256];
    char* linetok;
  TCPServerSetup(argc, argv);
  UDPClientSetup(argc, argv);
  gethostname(bufferreg, 128);
  auxptr=gethostbyname(bufferreg);
  a = (struct in_addr*)auxptr->h_addr_list[0];
  


  
  strcpy(bufferreg, "REG ");
  strcat(bufferreg, inet_ntoa(*a));
  strcat(bufferreg, " ");
  sprintf(bufferreg, "%s%d",bufferreg,porttcp);
  strcat(bufferreg, "\n");
  

  addrlenudp=sizeof(serveraddrudp);
  if(sendto(fdudp,bufferreg,strlen(bufferreg) ,0,(struct sockaddr*)&serveraddrudp,addrlenudp) == -1){
    printf("erro no envio de mensagem do cliente\n");
    exit(-1);
  }
  if((in = fopen("foo.txt", "rt")) == NULL)
  	printf("Erro a abrir o ficheiro de leitura\n");
  	
while(1){
  listen(fdtcp,5);

  clientlentcp=sizeof(clientaddrtcp);
  newfdtcp=accept(fdtcp,(struct sockaddr*)&clientaddrtcp,
  &clientlentcp);

  read(newfdtcp,buffertcp, sizeof(buffertcp));

  if(strncmp("REQ", buffertcp, 3)==0){
    i = 0;
    while(buffertcp[i+4]!='\n'){
      buffername[i] = buffertcp[i+4];
      buffertcp[i] = '\0';
      bufferreg[i] = '\0';
      i++;
    }
    strcpy(bufferreg, "UPD ");
    strcat(bufferreg, inet_ntoa(*a));
	strcat(bufferreg, " ");
	sprintf(bufferreg, "%s%d",bufferreg,porttcp);
    strcat(bufferreg, " ");
    strcat(bufferreg, buffername);
    strcat(bufferreg, "\n");
    
    addrlenudp=sizeof(serveraddrudp);
    if(sendto(fdudp,bufferreg,strlen(bufferreg) ,0,(struct sockaddr*)&serveraddrudp,addrlenudp) == -1){
      printf("erro no envio de mensagem do cliente\n");
      exit(-1);
    }
  }
  
  //read file


    if(!read_line(in, line, sizeof line )){
      fclose(in);
       if((in = fopen("foo.txt", "rt")) == NULL)
  		printf("Erro a abrir o ficheiro de leitura\n"); 
  		read_line(in, line, sizeof line ) ;    
      }
      
  linetok = strtok(line, "\n");
  strcpy(bufferenvio, line);
  strcat(bufferenvio, buffername);
  strcat(bufferenvio, "\n");

  write(newfdtcp,bufferenvio,strlen(bufferenvio)+1);


  close(newfdtcp);

  if(sendto(fdudp,ip,sizeof(ip) ,0,(struct sockaddr*)&serveraddrudp,addrlenudp) == -1){
    printf("erro no envio de mensagem do cliente\n");
    exit(-1);
  }
    
  addrlenudp=sizeof(serveraddrudp);
    
  if(recvfrom(fdudp,bufferudp,sizeof(bufferudp),0, (struct sockaddr*)&clientaddrudp, &addrlenudp) == -1){
    printf("erro na recepção de mensagem do cliente\n");
    exit(-1);
  
}    

i=0;
while(i<128){
buffername[i]= '\0';
i++;
}
}
    
  close(fdudp);

}

