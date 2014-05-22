#definition du compilateur
CC = gcc

# specification des options du compilateur
CFLAGS = -ansi -Wall -g

#inclusion de la glib
#GLIBFLAGS = -I /usr/include/glib-2.0 -I /usr/lib/glib-2.0/include
#LIENFLAGS = -lglib-2.0

# nom de l'executable
PROG1 = serveurCentral
PROG2 = pair
PROG3 = pairServeur

# defintion de la cible a reconstruire
all : $(PROG1) $(PROG2) $(PROG3)
 
#edition de liens et production de l'executable
$(PROG1) :  serveurCentral.o struct.o utils.o
	$(CC) $(CFLAGS) serveurCentral.o struct.o utils.o -o $(PROG1) 

$(PROG2) :  pair.o struct.o utils.o
	$(CC) $(CFLAGS) pair.o struct.o utils.o -o $(PROG2) 

$(PROG3) :  pairServeur.o struct.o utils.o
	$(CC) $(CFLAGS) pairServeur.o struct.o utils.o -o $(PROG3) 

# compilation du fichier serveurCentral
serveurCentral.o : serveurCentral.c rsa.h
	$(CC) $(CFLAGS) -c serveurCentral.c 

# compilation du fichier pair
pair.o : pair.c rsa.h
	$(CC) $(CFLAGS) -c pair.c 

# compilation du fichier pairServeur
pairServeur.o : pairServeur.c 
	$(CC) $(CFLAGS) -c pairServeur.c 

# compilation du fichier struct
struct.o : struct.c rsa.h
	$(CC) $(CFLAGS) -c struct.c 

# compilation du fichier utils
utils.o : utils.c
	$(CC) $(CFLAGS) -c utils.c 

# compilation du fichier main
#main.o : main.c Agenda.h
#	$(CC) $(CFLAGS) -c main.c  

#destruction de tous les fichiers intermediaires crees
clean :
	-rm -f *.o

# suppression de tout ce qui peut etre regenere
mrproper : clean
	-rm -f $(PROG1) $(PROG2) $(PROG3)
	
	
