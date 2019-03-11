#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "6500"


void *get_in_addr(struct sockaddr *sa) {
    return &((struct sockaddr_in*)sa)->sin_addr;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("you have to enter a valid filename");
        return 1;
    }
    int sockfd;
    struct addrinfo hints, *servinfo;
    int rv;
    char s[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    if (servinfo == NULL) {
        printf("Could not establish a connection to server");
        return 2;
    }

    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return 3;
    }

    if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(sockfd);
        fprintf(stderr, "connect error: %s\n", strerror(errno));
        return 4;
    }

    inet_ntop(servinfo->ai_family, get_in_addr((struct sockaddr *)servinfo->ai_addr), s, sizeof(s));

    printf("Connecting to: %s\n", s);

    freeaddrinfo(servinfo);

    char buffer[1024];
    bzero(buffer, 1024);

    FILE *fptr;
    fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        printf("Error in opening file");
        return 3;
    }

    send(sockfd, argv[1], strlen(argv[1]), 0);

    while (fgets(buffer, 1024, fptr)) {
        printf("%s\n", buffer);
        send(sockfd, (char*)buffer, strlen(buffer), 0);
        bzero(buffer, 1024);
    }

    printf("Closing socket\n");

    close(sockfd);

    return 0;
}
