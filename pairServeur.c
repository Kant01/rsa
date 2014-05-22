/*********************************************************************
 *                                                                   *
 * FICHIER: PairServeur                                        		 *
 *                                                                   *
 * DESCRIPTION: Utilisation de TCP pour transferer un fichier		 *													 *
 *                                                                   *
 * principaux appels (du point de vue client)                        *
 *     socket()                                                      *
 *                                                                   * 
 *     sendto()                                                      *
 *                                                                   *
 *     recvfrom()                                                    *
 *                                                                   *
 *********************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include     <arpa/inet.h>
#include     <netdb.h>
#include 	 <unistd.h>

#include <errno.h>



#define SERV_PORT 2223

/*
 * Main
 */
int main (int argc, char *argv[])
{
	struct sockaddr_in serv_addr;
	//Socket de connexion
	int serverSocket;
	//Socket de dialogue
	int dialogSocket;
	int clilen;
	struct sockaddr_in cli_addr;
	
	/*
		Ouvrir socket (socket STREAM)
	*/
	if ( (serverSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("erreur socket \n");
		exit(1);
	}

	/*
		Lier l'adresse locale à la socket
	*/
	memset (&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_PORT);

	int on=1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	if (bind(serverSocket, (struct sockaddr *)& serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("servecho: erreur bind \n");
		exit(1);
	}

	/* 
		Parametrer le nombre de connexion pending
	*/
	if (listen(serverSocket, SOMAXCONN) < 0)
	{
		perror("servecho: erreur listen \n");
		exit(1);
	}

	/*
		Creation d'une socket qui servira au dialogue entre l'appelant et l'entité
		en attente de traitement de la connexion
	*/
	clilen = sizeof(cli_addr);


	for(;;)
	{
		dialogSocket = accept(serverSocket, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
		if( dialogSocket < 0)
		{
			perror("servecho: erreur accept \n");
			exit(1);
		}

		char buf[100];
		read(dialogSocket, buf, sizeof(buf));
		
		//Si le fichier existe
		if( access( buf, F_OK ) != -1 ) {
			
			write(dialogSocket, "OK", 2);
		
			FILE *f = fopen ( buf, "r" );
			if(f != NULL)
			{
				char line[256];
				
				while(fgets(line, sizeof(line), f) != NULL)
				{
					write(dialogSocket, line, sizeof(line));
				}
				
				close(dialogSocket);
				fclose(f);
			}
			else
			{
				printf("Pas de lecture %s \n",buf);
			}
		}
		else
		{
			write(dialogSocket, "NONE", 4);
		}
	}
	
	
	
	close(dialogSocket);
	return 0;
}
