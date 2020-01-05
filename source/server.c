#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<assert.h>
#define MAX_LISTEN_NUM 20
#define MAX_RECV_LEN 256
int main(int argc,char *argv[])
{
    if(argc < 2)
    {
      perror("argv[2] need port num\n");
	  exit(0);
    }

    int lfd,cfd;
	struct sockaddr_in serv_addr,cli_addr;
	int cliaddr_len = sizeof(cli_addr);
	int ret;

    char recvbuf[MAX_RECV_LEN] = {0},sendbuf[MAX_RECV_LEN] = {0};

	lfd = socket(AF_INET,SOCK_STREAM,0);
	assert(lfd);
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	ret = bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	if(ret <0)
	{
		perror("bind failed\n");
		exit(0);
	}

	ret = listen(lfd,MAX_LISTEN_NUM);
	if(ret <0)
	{
		perror("listen failed\n");
		exit(0);
	}

	while(1)
	{
        cfd = accept(lfd,(struct sockaddr*)&cli_addr,&cliaddr_len);
		if(cfd == -1)
        {
            perror("accept error!\n");
			exit(-1);
        }
		read(cfd,recvbuf,sizeof(recvbuf));
		printf("sever recv buf:%s\n",recvbuf);
		
	}
}
