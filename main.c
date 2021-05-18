#include "helper1.h"

#include <time.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>


#define PORT "8053"


int main(int argc, char* argv[]) {
    int sockfd, n;
	struct addrinfo hints, *res;
	struct sockaddr_storage client_addr;
	socklen_t client_addr_size;

	// open file for write
	FILE *fp;
    fp  = fopen ("dns_svr.log", "w");
	fflush(fp);

	// not enough information
	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

    printf("test: first\n");
    // Create address we're going to listen on (with given port number)
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET6;      
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE;     
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
	// incoming connection requests will be queued
	if (listen(sockfd, 5) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	// in a while loop, so will not terminate itself
	while(1){
		// accept a connection
		int newsockfd;
		printf("first while loop\n");
		client_addr_size = sizeof client_addr;
		printf("first while before accept\n");
		newsockfd =
			accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_size);
		printf("newsockfs acept = %d\n", newsockfd);
		if (newsockfd == 0){
			printf("newsockfd = 0\n");
			continue;
		}
		if (newsockfd < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		// memset(length, 0, 256);
		printf("test: before read\n");

///////////////////////////////////////////////////////
		// int size = 0;
		// unsigned char *buffer_data;
		// // while(3){
		// 	printf("while loop\n");
		// 	fflush(stdout);
		// 	int size_temp = 0;
		// 	unsigned char length_temp[2];
		// 	memset(length_temp, 0, 2);
		// 	printf("newsockfd before read length = %d\n",newsockfd);
		// 	n = read(newsockfd, length_temp, 2);
		// 	printf("newsockfd after read length = %d\n",newsockfd);
		// 	printf("n after read = %d\n",n);
		// 	if (n == 0){
		// 		break;
		// 	}
		// 	else if (n < 0){
		// 		perror("read lenght fail: client");
		// 		exit(EXIT_FAILURE);
		// 	}
		// 	// printf("n = %d\n",n);
		// 	size_temp = (((int)length_temp[0])*16*16) + (int)length_temp[1];
		// 	printf("size_temp = %d\n",size_temp);

		// 	if(size == 0){
		// 		buffer_data = malloc(size_temp * sizeof(unsigned char));
		// 		memset(buffer_data, 0, size_temp);
		// 	}
		// 	else{
		// 		buffer_data = realloc(buffer_data, (size + size_temp)*sizeof(unsigned char));
		// 		memset(buffer_data + size, 0 , size_temp);
		// 	}
		// 	printf("newsockfd before read data = %d\n",newsockfd);
		// 	n = read(newsockfd, buffer_data + size, size_temp);
		// 	printf("newsockfd after read data = %d\n",newsockfd);
		// 	if (n < 0){
		// 		perror("read data fail: client");
		// 		exit(EXIT_FAILURE);
		// 	}
		// 	size += size_temp;
		// // }
		// printf("newsockfd after while loop = %d\n",newsockfd);
		// printf("client input size = %d\n",size);
		// unsigned char buffer[size + 2];
		// memset(buffer, 0, size+2);
		// unsigned char length[2];
		// memset(length, 0, 2); 
		// length[0] = (unsigned char)floor(size/(16*16));
		// length[1] = (unsigned char)size%(16*16);

		// printf("after read data from client");
		// fflush(stdout);

//////////////////////////////////////////////////////
		
// #########################################
		// here the input come
		// read the size of the input
		unsigned char length[2];
		memset(length, 0, 256);
		int size = 0;
		printf("newsockfd bedore read length = %d\n", newsockfd);
		n = read(newsockfd, length, 2);
		size = (((int)length[0])*16*16) + (int)length[1];
		printf("size = %d\n",size);

		unsigned char buffer_data[size];
		memset(buffer_data,0,size);
		
		unsigned char buffer[size + 2];
		memset(buffer,0,size+2); 
		printf("newsockfd before read data = %d\n",newsockfd);
		int temp_read_bytes = 0;
		n = read(newsockfd, buffer_data, size); // n is number of characters read
		if (n < size){
			temp_read_bytes = n;
			while(temp_read_bytes < size){
				n = read(newsockfd, buffer_data + n, size);
				temp_read_bytes += n;
			}
		}
		printf("newsockfd in old = %d\n",newsockfd);

		if (n < 0) {
			printf("error in read socket client, read length\n");
			perror("ERROR reading from socket in the read length");
			exit(EXIT_FAILURE);
		}
// #########################################		


		// connect length array and buffer_temp array to become a single buffer array
		for (int i=0; i< 2; i++){
			buffer[i] = length[i];
		}
		for (int i = 0; i< size; i++){
			buffer[i+2] = buffer_data[i];
		}

		printf("buffer from client: \n");
		fflush(stdout);
		for (int i = 0; i< size+2; i++){
			if(i % 16 == 0){
				printf("\n");
				fflush(stdout);
			}
			printf("%x  ", buffer[i]);
			fflush(stdout);
		}
		printf("\n");
		fflush(stdout);
		
		// call the read_input function(in the function, it will produce the dns_svr.log)
		// and check for rcode = 4 
		int n3, isValid = 1;
		printf("before read input client\n");
		fflush(stdout);
		isValid = read_input(buffer_data, size);

		// if the input is not valid
		// i.e. not implemented
		if(isValid != 1){ 
			printf("not valid req\n");
			fflush(stdout);
			printf("buffer[5] before = %x\n",buffer[5]);
			printf("buffer[4] before = %x\n",buffer[4]);
			buffer[5] = (int) isValid;
			buffer[4] = (int)buffer[4] + 127;
			printf("buffer[5] after = %x\n",buffer[5]);
			printf("buffer[4] after = %x\n",buffer[4]);
			n3 = write(newsockfd, buffer , size+2);
			if(n3 < 0){
				printf("error: write to client when rcode = 4");
				perror("ERROR write to client rcode = 4");
				exit(EXIT_FAILURE);
			}
			continue;
		}
		printf("after read input client\n");
		fflush(stdout);



		printf("newsockfd before quetry up stream = %d\n",newsockfd);


		// the code in below try to connect to a up stream server
		printf("connect to upstream");
		fflush(stdout);
		printf("test: first up \n");
		int sockfd2, n2;
		struct addrinfo hints2, *servinfo2, *rp2;
		unsigned char length2[2];

		// Create address
		memset(&hints2, 0, sizeof hints2);
		hints2.ai_family = AF_INET;
		hints2.ai_socktype = SOCK_STREAM;

		printf("test: before getaddrinfo up \n");
		if ((getaddrinfo(argv[1], argv[2], &hints2, &servinfo2)) < 0) {
			printf("error in getaddrinfo 2\n");
			perror("getaddrinfo2");
			exit(EXIT_FAILURE);
		}
		printf("test: before sockfd up\n");
		
		// create sockets
		for (rp2 = servinfo2; rp2 != NULL; rp2 = rp2->ai_next) {
			sockfd2 = socket(rp2->ai_family, rp2->ai_socktype, rp2->ai_protocol);
			if (sockfd2 == -1)
				continue;
			if (connect(sockfd2, rp2->ai_addr, rp2->ai_addrlen) != -1)
				break; 
			close(sockfd2);
		}
		if (rp2 == NULL) {
			fprintf(stderr, "client: failed to connect to up stream\n");
			exit(EXIT_FAILURE);
		}

		// write query to the up stream server
		printf("test: before write to upstream \n");
		n2 = write(sockfd2, buffer, size+2);
		if (n2 < 0) {
			perror("socket2");
			exit(EXIT_FAILURE);
		}

		// #########################################
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
		n2= read(sockfd2, buffer2_temp, size2);
		if (n2 < 0) {
			perror("read2");
			exit(EXIT_FAILURE);
		}

// #########################################

		// call the read_input function in the helper1.c file
		// it will read the inforamtion of the input from up stream server
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


		// write back to the client
		printf("newsockfd before write to client = %d\n", newsockfd);
		n2 = write(newsockfd, buffer2 , size2+2);
		printf("write back to client n2 = %d\n", n2);
		printf("newsockfd after write back to client = %d\n",newsockfd);


		close(sockfd2);
		freeaddrinfo(servinfo2);
		close(newsockfd);
	}

	// close sockets from client
	freeaddrinfo(res);
	
	close(sockfd);
	
    return 0;
}
