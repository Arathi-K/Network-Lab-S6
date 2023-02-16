// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
	
#define PORT	 8080
	
// Driver code
int main() {
	int sock_des;
	char client_msg[2000],server_msg[2000];
	struct sockaddr_in servaddr;
	
	// Creating socket file descriptor
	if ( (sock_des = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		printf("socket creation failed");
		exit(1);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;
		
	int n, len=sizeof(servaddr);
	//printf("Enter 1 to stop the loop\n");
	//while(1){
		printf("Enter-the-domain-name: ");
		scanf("%[^\n]",client_msg);
		sendto(sock_des, (const char *)client_msg, strlen(client_msg),MSG_CONFIRM, (const struct sockaddr *) &servaddr,len);
		n= recvfrom(sock_des, (char *)server_msg,2000,MSG_WAITALL, (struct sockaddr *) &servaddr,&len);
		server_msg[n] = '\0';
		printf("ip-address: %s\n",server_msg);
		//if(server_msg=="1")
		//	break;
		
	//}
	close(sock_des);
	return 0;
}

