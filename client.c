//gcc -ansi -std=c99 -pedantic -Wall client.c -o cliente
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>


#define LONGDADES	100
#define h_addr h_addr_list[0]

#define SUBS_REQ 0x00;
#define SUBS_ACK 0x01;
#define SUBS_INFO 0x02;
#define INFO_ACK 0x03;
#define SUBS_NACK 0x04;
#define SUBS_REJ 0x05;

#define DISCONNECTED 0xa0;
#define NOT_SUBSCRIBED 0xa1;
#define WAIT_ACK_SUBS 0xa2;
#define WAIT_ACK_INFO 0xa3;
#define SUBSCRIBED 0xa4;
#define SEND_HELLO 0xa5;
#define WAIT_INFO 0xa6;

void tokenizer();
void socksere();
void creacioSock();
void bindProgram();
void creacio_SUBS_REQ();
int sendtoProgram();
int recvfromProgram();
void hora();
void enviarsubsinfo();
void creacio_SUBS_INFO();
int sento_info();
int recvfrom_info();
void bindProgram_info();

char estat[100] = "NOT_SUBSCRIBED";

struct client {
    char Name[100];
    char Situation[100];
    char Elements[100];
    char MAC[100];
    char Local[100];
    char Server[100];
    char Srv_UDP[100];
}client;

struct paquet {
    unsigned char tipus;
    char adreca_mac[13];
    char numero_aleatori[9];
    char dades[80];
}paquet;

struct paquet_inf0 {
    unsigned char tipus;
    char adreca_mac[13];
    char numero_aleatori[9];
    char dades[80];
}paquet_info;



int sendbe;
int recvbe;
struct timeval timetemporitzacio;
int temporitzador;
fd_set fds;
int t=1;
int u=3;
int n=8;
int o=3;
int p=3;
int q=4;




socklen_t fromlen;

struct hostent *ent;
int 			sock,port,laddr_cli,a,nprimers,i=0;
struct sockaddr_in	addr_server,addr_cli;
char			dadcli[LONGDADES];
time_t			temps;

 
void hora(){
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
    char output[128];
    strftime(output,128,"%H:%M:%S",tlocal);
    printf("%s\n",output);
}

void tokenizer(char tmp[100]){
    char* token;
    token = strtok(tmp," ");
    token = strtok(NULL," ");
    token = strtok(NULL," ");
    strcpy(tmp,token);
    token = strtok(tmp,"\n");
}



void socksere(){
	creacioSock();
	bindProgram();
	creacio_SUBS_REQ();
	sendbe=sendtoProgram();
	

	// recvbe=recvfromProgram();
}



void creacioSock(){

    sock=socket(AF_INET,SOCK_DGRAM,0);
	if(sock<0){	
		fprintf(stderr,"No puc obrir socket!!!\n");
		exit(-1);
	}
}

void bindProgram(){

	/* Addre�a del bind del client */
	memset(&addr_cli,0,sizeof (struct sockaddr_in));
	addr_cli.sin_family=AF_INET;
	addr_cli.sin_addr.s_addr=htonl(INADDR_ANY);
	addr_cli.sin_port=htons(0);

	if(bind(sock,(struct sockaddr *)&addr_cli,sizeof(struct sockaddr_in))<0){	
		fprintf(stderr,"No puc fer el binding del socket!!!\n");
            exit(-2);
	}

    ent=gethostbyname("127.0.0.1");
	/* Adre�a del servidor */
	memset(&addr_server,0,sizeof (struct sockaddr_in));
	addr_server.sin_family=AF_INET;
	addr_server.sin_addr.s_addr=(((struct in_addr *)ent->h_addr)->s_addr);
	addr_server.sin_port=htons(2345);
}


void bindProgram_info(){

	/* Addre�a del bind del client */
	memset(&addr_cli,0,sizeof (struct sockaddr_in));
	addr_cli.sin_family=AF_INET;
	addr_cli.sin_addr.s_addr=htonl(INADDR_ANY);
	addr_cli.sin_port=htons(0);

	if(bind(sock,(struct sockaddr *)&addr_cli,sizeof(struct sockaddr_in))<0)
	{
		fprintf(stderr,"No puc fer el binding del socket!!!\n");
            exit(-2);
	}

    ent=gethostbyname("127.0.0.1");
	/* Adre�a del servidor */
	memset(&addr_server,0,sizeof (struct sockaddr_in));
	addr_server.sin_family=AF_INET;
	addr_server.sin_addr.s_addr=(((struct in_addr *)ent->h_addr)->s_addr);
	addr_server.sin_port=htons(atoi(paquet.dades));
}



void creacio_SUBS_REQ(){  
    char temp[100];
	paquet.tipus=SUBS_REQ;
	strcpy(paquet.adreca_mac,client.MAC);
	strcpy(paquet.numero_aleatori,"00000000");
	strcpy(temp,strcat(client.Name,","));
	strcpy(paquet.dades,strcat(temp,client.Situation));
}


void creacio_SUBS_INFO(){ 
    char temp[100];
    paquet_info.tipus=SUBS_INFO
    strcpy(paquet_info.adreca_mac,client.MAC);
    strcpy(paquet_info.numero_aleatori,paquet.numero_aleatori);
    strcpy(temp,strcat(client.Local,","));
    strcpy(paquet_info.dades,strcat(temp,client.Elements));
}


