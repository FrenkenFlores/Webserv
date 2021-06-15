#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>


#define PORT "3490"								//port that users need to connect
#define BACKLOG 10								//number of pending connections

//return the ip address depending on it's version
void *get_inet_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {				//if IPV4
		return &(((struct sockaddr_in*)sa)->sin_addr);
	} else if (sa->sa_family == AF_INET6) {		//if IPv6
		return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}
	return NULL;
}

int main(void) {
	struct addrinfo info;
	struct addrinfo *info_list;
	int yes = 1;
	int return_value;

	memset(&info, 0, sizeof(struct addrinfo));	//clean buffer
	info.ai_family = AF_UNSPEC;				//IPV4 or IPV6
	info.ai_socktype = SOCK_STREAM;			//TCP connection
	info.ai_flags = AI_PASSIVE;				//use pc IP address

	//initialize data structures
	if ((return_value = getaddrinfo(NULL, PORT, &info, &info_list)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(return_value));
		exit(1);
	}
	//loop over the info_list till we bind
	struct addrinfo *ptr = info_list;
	int sockfd;
	while (ptr != NULL) {
		// get socket file descriptor, if fails checks the next node
		if ((sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1) {
			perror("socket error");
			ptr = ptr->ai_next;
			continue;
		}
		// if the address is already in use
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
			perror("setsockopt error");
			exit (2);
		}
		// bind the socket file descriptor with the ip and port. If fails, then try the next node
		if ((bind(sockfd, ptr->ai_addr, ptr->ai_addrlen)) == -1) {
			close (sockfd);
			perror("bind error");
			ptr = ptr->ai_next;
			continue;
		}
		break;
	}
	// free allocated memory for list after the connection been set
	freeaddrinfo(info_list);
	// check socket
	if (ptr == NULL) {
		perror("could not bind socket");
		close(sockfd);
		exit(3);
	}
	// listen
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		close(sockfd);
		exit(4);
	}
	// reap all dead processes
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(5);
	}
	struct sockaddr_storage client_addr;
	int new_sockfd;
	socklen_t sin_size;
	char client_ip[INET6_ADDRSTRLEN];
	// accept loop
	while (1) {
		printf("Server is waiting for connection ...\n");
		sin_size = sizeof(struct sockaddr_storage);
		if ((new_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &sin_size)) == -1) {
			perror("accept");
			continue;
		}
		inet_ntop(client_addr.ss_family, get_inet_addr((struct sockaddr*)&client_addr), client_ip, sizeof (client_ip));
		printf("server: got connection from %s\n", client_ip);
		if (fork() == 0) {			// child process
			close (sockfd);
			if ((send(new_sockfd, "Hello from Frenken Flores!\n", strlen("Hello from Frenken Flores!"), 0)) == -1) {
				perror("send");
				close(new_sockfd);
				exit(6);
			}
			close(new_sockfd);		// close connection
			exit(0);				// finish child process
		} else						// parent process
			close (new_sockfd);
	}
	return (0);
}