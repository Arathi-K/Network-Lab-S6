#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#define PORT 8080

int main(){
	int socket_desc;
	struct sockaddr_in server_addr;
	char server_message[2000],client_message[2000];
	
	
	memset(server_message,'\0',sizeof(server_message));
	memset(client_message,'\0',sizeof(client_message));
	
	socket_desc=socket(AF_INET,SOCK_STREAM,0);
	if(socket_desc <0){
		printf("Error while creating socket\n");
		exit(0);
	}
	//else
	//	printf("Socket created successfully!!\n");


	//assign port address and ip address and biind it to the created socket
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(PORT);
	server_addr.sin_addr.s_addr=INADDR_ANY;
	
	//send connection request to server
	if(connect(socket_desc,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
		printf("Unable to connect...\n");
		exit(0);
	}
	//else
	//	printf("Connected with server successfully...\n");
	
	//get input from the user
	printf("Enter a message to the server:\n");
	//gets(client_message);
	scanf("%[^\n]",client_message);
	//send the message to the server
	if(send(socket_desc,client_message,sizeof(client_message),0)<0){
		printf("Unable to send message...\n");
		exit(0);
	}
	

	//Receive servers's message
	
	if(recv(socket_desc,server_message,sizeof(server_message),0)<0){
		printf("Couldn't receive message from the server...\n");
		exit(0);
	}
	else
		printf("Message from the server: %s\n",server_message);

	close(socket_desc);
	return 0;
}
