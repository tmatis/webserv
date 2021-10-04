# AUTHORIZED FUNCTIONS

## htonl(), htons()

### why ?

In a computer bytes are ordered in a way that is not the same as in the network so if we want to transmit 
data over the network, we have to use this function to order our bytes in the right way.

### prototype

```c
uint32_t htonl(uint32_t hostlong); // if we want to convert a long
uint16_t htons(uint16_t hostshort); // if we want to convert a short
```

### example

```c
#include <netinet/in.h>
#include <stdio.h>

int main(void)
{
	uint16_t n_host = 8;
	uint16_t n_net = htons(n_host); // we order byte to network
	
	printf("n_host = %#x\n", n_host);
	printf("n_net = %#x\n", n_net);
}
```

this code will give us the following output:
```
n_host = 0x8
n_net = 0x800
```

## ntohs(), ntohl()

### why ?

We saw above that we wanted to order our bytes to be able to transmit over the network.
So, if we need to read from the network we need to re-order byte.

### example
```c
#include <netinet/in.h>
#include <stdio.h>

int main(void)
{
	uint16_t n_host = 8;
	uint16_t n_net = htons(n_host); // we order byte to network
	uint16_t n_net_rev = ntohs(n_net);
	
	printf("n_host = %#x\n", n_host);
	printf("n_net = %#x\n", n_net);
	printf("n_net_rev = %#x\n", n_net_rev);
}
```
this code will give us the following output:
```
n_host = 0x8
n_net = 0x800
n_net_rev = 0x8
```
## select(), pool() and kqueue() / epool()

### which one to take ?
Though these 4 functions do things with different accuracies, they're working pretty much the same way along.

#### *select()* is the most deprecated one and also the most portable
```c
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```
It's used to watch over some bitmasks and see if they could be manipulated.
The first argument is the biggest fd to watch passed into the bitmasks.
The three nexts are respectively bitmasks for reading, writing and catch exceptions though NULL values can be passed instead of real bitmasks if there is nothing to watch over.
The last one is a *timeval* struct for timeout-ing the passive listening of select (could be NULL like the previous three).

*fd_set* is a fixed-size buffer who can be filled with use of macros associated with *select()*, which are :
```c
void FD_ZERO(fd_set *set);
void FD_SET(int fd, fd_set *set);
void FD_CLR(int fd, fd_set *set);
int  FD_ISSET(int fd, fd_set *set);
```
*FD_ZERO* is used to intialize / reset a bitmask, *FD_SET* and *FD_CLR* are respectively used to add and delete a fd in a bitmask and *FD_ISSET* is used to find if a fd is present in a bitmask.

The *select()* function works in this way : if some fds are filled into some bitmasks and are unavailable, the function will sleep until one or multiple fds switch back in a more available state. It edits the bitmasks with the fds whose state has been modified and returns the number of modified fds (let the user using *FD_ISSET* to parse the bitmasks).
-1 is returned in case of error, also fills *errno*.
0 is returned if timeout is filled and no state are updated.

##### Usage's pros / cons of *select()*
Cons :
* Although it's modifiable on some kernels, the FD_SETSIZE is set to 1024 and won't let select() watch over fds who are over this value.
* select() edits the bitmasks at each of his runtime, leave the user to memcpy() their content to reuse them each time.
* the edited bitmasks returned by select() needs to be parse by the user to see which fds has been modified.

Pros :
* seems to be pretty useful for [polling](https://en.wikipedia.org/wiki/Polling_(computer_science)).

## socket()

### what is a socket ?
A socket is a software structure that serves for sending and receiving data across a network. The structure and properties of a socket are defined by an application programming interface (API) for the networking architecture. In Unix-like operating systems, sockets work like file descriptors.

### prototype

```c
int socket(int domain, int type, int protocol);
```

#### domain
Address family the socket can communicate with.
#### type
The two mostly used socket types are **SOCK_STREAM** and **SOCK_DGRAM**, the first one provides a reliable, connection-based bytes stream and the second one supports datagrams (connectionless and may lead to data loss).
#### protocol
Normally only one single protocol exists within the specified domain and type so protocol will be set to 0 but the protocols identifiers can be found in /etc/protocols.

### example

```c
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

int	main(void)
{
	int	serverSocket = socket(AF_INET, SOCK_STREAM, 6);
	if (serverSocket == -1)
	{
		perror("socket() failed ");
		return (EXIT_FAILURE);
	}
	close(serverSocket);
}
```
This exemple creates a socket of *type* **SOCK_STREAM** which can communicate with IPv4 addresses. As *protocol* equals 0, i guess it is TCP but if we want to be sure about that we can specify the value 6 according to /etc/protocols.

### notes
* The type argument may also include an OR bitwise operation with the value **SOCK_NONBLOCK** to mark the socket as nonblocking.
* All sockets MUST BE closed with **close()** before exiting the program.

## bind()

### why ?

When a socket is created it has no address assigned to it. In the case of **SOCK_STREAM** socket, it is necessary to assign it an address with **bind()** before receiving connections.

### prototype

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

### exemple

**struct sockaddr** is a structure describing a generic socket address but for a internet socket address we will rather use **struct sockaddr_in**.

**sockaddr_in** has four members:
* sin_family	= address family
* sin_addr		= unsigned integer to represent the address
* sin_port		= port in network byte order
* sin_zero		= i don't get its purpose yet

```c
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

int	main(void)
{
	int	serverSocket = socket(AF_INET, SOCK_STREAM, 0); // create socket
	
	// create address structure for the socket
	sockaddr_in	hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(8080);
	hint.sin_addr.s_addr = inet_addr("0.0.0.0");
	if (bind(socket, (sockaddr*)&hint, sizeof(hint)) == -1) // bind the address to the socket
	{
		perror("bind() failed ");
		return (EXIT_FAILURE);
	}
}
```

### notes
* Binding a server to an address equals to "0.0.0.0" means that the server will listen to all addresses.
* see **inet_addr()** for a better understanding of the above example.
* i didn't check for socket() failure in this exemple for readability reasons but of course in pratice we should do it

## inet_addr()

### why ?

We usually see and use IPv4 addresses written in numbers-and-dots notation but the **sockaddr_in** structure use unsigned integers.
**inet_addr()** allow us to easily convert an address in a string format to binary in network byte order.

### prototype

```c
in_addr_t inet_addr(const char *cp);
```

### exemple
 
```c
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

int	main(void)
{
	int	localhost		= inet_addr("127.0.0.1");
	int	all_address		= inet_addr("0.0.0.0");
	int	random_address	= inet_addr("192.168.0.24");

	std::cout << "localhost\t= " << localhost << "\n";
	std::cout << "all_address\t= " << all_address << "\n";
	std::cout << "random_address\t= " << random_address << std::endl;
}
```
this code will give us the following output:
```
localhost       = 16777343
all_address     = 0
random_address  = 402696384
```

## listen()

### why ?

Marks a socket as passive, a passive socket is a socket that will be used to accept incoming connections with **accept()**.

### prototype

```c
int listen(int sockfd, int backlog);
```
#### sockfd
socket that will be listening.
#### backlog
max size of the queue of pending connections

### exemple

```c
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int	main(void)
{
	/* ** insert here bind() exemple ** */

	if (listen(serverSocket, SOMAXCONN) == -1)
	{
		perror("listen() failed ");
		return (EXIT_FAILURE);
	}
}
```

### notes
* The maximum value of *backlog* the implementation supports is **SOMAXCONN** (defined in sys/socket.h).