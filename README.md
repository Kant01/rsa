FLAMENT Damien & TONNELIER Quentin
2A IL
RSA Project

Vous devez commencer par ouvrir 3 terminaux:
	
		- Dans le 1er, lancez le serveur central en tapant ./serveurCentral
		- Dans le 2ème, lancez le pair "serveur" en tapant ./pairServeur
		- Dans le 3ème, lancez le pair "client":
				- Si vous souhaitez publier, tapez la commande:
						 ./pair @ IP_source n°port PUBLISH nomDuFichier motClé
				- Si vous souhaitez effectuer une recherche, tapez la commande:
						 ./pair @IP_source n°port SEARCH motClé

ATTENTION : Veillez à ce qu 'il existe un fichier "save.txt" à coté du fichier "pairServeur"

ATTENTION : La fonction pour comparer le hashage du fichier que l'on vient de télécharger ne fonctionne pas. Même si il est affiché que le fichier n'est pas identique, c'est faux.
Le fichier a BIEN été téléchargé. (décomenter la ligne 335 de "pair.c" pour vraiment supprimé)

