#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LEN 20

int main(){
    char input[MAX_LEN] = {0};
    int status = 0;
    int pipefd[2];

    printf("Аналог командного инерпритатора bash\n");
    printf("(для выхода введите 'exit')\n");

    while(1){
        printf("$ ");
        fflush(stdout);

        if(!fgets(input,  MAX_LEN+1, stdin)){
            perror("input failed");
            exit(EXIT_FAILURE);
        }

        if(strlen(input) == 10 && input[9] != '\n') 
            while(getchar() != '\n');
        input[strcspn(input, "\n")] = '\0';

        if(!strncmp(input, "exit", 4)) exit(EXIT_SUCCESS);

        if(pipe(pipefd) < 0){
            perror("failed pipe");
            exit(EXIT_FAILURE);
        }

        char *token = strtok(input, " ");
        char *args[MAX_LEN] = {0};
        int i = 0;

        while(token != NULL && strncmp(token, "|", 1)){
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;


        pid_t child_pid = fork();

        if(child_pid < 0){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if(child_pid == 0){
            if(token != NULL && strncmp(token, "|", 1) == 0){
                close(pipefd[0]);
                dup2(pipefd[1], 1);
            }
            
            int ex = execvp(args[0], args);

            exit(ex);
        } else{
            if(token == NULL){
                close(pipefd[0]);
                close(pipefd[1]);
            }
            wait(&status);
            if(WEXITSTATUS(status) != 0)
                perror("Command not found");
            if(token != NULL && strncmp(token, "|", 1) == 0){
                i = 0;
                token = strtok(NULL, " ");
                while(token != NULL){
                    args[i++] = token;
                    token = strtok(NULL, " ");
                }
                args[i] = NULL;

                child_pid = fork();

                if(child_pid < 0){
                    perror("fork failed");
                    exit(EXIT_FAILURE);
                }
                if (child_pid == 0){
                    close(pipefd[1]);
                    dup2(pipefd[0], 0);
                    int ex = execvp(args[0], args);

                    exit(ex);
                } else{
                    close(pipefd[0]);
                    close(pipefd[1]);
                    wait(&status);
                    if(WEXITSTATUS(status) != 0)
                        perror("Command not found");
                }
            }
        }
    }
    return 0;
}
