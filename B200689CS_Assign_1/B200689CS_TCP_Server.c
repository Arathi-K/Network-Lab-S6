#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#define PORT 8080

int main(){
	int socket_desc,client_sock,client_size;
	struct sockaddr_in server_addr,client_addr;
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
	
	if(bind(socket_desc,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
		printf("Couldn't bind to the port\n");
		exit(0);
	}
	//else
	//	printf("Socket binded successfully\n");
	
	//listen for client
	if(listen(socket_desc,1)<0){
		printf("Listen failed\n");
		exit(0);
	}
	//else
	//	printf("Listening for incoming connections...\n");

	//accept an incoming connection 
	client_size=sizeof(client_addr);
	client_sock=accept(socket_desc,(struct sockaddr*)&client_addr,&client_size);

	if(client_sock<0){
		printf("Server accept failed...\n");
		exit(0);
	}
	//else
	//	printf("Accepted...\n");

	//Receive client's message
	
	if(recv(client_sock,client_message,sizeof(client_message),0)<0){
		printf("Couldn't receive message from the client...\n");
		exit(0);
	}
	else
		printf("Message from the client: %s\n",client_message);

	
	//Respond to client
	int l=strlen(client_message);
	for(int i=0;i<l;i++)
		server_message[i]=client_message[l-i-1];
	//strcpy(server_message,"This is the server's message");
	server_message[l]='\0';
	if(send(client_sock,server_message,strlen(server_message),0)<0){
		printf("Error in sending message to client...\n");
		exit(0);
	}
	//else
	//	printf("Message sent succesfully to the client...\n");
	
	printf("The server's message is : %s\n",server_message);
	close(socket_desc);

	return 0;
}
