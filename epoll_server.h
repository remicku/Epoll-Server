#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "connection.h"

/**
 * \brief The length of the event array, must be greater than zero
 */
#define MAX_EVENTS 64

#define DEFAULT_BUFFER_SIZE 2048

/**
 * \brief Iterate over the struct addrinfo elements to create and bind a socket
 *
 * \param addrinfo: struct addrinfo elements
 *
 * \return The created socket or exit with 1 if there is an error
 *
 * Try to create and connect a socket with every addrinfo element until it
 * succeeds
 *
 */
int create_and_bind(struct addrinfo *addrinfo);

/**
 * \brief Initialize the Addrinfo struct and call create_and bind() and
 * listen(2)
 *
 * \param ip: IP address of the server
 * \param port: Port of the server
 *
 * \return The created socket
 *
 * Initialize the struct addrinfo needed by create_and_bind() before calling
 * it. When create_and_bind() returns a valid socket, set the socket to
 * listening and return it.
 */
int prepare_socket(const char *ip, const char *port);

#endif /* !EPOLL_SERVER_H */
