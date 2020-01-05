#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<assert.h>

#define MAX_RECV_LEN 256
#define MAX_SEND_LEN 256

int main(int argc,char *argv[])
{
    if(argc < 2)
    {
      perror("argv[2] need port num\n");
	  exit(0);
    }

    int cfd;
	struct sockaddr_in serv_addr,cli_addr;
	int cliaddr_len;
	int ret;

    char recvbuf[MAX_RECV_LEN] = {0},sendbuf[MAX_RECV_LEN] = {0};

	cfd = socket(AF_INET,SOCK_STREAM,0);
	assert(cfd);
	
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	//serv_addr.sin_addr.s_addr = htonl(0);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(atoi(argv[1]));

	ret = connect(cfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	if(ret <0)
	{
		perror("connect failed\n");
		exit(0);
	}

	while(1)
	{
		fgets(sendbuf,sizeof(sendbuf),stdin);
		send(cfd,sendbuf,sizeof(sendbuf),0);
		printf("client send buf:%s\n",sendbuf);
	}
}

