#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_SIZE 16

int main(void){
    char *name = "./server_shm";
    char *msg = "Hello!";

    key_t mem_key = ftok(name, 1);
    if(mem_key == -1){
        perror("failed ftok for shared memory");
        exit(EXIT_FAILURE);
    }

    int mem_id = shmget(mem_key, SHM_SIZE, 0);
    if(mem_id == -1){
        perror("failed shmget");
        exit(EXIT_FAILURE);
    }

    char *shm_ptr = (char*)shmat(mem_id, NULL, 0);
    if (shm_ptr == (void*)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    key_t sem1_key = ftok(name, 2);
    key_t sem2_key = ftok(name, 3);
    if(sem1_key == -1 || sem2_key == -1){
        shmdt(shm_ptr);
        perror("failed ftok for semaphore");
        exit(EXIT_FAILURE);
    }

    int sem1_id = semget(sem1_key, 1, 0);
    int sem2_id = semget(sem2_key, 1, 0);
    if(sem1_id == -1 || sem2_id == -1){
        shmdt(shm_ptr);
        perror("failed semget");
        exit(EXIT_FAILURE);
    }

    struct sembuf op = {0, 1, 0};
    struct sembuf wait_op = {0, -1, 0};

    if (semop(sem1_id, &op, 1) == -1) {
        shmdt(shm_ptr);
        perror("semop increment1 failed");
        exit(EXIT_FAILURE);
    }

    if (semop(sem2_id, &wait_op, 1) == -1) {
        shmdt(shm_ptr);
        perror("semop decrement failed");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", shm_ptr);

    strncpy(shm_ptr, msg, SHM_SIZE - 1);
    shm_ptr[SHM_SIZE - 1] = '\0';

    if (semop(sem1_id, &op, 1) == -1) {
        shmdt(shm_ptr);
        perror("semop increment2 failed");
        exit(EXIT_FAILURE);
    }

    shmdt(shm_ptr);
    return 0;
}
