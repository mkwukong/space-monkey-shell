#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


/*Builtins commands previsti da Space Monkey Shell*/
const char* builtins[] = {
    "cd",
    "exit",
    "help"
};

/*Builtins functions typedef*/
typedef int (*builtins_type)(char**);

// spm -> Space Monkey 
char **spm_parse(char*);
int spm_execute(char**);
void spm_launch(char**);

int spm_exit_builtin(char**);
int spm_cd_builtin(char**);

/*Address dei builtins di sistema*/
int (*builtins_addr[]) (char**) = {
    &spm_cd_builtin,
    &spm_exit_builtin
};


int main(int argc, char *argv[]) {

    while(1) {
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

        parsed_commands = spm_parse(line_ptr);

        spm_execute(parsed_commands);

        free(parsed_commands);
        free(line_ptr);
    }
    //exit(EXIT_SUCCESS);
}

/*Parsing dell'input in stdin*/
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


/*Exit builtin*/
int spm_exit_builtin(char** args) {
    printf("I'm exiting !!");
    exit(EXIT_SUCCESS);
}

/*Change Directory (CD) builtin*/
int spm_cd_builtin(char** args) {

    printf("Changing directory ... \n");
    char* cd_target = args[1];
    
    char buf[1024];
    char* wd;

    /*Print WD and then TD (target directory)*/

    wd = getcwd(buf, 1024);
    if(wd == NULL) {
        perror("Failed getcwd");
        exit(EXIT_FAILURE);
    }

    printf("CD FROM %s \n", buf);

    if(chdir(cd_target) == 0) {
        wd = getcwd(buf, 1024);

        if(wd == NULL) {
            perror("Failed getcwd \n");
            exit(EXIT_FAILURE);
        } 
        printf("TO %s \n", buf);

    } else {
        perror("Failed change directory \n");
        exit(EXIT_FAILURE);
    }

}


int spm_execute(char **args) {

    char* main_command = args[0];

    if(main_command ==  NULL) {
        return 1;
    }

    int builtins_num = sizeof(builtins) / sizeof(char *);

    for(int i=0; i<builtins_num; i++) {
        if(strcmp(main_command, builtins[i]) == 0) {
            printf("FOUND \n");
            return (*builtins_addr[i])(args);
        }
    }
}

/**/
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
        puts("I'm the child !! \n");
        char* binary_path = "/bin/cat";
        char* call_args[] = {binary_path, args[0], NULL};
        char *env[] = {NULL};

        /*
        if(execve(binary_path, call_args, env) == -1) {
            perror("Error in execve");
        }
        */

        exit(EXIT_SUCCESS);
        break;

    default:

        printf("Child Pid : %jd\n", (__intmax_t) curr_pid);
        puts("Im the parent !!");
        
        __pid_t exit_pid;
        int status;
        
        exit_pid = waitpid(curr_pid, &status, WUNTRACED);

        if(exit_pid == -1) {
            perror("Waitpid error");
            exit(EXIT_FAILURE);
        }
        break;
    }

}