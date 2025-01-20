
# Epoll Server

## Overview

This project is a simple yet powerful multi-client TCP server implemented using the `epoll` Linux syscall. The server handles multiple client connections efficiently, using non-blocking I/O and epoll for event-driven communication.

## Features

- **Efficient Multiplexing:** Uses `epoll` to monitor multiple sockets.
- **Broadcast Functionality:** Messages from one client are sent to all connected clients.
- **Graceful Client Management:**
  - Handles client disconnection properly.
  - Supports receiving and sending fragmented messages.

## Usage

### Build Instructions

This project uses the Meson build system. To build the server:

1. Install Meson if it's not already installed:
   ```bash
   sudo apt install meson
   ```
2. Set up the build directory:
   ```bash
   meson setup build
   ```
3. Compile the project:
   ```bash
   meson compile -C build
   ```

### Run the Server

The server requires two arguments: an IP address and a port number.

Example:
   ```bash
   ./epoll_server 127.0.0.1 8000
   ```

### Testing

1. To test the server, you can use `netcat`:
   ```bash
   ./epoll_server 127.0.0.1 8000
   ```
2. Connect clients using:
   ```bash
   nc 127.0.0.1 8000
   ```
3.  Type messages in one client to see them broadcasted to others.

### Example Interaction

**Client A:**
   ```plaintext
Hello from A!
   ```
**Client B receives:**
   ```plaintext
Hello from A!
   ```

## Project Structure

-   `epoll_server.c`: Main server logic, including epoll integration and the event loop.
-   `connection.c`: Helper functions for managing client connections.
-   `utils/`:
    -   `xalloc.c`: Implements safer versions of standard memory management functions:
-   `meson.build`: Build configuration file for the project.

## Key Functions and Concepts

-   **epoll_create1:** Creates an epoll instance for monitoring file descriptors.
-   **epoll_ctl:** Adds or modifies file descriptors in the epoll interest list.
-   **epoll_wait:** Waits for events on monitored file descriptors.
-   **Non-blocking I/O:** Prevents the server from stalling on I/O operations.

## Challenges and Lessons

This project reinforced key networking and system programming concepts:

-   Using non-blocking sockets to avoid server bottlenecks.
-   Managing dynamic lists of clients efficiently.
-   Understanding and handling edge cases like fragmented messages and client disconnections.
