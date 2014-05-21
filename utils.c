/*********************************************************************
 *                                                                   *
 * FICHIER: utils			                                         *
 *                                                                   *
 * DESCRIPTION: Definition des methodes utilitaires  				 *
 *																	 *
 *                                                                   *
 *********************************************************************/

#include     <stdio.h>
#include     <sys/un.h>
#include     <sys/uio.h>
#include     <stdlib.h>
#include     <assert.h>
#include	    <string.h>


/**
 * Methode permettant de split une chaine de caracter selon un caractere
 */
char** str_split(char* a_str, const char a_delim)
{
    char** result   = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}


/**
 * Methode permettant de Hasher le contenu d un fichier
 * */
char * hashSha1(char * arg)
{
	FILE *fp;
	int status;
	char path[1035];
	char * cmd;
	char * res;
	cmd = malloc(strlen(arg) + strlen("sha1sum ") + 1);
	strcpy(cmd,  "sha1sum ");
	strcat(cmd, arg);

	/* Open the command for reading. */
	fp = popen(cmd, "r");
	if (fp == NULL) {
	printf("Failed to run command\n" );
	exit;
	}

	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof(path)-1, fp) != NULL) {
		res =  path;
	}

	/* close */
	pclose(fp);
	
	res[strlen(res)-1] = '\0';
	
	return res;
}

