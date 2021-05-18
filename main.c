#include "helper1.h"

#include <time.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define PORT "8053"


int main(int argc, char* argv[]) {
    int sockfd, newsockfd, n;
	unsigned char length[2];
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


		
// #########################################
		// here the input come
		// read the size of the input
		// int read_length_bytes = 0;
		// unsigned char temp_length_read[2];
		memset(length, 0, 256);
		// memset(temp_length_read,0,2);
		int size = 0;
		n = read(newsockfd, length, 2);
		 // n is number of characters read
		// if(n == 2){
		// 	for(int i = 0; i < 2; i++){
		// 		length[i] = temp_length_read[i];
		// 	}
		// }
		// while (n + read_length_bytes < 2){
		// 	for (int i = 0; i< n; i++){
		// 		length[read_length_bytes] = temp_length_read[i];
		// 		read_length_bytes++;
		// 	}
		// 	memset(temp_length_read,0,2);
		// 	if (read_length_bytes >= 2){
		// 		break;
		// 	}
		// n = read(newsockfd, temp_length_read, 2);
		// }
		
		size = (((int)length[0])*16*16) + (int)length[1];
		printf("size = %d\n",size);

		unsigned char buffer_temp[size];
		memset(buffer_temp,0,size);
		
		unsigned char buffer[size + 2];
		memset(buffer,0,size+2); 
		
		// unsigned char buffer[256];
		// memset(buffer,0,256);
		// int read_bytes = 0;
		// unsigned char temp_read[size];

		// memset(temp_read,0,size);
		n = read(newsockfd, buffer_temp, size); // n is number of characters read

		// if(n == size){
		// 	for(int i = 0; i < size; i++){
		// 		buffer_temp[i] = temp_read[i];
		// 	}
		// }
		// while (n + read_bytes < size){
		// 	for (int i = 0; i< n; i++){
		// 		buffer_temp[read_bytes] = temp_read[i];
		// 		read_bytes++;
		// 	}
		// 	memset(temp_read,0,size);
		// 	if (read_bytes >= size){
		// 		break;
		// 	}
		// 	n = read(newsockfd, temp_read, size);
		// }

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
		
		// call the read_input function(in the function, it will produce the dns_svr.log)
		// and check for rcode = 4 
		int n3, isValid = 1;
		printf("before read input client\n");
		isValid = read_input(buffer_temp, size);

		// if the input is not valid
		// i.e. not implemented
		if(isValid != 1){ 
			printf("not valid req\n");
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





		// the code in below try to connect to a up stream server
		printf("connect to upstream");
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
		// read res from the upstream server
		// int read_length_bytes2 = 0;
		// unsigned char temp_length_read2[2];
		// memset(temp_length_read2,0,2);
		memset(length2, 0,2);
		n2 = read(sockfd2, length2, 2);
		// if(n2 == 2){
		// 	for(int i = 0; i < 2; i++){
		// 		length2[i] = temp_length_read2[i];
		// 	}
		// }
		// while (n2 + read_length_bytes2 < 2){
		// 	for (int i = 0; i< n2; i++){
		// 		length2[read_length_bytes2] = temp_length_read2[i];
		// 		read_length_bytes2++;
		// 	}
		// 	memset(temp_length_read2,0,2);
		// 	if (read_length_bytes2 >= 2){
		// 		break;
		// 	}
		// 	n2 = read(sockfd2, temp_length_read2, 2);
		// }
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
		// int read_bytes2 = 0;
		// unsigned char temp_read2[size2];

		// memset(temp_read2,0,size2);
		n2= read(sockfd2, buffer2_temp, size2);
		// if(n2 == size2){
		// 	for(int i = 0; i < size2; i++){
		// 		buffer2_temp[i] = temp_read2[i];
		// 	}
		// }
		// while (n2 + read_bytes2 < size2){
		// 	for (int i = 0; i< n2; i++){
		// 		buffer2_temp[read_bytes2] = temp_read2[i];
		// 		read_bytes2++;
		// 	}
		// 	memset(temp_read2,0,size2);
		// 	if (read_bytes2 >= size2){
		// 		break;
		// 	}
		// 	n2 = read(sockfd2, temp_read2, size2);
		// }
		// n2 = read(sockfd2, buffer2_temp, size2);
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
		close(sockfd2);
		freeaddrinfo(servinfo2);


		// write back to the client
		n2 = write(newsockfd, buffer2 , size2+2);
	}

	// close sockets from client
	freeaddrinfo(res);
	close(newsockfd);
	close(sockfd);
	
    return 0;
}
