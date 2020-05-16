#include <unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "webCServer.h"
void arreter_serveur(int numero_signal);
void attendre_sous_serveur(int numero_signal);
int fd_serveur; /* variable globale, pour partager avec traitement signal fin_serveur */

void demarrer_serveur(int numero_port, char repertoire[])
{
	int numero_client = 0;
    int fd_client;
    struct sigaction action_int, action_chld;
    fd_serveur = serveur_tcp(numero_port);

    /* arrêt du serveur si signal SIGINT */
    action_int.sa_handler = arreter_serveur;
    sigemptyset(&action_int.sa_mask);
    action_int.sa_flags = 0;
    sigaction(SIGINT, &action_int, NULL);

    /* attente fils si SIGCHLD */
    action_chld.sa_handler = attendre_sous_serveur;
    sigemptyset(&action_chld.sa_mask);
    action_chld.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &action_chld, NULL);

    printf("> Serveur " VERSION " (port=%d, répertoire de documents=\"%s\")\n",numero_port, repertoire);
    while (1)
    {
    	struct sockaddr_in a;
        size_t l = sizeof a;
        fd_client = attendre_client(fd_serveur);

        getsockname(fd_client, (struct sockaddr *) &a, &l);
        numero_client++;
        printf("> client %d [%s]\n", numero_client,inet_ntoa(a.sin_addr));
        if (fork() == 0)
        {
        /* le processus fils ferme le socket serveur et s'occupe du client */
        	close(0);
            close(1);
            close(2);
            close(fd_serveur);
            servir_client(fd_client,repertoire);
            close(fd_client);
            exit(EXIT_SUCCESS);
        }
       /* le processus père n'a plus besoin du socket client.Il le ferme et repart dans la boucle */
        close(fd_client);
    }
}

/*Traitement des signaux*/  
void arreter_serveur(int numero_signal)
{
	printf("=> fin du serveur\n");
    shutdown(fd_serveur, 2);    /* utile ? */
    close(fd_serveur);
    exit(EXIT_SUCCESS);
}
void attendre_sous_serveur(int numero_signal)
{
/* cette fonction est appelée chaque fois qu'un signal SIGCHLD indique la fin d'un processus fils _au moins_.  */
    while (waitpid(-1, NULL, WNOHANG) > 0) 
    {
        /* attente des fils arrêtés, tant qu'il y en a */
    }
}
     /* -------------------------------------------------------------*/
void usage(char prog[])
{
    printf("Usage : %s [options\n\n", prog);
    printf("Options :" "-h\tcemessage\n" "-p port\tport du serveur [%d]\n" PORT_PAR_DEFAUT, REPERTOIRE_PAR_DEFAUT);
}
     /* -------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    int port = PORT_PAR_DEFAUT;
    char *repertoire = REPERTOIRE_PAR_DEFAUT;   /* la racine des documents */
    char c;
    while ((c = getopt(argc, argv, "hp:d:")) != -1)
    {
    	switch(c)
    	{
    		case 'h':
                usage(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'd':
                repertoire = optarg;
                break;
            case '?':
                fprintf(stderr, "Option inconnue -%c. -h pour aide.\n",optopt);
                break;
    	}
    }
    demarrer_serveur(port, repertoire);
    exit(EXIT_SUCCESS);
}