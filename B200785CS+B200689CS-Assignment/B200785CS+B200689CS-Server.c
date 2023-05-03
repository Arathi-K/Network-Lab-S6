#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#define PORT 9999
#define PACKET_SIZE 1024
typedef struct {
    int type;
    int seq_num;
    char payload[PACKET_SIZE];
    char trailer;
} packet_t;

typedef struct {
    int sockfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;
} details;
int type1_count = 0;
int type2_count = 0;
sem_t type1_mutex, type2_mutex;
void* process_type1_packets(void* arg){
    sem_wait(&type1_mutex);
    packet_t* packet=(packet_t*)arg;
     printf("Processing packet of Type:%d, sequence number:%d, payload=%s\n\n",packet->type,packet->seq_num,packet->payload);
    type1_count++;
    sem_post(&type1_mutex);
    return NULL;
}
void* process_type2_packets(void* arg){
    sem_wait(&type2_mutex);
    packet_t* packet=(packet_t*)arg;
    printf("Processing packet of Type:%d, sequence number:%d, payload=%s\n\n",packet->type,packet->seq_num,packet->payload);
    type2_count++;
    sem_post(&type2_mutex);
    return NULL;
}
void* print_packet_counts(void* arg){
    while(1){
        usleep(300000);
        printf("Count of Type1 packets received as of now = %d\n ",type1_count);

        printf("Count of Type2 packets received as of now = %d\n ",type2_count);
	printf("\n");
    }
}
void *receive_packets(void* arg) {
    details* x=(details*)arg;
    int sockfd=x->sockfd;
    struct sockaddr_in servaddr=x->servaddr;
    struct sockaddr_in clientaddr=x->clientaddr;
    socklen_t len = sizeof(clientaddr);
    packet_t packet;
    char checksum;
    pthread_t type1_thread,type2_thread;
    while (1) {
        int n = recvfrom(sockfd, &packet, sizeof(packet), MSG_WAITALL, (struct sockaddr *)&clientaddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }
        checksum = 0;
        for (int i = 0; i < PACKET_SIZE; i++) {
            checksum ^=packet.payload[i];
        }
        if (checksum != packet.trailer) {
            printf("Error: Packet checksum does not match\n");
            continue;
        }
        switch (packet.type) {
            case 1:pthread_create(&type1_thread, NULL, process_type1_packets,(void*)&packet);
                pthread_join(type1_thread, NULL);
                break;
            case 2:pthread_create(&type2_thread, NULL, process_type2_packets,(void*) &packet);
                pthread_join(type2_thread, NULL);
                break;
            default: printf("Error: Unknown packet type %d\n", packet.type);
        }
    }
    return NULL;
}
int main() {
    int sockfd;
    struct sockaddr_in servaddr,clientaddr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&clientaddr, 0, sizeof(clientaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
   
    sem_init(&type1_mutex, 0, 1);
    sem_init(&type2_mutex, 0, 1);
    
    pthread_t recv_thread,print_thread;
    details x;
    x.sockfd=sockfd;
    x.servaddr=servaddr;
    x.clientaddr=clientaddr;
    int rc = pthread_create(&recv_thread, NULL, receive_packets, (void*)&x);
    if (rc != 0) {
        fprintf(stderr, "pthread_create() failed: %s\n", strerror(rc));
        exit(EXIT_FAILURE);
    }
    pthread_create(&print_thread, NULL, print_packet_counts, NULL);
    
    // Wait for all packets to be received and printed
    pthread_join(recv_thread, NULL);
    pthread_join(print_thread, NULL);
    
    // Destroy semaphores
    sem_destroy(&type1_mutex);
    sem_destroy(&type1_mutex);
    
    close(sockfd);
    return 0;
}
