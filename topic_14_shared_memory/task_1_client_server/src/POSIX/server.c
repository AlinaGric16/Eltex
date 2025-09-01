#include <sys/stat.h> 
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

#define SHM_SIZE 16

int main(void){
    char *name_mem = "/shm_server";
    char *name_sem1 = "/sem1_server";
    char *name_sem2 = "/sem2_server";
    char *msg = "Hi!";

    int fd = shm_open(name_mem, O_CREAT | O_RDWR, 0666);
    if(fd == -1){
        perror("failed shm_open");
        exit(EXIT_FAILURE);
    }

    if(ftruncate(fd, SHM_SIZE) == -1){
        shm_unlink(name_mem);
        close(fd);
        perror("failed ftruncate");
        exit(EXIT_FAILURE);
    }

    char *shm_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shm_ptr == MAP_FAILED){
        shm_unlink(name_mem);
        close(fd);
        perror("failed mmap");
        exit(EXIT_FAILURE);
    }

    sem_t *sem1 = sem_open(name_sem1, O_CREAT, 0666, 0);
    sem_t *sem2 = sem_open(name_sem2, O_CREAT, 0666, 0);
    if(sem1 == SEM_FAILED || sem2 == SEM_FAILED){
        munmap(shm_ptr, SHM_SIZE);
        shm_unlink(name_mem);
        close(fd);
        perror("failed sem_open");
        exit(EXIT_FAILURE);
    }

    if(sem_wait(sem1) == -1){
        sem_close(sem1);
        sem_close(sem2);
        sem_unlink(name_sem1);
        sem_unlink(name_sem2);
        munmap(shm_ptr, SHM_SIZE);
        shm_unlink(name_mem);
        close(fd);
        perror("failed sem_wait");
        exit(EXIT_FAILURE);
    }

    strncpy(shm_ptr, msg, SHM_SIZE-1);
    shm_ptr[SHM_SIZE - 1] = '\0';

    if(sem_post(sem2) == -1){
        sem_close(sem1);
        sem_close(sem2);
        sem_unlink(name_sem1);
        sem_unlink(name_sem2);
        munmap(shm_ptr, SHM_SIZE);
        shm_unlink(name_mem);
        close(fd);
        perror("failed sem_wait");
        exit(EXIT_FAILURE);
    }

    if(sem_wait(sem1) == -1){
        sem_close(sem1);
        sem_close(sem2);
        sem_unlink(name_sem1);
        sem_unlink(name_sem2);
        munmap(shm_ptr, SHM_SIZE);
        shm_unlink(name_mem);
        close(fd);
        perror("failed sem_wait");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", shm_ptr);
    
    sem_close(sem1);
    sem_close(sem2);
    sem_unlink(name_sem1);
    sem_unlink(name_sem2);
    munmap(shm_ptr, SHM_SIZE);
    shm_unlink(name_mem);
    close(fd);
    return 0;
}
