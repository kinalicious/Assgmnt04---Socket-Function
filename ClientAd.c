#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<unistd.h>
#include<netdb.h>
#include<netinet/in.h>

/*struct hostent{
	char *h_name;
	char **h_aliases;
	int h_addrtype;
	int h_length;
	char **h_addr_list;
};*/

#define h_addr h_addr_list[0]
#define MAx_HOSTNAME 255
#define BLOCKING 0
#define NONBLOCKING 1

int main(int argc , char *argv[])
{
	int getsockname(int sock, struct sockaddr *addr,socklen_t *len);
	int getpeername(int sock,struct sockaddr *addr,socklen_t *len);
	int getsockopt(int sock, int level, int optname, void *optval, socklen_t *optlen);
	int gethostname(char *name, size_t len);
	int sethostname(const char *name, size_t len);
	//struct hostent *gethostbyaddr(const char *addr, int len, int type);
	int la_len, ret, sock, size, len, on, mode;
	struct sockaddr_in server;
	struct in_addr in;
	struct hostent *hp;
	struct timeval tv;
	struct protoent *pp;
	char buffer[100], oobdata;
	fd_set rfds;
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	server.sin_addr.s_addr = inet_addr("192.168.87.129");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8000 );

	//bind socket
	if(bind(sock, (struct sockaddr *)&server, sizeof(server)) <0)
 	{printf("Error on binding");}

	//create getsock function
	la_len = sizeof(server);
	ret = getsockname(sock,(struct sockaddr *)&server,&la_len);

	if(ret==0){
		printf("Local Address is : %s\n", inet_ntoa(server.sin_addr));
		printf("Local Port is : %d\n",server.sin_port);
	}
	else
		printf("Error \n");

	//create getpeername function
	la_len = sizeof(server);
	ret = getpeername(sock,(struct sockaddr *)&server,&la_len);
	if(ret==0){
		printf("Peer address is : %s\n", inet_ntoa(server.sin_addr));
		printf("Peer port is : %d\n", server.sin_port);
	}
	else
		printf("Error : peer function not established \n");


	//create getsockopt/setsockopt function
	len = sizeof(size);
	ret = getsockopt(sock, SOL_SOCKET, SO_SNDBUF,(void *)&size, (socklen_t *)&len);
	size = size*2;
	ret = setsockopt(sock, SOL_SOCKET, SO_SNDBUF,(void *)&size, sizeof(size));

	//create gethostname/sethostname functions
	char hostbuffer[MAx_HOSTNAME+1];
	ret = gethostname(hostbuffer, MAx_HOSTNAME);
	if(ret ==0)
		{printf("Hostname is %s\n", hostbuffer);}

	strcpy(hostbuffer,"Elise");
	ret=sethostname(hostbuffer,strlen(hostbuffer));
	if(ret==0)
		{printf("Hostname is now %s\n", hostbuffer);}

	//create gethostbyaddr function
	inet_aton("192.168.87.129", &in);
	if(hp=gethostbyaddr((char *)&in.s_addr, sizeof(in.s_addr), AF_INET))
		{printf("Hostname is %s\n", hp->h_name);}

	//Out-of-band (OOB) Data
	ret = send(sock, buffer, len, MSG_OOB);
	ret = recv(sock, buffer, len, MSG_OOB);
	on=1;
	ret = setsockopt(sock, SOL_SOCKET, SO_OOBINLINE, &on, sizeof(on));
	if(sockatmark(sock)){
		/*we're at the OOB byte position*/
		ret = read(sock, &oobdata, 1);
	}
	else{
		/*normal data coming...*/
		ret = read(sock, buffer, sizeof(buffer));
	}
	return 0;

	//Nonblocking sockets
	mode = NONBLOCKING;
	ret = ioct1(sock, FIONBIO, &mode);

	//Resolving a domain name to an IP address
	char address[] = {"192.168.1.1"};
	server.sin_addr.s_addr = inet_addr(address);
	if(server.sin_addr.s_addr = 0xffffffff) {
		struct hostent *hptr = (struct hostent *)gethostbyname(address);
		if(hptr == NULL) {}
		else{
		 struct in_addr **addrs;
		 addrs = (struct in_addr **)hptr->h_addr_list;
		 memcpy(&server.sin_addr, *addrs, sizeof(struct in_addr));
		} 
	}

	//timing out a read or write operation
	FD_ZERO(&rfds);
	FD_SET( sock, &rfds);
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	ret = select(sock+1, &rfds, NULL, NULL,  &tv);
	if(ret>0){
	 if(FD_ISSET(sock, &rfds)){
	  printf("Data is available for read");}
	 else
	  {printf("Timeout");}
	}

	//determining the protocol argument for socket
	pp = getprotobyname("tcp");
	if(pp) printf("tcp = %d\n",pp->p_proto);

	pp=getprotobyname("tcp");
	if(pp) {
	 sock = socket (AF_INET, SOCK_STREAM, pp->p_proto);
	}
}
