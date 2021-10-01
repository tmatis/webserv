# AUTHORIZED FUNCTIONS

## htonl(), htons()

### why ?

In a computer byte are ordered in a way that is not the same as in the network so if we want to transmit 
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
