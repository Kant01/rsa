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
#include     <unistd.h>
#include     <netinet/in.h>
#include     <sys/un.h>
#include     <arpa/inet.h>
#include     <sys/uio.h>
#include     <netdb.h>
#include     <stdlib.h>

#include "rsa.h"
#define SERV_PORT 2222


/**
 * Main du programme
 * */
int main (int argc, char *argv[])
{

	int serverSocket,  n;
	struct sockaddr_in  serv_addr;

	char data[1500],buf[1500];
	char * ack;
	char * search;
	char * error; 
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
		strcpy(buf,data);
		char ** tokens = str_split(buf, '|');
	 
		/*Si on recoit un publish*/
		if(strcmp((*tokens+0), "PUBLISH")==0)
		{
			/*Reception Struct*/
			if ( (n= recvfrom (serverSocket, &f, sizeof(struct file),0, 
			(struct sockaddr *)&serv_addr, &len)) <0)  {
				printf ("erreur recvfrom");
				exit (1);
			}
			
			/*Stockage des metadonnees des fichiers*/
			fil = fopen("save.txt","a+");
			fprintf(fil,"%s",stringFile(f, inet_ntoa(serv_addr.sin_addr)));
			fclose(fil);
			
			/*Envoi PUBLISH_ACK*/
			if ( (n= sendto (serverSocket, ack, strlen(ack),0, 
			(struct sockaddr *)&serv_addr, sizeof(serv_addr)
				  )) != strlen(ack))  {
				perror ("erreur sendto");
				exit (1);
			}
		}
		/*Si on recoit un SEARCH*/
		else if(strcmp((*tokens+0), "SEARCH")==0){
			char *part = (char*)malloc((strlen(data))*sizeof(char));
			part = strtok(data,"|");
			/*printf("test=%s\n",part);*/
			part = strtok(NULL,"|");
			/*printf("test=%s\n",part);*/


			/*lire le fichier save.txt
			 si mot cle alors prendre la ligne
			Envoi SEARCH_RESP*/
			FILE *f;
			int nbligne;
			system("wc -l save.txt > tmp" );
			f = fopen("tmp","rw" );
			fscanf(f,"%d",&nbligne);
			struct reponse tab_struct[nbligne];

			/*lire le fichier save.txt
			 si mot cle alors prendre la ligne*/
			fil = fopen("save.txt", "r");
			if(fil!=NULL)
			{
				char * line = NULL;
				size_t len = 0;
				ssize_t read;			
				int i=0;
				while((read = getline(&line, &len, fil)) != -1)
				{
					
					char * nom = malloc(sizeof(line));
					char * tp = malloc(sizeof(line));
					char * words = malloc(sizeof(line));
					char * hash = malloc(sizeof(line));
					char * ip = malloc(sizeof(line));
					
					if(strstr(line, part) != NULL)
					{						
						nom = strtok(line, "|");
						tp = strtok(NULL, "|");
						hash = strtok(NULL, "|");
						words = strtok(NULL, "|");
						ip = strtok(NULL, "|");
						
						strcpy(tab_struct[i].ip, ip);
						strcpy(tab_struct[i].name, nom);
						strcpy(tab_struct[i].type, tp);
						strcpy(tab_struct[i].hash, hash);
						strcpy(tab_struct[i].keywords, words);

						i++;
					  }
					  strcpy(tab_struct[i].ip, "");
					  strcpy(tab_struct[i].name, "");
					  strcpy(tab_struct[i].type , "");
					  strcpy(tab_struct[i].hash , "");
					  strcpy(tab_struct[i].keywords , "");
				   }				
				   fclose(fil);
			    }
			    else{
					if ( (n= sendto (serverSocket, error, strlen(error),0, 
					(struct sockaddr *)&serv_addr, sizeof(serv_addr))) != strlen(error))  {
					perror ("erreur sendto");
					exit (1);
				}
			    }
			
			/*Envoi SEARCH_RESP*/
			if ( (n= sendto (serverSocket, search, strlen(search),0, 
			(struct sockaddr *)&serv_addr, sizeof(serv_addr)
				  )) != strlen(search))  {
				perror ("erreur sendto");
				exit (1);
			}
			
			/*Envoi tableau de struct*/
			if ( (n= sendto (serverSocket, &tab_struct, sizeof(tab_struct),0, 
				(struct sockaddr *)&serv_addr, len
			  )) != sizeof(tab_struct))  {
				perror ("erreur sendto \n");
				exit (1);
			}
			
		}
		
		/*Sinon on envoie un message d erreur*/
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
