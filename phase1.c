#define _POSIX_C_SOURCE 200112L
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char* argv[]) {
    int sockfd, n;
	struct addrinfo hints, *servinfo, *rp, *res;;
	unsigned char buffer[256];

    
    if (argc < 2) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}
    // Create address we're going to listen on (with given port number)
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;       // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE;     // for bind, listen, accept
	// node (NULL means any interface), service (port), hints, res
	// getaddrinfo(NULL, argv[1], &hints, &res);

	// // Create socket
	// sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	// if (sockfd < 0) {
	// 	perror("socket");
	// 	exit(EXIT_FAILURE);
	// }


    // int enable = 1;
    // if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    //     perror("setsockopt");
    //     exit(1);
    // }
    
    int size;
    // Read message from server
    memset(buffer, 0, 256);
    // while(1){
        n = read(0, buffer, 2);
        if (n < 0) {
            // break;
            perror("read");
            exit(EXIT_FAILURE);
        }
        printf("n = %d\n",n);
        printf("buffer[0] = %x\n", buffer[0]);
        printf("buffer[1] = %x\n", buffer[1]);
        // printf("buffer[1] in int = %d\n", buffer[1]);

        size = (((int)buffer[0])*16*16) + (int)buffer[1];
        printf("size = %d\n",size);


        for (int count = 0; count < size; count++){

            n = read(0, buffer, 1);
            if (n < 0) {
                // break;
                perror("read");
                exit(EXIT_FAILURE);
            }
            // printf("n = %d\n",n);
            printf("buffer = %x\n",buffer[0]);

        }
    // }




    return 0;
}
