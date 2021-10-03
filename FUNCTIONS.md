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
