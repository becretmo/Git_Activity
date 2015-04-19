#include "defobj.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <strings.h>
#include <stdio.h>

int sd, sds;
struct sockaddr_in addr_srv;
int addr_srv_l;
struct sigaction sig;

void reception(int socket){
	obj echoBuffer;
	int recvMsgSize;

	printf("Fils %d, traitement d'un message ...\n", getpid());
	if((recvMsgSize = recv(socket, &echoBuffer, sizeof(obj), 0)) < 0) {
		perror("erreur recv");
		exit(0);
	} else {	
		while(echoBuffer.ii !=-1) {
			printf("Message reçu : %s %s %d\n", echoBuffer.str1, echoBuffer.str2, echoBuffer.ii);	
			if((recvMsgSize = recv(socket, &echoBuffer, sizeof(obj), 0)) < 0) { // réception en mode connecté sur socket et stockage à l'adresse echobuffer pour une taille max de obj
				perror("erreur recv");
			}
		}
	}
	sleep(3);
	close(socket);
}

void handler1() {
	int status;
	pid_t term_pid;
	term_pid = waitpid(-1, &status, 0); 
	if(WIFEXITED (status)) {
		printf("Le fils %d se termine.\n",term_pid);
	} else {
		printf("Erreur de terminaison d'un processus fils");
	}
	sleep(1);
}

int main(int argc, char* argv[]){
	sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	sig.sa_handler = handler1;
	sigaction(SIGCHLD, &sig, 0);
	if(sd != -1){
		addr_srv_l = sizeof(addr_srv);
		bzero(&addr_srv, addr_srv_l);
		addr_srv.sin_family = AF_INET;
		addr_srv.sin_port = htons(atoi(argv[1]));
		addr_srv.sin_addr.s_addr = htonl(INADDR_ANY);
		bind(sd, (const struct sockaddr*) &addr_srv, addr_srv_l);
		listen(sd, SOMAXCONN);

		while(1){
			printf("Attente d'une connexion\n");
			if((sds = accept(sd, (struct sockaddr*)0, 0 ))> 0){
				printf("Fork !\n");
				int pid = fork();
				if(pid == -1){
					perror("erreur fork\n");
				} else if( pid > 0) {
					printf("Je suis le père !\n");
				} else {
					printf("Je suis le fils ! PID = %d\n",getpid());
					reception(sds);
					return 0;
				}
			}
		}
	} else {
		perror("erreur socket");
	}
	return 0;
}