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


typedef struct sockaddr_in SOCKADDR_IN;

int main(int argc, const char * argv[]) {
    
    SOCKADDR_IN serverAddress, clientAddress;
    
    int exitMethod;
    int establishedConnectionFD;
    int portNumber;
    pid_t spawnPID;
    ssize_t charsRead;
    socklen_t sizeOfClientInfo;
    
    char buffer[BUFFER_SIZE], buffer2[BUFFER_SIZE];
    char messageReceived[30];
    
    memset(messageReceived, '\0', sizeof(messageReceived));
    sprintf(messageReceived, "Message Received");
    
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) { fprintf(stderr, "Socket creation failed.\n"); exit(1); }
    
    portNumber = atoi(argv[1]);
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNumber);          /* Converts host/PC byte order to network byte order */
    serverAddress.sin_addr.s_addr = INADDR_ANY; /* Allows connections from any IP address */
    

    /* Binding the socket to the port */
    if (bind(server_socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        fprintf(stderr, "Binding socket failed.\n");
        exit(1);
    }

    /* Socket begins listening for connections */
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
            printf("Message: %s\n", buffer);
            
            
            charsRead = send(establishedConnectionFD, messageReceived, sizeof(messageReceived), 0);
            if (charsRead < 0) { fprintf(stderr, "Error writing to the socket.\n"); }
            
//            charsRead = recv(establishedConnectionFD, buffer2, BUFFER_SIZE - 1, 0);
//            if (charsRead < 0) { fprintf(stderr, "Message receive error.\n"); }
//            printf("Message: %s\n", buffer2);
//
//            charsRead = send(establishedConnectionFD, messageReceived, sizeof(messageReceived), 0);
//            if (charsRead < 0) { fprintf(stderr, "Error writing to the socket.\n"); }
            
            close(establishedConnectionFD);

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
    
    
    
    close(server_socket);
    return 0;
}
