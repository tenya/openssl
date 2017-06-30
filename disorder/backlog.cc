#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

static bool stop = false;
static void handle_term( int sig )
{
		printf("receive interupt %d\n",sig);
    stop = true;
}
static void handle_child( int sig )
{
		printf("receive child exit %d\n",sig);
		int status = 0;
		wait(&status);
}

void doWork(int fd)
{
				//children doing thing here.
				printf("accept a client %d \n",fd);
				char buff[128];
				bzero(buff,sizeof buff);
				printf("s:%d\n",time(NULL));
				int rs = recv(fd,buff,sizeof buff,0);
				if(rs == -1) //no block,EAGAIN
				{
					printf("recv error:%d,%s\n",errno,strerror(errno));
				}
				printf("e:%d\n",time(NULL));
				printf("%s\n",buff);
				int ss = send(fd,buff,rs,0);
				if(ss < 0)
				{
						printf("send fail\n");
				}
				sleep(1);
				while(recv(fd,buff,sizeof buff,0) > 0){
				}
				printf("close socket\n");
				close(fd);
				sleep(15);
//			exit(0);
}
int main( int argc, char* argv[] )
{
    signal( SIGTERM, handle_term );
    signal( SIGINT, handle_term );
    signal( SIGCHLD, handle_child );

    if( argc <= 3 )
    {
        printf( "usage: %s ip_address port_number backlog\n", basename( argv[0] ) );
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi( argv[2] );
    int backlog = atoi( argv[3] );

    int sock = socket( PF_INET, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0 );
    assert( sock >= 0 );

    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );

    int ret = bind( sock, ( struct sockaddr* )&address, sizeof( address ) );
		if(ret == -1)
		{
			printf("bind error:%d:%s\n",errno,strerror(errno));
		}
    assert( ret != -1 );

    ret = listen( sock, backlog );
    assert( ret != -1 );

    while ( ! stop )
    {
				struct sockaddr_in addr;
				socklen_t len = sizeof(addr);
				int fd = accept(sock,(sockaddr*)&addr,&len);
				if(fd > 0)
				{
						pid_t p = fork();
						if(p == 0 )
						{
							close(sock);
							//fcntl(fd,F_SETFL,O_NONBLOCK); 
							doWork(fd);
							return 0;
						}
						else if(p < 0){
							perror("fork error\n");
						}
						else {
							//child process will deal with it ,if it not closed,the fd always in 'CLOSE_WAIT' status.
							close(fd);	
						}
				}
        sleep(1);
    }
	/*
		int status = 0;
		while(wait(&status) > 0){
			printf("status:%d\n",status);
		}
	*/
    close( sock );
    return 0;
}
