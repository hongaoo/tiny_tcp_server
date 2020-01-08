#include<stdio.h>
#include<string.h>
#include<assert.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<unistd.h>

#define MAX_LISTEN_NUM 20
#define MAX_RECV_LEN 256

int main(int argc,char *argv[])
{
    if(argc < 2)
    {
      perror("argv[2] need port num\n");
	  exit(0);
    }

    int lfd,cfd,maxfd;
	struct sockaddr_in serv_addr,cli_addr;
	int cliaddr_len = sizeof(cli_addr);
	int ret,read_len;

    char recvbuf[MAX_RECV_LEN] = {0},sendbuf[MAX_RECV_LEN] = {0};

	//init listen socket
	lfd = socket(AF_INET,SOCK_STREAM,0);
	
	assert(lfd);
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	//bind listen socket
	ret = bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	if(ret <0)
	{
		perror("bind failed\n");
		exit(0);
	}


	/*use read_set store raw socket,kernel use temp_set detect socket change*/
	/*temp_set will be change by kernel*/
	fd_set read_set,temp_set;

	/* Watch lfd to see when it has input. */
	FD_ZERO(&read_set);
	FD_SET(lfd,&read_set);
	printf("FD_ISSET(lfd,&temp_set) = %d line:%d\n",FD_ISSET(lfd,&read_set),__LINE__);
	ret = listen(lfd,MAX_LISTEN_NUM);
	if(ret <0)
	{
		perror("listen failed\n");
		exit(0);
	}

	maxfd = lfd;
	while(1)
	{
		temp_set = read_set;
		
		printf("start select\n");
		ret  = select(maxfd+1,&temp_set,NULL,NULL,NULL);
		if(ret== -1)
		{
			perror("select error!\n");
		}

		printf("start detect fd_set\n");
		for(int i= lfd;i<maxfd+1;i++)
		{
			if(FD_ISSET(i,&temp_set))
			{
				printf("fd:%d detect\n",i);
				if(i == lfd)
				{
					cfd = accept(lfd,(struct sockaddr*)&cli_addr,&cliaddr_len);
					if(cfd == -1)
			    	{
						perror("accept error!\n");
			    	}
					else
					{
						FD_SET(cfd,&read_set);
						maxfd = (maxfd<cfd)?cfd:maxfd;
					}
				}
				else
				{
					read_len = read(i,recvbuf,sizeof(recvbuf));
					if(read_len == 0)
					{
						printf("client closed\n");
						FD_CLR(i,&read_set);
						break;
					}
					write(i,recvbuf,sizeof(recvbuf));
				}

			}
		}
		
	}

}

