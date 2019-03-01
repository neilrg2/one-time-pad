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
    
    char *key_ptr;
    
    int key_size = (int)strtol(argv[1], &key_ptr, 10);
    int i, random_value, file_descriptor;
    
    char generated_key[key_size + 1];
    
    for (i = 0; i < key_size; i++)
    {
        random_value = rand() % (91 - 32) + 32;
        while (random_value != 32 && (random_value < 65 || random_value > 90))
        {
            random_value = rand() % (91 - 32) + 32;
        }
        generated_key[i] = random_value;
    }
    generated_key[key_size] = '\n';
    
    if (argv[2] && !strcmp(argv[2], ">"))
    {
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
    
    write(STDOUT_FILENO, generated_key, key_size + 1);
    
    return 0;
}

