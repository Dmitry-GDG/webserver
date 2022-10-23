#include <arpa/inet.h>
// #include <assert.h>
// #include <errno.h>
#include <netinet/in.h>
// #include <signal.h>
// #include <stdlib.h>
// #include <stdio.h>
#include <string.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
// #include <fcntl.h>
#include <iostream>

#define SA      struct sockaddr
#define MAXLINE 4096
#define MAXSUB  1000
#define BUFSIZE 1024

// #define LISTENQ         1024

extern int h_errno;

ssize_t process_http(int sockfd, char *fileD)
{
	char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
	ssize_t n;
	// snprintf(sendline, MAXSUB,
	// 	 "POST %s HTTP/1.1\r\n"
	// 	 "Host: %s\r\n"
	// 	 "Content-Type: application/x-www-form-urlencoded\r\n"
	// 	 "Content-Length: %d\r\n"
	// 	 "Connection: Close\r\n\r\n"
	// 	 "%s", page, host, (int)strlen(poststr), poststr);
	snprintf(sendline, MAXSUB,
		"DELETE %s HTTP/1.1\r\n\r\n", fileD);

	// fcntl(sockfd, F_SETFL, O_NONBLOCK);
	std::cout << "\033[0;32m\tClient request:\n\033[0m" <<  sendline << std::endl;
	// write(sockfd, sendline, strlen(sendline));
	send(sockfd, sendline, strlen(sendline), 0);
	std::cout << "\033[0;32m\tServer answer:\n\033[0m";
	// while ((n = read(sockfd, recvline, MAXLINE)) > 0)
	// {
	// 	recvline[n] = '\0';
	// 	std::cout << recvline << "\n";
	// }
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
	// char *poststr = ("--AaB03x\r\nContent-Disposition: form-data; name='submit-name'\r\n\r\nLarry\r\n--AaB03x\r\nContent-Disposition: form-data; name=files\r\nContent-Type: multipart/mixed; boundary=BbC04y\r\n\r\n--BbC04y\r\nContent-Disposition: file; filename=file1.txt\r\nContent-Type: text/plain\r\n\r\n... contents of file1.txt ...\r\n--BbC04y\r\nContent-Disposition: file; filename=file2.gif\r\nContent-Type: image/gif\r\nContent-Transfer-Encoding: binary\r\n\r\n...contents of file2.gif...\r\n--BbC04y--\r\n--AaB03x--\r\n\r\n").c_str();
// 	std::string strPost = "--AaB03x\r\n\
// Content-Disposition: form-data; name=\"submit-name\"\r\n\r\n\
// Larry\r\n\
// --AaB03x\r\n\
// Content-Disposition: form-data; name=\"files\"\r\n\
// Content-Type: multipart/mixed; boundary=BbC04y\r\n\r\n\
// --BbC04y\r\n\
// Content-Disposition: file; filename=\"file1.txt\"\r\n\
// Content-Type: text/plain\r\n\r\n\
// ... contents of file1.txt ...\r\n\
// --BbC04y\r\n\
// Content-Disposition: file; filename=\"file2.gif\"\r\n\
// Content-Type: image/gif\r\n\
// Content-Transfer-Encoding: binary\r\n\r\n\
// ...contents of file2.gif...\r\n\
// --BbC04y--\r\n\
// --AaB03x--\r\n\r\n";
// 	char *poststr = (char *)strPost.c_str();
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

// c++ client_delete.cpp && ./a.out && rm a.out
