#include "iniobj.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <netdb.h>
#include <stdio.h>

int sd, sds;
struct sockaddr_in addr_clt, addr_srv;
int addr_clt_l, addr_srv_l;
struct hostent* hid;

int main(int argc, char* argv[]) {
	sd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); // création d'un socket
	if(sd != -1) {
		addr_clt_l=sizeof(addr_clt);
		bzero(&addr_clt,addr_clt_l);
		addr_clt.sin_family=AF_INET; // famille
		addr_clt.sin_port=0; // numéro de port
		addr_clt.sin_addr.s_addr=htonl(INADDR_ANY); // adresse IP
		bzero(&addr_srv, addr_srv_l);
		addr_srv_l=sizeof(addr_srv);
		addr_srv.sin_family=AF_INET; // famille
		addr_srv.sin_port=htons(atoi(argv[2])); // numéro de port
		addr_srv.sin_addr.s_addr=htonl(INADDR_ANY); // adresse IP
		
		hid=gethostbyname(argv[1]);
		if(hid==NULL) {
			perror("erreur gethostbyname");
			return -1;
		}
		bcopy(hid->h_addr,&(addr_srv.sin_addr.s_addr),hid->h_length);
		bind(sd,(const struct sockaddr*) &addr_clt,addr_clt_l); // liaison du socket avec la structure addr_clt
		if(connect(sd,(struct sockaddr*)&addr_srv,addr_srv_l)<0) { // établissement de la connexion avec le serveur à l'adresse spécifiée dans la structure addr_srv
			perror("erreur de connexion");
		} else {
			int i;
			for(i=0; i<TABLEN;i++) {
				sleep(3);
				printf("message envoyé par le client");
				printf("%s %s \n ",tabobj[i].str1, tabobj[i].str2);
				if(send(sd,&tabobj[i],sizeof(obj),0) != sizeof(obj)){ // envoi en mode connecté sur le socket sd des données pointées par tabobj[i] et de taille obj
					perror("erreur d'envoi");
				}
			}
		}
	} else {
		perror("erreur socket");
	}
	return 0;
}

