#include "network.h"

int main(int argc, char *argv[]) {
    char done = 0;
    int sockfd, sent_size, recv_size;
    char buffer[BUFFER_SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char port[PORT_LENGTH];
    char remote[INET6_ADDRSTRLEN];

    if(argc == 1) {
        read_config(remote, port);
        printf("Configured for Remote: '%s'\n", remote);
        printf("Configured for Port: '%s'\n", port);
    } else if(argc == 3) {
        //remote = argv[1];
        //port = argv[2];
    } else {
        printf("Provide no arguments to read config.txt or provide a hostname and port\n");
        printf("%s host port", argv[0]);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((rv = getaddrinfo(remote, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if(p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            remote, sizeof remote);
    printf("client: connecting to %s\n", remote);

    while(!done) {
        fgets(buffer, BUFFER_SIZE, stdin);

        sent_size = strlen(buffer);

        sent_size = send(sockfd, buffer, sent_size, 0);
        if( sent_size == -1) {
            perror("send");
            break;
        }

        recv_size = recv(sockfd, buffer, BUFFER_SIZE-1, 0);
        if( recv_size == -1) {
            perror("recv");
            break;
        }

        buffer[recv_size] = '\0';
        printf("client received: %s", buffer);
    }
    close(sockfd);
    return 0;
}
