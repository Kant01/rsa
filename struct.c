/*********************************************************************
 *                                                                   *
 * FICHIER: struct			                                         *
 *                                                                   *
 * DESCRIPTION: Mise en place des structures a utiliser				 *
 *																	 *
 *                                                                   *
 *********************************************************************/

#include     <stdlib.h>
#include	 <string.h>

/**
 * Structure d'un fichier qui peut etre envoye
 * 
 * Possede : 
 * 		un nom
 * 		un type
 * 		une liste de mot cles
 * 		le hash du contenu
 * */
#pragma pack(1)
struct file {
	
	char  name[255];
	
	char type[255];
	
	char keywords[255];
	
	char hash[255];
	
};
#pragma pack(0) 

/**
 * Methode permettant de retourner une structure File sous forme d une 
 * chaine de caractere.
 * 
 */
char * stringFile(struct file f, char * ip)
{
	char * res;
	char * inter = "|";
	char * retourChariot = "\n";
	res = malloc(sizeof(f.hash) +sizeof(inter)*4+ sizeof(f.keywords)+ sizeof(f.name)+ sizeof(f.type) + sizeof(ip)+ sizeof('\n')+ 1);
	strcpy(res, f.name);
	strcat(res, inter);
	strcat(res, f.type);
	strcat(res, inter);
	strcat(res, f.hash);
	strcat(res, inter);
	strcat(res, f.keywords);
	strcat(res, inter);
	strcat(res, ip);
	strcat(res, retourChariot);
	
	return res;
}
