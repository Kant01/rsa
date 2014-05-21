/*********************************************************************
 *                                                                   *
 * FICHIER: Serveur Central                                          *
 *                                                                   *
 * DESCRIPTION: Stock les métadonnées des fichiers à échanger		 *
 *	Met en relation les pairs										 *
 *                                                                   *
 * principaux appels (du point de vue client)                        *
 *     socket()                                                      *
 *                                                                   * 
 *     sendto()                                                      *
 *                                                                   *
 *     recvfrom()                                                    *
 *                                                                   *
 *********************************************************************/
#include     <stdio.h>
#include     <netinet/in.h>
#include     <sys/un.h>
#include     <arpa/inet.h>
#include     <sys/uio.h>
#include     <netdb.h>

#include "struct.c"
#include "utils.c"

#define SERV_PORT 2222


/**
 * Main du programme
 * */
int main (int argc, char *argv[])
{

	int serverSocket, unixSocket,  n, retread;
	struct sockaddr_in  serv_addr;
	struct sockaddr_un  unixAddr;

	char data[1500];
	char * ack;
	char * search;
	char * error;
	struct hostent *hp;  
	int ttl;	
	socklen_t len=sizeof(serv_addr);
	struct file f;

	FILE * fil;
  
   
	/*
	* Ouvrir socket UDP
	*/
	if ((serverSocket = socket(PF_INET, SOCK_DGRAM, 0)) <0) {
		perror ("erreur socket");
		exit (1);
	}

	ttl=1;
	if (setsockopt(serverSocket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) <0){
		perror ("setsockopt");
		exit (1);
	}

	memset (&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_PORT);

	if (bind(serverSocket, (struct sockaddr *)& serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("servecho: erreur bind \n");
		exit(1);
	}

	ack = "PUBLISH_ACK";
	search = "SEARCH_RESP";
	error = "ERROR";

	for(;;)
	{
		if ( (n= recvfrom (serverSocket, data, sizeof(data)-1,0, 
		(struct sockaddr *)&serv_addr, &len)) <0)  {
			printf ("erreur recvfrom");
			exit (1);
		}
		
		data[n]='\0';
		
		char ** tokens = str_split(data, '|');
	 
		//Si on recoit un PUBLISH
		if(strcmp((*tokens+0), "PUBLISH")==0)
		{
			//Reception Struct
			if ( (n= recvfrom (serverSocket, &f, sizeof(struct file),0, 
			(struct sockaddr *)&serv_addr, &len)) <0)  {
				printf ("erreur recvfrom");
				exit (1);
			}
			
			//Stockage des metadonnees des fichiers
			fil = fopen("save.txt","a+");
			fprintf(fil,"%s",stringFile(f, inet_ntoa(serv_addr.sin_addr)));
			fclose(fil);
			
			//Envoi PUBLISH_ACK
			if ( (n= sendto (serverSocket, ack, strlen(ack),0, 
			(struct sockaddr *)&serv_addr, sizeof(serv_addr)
				  )) != strlen(ack))  {
				perror ("erreur sendto");
				exit (1);
			}
		}
		//Si on recoit un SEARCH
		else if(strcmp((*tokens+0), "SEARCH")==0){
			printf("%s\n", (*tokens+2));
			//Envoi SEARCH_RESP
			if ( (n= sendto (serverSocket, search, strlen(search),0, 
			(struct sockaddr *)&serv_addr, sizeof(serv_addr)
				  )) != strlen(search))  {
				perror ("erreur sendto");
				exit (1);
			}
		}
		//Sinon on envoie un message d erreur
		else{
			if ( (n= sendto (serverSocket, error, strlen(error),0, 
			(struct sockaddr *)&serv_addr, sizeof(serv_addr)
				  )) != strlen(error))  {
				perror ("erreur sendto");
				exit (1);
			}
		}
	}

	close(serverSocket);
}