int sendtoProgram(){
	int abc=sendto(sock,&paquet,sizeof(paquet),0,(struct sockaddr*)&addr_server,sizeof(addr_server));
    if(abc<0){       
        fprintf(stderr,"Error al sendto\n");
        // perror(argv[0]);
        exit(-2);
    }


	FD_ZERO(&fds);
	FD_SET(sock, &fds);
	timetemporitzacio.tv_sec=temporitzador;

	if(abc>0){
		strcpy(estat,"WAIT_ACK_SUBS");
	}

	int elselect = select(sock+1, &fds, NULL, NULL, &timetemporitzacio);
	          
	if(elselect){
		if(FD_ISSET(sock,&fds)){
				
	 /* Rebem la confirmaci� per part del server, amb el nombre de primers
	    ens enviara realment (hi ha un maxim) */
       		int cba=recvfrom(sock,&paquet,sizeof(paquet),0,(struct sockaddr *)&addr_cli,&fromlen);
       	       // printf("%u\n",paquet.tipus);
		    if(cba<0){
           		fprintf(stderr,"Error al recvfrom\n");
            // perror(argv[0]);
            	exit(-2);
        	}
        return cba;
        }
    }
   	return 0;//si falla, estaria fora del if del elselect
}


void enviarsubsinfo(){
	//printf("tipus %u\n", paquet.tipus);
	//printf("mac %s\n", paquet.adreca_mac);
	//printf("random %s\n", paquet.numero_aleatori);
	//printf("dades %s\n", paquet.dades);

	creacioSock();
	bindProgram_info();
	creacio_SUBS_INFO();

	printf("tipus %u\n", paquet_info.tipus);
	printf("mac %s\n", paquet_info.adreca_mac);
	printf("random %s\n", paquet_info.numero_aleatori);
	printf("dades %s\n", paquet_info.dades);
	sento_info();
	recvfrom_info();
	printf("%u\n", paquet_info.tipus);
	printf("%s\n", estat);
 	if(paquet_info.tipus==3 && (strcmp(estat,"WAIT_ACK_INFO"))==0 ){
 		printf("holahola\n");	
	}
}

int sento_info(){
	int abc=sendto(sock,&paquet_info,sizeof(paquet_info),0,(struct sockaddr*)&addr_server,sizeof(addr_server));
        if(abc<0){
            fprintf(stderr,"Error al sendto\n");
            // perror(argv[0]);
            exit(-2);
        }

	if(abc>0){
		strcpy(estat,"WAIT_ACK_INFO");
	}
	return abc;
}


int recvfrom_info(){
    int cba=recvfrom(sock,&paquet_info,sizeof(paquet_info),0,(struct sockaddr *)&addr_cli,&fromlen);
       	       // printf("%u\n",paquet.tipus);
	if(cba<0){
            fprintf(stderr,"Error al recvfrom\n");
            // perror(argv[0]);
            exit(-2);
	}
	return cba;
	exit(0);
}




int main(int argc,char *argv[]){



    FILE* file;
    file = fopen("client.cfg","r");
  
    fgets(client.Name,100,file);
    tokenizer(client.Name);
    //printf("%s\n",client.Name);

    fgets(client.Situation,100,file);
    tokenizer(client.Situation);
    //printf("%s\n",client.Situation);

    fgets(client.Elements,100,file);
    tokenizer(client.Elements);
    //printf("%s\n",client.Elements);

    fgets(client.MAC,100,file);
    tokenizer(client.MAC);
    //printf("%s\n",client.MAC);

    fgets(client.Local,100,file);
    tokenizer(client.Local);
    //printf("%s\n",client.Local);

    fgets(client.Server,100,file);
    tokenizer(client.Server);
    //printf("%s\n",client.Server);

    fgets(client.Srv_UDP,100,file);
    tokenizer(client.Srv_UDP);
    //printf("%s\n",client.Srv_UDP);

    fclose(file);



    for(int i=0;i<o;i++){
  	    temporitzador=t;

  		//si acaba els o processos, el client dira que no sha pogut contactar amb el servidor
	
  		for(int j=0;j<n;j++){
  			hora();

  			if(j>=p) {
  	   			temporitzador = temporitzador + 1 ;
  			}

  			if(temporitzador>4){
  				temporitzador=4;
  			}


  			socksere();

  		
  			if(paquet.tipus==1 && (strcmp(estat,"WAIT_ACK_SUBS"))==0 ){
  				//printf("ha entrat\n");
  				enviarsubsinfo();
  				exit(0);
  			}

  			if(j==(n-1)){
  				printf("Comensara un nou proces de subscripcio al passar u segons\n");
  			}


  		}
	
  		if(i==(o-1)){
  		printf("no sha pogut contactar amb el servidor\n");
   		sleep(u);
   		}
   	}
  		//si la p es inferior que la n, llavors el temps es t, si la p es igual o superior a n
  		//linterval va augmentant

  		//un cop enviats els n paquets, que sespero u segons
  	







