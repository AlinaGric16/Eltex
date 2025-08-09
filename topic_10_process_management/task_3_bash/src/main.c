#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LEN 10

int main(){
    char input[MAX_LEN] = {0};
    int status = 0;

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

        pid_t child_pid = fork();

        if(child_pid < 0){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if(child_pid == 0){

            char *token = strtok(input, " ");
            char *args[MAX_LEN] = {0};
            int i = 0;

            while(token != NULL){
                args[i++] = token;
                token = strtok(NULL, " ");
            }
            args[i] = NULL;

            int ex = execvp(args[0], args);

            exit(ex);
        } else{
            wait(&status);
            if(WEXITSTATUS(status) != 0)
                perror("Command not found");
        }
    }
    return 0;
}