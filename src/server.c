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
    Opis:           TCP/IP Battleship
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <stdlib.h> //atoi
#include <stdbool.h> //bool

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27015
#define MAX_CLIENTS 2

//Define num of rows and collums
#define ROWS 3
#define COLS 3

//Define players
#define PLAYER_A 0
#define PLAYER_B 1

//Define cell states
#define WATER 0
#define SHIP 1
#define HIT 2
#define MISS 3

//Define orientation
#define SIDEWAYS 0
#define UPRIGHT 1

int main(int argc , char *argv[])
{
    //Game variables
    int win_a = 0;
    int win_b = 0;
    int win_num_a = 0;
    int win_num_b = 0;
    int winner;
    bool streak = true;
    int board_a[ROWS][COLS];
    int board_b[ROWS][COLS];
    int orient_a;
    int orient_b;
    char printedboard_a[9] = "";
    char printedboard_b[9] = "";
    char *start_message1 = "ENTER SMALL SHIP X COORDINATE";
    char *start_message2 = "ENTER SMALL SHIP Y COORDINATE";
    char *start_message3 = "ENTER BIG SHIP X COORDINATE  ";
    char *start_message4 = "ENTER BIG SHIP Y COORDINATE  ";
    char *start_message5 = "ENTER BIG SHIP ORIENTATION   ";
    char *bomb_message1 =  "ENTER BOMB X COORDINATE      ";
    char *bomb_message2 =  "ENTER BOMB Y COORDINATE      ";
    char *win_message =    "YOU ARE THE WINNER!!!!!!     ";
    char *lose_message =   "YOU LOST :(                  ";

    //Network variables
    int client_num = 0;
    int client_sock[MAX_CLIENTS];
    int socket_desc, c;
    struct sockaddr_in server , client;
    char client_msgA[DEFAULT_BUFLEN];
    char client_msgB[DEFAULT_BUFLEN];

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
        //Print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //Accept connection from an incoming client
    while(client_num < MAX_CLIENTS)
    {
        client_sock[client_num] = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock[client_num] < 0)
        {
            perror("accept failed");
            return 1;
        }
        puts("Connection accepted");
        client_num++;
    }

    //Init board A
    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            board_a[i][j] = WATER;
        }
    }

    //Init board B
    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            board_b[i][j] = WATER;
        }
    }

    //Send small ship x cord request to player A
    if(send(client_sock[PLAYER_A], start_message1, strlen(start_message1), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Send small ship x cord request to player B
    if(send(client_sock[PLAYER_B], start_message1, strlen(start_message1), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Receive small ship x cord from player A
    recv(client_sock[PLAYER_A], client_msgA, DEFAULT_BUFLEN, 0);

    //Receive small ship x cord from player B
    recv(client_sock[PLAYER_B], client_msgB, DEFAULT_BUFLEN, 0);

    //Convert string to integer for use in table array
    int x_a = atoi(client_msgA);

    //Convert string to integer for use in table array
    int x_b = atoi(client_msgB);

    //Send small ship y cord request to player A
    if(send(client_sock[PLAYER_A], start_message2, strlen(start_message2), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Send small ship y cord request to player B
    if(send(client_sock[PLAYER_B], start_message2, strlen(start_message2), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Receive small ship y cord from player A
    recv(client_sock[PLAYER_A], client_msgA, DEFAULT_BUFLEN, 0);

    //Receive small ship y cord from player B
    recv(client_sock[PLAYER_B], client_msgB, DEFAULT_BUFLEN, 0);

    //Convert string to integer for use in table array
    int y_a = atoi(client_msgA);

    //Convert string to integer for use in table array
    int y_b = atoi(client_msgB);

    //Set small ship on boards
    board_a[x_a][y_a] = SHIP;
    
    //Increase number of hits needed to win
    win_num_a++;

    //Set small ship on boards
    board_b[x_b][y_b] = SHIP;

    //Increase number of hits needed to win   
    win_num_b++;

    //Send big ship x cord request to player A
    if(send(client_sock[PLAYER_A], start_message3, strlen(start_message3), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Send big ship x cord request to player B  
    if(send(client_sock[PLAYER_B], start_message3, strlen(start_message3), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Receive big ship x cord from player A
    recv(client_sock[PLAYER_A], client_msgA, DEFAULT_BUFLEN, 0);

    //Receive big ship x cord from player B    
    recv(client_sock[PLAYER_B], client_msgB, DEFAULT_BUFLEN, 0);

    //Convert string to integer for use in table array
    x_a = atoi(client_msgA);

    //Convert string to integer for use in table array
    x_b = atoi(client_msgB);

    //Send big ship y cord request to player A
    if(send(client_sock[PLAYER_A], start_message4, strlen(start_message4), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Send big ship y cord request to player B
    if(send(client_sock[PLAYER_B], start_message4, strlen(start_message4), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Receive big ship y cords from player A
    recv(client_sock[PLAYER_A], client_msgA, DEFAULT_BUFLEN, 0);

    //Receive big ship y cords from player B    
    recv(client_sock[PLAYER_B], client_msgB, DEFAULT_BUFLEN, 0);

    //Convert string to integer for use in table array
    y_a = atoi(client_msgA);

    //Convert string to integer for use in table array
    y_b = atoi(client_msgB);

    //Send big ship orientation request to player A
    if(send(client_sock[PLAYER_A], start_message5, strlen(start_message5), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Send big ship orientation request to player B
    if(send(client_sock[PLAYER_B], start_message5, strlen(start_message5), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Receive big ship orientation from player A
    recv(client_sock[PLAYER_A], client_msgA, DEFAULT_BUFLEN, 0);

    //Receive big ship orientation from player B  
    recv(client_sock[PLAYER_B], client_msgB, DEFAULT_BUFLEN, 0);


    //Check if UPRIGHT or SIDEWAYS
    if(strcmp(client_msgA, "SIDEWAYS\n") == 0)
    {
        orient_a = SIDEWAYS;
    }
    else if(strcmp(client_msgA, "UPRIGHT\n") == 0)
    {
        orient_a = UPRIGHT;
    }
    else
    {
        //Convert string to integer for use in table array
        orient_a = atoi(client_msgA);
    }

    //Check if UPRIGHT or SIDEWAYS
    if(strcmp(client_msgB, "SIDEWAYS\n") == 0)
    {
        orient_b = SIDEWAYS;
    }
    else if(strcmp(client_msgB, "UPRIGHT\n") == 0)
    {
        orient_b = UPRIGHT;
    }
    else
    {
        //Convert string to integer for use in table array
        orient_b = atoi(client_msgB);
    }

    //Set big ship on board A
    if(orient_a == SIDEWAYS)
    {
        //Check if ship already on that spot, do nothing if true
        if(board_a[x_a][y_a] == SHIP){}
        else
        {
            //Set ship on board
            board_a[x_a][y_a] = SHIP;

            //Increase number of hits needed to win
            win_num_a++;
        }

        //Check if ship already on that spot, do nothing if true
        if(board_a[x_a][y_a+1] == SHIP){}

        //Check if ship already on that spot, do nothing if true
        else if(board_a[x_a][y_a-1] == SHIP){}

        else
        {   
            //Check if ship out of bounds
            if(y_a+1 > 2)
            {
                //If out of bounds, change direction
                board_a[x_a][y_a-1] = SHIP;

                //Increase number of hits needed to win                
                win_num_a++;
            }
            else
            {
                //If not out of bounds, default direction
                board_a[x_a][y_a+1] = SHIP;

                //Increase number of hits needed to win                   
                win_num_a++;
            }
        }
    }
    //Check for orientation
    else if(orient_a == UPRIGHT)
    {
        //Check if ship already on that spot, do nothing if true
        if(board_a[x_a][y_a] == SHIP){}
        else
        {
            //Set ship on board
            board_a[x_a][y_a] = SHIP;

            //Increase number of hits needed to win              
            win_num_a++;
        }
        //Check if ship already on that spot, do nothing if true
        if(board_a[x_a+1][y_a] == SHIP){}

        //Check if ship already on that spot, do nothing if true        
        else if(board_a[x_a-1][y_a] == SHIP){}

        else
        {
            //Check if out of bounds
            if(x_a+1 > 2)
            {
            //If out of bounds, change direction
            board_a[x_a-1][y_a] = SHIP;

            //Increase number of hits needed to win     
            win_num_a++;               
            }
            else
            {
            //If not out of bounds, default direction
            board_a[x_a+1][y_a] = SHIP;

            //Increase number of hits needed to win            
            win_num_a++;
            }
        }
    }

    //Set big ship on board B
    if(orient_b == SIDEWAYS)
    {
        //Check if ship already on that spot, do nothing if true
        if(board_b[x_b][y_b] == SHIP){}
        else
        {
            //Set ship on board
            board_b[x_b][y_b] = SHIP;

            //Increase number of hits needed to win
            win_num_b++;
        }

        //Check if ship already on that spot, do nothing if true
        if(board_b[x_b][y_b+1] == SHIP){}

        //Check if ship already on that spot, do nothing if true        
        else if(board_b[x_b][y_b-1] == SHIP){}
        else
        {
            //Check if out of bounds
            if(y_b+1 > 2)
            {
                //If out of bounds, change direction
                board_b[x_b][y_b-1] = SHIP;
            
                //Increase number of hits needed to win
                win_num_b++;
            }
            else
            {
                //If not out of bounds, default direction
                board_b[x_b][y_b+1] = SHIP;

                //Increase number of hits needed to win
                win_num_b++;
            }
        }
    }
    //Check for orientation
    else if(orient_b == UPRIGHT)
    {
        //Check if ship already on that spot, do nothing if true
        if(board_b[x_b][y_b] == SHIP){}
        else
        {
            //Set ship on board
            board_b[x_b][y_b] = SHIP;

            //Increase number of hits needed to win
            win_num_b++;
        }

        //Check if ship already on that spot, do nothing if true
        if(board_b[x_b+1][y_b] == SHIP){}
        //Check if ship already on that spot, do nothing if true
        else if(board_b[x_b-1][y_b] == SHIP){}

        else
        {
            //Check if out of bounds
            if(x_b+1 > 2)
            {
            //If out of bounds, change direction
            board_b[x_b-1][y_b] = SHIP;
        
            //Increase number of hits needed to win
            win_num_b++;               
            }
            else
            {
            //If not out of bounds, default direction
            board_b[x_b+1][y_b] = SHIP;

            //Increase number of hits needed to win
            win_num_b++;
            }
        }
    }

    //Print board for player A
    //Variable for printable string
    int k = 0;

    //Check all spots and fill printable string
    for(int i = 0; i < ROWS; i++){
        for(int j = 0; j < COLS; j++){
            if(board_a[i][j] == WATER){
                printedboard_a[k] = 'W';
            }
            else if(board_a[i][j] == SHIP){
                printedboard_a[k] = 'S';
            }
            k++;
        }
    }

    //Print board for player B
    //Variable for printable string
    int h = 0;

    //Check all spots and fill printable string
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(board_b[i][j] == WATER){
                printedboard_b[h] = 'W';
            }
            else if(board_b[i][j] == SHIP){
                printedboard_b[h] = 'S';
            }
            h++;
        }
    }

    //Send printed board to player A
    if(send(client_sock[PLAYER_A], printedboard_a, 9, 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Send printed board to player B
    if(send(client_sock[PLAYER_B], printedboard_b, 9, 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Game loop
    while(1)
    {

    //Streak true on start
    streak = true;

    //Streak for player A
    while(1)
    {

    //Send bomb x cord request for player A
    if(send(client_sock[PLAYER_A], bomb_message1, strlen(bomb_message1), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Receive bomb cord x for player A
    recv(client_sock[PLAYER_A], client_msgA, DEFAULT_BUFLEN, 0);

    //Convert string to integer for use in table array
    int bomb_x_a = atoi(client_msgA);

    //Send bomb y cord request for player A
    if(send(client_sock[PLAYER_A], bomb_message2, strlen(bomb_message2), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Receive bomb cord y for player A
    recv(client_sock[PLAYER_A], client_msgA, DEFAULT_BUFLEN, 0);

    //Convert string to integer for use in table array
    int bomb_y_a = atoi(client_msgA);


    //Check if bomb hit
    if(board_b[bomb_x_a][bomb_y_a] == SHIP){       
        board_b[bomb_x_a][bomb_y_a] = HIT;
        //If hit, increase win variable
        win_a++;
    }else if(board_b[bomb_x_a][bomb_y_a] == WATER){
        board_b[bomb_x_a][bomb_y_a] = MISS;
        //If miss, end streak
        streak = false;
    }

    //Print board for player A
    //Variable for printable string   
    h = 0;

    //Check all spots and fill printable string
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(board_b[i][j] == HIT){
                printedboard_b[h] = 'S';
            }
            else if(board_b[i][j] == MISS){
                printedboard_b[h] = 'X';
            }
            else
            {
                printedboard_b[h] = 'O';
            }
            h++;
        }
    }

    //Send printed board to player A
    if(send(client_sock[PLAYER_A], printedboard_b, 9, 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Check winner, break while(1) if true
    if(win_a == win_num_b){
        winner = PLAYER_A;
        break;
    }

    //Check streak, break while(1) if true
    if(!streak)
    {
        break;
    }

    }

    //Check winner, break while(1) if true to go to end screen
    if(win_a == win_num_b){
        winner = PLAYER_A;
        break;
    }

    //If no winner, streak alive for player B
    streak = true;

    while(1)
    {

    //Send bomb x cord request for player B
    if(send(client_sock[PLAYER_B], bomb_message1, strlen(bomb_message1), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Receive bomb cord x for player B
    recv(client_sock[PLAYER_B], client_msgB, DEFAULT_BUFLEN, 0);

    //Convert string to integer for use in table array
    int bomb_x_b = atoi(client_msgB);

    //Send bomb y cord request for player B
    if(send(client_sock[PLAYER_B], bomb_message2, strlen(bomb_message2), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Receive bomb cord y for player B
    recv(client_sock[PLAYER_B], client_msgB, DEFAULT_BUFLEN, 0);

    //Convert string to integer for use in table array
    int bomb_y_b = atoi(client_msgB);

    //Check if bomb hit
    if(board_a[bomb_x_b][bomb_y_b] == SHIP){
        board_a[bomb_x_b][bomb_y_b] = HIT;
        //If hit, increase win variable        
        win_b++;
    }else if(board_a[bomb_x_b][bomb_y_b] == WATER){
        board_a[bomb_x_b][bomb_y_b] = MISS;
        //If miss, end streak
        streak = false;
    }

    //Print board for player B
    //Variable for printable string
    k = 0;

    //Check all spots and fill printable string
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(board_a[i][j] == HIT){
                printedboard_a[k] = 'S';
            }
            else if(board_a[i][j] == MISS){
                printedboard_a[k] = 'X';
            }
            else
            {
                printedboard_a[k] = 'O';
            }
            k++;
        }
    }

    //Send printed board to player B
    if(send(client_sock[PLAYER_B], printedboard_a, 9, 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Check winner, break while(1) if true
    if(win_b == win_num_a){
        winner = PLAYER_B;
        break;
    }

    //Check streak, break while(1) if true
    if(!streak){
        break;
    }
    }

    //Check winner, break while(1) if true go to end screen
    if(win_b == win_num_a){
        winner = PLAYER_B;
        break;
    }
    }

    //Check if player A won
    if(winner == PLAYER_A){
        //Send winning message to player A
        if(send(client_sock[PLAYER_A], win_message, strlen(win_message), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        //Send losing message to player B
        if(send(client_sock[PLAYER_B], lose_message, strlen(lose_message), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        //Block program
        while(1);
    }

    //Check if player B won
    else if(winner == PLAYER_B){
        //Send winning message to player B
        if(send(client_sock[PLAYER_B], win_message, strlen(win_message), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        //Send winning message to player A
        if(send(client_sock[PLAYER_A], lose_message, strlen(lose_message), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        //Block program
        while(1);
    }

    return 0;
}