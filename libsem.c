#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>

// TODO 1: Obtain a binary semaphore id, allocating if necessary
int binary_semaphore_allocate(key_t key, int sem_flags)
{
    return (semget(key, 1, IPC_CREAT | sem_flags));
}

// TODO 2: Deallocate a binary semaphore
int binary_semaphore_deallocate(int semid)
{
    return (semctl(semid, 1, IPC_RMID));
}

// TODO 3: Obtain a pre-existing binary semaphore
int binary_semaphore_get(key_t key, int sem_flags)
{
    return (semget(key, 1, sem_flags));
}

// TODO 4: Initialize a binary semaphore with a value of 1
int binary_semaphore_set(int semid)
{
    return semctl(semid, 0, SETVAL, 1); 
}

// TODO 5: Wait for the binary semaphore. Decrement the value by -1
int binary_semaphore_wait(int semid)
{
    struct sembuf operations[1];

    /* Use the first (and only) semaphore. */
    operations[0].sem_num = 0;
    /* Decrement by 1. */
    operations[0].sem_op = -1;
    /* Permit undoing. */
    operations[0].sem_flg = 0;
    /* Perform the operation with semop*/
    return semop(semid, operations, 1);
}

// TODO 6: Post to a binary semaphore: Increment its value by 1
int binary_semaphore_post(int semid)
{
    struct sembuf operations[1];

    /* Use the first (and only) semaphore. */
    operations[0].sem_num = 0;
    /* Increment by 1. */
    operations[0].sem_op = 1;
    /* Permit undoing. */
    operations[0].sem_flg = 0;
    /* Perform the operation with semop*/
    return semop(semid, operations, 1);
}
