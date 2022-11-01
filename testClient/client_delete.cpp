#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>

#define SA      struct sockaddr
#define MAXLINE 4096
#define MAXSUB  1000
#define BUFSIZE 1024

extern int h_errno;

ssize_t process_http(int sockfd, char *fileD)
{
	char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
	ssize_t n;
	snprintf(sendline, MAXSUB,
		"DELETE %s HTTP/1.1\r\n\r\n", fileD);

	std::cout << "\033[0;32m\tClient request:\n\033[0m" <<  sendline << std::endl;
	send(sockfd, sendline, strlen(sendline), 0);
	std::cout << "\033[0;32m\tServer answer:\n\033[0m";
	char buff[BUFSIZE + 1];
    int qtyBytes = recv(sockfd, buff, BUFSIZE, 0);
	while (qtyBytes > 0)
	{
		buff[BUFSIZE] = '\0';
		std::cout << buff << "\n";
		qtyBytes = recv(sockfd, buff, BUFSIZE, 0);
	}
	return n;

}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cerr << "Error! Use it with arguments: <port> <fileToDelete>\n";
		exit (1);
	}
	int sockfd;
	struct sockaddr_in servaddr;

	char **pptr;
	//********** You can change. Put any values here *******
	// char *hname = (char *)("souptonuts.sourceforge.net");
	char *hname = (char *)("localhost");
	int	port = atoi(argv[1]);
	char *fileD = argv[2];
	//*******************************************************

	char str[50];
	struct hostent *hptr;
	if ((hptr = gethostbyname(hname)) == NULL)
	{
		std::cerr << " gethostbyname error for host: " << hname << ": " << hstrerror(h_errno) << "\n";
		exit(1);
	}
	std::cout << "hostname: " << hptr->h_name << "\n";
	if (hptr->h_addrtype == AF_INET && (pptr = hptr->h_addr_list) != NULL)
		std::cout << "address: " << inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)) << ":" << port << "\n";
	else
		std::cerr << "Error call inet_ntop \n";

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, str, &servaddr.sin_addr);

	connect(sockfd, (SA *) & servaddr, sizeof(servaddr));
	process_http(sockfd, fileD);
	close(sockfd);
	exit(0);

}

// https://souptonuts.sourceforge.net/code/http_post.c.html
// https://www.cyberforum.ru/post2446230.html

// c++ client_delete.cpp && ./a.out 8080 files/1.txt && rm a.out
