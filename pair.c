/*********************************************************************
 *                                                                   *
 * FICHIER: Pair                                               		 *
 *                                                                   *
 * DESCRIPTION: Utilisation de UDP/TCP socket par une application client *
 * pour realiser les fonctionnalités de publication, de recherche    *
 * et d'echange														 *
 *                                                                   *
 * principaux appels (du point de vue client)                        *
 *     socket()                                                      *
 *                                                                   * 
 *     sendto()                                                      *
 *                                                                   *
 *     recvfrom()                                                    *
 *                                                                   *
 *********************************************************************/
#include     <netinet/in.h>
#include     <arpa/inet.h>
#include     <netdb.h>
#include 	 <unistd.h>
#include     <stdlib.h>
#include <stdio.h> 
#include <sys/time.h>

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include  <fcntl.h>

#include <string.h>

#include "rsa.h"

#define MAXLINE 80
#define LENGTH 512

void usage(){
	printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur (PUBLISH|SEARCH)\n");
}

void usagePublish(){
	printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur PUBLISH file keyWordList\n");
}

void usageSearch(){
	printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur SEARCH keyWord\n");
}


/*
 * Main
 */
int main (int argc, char *argv[])
{

	int serverSocket, n;
	struct sockaddr_in  serv_addr;
	/*struct sockaddr_un  unixAddr;*/
	char *request;
	char *type;
	char *word;
	char sendbuf[1500];
	struct hostent *hp;  
	int ttl;	
	socklen_t len=sizeof(serv_addr);

	struct file f;

	/* Verifier le nombre de paramètre en entrée */
	/* clientUDP <hostname> <numero_port>        */
	if (argc <= 3){
		usage();
		exit(1);
	}

	/*
	 * Test du type de requete
	 * 
	 */
	type = argv[3];

	/*Si PUBLISH*/
	if(strcmp(type, "PUBLISH")==0){
		if(argc < 6){
			usagePublish();
			exit(1);
		}
		else{
			if( access( argv[4], F_OK ) != -1 ) {

				strcpy(f.hash , hashSha1(argv[4]));
				strcpy(f.name , argv[4]);
				strtok(argv[4], ".");
				strcpy(f.type, strtok(NULL, "."));
				
				int i;
				for(i = 5; i < argc; i++)
				{
					strcat(f.keywords, argv[i]);
					strcat(f.keywords, "/");
				}
				
				/*strcpy(f.keywords, "Test");*/

				request = "PUBLISH";
			}
			else{
				printf("This file doesn't exist \n");
				exit(1);
			}
		}
	}
	/*Si SEARCH*/
	else if(strcmp(type, "SEARCH")==0){
		if(argc < 5){
			usageSearch();
			exit(1);
		}
		else{
			word = argv[4];
			request = malloc(strlen(word) + strlen("SEARCH|") + 1);
			strcpy(request, "SEARCH|");
			strcat(request, word);
		}
	}


	/* 
	* Remplir la structure  serv_addr avec l'adresse du serveur 
	*/
	memset( (char *) &serv_addr,0, sizeof(serv_addr) );
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
  
	hp = (struct hostent *)gethostbyname (argv[1]);
	if (hp == NULL) {
		fprintf(stderr, "%s: %s non trouve dans in /etc/hosts ou dans le DNS\n", argv[0], argv[1]);
		exit(1);
	}
	/*  bcopy( (char *) hp->h_addr,  (char *)& serv_addr.sin_addr, hp->h_length);*/
	/*memcpy( & serv_addr.sin_addr ,  hp->h_addr,  hp->h_length);*/
	/*printf ("IP address: %s\n", inet_ntoa (serv_addr.sin_addr));*/
 
   
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

	/*
	 * Envoie message
	 */
	if(strcmp(type, "PUBLISH")==0){
			
		printf("[Message] Envoie : %s \n", request);
		if ( (n= sendto (serverSocket, request, strlen(request),0, 
			(struct sockaddr *)&serv_addr, sizeof(serv_addr)
				  )) != strlen(request))  {
				perror ("erreur sendto");
				exit (1);
		}
		
		if ( (n= sendto (serverSocket, &f, sizeof(struct file),0, 
		  (struct sockaddr *)&serv_addr, len
			  )) != sizeof(f))  {
				perror ("erreur sendto \n");
				exit (1);
		}
	}
	else if(strcmp(type, "SEARCH")==0){
		printf("[Message] Envoie : %s \n", request);
		if ( (n= sendto (serverSocket, request, strlen(request),0, 
			(struct sockaddr *)&serv_addr, sizeof(serv_addr)
				  )) != strlen(request))  {
				perror ("erreur sendto");
				exit (1);
		}
	}
	
	/*
	 * Reception message
	 */
	if ( (n= recvfrom (serverSocket, sendbuf, sizeof(sendbuf)-1,0, 
		(struct sockaddr *)&serv_addr, &len)) < 0 )  {
			printf ("erreur recvfrom : %s\n", sendbuf);
			exit (1);
	}

	sendbuf[n]='\0';

	if(strcmp(sendbuf, "PUBLISH_ACK")==0){
		printf("[Message] Le fichier a bien été publié : %s \n", sendbuf);
	}
	else if(strcmp(sendbuf, "SEARCH_RESP")==0){
		printf("[Message] Résultat de la recherche : %s \n", sendbuf);
		struct reponse tab_struct[50];

		/* Reception des structures correspondant a la recherche */
		if ( (n= recvfrom (serverSocket, tab_struct, sizeof(tab_struct)-1,0,
			(struct sockaddr *)&serv_addr, &len)) < 0 )  {
				printf ("erreur recvfrom \n");
				exit (1);
		}
		
		/* 
		 * Envoyer requete vers serveur
		 * 	Si il a le fichier -> le recevoir
		 *  Sinon dire qu il ne l a pas 
		 * 			chercher sur un autre 
		 * */
		 int i = 0;
		 int res = -1;
		 while( (strcmp(tab_struct[i].hash, "") != 0) && res != 0)
		 {
			printf("[Message] Tentative de récupération du fichier sur l hote n° %i \n", i);
			res = recuperationFichier( tab_struct[i].ip, tab_struct[i]);
			
			if(res == 2)
			{
				printf("[Message] Echec de la récupération. Fichier introuvable \n");
			}
			
			i++;
		 }
		 
		 if(res == -1)
		 {
			 printf("[Message] Aucun fichier correspondant à la recherche. \n");
		 }
		 
		
	}
	else{
		printf("[Message] Erreur lors de la requête: %s \n", sendbuf);
	}
 
 
	close(serverSocket);

	return 0;
 }
 
 
