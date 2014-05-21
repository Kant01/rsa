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

#include "struct.c"
#include "utils.c"


#define MAXLINE 80
usage(){
	printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur (PUBLISH|SEARCH)\n");
}

usagePublish(){
	printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur PUBLISH file \n");
}

usageSearch(){
	printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur SEARCH keyWord\n");
}


/*
 * Main
 */
int main (int argc, char *argv[])
{

	int serverSocket, unixSocket,  n, retread;
	struct sockaddr_in  serv_addr;
	struct sockaddr_un  unixAddr;
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

	//Si PUBLISH
	if(strcmp(type, "PUBLISH")==0){
		if(argc < 5){
			usagePublish();
			exit(1);
		}
		else{
			if( access( argv[4], F_OK ) != -1 ) {

				strcpy(f.hash , hashSha1(argv[4]));
				strcpy(f.name , argv[4]);
				strtok(argv[4], ".");
				strcpy(f.type, strtok(NULL, "."));
				strcpy(f.keywords, "Test");

				request = "PUBLISH";
			}
			else{
				printf("This file doesn't exist \n");
				exit(1);
			}
		}
	}
	//Si SEARCH
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
	//  bcopy( (char *) hp->h_addr,  (char *)& serv_addr.sin_addr, hp->h_length);
	memcpy( & serv_addr.sin_addr ,  hp->h_addr,  hp->h_length);
	//printf ("IP address: %s\n", inet_ntoa (serv_addr.sin_addr));
 
   
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
			
		printf("Message envoye : %s \n", request);
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
		printf("Message envoye : %s \n", request);
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
		printf("Le fichier a bien été publié : %s\n", sendbuf);
	}
	else if(strcmp(sendbuf, "SEARCH_RESP")==0){
		printf("Résultat de la recherche : %s\n", sendbuf);
	}
	else{
		printf("Erreur lors de la requête: %s \n", sendbuf);
	}
 
 
 close(serverSocket);
 }
