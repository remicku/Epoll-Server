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

struct connection_t *accept_client(int epoll_instance, int server_socket,
                                   struct connection_t *connection)
{
    int sfd = accept(server_socket, NULL, NULL);

    if (sfd == -1)
    {
        return connection;
    }

    fcntl(sfd, F_GETFL);

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = sfd;

    if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, sfd, &event) == -1)
    {
        close(sfd);
        errx(EXIT_FAILURE, "error can't add client to epoll");
    }

    return add_client(connection, sfd);
}

void aux_send(struct connection_t *tmp, char *buff, ssize_t r)
{
    while (tmp != NULL)
    {
        send(tmp->client_socket, buff, r, 0);
        tmp = tmp->next;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        return 1;
    }

    int sfd = prepare_socket(argv[1], argv[2]);
    fcntl(sfd, F_GETFL);

    int epoll_instance = epoll_create1(0);

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = sfd;

    if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, sfd, &event) == -1)
    {
        err(EXIT_FAILURE, "error add server socket to epoll");
    }

    struct connection_t *connection = NULL;
    struct epoll_event events[MAX_EVENTS];

    while (1)
    {
        int events_count = epoll_wait(epoll_instance, events, MAX_EVENTS, -1);

        if (events_count == -1)
        {
            err(EXIT_FAILURE, "error epoll_wait");
        }

        for (int i = 0; i < events_count; i++)
        {
            if (events[i].data.fd == sfd)
            {
                connection = accept_client(epoll_instance, sfd, connection);
            }
            else
            {
                char buff[DEFAULT_BUFFER_SIZE];
                ssize_t r =
                    recv(events[i].data.fd, buff, DEFAULT_BUFFER_SIZE - 1, 0);

                if (r <= 0)
                {
                    if (r == 0)
                        connection =
                            remove_client(connection, events[i].data.fd);
                }
                else
                {
                    buff[r] = '\0';
                    struct connection_t *tmp = connection;

                    aux_send(tmp, buff, r);
                }
            }
        }
    }

    close(sfd);
    close(epoll_instance);

    return 0;
}
