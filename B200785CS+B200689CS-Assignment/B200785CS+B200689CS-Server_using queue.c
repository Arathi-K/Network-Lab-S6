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
struct Queue{
    packet_t arr[9999];
    int p1;//points to from where packet of type 1 has to be searched
    int p2;
    int tail;
};
typedef struct Queue *queue;
typedef struct {
    int sockfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;
} details;
int type1_count = 0;
int type2_count = 0;
queue Q;
sem_t mutex1;
void* process_type1_packets(void* arg){
while(1){
    if(Q->tail>0){
        sem_wait(&mutex1);
        while(Q->arr[Q->p1].type!=1 && Q->p1!=Q->tail)
            Q->p1++;
        if(Q->arr[Q->p1].type==1){
            type1_count++;
        printf("Processing packet of Type:%d, sequence number:%d, payload=%s\n\n",Q->arr[Q->p1].type,Q->arr[Q->p1].seq_num,Q->arr[Q->p1].payload);
            Q->p1++;
        }    
       sem_post(&mutex1);
   }
 }
    return NULL;
}
void* process_type2_packets(void* arg){
while(1){
    if(Q->tail>0){
    sem_wait(&mutex1);
    while(Q->arr[Q->p2].type!=2 && Q->p2!=Q->tail)
            Q->p2++;
    if(Q->arr[Q->p2].type==2){
        //printf("type1 mutex locked\n");
            type2_count++;
            printf("Processing packet of Type:%d, sequence number:%d, payload=%s\n\n",Q->arr[Q->p2].type,Q->arr[Q->p2].seq_num,Q->arr[Q->p2].payload);
            Q->p2++;
        }   
    sem_post(&mutex1);
    }
  }
    return NULL;
}
void* print_packet_counts(void* arg){
    while(1){

        usleep(300000);
        printf("Count of Type1 packets received as of now = %d\n ",type1_count);
        printf("Count of Type2 packets received as of now = %d\n\n ",type2_count);
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

        if(Q->tail<9998){
           // printf("added to queue type %d\n",packet.type);
            Q->arr[Q->tail++]=packet;
            }
        else if(Q->p1>0 && Q->p2>0){
          //  printf("added to queue type %d\n",packet.type);
            Q->tail=0;
            Q->arr[Q->tail++]=packet;
        }
        else
            printf("Buffer full\n");
        printf("seq num:%d, payload=%c\n",packet.seq_num,packet.payload[0]);
        // switch (packet.type) {
        //     case 1:sem_post(&type1_mutex);
        //     		//printf("type1 mutex released\n");
        //         break;
        //     case 2:sem_post(&type2_mutex);
        //     	//printf("type1 mutex released\n");
        //         break;
        //     default: printf("Error: Unknown packet type %d\n", packet.type);
        // }
    }
    return NULL;
}
int main() {
    int sockfd;
    Q = (queue)malloc(sizeof(struct Queue));
    Q->p1=0;
    Q->p2=0;
    Q->tail=0;
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
   
    sem_init(&mutex1, 0, 1);
    //sem_init(&mutex2, 0, 1);
    pthread_t type1_thread,type2_thread, recv_thread,print_thread;
    details x;
    x.sockfd=sockfd;
    x.servaddr=servaddr;
    x.clientaddr=clientaddr;
    int rc = pthread_create(&recv_thread, NULL, receive_packets, (void*)&x);
    if (rc != 0) {
        fprintf(stderr, "pthread_create() failed: %s\n", strerror(rc));
        exit(EXIT_FAILURE);
    }
    pthread_create(&type1_thread, NULL, process_type1_packets, NULL);
    pthread_create(&type2_thread, NULL, process_type2_packets, NULL);
    pthread_create(&print_thread, NULL, print_packet_counts, NULL);
    
    // Wait for all packets to be received and printed
    pthread_join(recv_thread, NULL);
    pthread_join(print_thread, NULL);
    pthread_join(type1_thread, NULL);
    pthread_join(type2_thread, NULL);
    // Destroy semaphores
    sem_destroy(&mutex1);
 //   sem_destroy(&mutex2);
    close(sockfd);
    return 0;
}
