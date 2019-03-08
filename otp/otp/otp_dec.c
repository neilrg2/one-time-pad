#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#define FALSE       "false"
#define TRUE        "true"

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct hostent HOSTENT;

int main(int argc, const char *argv[])
{
    int socketFD, portNumber, charsWritten, charsRead;
    int plain_text_file_descriptor, key_file_descriptor;
    int i, key_char_count, plain_text_char_count, count = 0;
    
    char charBuffer, bufferCheck[20];
    
    SOCKADDR_IN serverAddress;
    HOSTENT *serverHostInfo;
    
    
    /* Ensure appropriate number of arguments - FORMAT: otp_enc plaintext key port */
    if (argc < 4)
    {
        fprintf(stderr, "USAGE: %s hostname key port\n", argv[0]);
        exit(0);    /* Check usage and args */
    }
    
    /*****************************************************************************************/
    
    /* Block contains file validation: 1) Checks that plain text and generated key file are valid
     files and can be opened
     2) Retrieves character counts for both files and checks to see
     that the generated key is large enough to handle the plain text file
     3) Checks for any bad characters contained in both files
     
     Program will exit prematurely with exit code 1 if all of the above is not fulfilled */
    
    /* Open plain text file for reading */
    plain_text_file_descriptor = open(argv[1], O_RDONLY);
    if (plain_text_file_descriptor < 0)
    {
        fprintf(stderr, "\"%s\" is an invalid file.\n", argv[1]);
        exit(1);
    }
    
    /* Open generated key file for reading */
    key_file_descriptor = open(argv[2], O_RDONLY);
    if (key_file_descriptor < 0)
    {
        fprintf(stderr, "\"%s\" is an invalid file.\n", argv[2]);
        exit(1);
    }
    
    /* Get character counts for both the plain text file and the generated key file */
    plain_text_char_count = (int)lseek(plain_text_file_descriptor, 0, SEEK_END);
    key_char_count = (int)lseek(key_file_descriptor, 0, SEEK_END);
    
    if (key_char_count < plain_text_char_count)
    {
        fprintf(stderr, "ERROR: Plain text length exceeds the key length.\n");
        
        close(plain_text_file_descriptor);
        close(key_file_descriptor);
        exit(1);
    }
    
    /* Set file pointers back to beginning */
    lseek(plain_text_file_descriptor, 0, SEEK_SET);
    lseek(key_file_descriptor, 0, SEEK_SET);
    
    /* Check for bad characters in plain text file (excludes newline character at the end) */
    for (i = 0; i < plain_text_char_count - 1; i++)
    {
        read(plain_text_file_descriptor, &charBuffer, sizeof(charBuffer));
        if (charBuffer != 32 && (charBuffer < 65 || charBuffer > 90))
        {
            fprintf(stderr, "ERROR: Bad character detected in \"%s\".\n", argv[1]);
            
            close(plain_text_file_descriptor);
            close(key_file_descriptor);
            exit(1);
        }
    }
    
    /* Check for bad characters in generated key file (excludes newline character at the end) */
    for (i = 0; i < key_char_count - 1; i++)
    {
        read(key_file_descriptor, &charBuffer, sizeof(charBuffer));
        if (charBuffer != 32 && (charBuffer < 65 || charBuffer > 90))
        {
            fprintf(stderr, "Bad character detected in \"%s\".\n", argv[2]);
            
            close(plain_text_file_descriptor);
            close(key_file_descriptor);
            exit(1);
        }
    }
    
    lseek(plain_text_file_descriptor, 0, SEEK_SET);
    lseek(key_file_descriptor, 0, SEEK_SET);
    
    /*****************************************************************************************/
    
    
    /* Set up server address struct */
    memset((char *)&serverAddress, '\0', sizeof(serverAddress));    // clear out struct
    portNumber = atoi(argv[3]); // get the port number convert to an integer from a string
    if (portNumber == 0)
    {
        fprintf(stderr, "Not a valid port value.\n");
        exit(1);
    }
    
    
    /* Set up port and server information */
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNumber);
    serverHostInfo = gethostbyname("localhost");        // convert the machine name into a special address
    if (!serverHostInfo)
    {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }
    
    /* Copy in the address */
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address
    
    /* Create socket */
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0)
    {
        fprintf(stderr, "Socket creation failed.\n");
        exit(1);
    }
    
    /* Connect to server */
    if (connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        fprintf(stderr, "CLIENT: ERROR connecting to port %d.\n", portNumber);
        exit(2);
    }
    
    /* Ensure client is connecting to a server it is allowed to connect to.
       otp_dec cannot connect to otp_enc_d */
    memset(bufferCheck, '\0', sizeof(bufferCheck));
    
    charsWritten = (int)send(socketFD, argv[0], strlen(argv[0]), 0);
    if (charsWritten < 0)
    {
        fprintf(stderr, "CLIENT: ERROR writing to socket.\n");
    }
    
    charsRead = (int)recv(socketFD, bufferCheck, sizeof(bufferCheck) - 1, 0);
    if (!strcmp(FALSE, bufferCheck))
    {
        fprintf(stderr, "ERROR: Connect permission denied on port %d\n", portNumber);
        exit(2);
    }
    
    
    /* Preparing to send plain text and generated key to server */
    char buffer[plain_text_char_count];
    char buffer2[key_char_count];
    memset(buffer, '\0', plain_text_char_count);
    memset(buffer2, '\0', key_char_count);
    
    dup2(plain_text_file_descriptor, 0);    /* redirect stdin */
    while (1)
    {
        charBuffer = getchar();
        if (charBuffer == EOF) { break; }
        if (charBuffer == '\n')
        {
            count = 0;
            break;
        }
        
        buffer[count++] = charBuffer;
    }
    
    dup2(key_file_descriptor, STDIN_FILENO);    /* redirect stdin */
    while (1)
    {
        charBuffer = getchar();
        if (charBuffer == EOF) { break; }
        if (charBuffer == '\n')
        {
            count = 0;
            break;
        }
        
        buffer2[count++] = charBuffer;
    }
    
    
    /* Write to the server */
    charsWritten = (int)send(socketFD, buffer, strlen(buffer), 0);
    
    if (charsWritten < 0)
    {
        fprintf(stderr, "CLIENT: ERROR writing to socket.\n");
    }
    if (charsWritten < strlen(buffer))
    {
        printf("CLIENT: WARNING: Not all data written to socket!\n");
    }
    
    memset(buffer, '\0', sizeof(buffer));
    
    /* Get return message from server */
    charsRead = (int)recv(socketFD, buffer, sizeof(buffer) - 1, 0);  /* Leave \0 at the end */
    if (charsRead < 0)
    {
        fprintf(stderr, "CLIENT: ERROR reading from socket.\n");
    }
    
    
    charsWritten = (int)send(socketFD, buffer2, strlen(buffer2), 0);
    if (charsWritten < 0)
    {
        fprintf(stderr, "CLIENT: ERROR writing to socket.\n");
    }
    if (charsWritten < strlen(buffer2))
    {
        printf("CLIENT: WARNING: Not all data written to socket!\n");
    }
    
    memset(buffer, '\0', sizeof(buffer));
    
    charsRead = (int)recv(socketFD, buffer, sizeof(buffer) - 1, 0);  /* Leave \0 at the end */
    if (charsRead < 0)
    {
        fprintf(stderr, "CLIENT: ERROR reading from socket.\n");
    }
    
    printf("%s\n", buffer);
    
    close(socketFD);
    close(plain_text_file_descriptor);
    close(key_file_descriptor);
    
    return 0;
}
