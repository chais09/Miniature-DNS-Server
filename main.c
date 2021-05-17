#include "helper1.h"

#include <time.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



#define PORT "8053"


int main(int argc, char* argv[]) {
    
    int sockfd, newsockfd, n, re, i;
	unsigned char length[2];
	struct addrinfo hints, *res;
	struct sockaddr_storage client_addr;
	socklen_t client_addr_size;

	// struct tm *info;
    // time_t raw_time;
    char time_buffer[256];

	// open file for write
	FILE *fp;
    fp  = fopen ("dns_svr.log", "w");
	// fprintf(fp, "first test\n");
	fflush(fp);

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
	while(1){
    // printf("test: before accept\n");
	// Accept a connection - blocks until a connection is ready to be accepted
	// Get back a new file descriptor to communicate on
	client_addr_size = sizeof client_addr;
	newsockfd =
		accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_size);
	if (newsockfd == 0){
		continue;
	}
	if (newsockfd < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}
    memset(length, 0, 256);
    printf("test: before read\n");


	// while(1){
		// here the input come
		// read the size of the input
		memset(length, 0, 256);
		int size = 0;
		n = read(newsockfd, length, 2);
		
		// if (n == 0) {
		// 	continue;
		// }
		// else if(n < 0) {
		// 	break;
		// }
		size = (((int)length[0])*16*16) + (int)length[1];
		printf("size = %d\n",size);

		unsigned char buffer_temp[size];
		memset(buffer_temp,0,size);
		
		unsigned char buffer[size + 2];
		memset(buffer,0,size+2); 
		
		// unsigned char buffer[256];
		// memset(buffer,0,256);
		n = read(newsockfd, buffer_temp, size); // n is number of characters read
		if (n < 0) {
			printf("error in read socket client, read length\n");
			perror("ERROR reading from socket in the read length");
			exit(EXIT_FAILURE);
		}

		printf("before read input client\n");
		read_input(buffer_temp, size);
		printf("after read input client\n");





		for (int i=0; i< 2; i++){
			buffer[i] = length[i];
		}
		for (int i = 0; i< size; i++){
			buffer[i+2] = buffer_temp[i];
		}
		if (n < 0) {
			perror("ERROR reading from socket");
			exit(EXIT_FAILURE);
		}
		printf("buffer from client: \n");
		for (int i = 0; i< size+2; i++){
			if(i % 16 == 0){
				printf("\n");
			}
			printf("%x  ", buffer[i]);
		}
		printf("\n");
		






		//connect upStream
		printf("connect to upstream");
		printf("test: first up \n");
		int sockfd2, n2;
		struct addrinfo hints2, *servinfo2, *rp2;
		unsigned char length2[2];
		// unsigned char buffer2[256];

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
		n2 = write(sockfd2, buffer, size+2);
		if (n2 < 0) {
			perror("socket2");
			exit(EXIT_FAILURE);
		}

		// Read message from server
		memset(length2, 0,2);
		n2 = read(sockfd2, length2, 2);
		if (n2 < 0) {
			printf("error in read lenght 2\n");
			perror("read length 2");
			exit(EXIT_FAILURE);
		}

		// the size of the message(excluding 2 bytes length)
		int size2 = 0;
		size2 = (((int)length2[0])*16*16) + (int)length2[1];
		printf("size2 from server = %d\n",size2);

		unsigned char buffer2[size2 + 2];
		unsigned char buffer2_temp[size2];


		memset(buffer2, 0, size2 + 2);
		memset(buffer2_temp, 0, size2);

		printf("test: before reading from upstream \n");
		n2 = read(sockfd2, buffer2_temp, size2);
		if (n2 < 0) {
			perror("read2");
			exit(EXIT_FAILURE);
		}

		printf("before read input upstream\n");
		read_input(buffer2_temp, size2);
		printf("after read input upstream\n");


		// combined length array and buffer_temp2 array into a single array
		// and send back to our client
		for (int i = 0; i< 2; i++){
			buffer2[i] = length2[i];
		}
		for (int i = 0; i< size2; i++){
			buffer2[i+2] = buffer2_temp[i];
		}

		printf("Buffer2 from up stream: \n");
		for (int i = 0; i< size2+2; i++){
			if(i % 16 == 0){
				printf("\n");
			}
			printf("%x  ", buffer2[i]);
		}
		printf("\n");


		//close sockets used to connect to upstream
		close(sockfd2);
		freeaddrinfo(servinfo2);

		// write back to the client
		n2 = write(newsockfd, buffer2,size2+2);


	}
	// close sockets from client
	freeaddrinfo(res);
	close(newsockfd);
	close(sockfd);
	
    return 0;
}
