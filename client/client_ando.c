#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include<ctype.h>

void error(const char *msg)           //fonction erreur
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;          //Pour accéder à une socket sur une machine il faut: le port    
    struct sockaddr_in serv_addr;   //une adresse
    struct hostent *server;         //et le nom de la machine d'Hote 

    char buffer[512];
    if (argc < 3)                     //réception des paramètres de la ligne de commande 
    {
       fprintf(stderr,"usage %s nom du port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);                      //Initialisation du socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);    //socket(domaine, type, protocole)
    if (sockfd < 0) 
        error("Erreur d'ouverture de socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"Erreurn hote introuvable\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  //donne la famille d'adresses, qui vaut AF_INET
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);            //Longueur de l'adresse.
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)   //connexion au serveur
        error("ERrreur de connection");
  
        bzero(buffer,512);
    
    //Envoie du fichier    
    FILE *f;
    
    int words = 0;
    char c;
    f=fopen("texte.txt","r");
    while((c=getc(f))!=EOF)         
    {   
        fscanf(f , "%s" , buffer);
        if(isspace(c)||c=='\t')
        words++;    
    }       
      
    write(sockfd, &words, sizeof(int));
        rewind(f);

    char ch ;
    while(ch != EOF)
    {
        fscanf(f , "%s" , buffer);
        write(sockfd,buffer,512);
        ch = fgetc(f);
    }
    
    printf("Le fichier est envoyer avec succès\n");   //Message afficher
    close(sockfd);
    return 0;        //FIN
}