/*
 * Methode permettant 
 * */
int recuperationFichier(char * ip, struct reponse rep)
{

	int serverSocket;
	struct sockaddr_in  serv_addr; 
	char fileExiste[5];
	char buffer[LENGTH];
	int value = 1;
	  
  
	/* Ouvrir socket (socket STREAM)
	*/
	if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) <0) {
		perror ("[ERROR] Failed to obtain Socket Descriptor \n");
		return 1;
	}

	memset ( (char *) &serv_addr, 0, sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(2223);
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	
	/* Connect to the serveur 
	*/
	if (connect (serverSocket, (struct sockaddr *) &serv_addr,  
		sizeof(serv_addr) ) < 0){
			perror ("[ERROR] Failed to connect to the host \n");
			return 1;
	}
	else{
		printf("[Message] Connected to server\n");
	}
	
	int on=1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	
	write(serverSocket, rep.name, sizeof(rep.name));
	
	int i = read(serverSocket, fileExiste, sizeof(fileExiste));
	fileExiste[i] = '\0';
	
	if(strcmp(fileExiste, "OK") == 0)
	{
		printf("[Message] Fichier trouvé \n");
		printf("[Message] Début transmission \n");
		
		FILE *f = fopen ( rep.name, "wb" );
		if(f != NULL)
		{
			int tab[256];
			
			while(read(serverSocket, tab, sizeof(tab))!= NULL)
			{
				write(f, tab, sizeof(tab));
			}
			fclose(f);
			printf("[Message] Fin transmission \n");
		}
		
		printf("[Message] Verification du contenu \n");
		
		if(strcmp(hashSha1(rep.name), rep.hash) == 0)
		{
			printf("[Message] Contenu identique \n");
			
		}
		else{
			printf("[Message] Contenu différent : Suppression du fichier téléchargé \n");
			/*remove(rep.name);*/
			return 3;
		}
	}
	else
	{
		printf("[Message] Fichier non exisant \n");
		close(serverSocket);
		return 2;
	}
	
	close (serverSocket);
    printf("[Message] Connection closed.\n");
    return 0;

}

 
