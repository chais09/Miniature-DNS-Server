#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define PORT "8053"


int main(int argc, char* argv[]) {
    
    int sockfd, newsockfd, n, re, i;
	unsigned char buffer[256];
	struct addrinfo hints, *res;
	struct sockaddr_storage client_addr;
	socklen_t client_addr_size;

    printf("test: first\n");
    // Create address we're going to listen on (with given port number)
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET6;       // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE;     // for bind, listen, accept
	// node (NULL means any interface), service (port), hints, res
	getaddrinfo(NULL, PORT, &hints, &res);

    printf("test: before sockfd\n");
	// Create socket
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

    printf("test: before setsockopt\n");
    // Reuse port if possible
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt");
        exit(1);
    }
	
    printf("test: before bind\n");
	// Bind address to the socket
	if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

    printf("test: before listen\n");
	// Listen on socket - means we're ready to accept connections,
	// incoming connection requests will be queued, man 3 listen
	if (listen(sockfd, 5) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

    printf("test: before accept\n");
	// Accept a connection - blocks until a connection is ready to be accepted
	// Get back a new file descriptor to communicate on
	client_addr_size = sizeof client_addr;
	newsockfd =
		accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_size);
	if (newsockfd < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}
    memset(buffer, 0, 256);
    printf("test: before read\n");
    n = read(newsockfd, buffer, 255); // n is number of characters read
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(EXIT_FAILURE);
    }
    printf("buffer from client: \n");
    for (int i = 0; i< n; i++){
        if(i % 16 == 0){
            printf("\n");
        }
        printf("%x  ", buffer[i]);
    }
    printf("\n");
    // for (int i=0; i< n; i++){
    //     printf("buffer[%d] = %x\n", i, buffer[i]);
    // }  


    //connect upStream
    printf("connect to upstream");
    printf("test: first up \n");
    int sockfd2, n2;
	struct addrinfo hints2, *servinfo2, *rp2;
	unsigned char buffer2[256];

	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Create address
	memset(&hints2, 0, sizeof hints2);
	hints2.ai_family = AF_INET;
	hints2.ai_socktype = SOCK_STREAM;

    printf("test: before getaddrinfo up \n");
	// Get addrinfo of server. From man page:
	// The getaddrinfo() function combines the functionality provided by the
	// gethostbyname(3) and getservbyname(3) functions into a single interface
	if ((getaddrinfo(argv[1], argv[2], &hints2, &servinfo2)) < 0) {
        printf("error in getaddrinfo 2\n");
		perror("getaddrinfo2");
		exit(EXIT_FAILURE);
	}
    printf("sizeof(unsinged char) = %d\n", sizeof(unsigned char));
    printf("test: before sockfd up\n");
	// Connect to first valid result
	// Why are there multiple results? see man page (search 'several reasons')
	// How to search? enter /, then text to search for, press n/N to navigate
	for (rp2 = servinfo2; rp2 != NULL; rp2 = rp2->ai_next) {
		sockfd2 = socket(rp2->ai_family, rp2->ai_socktype, rp2->ai_protocol);
		if (sockfd2 == -1)
			continue;

		if (connect(sockfd2, rp2->ai_addr, rp2->ai_addrlen) != -1)
			break; // success

		close(sockfd2);
	}
	if (rp2 == NULL) {
		fprintf(stderr, "client: failed to connect to up stream\n");
		exit(EXIT_FAILURE);
	}

    printf("test: before write to upstream \n");
    n2 = write(sockfd2, buffer, n);
    if (n2 < 0) {
        perror("socket2");
        exit(EXIT_FAILURE);
    }

    // Read message from server
    memset(buffer2, 0, 256);
    printf("test: before reading from upstream \n");
    n2 = read(sockfd2, buffer2, 255);
    if (n2 < 0) {
        perror("read2");
        exit(EXIT_FAILURE);
    }

    printf("Buffer2 from up stream: \n");
    for (int i = 0; i<n2; i++){
        if(i % 16 == 0){
            printf("\n");
        }
        printf("%x  ", buffer2[i]);
    }
    printf("\n");


    //close sockets used to connect to upstream
    close(sockfd2);
	freeaddrinfo(servinfo2);




    // close sockets from client
    freeaddrinfo(res);
	close(newsockfd);
	close(sockfd);
    return 0;
}
