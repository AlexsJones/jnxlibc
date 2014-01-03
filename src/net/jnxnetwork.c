#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "jnxfile.h"
#include "jnxnetwork.h"
#include "jnxlist.h"
#define true 1
#define false 0
int running = true;
void jnx_network_cancel_listener(void)
{
	if (running) {
		running = false;
	}
}
void* jnx_network_get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == ADDRESSFAMILY)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int jnx_network_setup_listener(int port,int max_connections, void (*jnx_network_listener_callback)(char*,size_t,char*))
{
	int sockfd, newsockfd, clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int  n = -1;
	int optval = 1;
	sockfd = socket(ADDRESSFAMILY, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("jnx_network_setup_listener error opening socket");
		return 1;
	}
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = ADDRESSFAMILY;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) > 0) {
		perror("jnx_network_setup_listener set socketopts:");
		close(sockfd);
		return 1;
	}
	if (bind(sockfd, (struct sockaddr*) &serv_addr,
				sizeof(serv_addr)) < 0) {
		perror("jnx_network_setup_listener error on binding");
		close(sockfd);
		return 1;
	}
	listen(sockfd, max_connections);
	while (running) {
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr,
				(socklen_t*)&clilen);
		if (newsockfd < 0) {
			perror("jnx_network_setup_listener error on accept");
			close(sockfd);
			return 1;
		}
		FILE *temp_file = tmpfile();
		if(temp_file == NULL) 
		{
			printf("jnx_network_setup_listener: %s\n",strerror(errno));
			close(sockfd);
			close(newsockfd);
			return 1;
		}
		size_t bytes = 0;
		while(n != 0)
		{
			char buffer[MAXBUFFER];	
			n= read(newsockfd, buffer,MAXBUFFER);
			bytes = bytes + n;
			fwrite(buffer,1,n,temp_file);
		}
		int len = ftell(temp_file);
		rewind(temp_file);
		char *outbuffer = calloc(len + 1, sizeof(char));	
		fread(outbuffer,1,len,temp_file);
		fclose(temp_file);	

		if (n < 0) {
			perror("jnx_network_setup_listener error reading from socket");
			close(sockfd);
			close(newsockfd);
			return 1;
		}
		char client_ip_addr[INET6_ADDRSTRLEN];
		inet_ntop(ADDRESSFAMILY, &(cli_addr.sin_addr),client_ip_addr,INET6_ADDRSTRLEN);
		(*jnx_network_listener_callback)(outbuffer,bytes,client_ip_addr);      //function pointer callback

		n = -1;
		close(newsockfd);
	}
	close(sockfd);
	return 0;
}
int jnx_network_send_message(char* host, int port, char* msg,size_t msg_len)
{
	if(host == NULL || port < 0 || msg == NULL) 
	{
		printf("jnx_network_send_message error in input values\n");
		return -1;
	}
	struct hostent* send_server;
	struct sockaddr_in send_serv_addr;
	int send_fd = socket(ADDRESSFAMILY, SOCK_STREAM, 0);
	if (send_fd < 0) {
		printf("jnx_network_send_message couldn't open send port\n");
		return -1;
	}
	send_server = gethostbyname(host);
	if (send_server < 0) {
		printf(" jnx_network_send_message couldnt resolve ip\n");
		close(send_fd);
		return -1;
	}
	bzero((char*) &send_serv_addr, sizeof(send_serv_addr));
	send_serv_addr.sin_family = ADDRESSFAMILY;
	bcopy((char*) send_server->h_addr, (char*) &send_serv_addr.sin_addr.s_addr, send_server->h_length);
	send_serv_addr.sin_port = htons(port);
	if (connect(send_fd, (struct sockaddr*) &send_serv_addr, sizeof(send_serv_addr)) < 0) {
		printf("jnx_network_send_message error whilst connecting\n");
		close(send_fd);
		return -1;
	}
	ssize_t n;
	n = write(send_fd, msg, strlen(msg));
	if (n < 0) {
		perror("jnx_network_send_message error writing to socket");
		close(send_fd);
		return -1;
	}
	close(send_fd);
	printf("jnx_network_send_message sent %zu bytes\n",n);
	return n;
}
int jnx_network_send_broadcast(int port,char *broadcastgroup,char *message)
{
	struct sockaddr_in addr;
	int fd;
	if((fd=socket(ADDRESSFAMILY,SOCK_DGRAM,0)) < 0){
		perror(" jnx_network_send_broadcast broadcast");
		return 1;
	}
	memset(&addr,0,sizeof(addr));
	addr.sin_family=ADDRESSFAMILY;
	addr.sin_addr.s_addr=inet_addr(broadcastgroup);
	addr.sin_port=htons(port);
	if(sendto(fd,message,strlen(message),0,(struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		perror("jnx_network_send_broadcast sendto");
		close(fd);
		return 1;
	}
	close(fd);
	return 0;
}
void jnx_network_broadcast_listener(int port,char *broadcastgroup, void(*jnx_network_broadcast_listener_callback)(char*))
{
	struct sockaddr_in addr;
	int fd, nbytes, addrlen;
	struct ip_mreq mreq;
	char buffer[MAXBUFFER];
	u_int yes=1;
	if((fd=socket(ADDRESSFAMILY,SOCK_DGRAM,0)) < 0){
		perror("jnx_network_broadcast_listener broadcast listener");
		return;
	}
	if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0)
	{
		perror("jnx_network_broadcast_listener setsockopts");
		close(fd);
		return;
	}
	memset(&addr,0,sizeof(addr));
	addr.sin_family=ADDRESSFAMILY;
	addr.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
	addr.sin_port=htons(port);
	if(bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0){
		perror("jnx_network_broadcast_listener bind");
		close(fd);
		return;
	}
	mreq.imr_multiaddr.s_addr=inet_addr(broadcastgroup);
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);
	if(setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0)
	{
		perror("jnx_network_broadcast_listener setsockopt");
		close(fd);
		return;
	}    
	bzero(buffer,MAXBUFFER);
	while(1)
	{
		addrlen=sizeof(addr);
		if((nbytes=recvfrom(fd,buffer,MAXBUFFER,0,(struct sockaddr*)&addr,(socklen_t*)&addrlen)) < 0){
			perror("jnx_network_broadcast_listener recvfrom");
			close(fd);
			return;
		}
		(*jnx_network_broadcast_listener_callback)(buffer);
	}
	close(fd);
}
char* jnx_network_local_ip(char* interface)
{
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
	getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa ->ifa_addr->sa_family==AF_INET)
        {   // check it is IP4
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char *addressBuffer = malloc(sizeof(char) *INET_ADDRSTRLEN);
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

            if(strcmp(ifa->ifa_name,interface) == 0)
			{
				free(ifAddrStruct);
				return addressBuffer;
            }

        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
	return NULL;
}

