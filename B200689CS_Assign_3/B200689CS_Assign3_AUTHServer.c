#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 9999
#define cache_size 10
int f=0;
struct cache{
	char name[100];
	char ip[100];
};

typedef struct cache* cache;	
int isInCache(char client_msg[],char server_msg[],struct cache* arr){
	for(int i=0;i<cache_size;i++){
		if(strcmp(arr[i].name,client_msg)==0){
			strcpy(server_msg,arr[i].ip);
			return 1;
		}
	}
	return 0;
}		
			 
// Driver code
int main() {
	int sock_des;
	char server_msg[2000],client_msg[2000];
	struct sockaddr_in servaddr,cliaddr;
	// cache arr[5];
	struct cache* arr = (struct cache*) malloc(cache_size* sizeof(struct cache));
	strcpy(arr[0].name,"google.com");
	strcpy(arr[0].ip,"1.2.3.4");
	strcpy(arr[1].name,"facebook.com");
	strcpy(arr[1].ip,"10.20.30.40");
	strcpy(arr[2].name,"instagram.com");
	strcpy(arr[2].ip,"3.4.5.6");
	strcpy(arr[3].name,"twitter.com");
	strcpy(arr[3].ip,"30.40.50.60");
	strcpy(arr[4].name,"youtube.com");
	strcpy(arr[4].ip,"5.6.7.8");
    strcpy(arr[5].name,"codechef.com");
	strcpy(arr[5].ip,"50.60.70.80");
	strcpy(arr[6].name,"codeforces.com");
	strcpy(arr[6].ip,"9.1.2.3");
	strcpy(arr[7].name,"leetcode.com");
	strcpy(arr[7].ip,"90.10.20.30");
	strcpy(arr[8].name,"eduserver.com");
	strcpy(arr[8].ip,"6.5.4.0");
	strcpy(arr[9].name,"coursera.com");
	strcpy(arr[9].ip,"3.8.4.1");

	
	// Creating socket file descriptor
	if ( (sock_des = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		printf("socket creation failed\n");
		exit(1);
	}
		
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	// memset(&upper_servaddr, 0, sizeof(upper_servaddr));
	
		
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
	// int nr,len=sizeof(r_servaddr);
	while(1){
		n = recvfrom(sock_des, (char *)client_msg, 2000,MSG_WAITALL, ( struct sockaddr *) &cliaddr,&len);
		client_msg[n] = '\0';
		printf("Message from Client : %s\n",client_msg);
		
		if(isInCache(client_msg,server_msg,arr)==1){
            // printf("Server-message-is: %s\n",server_msg);
            sendto(sock_des, (const char *)server_msg, strlen(server_msg),MSG_CONFIRM, (const struct sockaddr *) &cliaddr,len);
		}
        else{
            strcpy(server_msg,"Not-found");
			sendto(sock_des, (const char *)server_msg, strlen(server_msg),MSG_CONFIRM, (const struct sockaddr *) &cliaddr,len);
        }
				
    }	
	return 0;
}

