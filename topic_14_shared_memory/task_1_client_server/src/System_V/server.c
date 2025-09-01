#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>

#define SHM_SIZE 16

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(void){
    char *name = "./server_shm";
    char *msg = "Hi!";

    int file_fd = open(name, O_CREAT | O_RDWR, 0666);
    if(file_fd == -1){
        perror("failed open");
        exit(EXIT_FAILURE);
    }
    close(file_fd);

    key_t mem_key = ftok(name, 1);
    if(mem_key == -1){
        unlink(name);
        perror("failed ftok for shared memory");
        exit(EXIT_FAILURE);
    }

    int mem_id = shmget(mem_key, SHM_SIZE, IPC_CREAT | 0666);
    if(mem_id == -1){
        unlink(name);
        perror("failed shmget");
        exit(EXIT_FAILURE);
    }

    char *shm_ptr = (char*)shmat(mem_id, NULL, 0);
    if (shm_ptr == (void*)-1) {
        shmctl(mem_id, IPC_RMID, NULL);
        unlink(name);
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    key_t sem1_key = ftok(name, 2);
    key_t sem2_key = ftok(name, 3);
    if(sem1_key == -1 || sem2_key == -1){
        perror("failed ftok for semaphore");
        shmdt(shm_ptr);
        shmctl(mem_id, IPC_RMID, NULL);
        unlink(name);
        exit(EXIT_FAILURE);
    }

    int sem1_id = semget(sem1_key, 1, IPC_CREAT | 0666);
    int sem2_id = semget(sem2_key, 1, IPC_CREAT | 0666);
    if(sem1_id == -1 || sem2_id == -1){
        shmdt(shm_ptr);
        shmctl(mem_id, IPC_RMID, NULL);
        unlink(name);
        perror("failed semget");
        exit(EXIT_FAILURE);
    }

    union semun arg;
    arg.val = 0;
    if (semctl(sem1_id, 0, SETVAL, arg) == -1 || semctl(sem2_id, 0, SETVAL, arg)) {
        shmdt(shm_ptr);
        shmctl(mem_id, IPC_RMID, NULL);
        semctl(sem1_id, 0, IPC_RMID);
        semctl(sem2_id, 0, IPC_RMID);
        unlink(name);
        perror("semctl SETVAL failed");
        exit(EXIT_FAILURE);
    }
    struct sembuf op = {0, 1, 0};
    struct sembuf wait_op = {0, -1, 0};

    if (semop(sem1_id, &wait_op, 1) == -1) {
        shmdt(shm_ptr);
        shmctl(mem_id, IPC_RMID, NULL);
        semctl(sem1_id, 0, IPC_RMID);
        semctl(sem2_id, 0, IPC_RMID);
        unlink(name);
        perror("semop decrement1 failed");
        exit(EXIT_FAILURE);
    }

    strncpy(shm_ptr, msg, SHM_SIZE - 1);
    shm_ptr[SHM_SIZE - 1] = '\0';

    if (semop(sem2_id, &op, 1) == -1) {
        shmdt(shm_ptr);
        shmctl(mem_id, IPC_RMID, NULL);
        semctl(sem1_id, 0, IPC_RMID);
        semctl(sem2_id, 0, IPC_RMID);
        unlink(name);
        perror("semop increment failed");
        exit(EXIT_FAILURE);
    }

    if (semop(sem1_id, &wait_op, 1) == -1) {
        shmdt(shm_ptr);
        shmctl(mem_id, IPC_RMID, NULL);
        semctl(sem1_id, 0, IPC_RMID);
        semctl(sem2_id, 0, IPC_RMID);
        unlink(name);
        perror("semop decrement2 failed");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", shm_ptr);

    shmdt(shm_ptr);
    shmctl(mem_id, IPC_RMID, NULL);
    semctl(sem1_id, 0, IPC_RMID);
    semctl(sem2_id, 0, IPC_RMID);

    unlink(name);
    return 0;
}
