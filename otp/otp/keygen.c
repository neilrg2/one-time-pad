#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, const char *argv[])
{
    srand(time(NULL));
    if (argc < 2) { fprintf(stderr, "Key length was not specified.\n"); exit(1); }
    
    char *key_ptr;
    
    int key_size = (int)strtol(argv[1], &key_ptr, 10); /* Convert second argument to long with int cast */
    int i, random_value, file_descriptor;
    
    char generated_key[key_size + 1];
    
    
    /* Generate random values from the allowed 27 characters and store into string */
    for (i = 0; i < key_size; i++)
    {
        random_value = rand() % (91 - 32) + 32;
        
        /* Follows ASCII values of characters (65-90 capital letters) and 32 is the space character;
          Continue to loop until the integer returned from rand is either 32 or a value between
          65 and 90 */
        while (random_value != 32 && (random_value < 65 || random_value > 90))
        {
            random_value = rand() % (91 - 32) + 32;
        }
        generated_key[i] = random_value;
    }
    generated_key[key_size] = '\n'; /* Append last character as a newline */
    
    
    /* Checks if there is an argument at index 2 and if there is, if that argument
       is a stdout redirection character */
    if (argv[2] && !strcmp(argv[2], ">"))
    {
        
        /* Opens file for redirection specified as command line argument at index 3;
          dup2 call for stdout redirection to specified file */
        if (argc == 4 && argv[3])
        {
            file_descriptor = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, 0755);
            if (file_descriptor < 0)
            {
                fprintf(stderr, "File: \"%s\" could not be opened.\n", argv[3]);
                exit(1);
            }
            
            dup2(file_descriptor, 1);
        }
        else
        {
            fprintf(stderr, "No specified file for stdout redirection.\n");
            exit(1);
        }
    }
    
    /* If file redirection occurred, the randomly generated string will be written
       to the specified file. Else, the string will be read to the screen as normal
       use of stdout */
    write(STDOUT_FILENO, generated_key, key_size + 1);
    close(STDOUT_FILENO);
    return 0;
}

