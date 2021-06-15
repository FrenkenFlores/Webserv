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

#define PORT "3490"								// port that users need to connect
#define SIZE 100								// requested data size


void *get_inet_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {				//if IPV4
		return &(((struct sockaddr_in*)sa)->sin_addr);
	} else if (sa->sa_family == AF_INET6) {		//if IPv6
		return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}
	return NULL;
}


int main(int argc, char **argv) {
	struct addrinfo info;
	struct addrinfo *info_list;
	int return_value;

	memset(&info, 0, sizeof(struct addrinfo));
	info.ai_family = AF_UNSPEC;
	info.ai_socktype = SOCK_STREAM;

	if (argc != 2) {
		printf("Usage: client hostname\n");
		return 0;
	}

	if ((return_value = getaddrinfo(argv[1], PORT, &info, &info_list)) != 0) {
		fprintf(stderr, "could not fill the info_list %s\n", gai_strerror(return_value));
		exit(1);
	}

	struct addrinfo *ptr = info_list;
	int sockfd;
	while (ptr != NULL) {
		if ((sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1) {
			perror("could not get a file descriptor");
			ptr = ptr->ai_next;
			continue;
		}
		if (connect(sockfd, ptr->ai_addr, ptr->ai_addrlen) == -1) {
			perror("could not connect to the host");
			ptr = ptr->ai_next;
			continue;
		}
		break;
	}
	if (ptr == NULL) {
		perror("filed to connect to the server");
		close (sockfd);
		exit(2);
	}

	char ip_addres[INET6_ADDRSTRLEN];
	inet_ntop(ptr->ai_family, get_inet_addr((struct sockaddr*)ptr->ai_addr), ip_addres, sizeof (ip_addres));
	printf("connecting to %s\n", ip_addres);
	freeaddrinfo(info_list);
	int received_bytes;
	char buf[SIZE];
	if ((received_bytes = recv(sockfd, buf, SIZE - 1, 0)) == -1) {
		perror("could not receive data");
		exit(3);
	}
	buf[received_bytes] = '\0';
	printf("client received: %s\n", buf);
	close (sockfd);
	return (0);
}