#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sig.h"
#include "em_registers.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "sock.h"

#define FIFO_NAME "myfifo"
#define PORT_NUM 2000
int main()
{
    int fd;
    int my_addr_len;
    struct em_registers reg;
    struct sockaddr_in my_addr;

    // TODO 1: Create the FIFO
    //mkfifo(FIFO_NAME, 0666);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket");
        return -1;
    }
    char ip_addr[100]="127.0.0.1";
    my_addr.sin_family = AF_INET; // address family
    my_addr.sin_port = htons(PORT_NUM); // short, network byte order
    my_addr.sin_addr.s_addr = inet_addr(ip_addr);
    bzero(&(my_addr.sin_zero), 8);

    //printf("Waiting for writers ...\n");
    // TODO 2: Open the FIFO
    //fd = open(FIFO_NAME, O_RDONLY);

     printf("Connecting socket to %s ... ", ip_addr);
    // TODO 3: Connect to the socket
    if (connect(fd, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0) {
        printf("connection with the server failed...\n");
        return -1;
    }

    printf("done\n");

    if (getsockname(fd, (struct sockaddr *)&my_addr, &my_addr_len) == -1)
    {
        perror("getsockname");
    }
    else
    {
        printf("Got (ephemeral) Port # %hu on IP %s\n",
                ntohs(my_addr.sin_port), inet_ntoa(my_addr.sin_addr));
    }

 
    printf("Got a writer:\n");

    while ((read_eth(fd, &reg, sizeof(struct em_registers))) > 0)
    {
        printf("Va = %u, Vb = %u Vc = %u\n", reg.va, reg.vb, reg.vc);
        printf("Va Max = %u, Vb Max = %u Vc Max = %u\n", reg.va_max, reg.vb_max, reg.vc_max);
    }
   
    close(fd);

    return 0;
}
