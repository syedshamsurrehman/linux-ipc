#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <sys/shm.h>

#include "sig.h"
#include "em_registers.h"
#include "sem.h"
#include "sock.h"
#include <pthread.h>
#include <unistd.h>

#define FIFO_NAME "myfifo"
#define KEY 0x12345 // Would retrieve the same if not detached
#define PORT_NUM 2000



void sig(int signum)
{
    printf("Received signal %d\n", signum);
}

int get_lib_count()
{
    int libcount = 0;
    // TODO 6: Open the pipe with ls lib*.so and return the lib count
    FILE *pipein_fp;
    char readbuf[2000];
    
    pipein_fp = popen("ls lib*.so", "r");
    if (!pipein_fp) { 
        printf("Could not open pipein");
        return -1;
    }
    
    while (!feof(pipein_fp)) {
       fgets(readbuf, 80, pipein_fp);
       printf("%s\n", readbuf);
       libcount++;
    }
    
    return libcount;
    
    return -1;
}

void *shm_allocate(key_t key, size_t shm_size,
        int flags, int *shm_id, void *addr)
{
    // TODO 7: Allocate the shared memory
      *shm_id = shmget(key, shm_size, flags);
    printf("The segment id: %d (0x%X)\n", *shm_id, *shm_id);

    // TODO 8: Attach the shared memory
    return(shmat(*shm_id, addr, 0));
}

struct em_registers reg;
pthread_cond_t pseudo_barrier_complete_cond;
pthread_mutex_t pseudo_barrier_mux;
int pseudo_barrier_complete_flag = 0;

void *process_client(void *arg)
{
    int *pfd;
    int fd;
    int num;
    if (arg == NULL)
    {   
        printf("Invalid arg\n");
        return NULL;
    }
    pfd = arg;
    fd = pfd;
    
    
    while (1)
    {
        while (!pseudo_barrier_complete_flag) {
            pthread_cond_wait( &pseudo_barrier_complete_cond, &pseudo_barrier_mux);
        }
        pseudo_barrier_complete_flag = 0;
        pthread_mutex_unlock( &pseudo_barrier_mux);
        num = write_eth(fd, &reg, sizeof(struct em_registers));
        if (num == -1) {
           perror("write");
        }
        else
        {
           printf("speak: wrote %d bytes\n", num);
        }
    }
    return NULL;
}

void *accept_thread(void *arg)
{
    int *fd;
    int efd;
    pthread_t thread;
    char remote_ip[100];

    if (arg == NULL)
    {
        printf("Invalid arg\n");
        return NULL;
    }
    fd = arg;
    printf("fd is %d\n", fd);
    while (1)
    {
       efd = get_socket(fd, remote_ip);
       if (efd == -1)
          return NULL;
       printf("Accept true client IP %s\n", remote_ip);
       pthread_create(&thread, NULL,
                          &process_client, (void *)efd);
    }
    return NULL;
}


#define MAX_CLIENT 20

int main()
{
    int fd, libcount;
    int shm_id;
    char *shm_addr = NULL;
    const int shm_size = sizeof(struct em_registers);
    int sem_id;
    pthread_t accept_thread_id;

    pthread_cond_init( &pseudo_barrier_complete_cond, NULL);
    pthread_mutex_init( &pseudo_barrier_mux, NULL);

    memset(&reg, 0, sizeof(struct em_registers));
    reg.va = 440;
    reg.vb = 438;
    reg.vc = 430;
    
    // TODO 1: Register handler sig for SIGINT and SIGPIPE
  sigset_t sa_mask;
    struct sigaction old_act;
    sigemptyset (&sa_mask);
    signal_register(SIGINT, sig, &old_act, &sa_mask);
    signal_register(SIGPIPE, sig, &old_act, &sa_mask);

    // TODO 2: Get the count of lib*.so files using pipe
    libcount = get_lib_count();
    if (libcount > 0)
    {
        printf("Library count = %d\n", libcount);
    }

    // TODO 9: Allocate the shared memory
  shm_addr = shm_allocate(KEY, shm_size, IPC_CREAT | S_IRUSR | S_IWUSR, &shm_id, shm_addr);
    printf("Shared memory attached at address %p\n", shm_addr);
    printf("Shared memory attached at address %p\n", shm_addr);

    // TODO 10: Allocate the binary semaphore
    sem_id = binary_semaphore_allocate(KEY, S_IRUSR | S_IWUSR);
    printf("Semaphore created with id: %d\n", sem_id);

    // TODO 11: Initialize the binary semaphore
    binary_semaphore_set(sem_id);
    printf("1: Semaphore set to 1\n");
    
    // TODO 3: Create the FIFO
    //mkfifo(FIFO_NAME, 0666);
    printf("Waiting for readers ...\n");
    // TODO 4: Open the FIFO
    //fd = open(FIFO_NAME, O_WRONLY);
    char ip_addr[100] = "127.0.0.1";
    fd = open_socket(ip_addr, PORT_NUM);
    printf("Passing fd %d\n", fd);
    pthread_create(&accept_thread_id, NULL, &accept_thread, (void *)fd);
    while (1) {
        
    // TODO 12: Get the semaphore
        // TODO 13: Synchronize the access to shared memory using semaphore
        binary_semaphore_wait(sem_id);
        memcpy(&reg, shm_addr, sizeof(struct em_registers)); 
        binary_semaphore_post(sem_id);
        // TODO 5: Write EM Registers to the FIFO

        pthread_mutex_lock( &pseudo_barrier_mux);
        pseudo_barrier_complete_flag = 1;
        pthread_mutex_unlock( &pseudo_barrier_mux);
        pthread_cond_broadcast( &pseudo_barrier_complete_cond);
 
        sleep(5);
        printf("Sent shared registers\n");
    }
    close(fd);

    return 0;
}
