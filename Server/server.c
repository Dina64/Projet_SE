#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>


void error(const char *msg)     //fonction pour les erreur
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])              //fonction principal
{
    int sockfd, newsockfd, portno;            //declaration des variavle qu'on va utiliser
    socklen_t clilen;
    char buffer[512];
    struct sockaddr_in serv_addr, cli_addr;   //creation du structure sockaddr_in pour les sin
    int n;
    if (argc < 2) //réception des paramètres de la ligne de commande
    {
        fprintf(stderr,"ERREUR, aucun port fourni\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);  //socket(domaine, type, protocole) 
    if (sockfd < 0) 
        error("ERREUR ouverture socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    //Remplissage adresse serveur
    serv_addr.sin_family = AF_INET;           //donne la famille d'adresses, qui vaut AF_INET
    serv_addr.sin_addr.s_addr = INADDR_ANY;   //pour l'adresse IP
    serv_addr.sin_port = htons(portno);       //pour le numéro de port
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) //Cette routine attache une adresse (port) à une
        error("ERREUR lors de la liaison");                                 //socket, la socket créée par la routine socket()
    listen(sockfd,5);  //cette routine donne la longueur maximale de la queue d'attente des demandes de connexions non servies (par exemple 5)
    clilen = sizeof(cli_addr);  
    newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen); //pour  dialoguer avec le client, attend les demandes de connexion
    if (newsockfd < 0) 
        error("ERREUR lors de l'acceptation");

    //Reception du fichier
    FILE *fp;
    int ch = 0;
    fp = fopen("texte.txt","a");
    int words;
    read(newsockfd, &words, sizeof(int));
    while(ch != words)
    {
        read(newsockfd , buffer , 512); 
        fprintf(fp , " %s" , buffer);   
        ch++;
    }

    //Message afficher
    printf("Le fichier a été reçu avec succès\n");
    printf("Le nouveau fichier créé est texte.txt\n");
    //Ferme et enlèvement de la liste des clients
    close(newsockfd);
    close(sockfd);
    return 0; 
}
