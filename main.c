#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// spm -> Space Monkey 
char **spm_parse(char*);
void spm_launch(char** args);

int main(int argc, char *argv[]) {

    char *line_ptr = NULL;
    size_t nread;
    size_t len = 0;
    char** parsed_commands;

    printf("$ ");
    fflush(stdin);

    nread = getline(&line_ptr, &len, stdin);
    
    if(nread == -1) {
        fprintf(stderr, "Error getting data");
        exit(EXIT_FAILURE);
    }

    parsed_commands = spn_parse(line_ptr);

    spm_launch(parsed_commands);


    /*
    char* curr = parsed_commands[0];
    int i = 1;
    while(curr !=  NULL) {
        printf("%s \n", curr);
        curr = parsed_commands[i];
        i++;
    }
    */

    free(parsed_commands);
    free(line_ptr);
    exit(EXIT_SUCCESS);
}

#define TOKENV_SIZE 64
#define TOKEN_DELIMITER " \t\r\n\a"
char **spm_parse(char *line) {


    int curr_buf_size = TOKENV_SIZE;
    char **token_vector;
    char *token;
    int counter = 0;

    token_vector = calloc(curr_buf_size, sizeof(char*));

    if(token_vector == NULL) { // or (!token_vector)
        perror("allocation error");
        exit(EXIT_FAILURE);
    }
    
    token = strtok(line, TOKEN_DELIMITER);
    printf("%s \n", token);

    if(token == NULL) {
        fprintf(stderr, "Failed to parse");
        exit(EXIT_FAILURE);
    }

    token_vector[counter] = token;
    counter++;

    while((token = strtok(NULL, TOKEN_DELIMITER)) != NULL) {

        printf("%s \n", token);

        if(counter >= curr_buf_size) {
            curr_buf_size += TOKENV_SIZE;
            token_vector = realloc(token_vector, curr_buf_size);

            if(!token_vector) {
                fprintf(stderr, "error reallocating during parse");
                exit(EXIT_FAILURE);
            }
        }

        token_vector[counter] = token;
        counter++;

    }

    token_vector[counter] = NULL;
    return token_vector;
}

void spm_launch(char **args) {

    __pid_t curr_pid;

    curr_pid = fork();

    switch (curr_pid)
    {
    case -1:
        perror("Fork error \n");
        exit(EXIT_FAILURE);
        break;
    case 0:
        char* binary_path = "/bin/cat";
        char* call_args[] = {binary_path, args[0], NULL};
        char *env[] = {NULL};
        
        puts("I'm the child !! \n");

        if(execve(binary_path, call_args, env) == -1) {
            perror("Error in execve");
        }

        exit(EXIT_SUCCESS);
        break;

    default:
       
        printf("Child Pid : %jd\n", (__intmax_t) curr_pid);
        puts("Im the parent !!");
        break;
    }

}