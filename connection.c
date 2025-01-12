#include "connection.h"

#include <err.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils/xalloc.h"

struct connection_t *add_client(struct connection_t *head, int client_socket)
{
    struct connection_t *new_connection = xmalloc(sizeof(struct connection_t));

    new_connection->client_socket = client_socket;
    new_connection->buffer = NULL;
    new_connection->nb_read = 0;
    new_connection->next = head;

    return new_connection;
}
