/* 
 * File:   Agenda.h
 * Author: damien
 *
 * Created on 19 mars 2013, 17:07
 */

#ifndef RSA_H
#define	RSA_H
#include <stdio.h>
#include <string.h>

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

char** str_split(char* a_str, const char a_delim);
char * hashSha1(char * arg);

char * stringFile(struct file f, char * ip);

#endif	/* RSA_H */

