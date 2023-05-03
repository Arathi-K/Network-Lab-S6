#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

#define PORT 9999

#define PACKET_SIZE 1024

typedef struct {
    int type;           // packet type (1 or 2)
    int seq_num;        // sequence number
    char payload[PACKET_SIZE];  // payload (1024 bytes)
    char trailer;       // checksum trailer (byte-wise XOR of packet)
} packet_t;

 // Initialize packet structure
    packet_t packet;
    int sockfd;
    struct sockaddr_in servaddr;
    
sem_t mutex1;
void *send_packets_1(void *args) {
	
    while (1) {
        // Send packet of type 1 every 100ms
        usleep(100000); // sleep for 100ms
        sem_wait(&mutex1);
        packet.type = 1;
        packet.seq_num++;
        packet.trailer = 0;
    	memset(packet.payload, 'B', PACKET_SIZE);
        for (int i = 0; i < PACKET_SIZE; i++) {
            //printf("i =%d ,Trailer is now %d \n",i,packet.trailer);
            packet.trailer ^= packet.payload[i];
        }
        printf("\n");
        sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
	printf("Sending packet of type:%d, sequence number:%d\n",packet.type,packet.seq_num);
        sem_post(&mutex1);
    }
}
void *send_packets_2(void *args) {
    while (1) {
        // Send packet of type 2 every 150ms
        usleep(150000); // sleep for 150ms
        sem_wait(&mutex1);
        packet.type = 2;
        packet.seq_num++;
        packet.trailer = 0;
    memset(packet.payload, 'C', PACKET_SIZE);
        for (int i = 0; i < PACKET_SIZE; i++) {
            //printf("i =%d ,Trailer is now %d \n",i,packet.trailer);
            packet.trailer ^= packet.payload[i];
        }
        printf("\n");
        sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
	printf("Sending packet of type:%d, sequence number:%d\n",packet.type,packet.seq_num);
       
	sem_post(&mutex1);
    }
}

int main() {
    
	
    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
	 printf("Socket created\n");
    // Set server address and port
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    //initialise packet values
    packet.type = 1;
    packet.seq_num = 0;
    sem_init(&mutex1, 0, 1);
    memset(packet.payload, 'A', PACKET_SIZE);
   // printf("1.seq number is %d ",packet.seq_num);
 	pthread_t send_thread_1,send_thread_2;
    int rc = pthread_create(&send_thread_1, NULL, send_packets_1, NULL);
    int mc= pthread_create(&send_thread_2, NULL, send_packets_2, NULL);
    if (rc != 0 || mc != 0) {
        fprintf(stderr, "pthread_create() failed: %s\n", strerror(rc));
        exit(EXIT_FAILURE);
    }

    // Wait for the send_packets thread to finish
    pthread_join(send_thread_1, NULL);
    pthread_join(send_thread_2, NULL);
    sem_destroy(&mutex1);
    close(sockfd);
    return 0;
    
    
    
}
