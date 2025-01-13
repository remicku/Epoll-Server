#include "epoll_server.h"

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "connection.h"

int create_and_bind(struct addrinfo *addrinfo)
{
    int sfd;

    // Pointer used to iterate over the linked list
    struct addrinfo *p;

    for (p = addrinfo; p != NULL; p = p->ai_next)
    {
        // Try to create a socket
        sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sfd == -1)
            continue;

        int optval = 1;

        int err =
            setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

        if (err == -1)
        {
            close(sfd);
            continue;
        }

        // Try to connect the socket
        err = bind(sfd, p->ai_addr, p->ai_addrlen);

        // If successful, break the loop (p != 0)
        // If error, close the socket
        if (err == 0)
            return sfd;

        close(sfd);
    }

    return -1;
}

int prepare_socket(const char *ip, const char *port)
{
    // Create the 'hints' structure.
    struct addrinfo hints;

    // Initialize all the fields to zero.
    memset(&hints, 0, sizeof(hints));

    // Specify the criteria
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // Server

    // Create a pointer to store the result
    struct addrinfo *result;

    // Get the linked list
    int err = getaddrinfo(ip, port, &hints, &result);
    if (err != 0)
    {
        exit(1);
    }

    int sfd = create_and_bind(result);
    freeaddrinfo(result);

    int status = listen(sfd, SOMAXCONN);

    if (status == -1)
    {
        close(sfd);
        errx(EXIT_FAILURE, "error socket can't be set to listen mode");
    }

    return sfd;
}
