#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
int main(int argc,char *argv[])
{
    if(argc<2)
    {
	printf("eg: ./a.out 8888");
	exit(0);
    }

    //init lfd
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    int ret = 0;
    char recv_buf[5] = {0},send_buf[128] = {0};
    int recv_len = 0;
    if(lfd < 0)
    {
        perror("create lfd failed\n");
	exit(0);
    }
    //init server addr
    struct sockaddr_in serv_addr,cli_addr;
    socklen_t cli_addr_len;
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    //bind lfd
    ret = bind(lfd,(struct sockaddr_in *)&serv_addr,sizeof(serv_addr));
    if(ret < 0)
    {
        perror("bind lfd failed\n");
	exit(0);
    }
    //listen lfd
    ret = listen(lfd,3);//max listen num 
    if(ret < 0)
    {
        perror("listen lfd failed\n");
	exit(0);
    }
    //create epoll root
    int epfd = epoll_create(30);//size argument is ignored

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ev);
    
    struct epoll_event all_ev[30] = {0};
    while(1)
    {
       ret = epoll_wait(epfd,all_ev,sizeof(all_ev)/sizeof(all_ev[0]),-1);
       printf("-----------epoll_wait--------\n");
       if(ret == -1)
       {
           perror("epoll_wait error\n");
	   exit(0);
       }
       for(int i = 0;i<ret;i++)
       {
	   int temp_fd = all_ev[i].data.fd;
           if(temp_fd == lfd)
	   {//do accept
		int cfd = accept(lfd,(struct sockaddr *)&cli_addr,&cli_addr_len);
	        if(cfd == -1)
       		{
          		 perror("accept error\n");
	  		 exit(0);
       		}	
		printf("client %s connect\n",inet_ntoa((cli_addr.sin_addr)));
		//
		ev.events = EPOLLIN;
		ev.data.fd = cfd;
		epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev);
	   }
	   else
	   {
		recv_len = recv(temp_fd,recv_buf,sizeof(recv_buf),0);
		if(recv_len < 0)
		{
		    perror("recv error\n");
		    exit(0);
		}
		else if(recv_len == 0)
		{
		    printf("client closed\n");
		    epoll_ctl(epfd,EPOLL_CTL_DEL,temp_fd,NULL);
		    close(temp_fd);
		}
		else
		{
		    send(temp_fd,recv_buf,sizeof(recv_buf),0);
		}

	   }
       }
    
    }
}
