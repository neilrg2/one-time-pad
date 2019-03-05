#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUFFER_SIZE         1024
#define ENCRYPT_ADDITION    130
#define ENCRYPT_MODULO      27
#define ASCII               65


typedef struct sockaddr_in SOCKADDR_IN;

int getCharacterValue(char);

int main(int argc, const char * argv[]) {
    
    SOCKADDR_IN serverAddress, clientAddress;
    
    int exitMethod, charVal1, charVal2;
    int establishedConnectionFD;
    int i, decrypt_value, portNumber;
    pid_t spawnPID;
    ssize_t charsRead;
    socklen_t sizeOfClientInfo;
    
    char buffer[BUFFER_SIZE], buffer2[BUFFER_SIZE];
    char messageReceived[30];
    char *plain_string;
    
    memset(messageReceived, '\0', sizeof(messageReceived));
    sprintf(messageReceived, "Message Received");
    
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) { fprintf(stderr, "Socket creation failed.\n"); exit(1); }
    
    portNumber = atoi(argv[1]);
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNumber);          /* Converts host/PC byte order to network byte order */
    serverAddress.sin_addr.s_addr = INADDR_ANY;         /* Allows connections from any IP address */
    
    
    /* Binding the socket to the port */
    if (bind(server_socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        fprintf(stderr, "Binding socket failed.\n");
        exit(1);
    }
    
    
    /* Socket begins listening for connections */
    while (1)
    {
        if (listen(server_socket, 5) < 0)
        {
            fprintf(stderr, "Socket listening failed.\n");
            exit(1);
        }
        
        sizeOfClientInfo = sizeof(clientAddress);
        
        establishedConnectionFD = accept(server_socket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
        if (establishedConnectionFD < 0) { fprintf(stderr, "Accept error on connection.\n"); }
        
        
        spawnPID = fork();
        switch (spawnPID)
        {
            case -1:
                fprintf(stderr, "Forking process failed.\n");
                
                close(establishedConnectionFD);
                close(server_socket);
                
                exit(1);
                break;
                
            case 0:
                /* Spawned child process */
                memset(buffer, '\0', BUFFER_SIZE);
                memset(buffer2, '\0', BUFFER_SIZE);
                
                charsRead = recv(establishedConnectionFD, buffer, BUFFER_SIZE - 1, 0);
                if (charsRead < 0) { fprintf(stderr, "Message receive error.\n"); }
                
                charsRead = send(establishedConnectionFD, messageReceived, strlen(messageReceived), 0);
                if (charsRead < 0) { fprintf(stderr, "Error writing to the socket.\n"); }
                
                charsRead = recv(establishedConnectionFD, buffer2, BUFFER_SIZE - 1, 0);
                if (charsRead < 0) { fprintf(stderr, "Message receive error.\n"); }
                
                plain_string = (char *)malloc(sizeof(char) * strlen(buffer));  /* Set plain string to length of cipher text (buffer #1) */
                memset(plain_string, '\0', strlen(buffer));
                
                
                
                
                
                /* Decryption technique */
                for (i = 0; i < strlen(buffer); i++)
                {
                    if (buffer[i] == 32) { buffer[i] = 91; }
                    
                    charVal1 = getCharacterValue(buffer[i]);
                    charVal2 = getCharacterValue(buffer2[i]);
                    
                    decrypt_value = charVal1 - charVal2;
                    
                    if (decrypt_value < 0)
                    {
                        plain_string[i] = (decrypt_value + ENCRYPT_MODULO) + ASCII;
                    }
                    else
                    {
                        plain_string[i] = decrypt_value + ASCII;
                    }

                    if (plain_string[i] == 91) { plain_string[i] = 32; }
                }

                
                

                charsRead = send(establishedConnectionFD, plain_string, strlen(plain_string), 0);
                if (charsRead < 0) { fprintf(stderr, "Error writing to the socket.\n"); }
                
                close(establishedConnectionFD);
                free(plain_string);
                
                exit(0);
                break;
                
            default:
                /* Parent process */
                waitpid(spawnPID, &exitMethod, 0);
                
                //            if (WIFEXITED(exitMethod))
                //            {
                //                printf("Child process exited with: %d\n", WEXITSTATUS(exitMethod));
                //                fflush(stdout);
                //            }
                //            else
                //            {
                //                printf("Child process was terminated with signal %d\n", WTERMSIG(exitMethod));
                //                fflush(stdout);
                //            }
        }
    }
    
    
    close(server_socket);
    return 0;
}

int getCharacterValue(char plain_char)
{
    int returnInt = -1;
    
    switch (plain_char)
    {
        case 'A':
            returnInt = 0;
            break;
            
        case 'B':
            returnInt = 1;
            break;
            
        case 'C':
            returnInt = 2;
            break;
            
        case 'D':
            returnInt = 3;
            break;
            
        case 'E':
            returnInt = 4;
            break;
        case 'F':
            returnInt = 5;
            break;
            
        case 'G':
            returnInt = 6;
            break;
            
        case 'H':
            returnInt = 7;
            break;
            
        case 'I':
            returnInt = 8;
            break;
            
        case 'J':
            returnInt = 9;
            break;
            
        case 'K':
            returnInt = 10;
            break;
            
        case 'L':
            returnInt = 11;
            break;
            
        case 'M':
            returnInt = 12;
            break;
            
        case 'N':
            returnInt = 13;
            break;
            
        case 'O':
            returnInt = 14;
            break;
            
        case 'P':
            returnInt = 15;
            break;
            
        case 'Q':
            returnInt = 16;
            break;
            
        case 'R':
            returnInt = 17;
            break;
            
        case 'S':
            returnInt = 18;
            break;
            
        case 'T':
            returnInt = 19;
            break;
            
        case 'U':
            returnInt = 20;
            break;
            
        case 'V':
            returnInt = 21;
            break;
            
        case 'W':
            returnInt = 22;
            break;
            
        case 'X':
            returnInt = 23;
            break;
            
        case 'Y':
            returnInt = 24;
            break;
            
        case 'Z':
            returnInt = 25;
            break;
            
        case ' ':
            returnInt = 26;
            break;
    }
    
    return returnInt;
}
