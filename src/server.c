/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2021/22
    Semestar:       Zimski (V)
    
    Ime fajla:      server.c
    Opis:           TCP Battleship
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //threading

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

#define MAX_CLIENTS 2

#define CLIENT_A 0
#define CLIENT_B 1

char result[DEFAULT_BUFLEN] = "";
char msg1[DEFAULT_BUFLEN] = "";
char msg2[DEFAULT_BUFLEN] = "";

struct send_data{
    int sock;
    char* msg;
};

void* send_msg(void* args){
    while(1){
    struct send_data* thread_args = (struct send_data*)args;
    int socket = thread_args->sock;
    char* message = thread_args->msg;

    send(socket , message , strlen(message), 0);
    }

    return 0;
}

void* recv_msg1(void* sock){
    while(1){
    int socket = *(int *)sock;

    recv(socket, msg1, sizeof(msg1), 0);
    }

    return 0;
}

void* recv_msg2(void* sock){
    int socket = *(int *)sock;

    recv(socket, msg2, sizeof(msg2), 0);

    return 0;
}

int main(int argc , char *argv[])
{
    //Network variables
    int client_num = 0;
    int socket_desc , client_sock[MAX_CLIENTS] , c , read_size;
    struct sockaddr_in server , client;
    char client_message[DEFAULT_BUFLEN];
    pthread_t send_thread[MAX_CLIENTS];
    pthread_t recv_thread[MAX_CLIENTS];
    struct send_data args[MAX_CLIENTS];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    while(client_num < MAX_CLIENTS){
        client_sock[client_num] = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock[client_num] < 0)
        {
            perror("accept failed");
            return 1;
        }
        puts("Connection accepted");
        client_num++;
    }

    args[CLIENT_A].sock = client_sock[CLIENT_A];
    args[CLIENT_A].msg = "BROOOOO";

    args[CLIENT_B].sock = client_sock[CLIENT_B];
    args[CLIENT_B].msg = "BROOOOOOOOBBBB";

    pthread_create(&send_thread[CLIENT_A], NULL, send_msg, (void*)&args[CLIENT_A]);

    pthread_create(&send_thread[CLIENT_B], NULL, send_msg, (void*)&args[CLIENT_B]);

    pthread_create(&recv_thread[CLIENT_A], NULL, recv_msg1, (void*)&client_sock[CLIENT_A]);
    
    pthread_create(&recv_thread[CLIENT_B], NULL, recv_msg2, (void*)&client_sock[CLIENT_B]);

    while(1){    
    puts(msg1);
    puts(msg2);
    }

    return 0;
}

