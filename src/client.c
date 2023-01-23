/*
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2021/22
    Semestar:       Zimski (V)

    Ime fajla:      client.c
    Opis:           TCP/IP Battleship

    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include <stdio.h>      //printf
#include <string.h>     //strlen
#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <fcntl.h>      //for open
#include <unistd.h>     //for close

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27015

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in server;
    // Variable to store default server messages
    char server_msg[DEFAULT_BUFLEN];
    // Variable to store printable board that the server sends
    char printboard[9];
    // Variable for start checking
    char start[DEFAULT_BUFLEN] = "";

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    // Game logo
    printf(" ___ _  _     _      ___ ___    _  __     ___  _\n");
    printf("  | /  |_)   |_)  /\\  |   | |  |_ (_  |_|  |  |_)\n");
    printf("  | \\_ |     |_) /--\\ |   | |_ |_ __) | | _|_ |\n\n");

    // Start check
    while (strcmp(start, "START\n") != 0)
    {
        puts("You must enter 'START' for game start");
        fgets(start, DEFAULT_BUFLEN, stdin);
    }

    for (int i = 0; i < 5; i++)
    {
        // Receive server message
        server_msg[0] = '\0';
        printf("\n");
        recv(sock, server_msg, DEFAULT_BUFLEN, 0);
        puts(server_msg);

        // Wait for user to input variable
        char x[DEFAULT_BUFLEN] = "";
        while (1)
        {
            fgets(x, DEFAULT_BUFLEN, stdin);
            if (strcmp(x, "1\n") == 0 || strcmp(x, "2\n") == 0 || strcmp(x, "0\n") == 0 || strcmp(x, "SIDEWAYS\n") == 0 || strcmp(x, "UPRIGHT\n") == 0)
            {
                break;
            }
            else
            {
                puts("Non-Valid Input (Must be '0', '1', '2' or 'SIDEWAYS', 'UPRIGHT')");
            }
        }
        // Send variable to server
        if (send(sock, x, strlen(x), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
    }
    // Print user board when setup is finished
    printboard[0] = '\0';
    recv(sock, printboard, 9, 0);
    printf("\n");
    printf("   YOUR BOARD\n");
    printf("              \n");
    printf("    0   1   2\n");
    printf("  ╔═══╦═══╦═══╗\n");
    int k = 0;
    for (int i = 0; i < 3; i++)
    {
        printf("%d ║ %c ║ %c ║ %c ║\n", i, printboard[k], printboard[k + 1], printboard[k + 2]);
        k += 3;
        if (i != 2)
            printf("  ╠═══╬═══╬═══╣\n");
    }
    printf("  ╚═══╩═══╩═══╝\n");
    printf("\n");

    while (1)
    {
        for (int i = 0; i < 2; i++)
        {
            // Place bomb
            server_msg[0] = '\0';
            printf("\n");
            recv(sock, server_msg, DEFAULT_BUFLEN, 0);
            if (strcmp(server_msg, "YOU LOST :(                  ") == 0)
            {
                printf("      _            _   __ ___\n");
                printf(" \\_/ / \\ | |   |  / \\ (_   |  \n");
                printf("  |  \\_/ |_|   |_ \\_/ __)  |  \n\n");
                while (1)
                    ;
            }
            else if (strcmp(server_msg, "YOU ARE THE WINNER!!!!!!     ") == 0)
            {
                printf("      _                _       \n");
                printf(" \\_/ / \\ | |   \\    / / \\ |\\ |\n");
                printf("  |  \\_/ |_|    \\/\\/  \\_/ | \\|\n\n");
                while (1)
                    ;
            }
            puts(server_msg);

            char x[DEFAULT_BUFLEN] = "";
            while (1)
            {
                fgets(x, DEFAULT_BUFLEN, stdin);
                if (strcmp(x, "1\n") == 0 || strcmp(x, "2\n") == 0 || strcmp(x, "0\n") == 0 || strcmp(x, "SIDEWAYS\n") == 0 || strcmp(x, "UPRIGHT\n") == 0)
                {
                    break;
                }
                else
                {
                    puts("Non-Valid Input (Must be '0', '1' or '2')");
                }
            }
            if (send(sock, x, strlen(x), 0) < 0)
            {
                puts("Send failed");
                return 1;
            }
        }
        // Print enemy board after placing bomb
        printboard[0] = '\0';
        recv(sock, printboard, 9, 0);
        printf("\n");
        printf("   ENEMY BOARD\n");
        printf("               \n");
        k = 0;
        printf("    0   1   2\n");
        printf("  ╔═══╦═══╦═══╗\n");
        for (int i = 0; i < 3; i++)
        {
            printf("%d ║ %c ║ %c ║ %c ║\n", i, printboard[k], printboard[k + 1], printboard[k + 2]);
            k += 3;
            if (i != 2)
                printf("  ╠═══╬═══╬═══╣\n");
        }
        printf("  ╚═══╩═══╩═══╝\n");
        printf("\n");
    }

    close(sock);

    return 0;
}