#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT	 8080
#define MAXLINE 1024
	
// Driver code
int main() {
	int sock_des;
	char server_msg[2000],client_msg[2000];
	struct sockaddr_in servaddr, cliaddr;
		
	// Creating socket file descriptor
	if ( (sock_des = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		printf("socket creation failed\n");
		exit(1);
	}
		
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
		
	// Bind the socket with the server address
	if ( bind(sock_des, (const struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
	{
		printf("bind failed\n");
		exit(1);
	}
		
	int n,len=sizeof(cliaddr);
	//while(1){
		n = recvfrom(sock_des, (char *)client_msg, 2000,MSG_WAITALL, ( struct sockaddr *) &cliaddr,&len);
		client_msg[n] = '\0';
		printf("Message from Client : %s\n",client_msg);
		
		for(int i=0;i<n;i++)
			server_msg[i]=client_msg[n-i-1];
		sendto(sock_des, (const char *)server_msg, strlen(server_msg),MSG_CONFIRM, (const struct sockaddr *) &cliaddr,len);
		printf("Server's msg:%s\n",server_msg);
	//	if(server_msg=="1")
	//		break;
	//	}
		
	return 0;
}

