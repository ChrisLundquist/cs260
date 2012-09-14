#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 10   // how many pending connections queue will hold
#define BUFFER_SIZE 4096
#define PORT_LENGTH 6

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if(sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void read_config(char* address, char* port) {
    FILE* config = fopen("config.txt", "r");
    fgets(address, INET6_ADDRSTRLEN, config);
    address = strtok(address," \t\r\n");
    fgets(port, PORT_LENGTH, config);
    port = strtok(port," \t\r\n");
    fclose(config);
}

int main(void) {
    char listening_port[PORT_LENGTH];
    char buffer[BUFFER_SIZE];
    int recv_size, send_size;
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes = 1;
    char address[INET6_ADDRSTRLEN];
    int rv;

    read_config(address, listening_port);
    printf("Configured for Port: '%s'\n", listening_port);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if((rv = getaddrinfo(NULL, listening_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                    sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    if(p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure
    if(listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    while(1) {  // main accept() loop

        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        if(new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                address, sizeof address);

        printf("server: got connection from %s\n", address);

        do {
            recv_size = recv(new_fd, buffer, BUFFER_SIZE - 1, 0);

            if(recv_size == -1) {
                perror("recv");
                break;
            }

            buffer[recv_size] = NULL;
            printf("server: got message from %s: %s\n", address, buffer);

            send_size = send(new_fd, buffer, recv_size, 0);
            if( send_size == -1 || send_size != recv_size)
                perror("send");
        } while(recv_size != 0);

        close(new_fd);  // parent doesn't need this
    }
    return 0;
}
