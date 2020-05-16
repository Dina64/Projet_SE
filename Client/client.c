#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#define CRLF "\r\n"
#define TAILLE_TAMPON 1000

void abandon(char message[])
{
	perror(message);
	exit(EXIT_FAILURE);
}

/* -- connexion vers un serveur TCP --- */
int ouvrir_connexion_tcp(char nom_serveur[], int port_serveur)
{
	struct sockaddr_in addr_serveur;
    struct hostent *serveur;
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0); /* création prise */
    if (fd < 0)
    	abandon("socket");
    serveur = gethostbyname(nom_serveur); /* recherche adresse serveur */
    if (serveur == NULL)
    	abandon("gethostbyname");
    addr_serveur.sin_family = AF_INET;
    addr_serveur.sin_port = htons(port_serveur);
    addr_serveur.sin_addr = *(struct in_addr *) serveur->h_addr;
    if (connect(fd,(struct sockaddr *) &addr_serveur,sizeof addr_serveur)< 0)  //connexion au serveur
        abandon("connect");
    return (fd);
}

void demander_document(int fd, char adresse_document[])
{
	char requete[TAILLE_TAMPON];
    int longueur;
    /* constitution de la requête, suivie d'une ligne vide */
    longueur = snprintf(requete, TAILLE_TAMPON,"GET %s HTTP/1.0" CRLF CRLF,adresse_document);
    write(fd, requete, longueur);       /* envoi */
}
void afficher_reponse(int fd)
{
    char tampon[TAILLE_TAMPON];
    int longueur;
    while (1) 
    {
        longueur = read(fd, tampon, TAILLE_TAMPON);     /* lecture par bloc */
        if (longueur <= 0)
        	break;
        write(1, tampon, longueur);     /* copie sur sortie standard */
    }
}
      
int main(int argc, char *argv[])
{
    char *nom_serveur, *adresse_document;
    int port_serveur;
    int fd;
    if (argc != 4)
    {
        printf("Usage: %s serveur port adresse-document\n", argv[0]);
        abandon("nombre de paramètres incorrect");
    }
    nom_serveur = argv[1];
    port_serveur = atoi(argv[2]);
    adresse_document = argv[3];
    fd = ouvrir_connexion_tcp(nom_serveur, port_serveur);
    demander_document(fd, adresse_document);
    afficher_reponse(fd);
    close(fd);
    return EXIT_SUCCESS;
}