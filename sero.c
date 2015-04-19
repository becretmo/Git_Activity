#include "defobj.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <strings.h>
#include <stdio.h>

int sd,sds;
struct sockaddr_in addr_srv;
int addr_srv_l;

void reception(int socket){
	printf("le serveur reçoit le message ... \n");
	obj echoBuffer;
	int recvMsgSize,i=0;
	if((recvMsgSize = recv(socket, &echoBuffer, sizeof(obj), 0)) < 0) { // réception en mode connecté sur socket et stockage à l'adresse echobuffer pour une taille max de obj
		perror("erreur recv");
		exit(0);
	} else {
		while(echoBuffer.ii !=-1) {
			printf("réception d'un message du client \n");
			printf("%s %s %d\n", echoBuffer.str1, echoBuffer.str2, echoBuffer.ii);
			if((recvMsgSize = recv(socket, &echoBuffer, sizeof(obj), 0)) < 0) { // réception en mode connecté sur socket et stockage à l'adresse echobuffer pour une taille max de obj
				perror("erreur recv");
			}
		}
	}
	close(socket); // fermeture du socket
}

int main(int argc, char* argv[]) {
	sd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP); // création d'un socket
	if(sd != -1) {
		addr_srv_l=sizeof(addr_srv);
		bzero(&addr_srv,addr_srv_l);
		addr_srv.sin_port=htons(atoi(argv[1]));  // numéro de port
		addr_srv.sin_addr.s_addr=htonl(INADDR_ANY); // adresse IP 
		addr_srv.sin_family=AF_INET; // famille
		printf("bind");
		bind(sd,(const struct sockaddr *) &addr_srv,addr_srv_l); //liaison du socket avec la structure addr_srv
		listen(sd,SOMAXCONN); // le serveur écoute pour savoir si quelqu'un essaie de se connecter
		while(1) {
			printf("accept");
			sds=accept(sd,(struct sockaddr*) 0, 0); // nouveau descripteur de socket pour gérer la connexion entre le serveur et le client particulier avec qui on a établi la connexion, accepte la connexion d'un socket sur le socket sd
			int pid=fork();
			if(pid==-1) {
				perror("erreur fork");
			} else if (pid> 0) { //pere attend le fils
				printf("je suis le pere \n");
				int status;
				if((waitpid(pid,&status,0)) < 0) {
					perror("erreur waitpid");
				} 
			} else { // fils traite le client
				printf("je suis le fils \n");
				reception(sds);
				return 0;
			}
		}
	} else {
		perror("erreur socket");	
	}
	return 0;
}

	
			
	