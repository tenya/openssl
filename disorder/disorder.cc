
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>

#define MAX_WORKER 2 
#define MAX_EVENTS 10
#define BUF_SIZE   500

static int create_and_bind(char *port);
static int make_socket_non_binding(int sfd);
static int process_work_process(int sfd);

void getsockinfo(int fd)
{
				int sndbuff = 1;
				socklen_t length = sizeof sndbuff;
				if(getsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,&sndbuff,&length) == -1)
				{
					fprintf(stderr,"ERROR : setsockopt %s\n",strerror(errno));
				}
				printf("getsockopt SO_KEEPALIVE=%d\n",sndbuff); 

				if(setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,&sndbuff,length) == -1)
				{
					fprintf(stderr,"ERROR : setsockopt %s\n",strerror(errno));
				}
				sndbuff = 50;
				if(setsockopt(fd,SOL_SOCKET,SO_RCVBUF,&sndbuff,length) == -1)
				{
					fprintf(stderr,"ERROR : setsockopt %s\n",strerror(errno));
				}
				if(getsockopt(fd,SOL_SOCKET,SO_RCVBUF,&sndbuff,&length) == -1)
				{
					fprintf(stderr,"ERROR : getsockopt %s\n",strerror(errno));
				}
				printf("getsockopt SO_RCVBUF=%d\n",sndbuff); 
}

int
main(int argc, char *argv[])
{
    int sfd, res, i;
    pid_t pid;

    if (argc != 2) {
        printf("usage : <port>\n");
        exit(0);
    }
    
    sfd = create_and_bind(argv[1]);
    if (sfd == -1) {
        perror("ERROR : create_and_bind\n");
        exit(1);
    }  
      
    res = make_socket_non_binding(sfd);
    if (res == -1) {
        perror("ERROR : make_sokcet_non_binding\n");
        exit(1);
    }   
       
    res = listen(sfd, SOMAXCONN);
    if (res == -1) {
        perror("ERROR : listen\n");
        exit(1);
    }
    
    for ( i = 0; i < MAX_WORKER; i++) {
        if ((pid = fork()) < 0) {
            perror("ERROR : fork\n");
            exit(1);
            
        } else if (pid == 0) {
            process_work_process(sfd);
            
        } else {
            printf("worker process started, pid = %d\n", pid);
        }
    }
    
    while (waitpid(-1, NULL, 0)) ;
    
    return 0;
}

static int
create_and_bind(char *port)
{
    struct addrinfo hint, *result;
    int res, sfd;
    
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family   = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    
    res = getaddrinfo("beefly", port, &hint, &result);
    if (res == -1) {
        perror("ERROR : getaddrinfo\n");
        return -1;
    }
    
    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1) {
        perror("ERROR : socket\n");
        return -1;
    }
   	int optval = 1;  
		setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));   
    res = bind(sfd, result->ai_addr, result->ai_addrlen);
		getsockinfo(sfd);                    
    if (res == -1) {
        perror("ERROR : bind\n");
        return -1;
    }
    
    freeaddrinfo(result);
    
    return sfd;
}


static int 
make_socket_non_binding(int sfd)
{
    int flags;
    
    flags = fcntl(sfd, F_GETFL);
    if (flags == -1) {
        perror("ERROR : fcntl F_GETFL\n");
        return -1;
    }
    
    flags |= O_NONBLOCK;
    flags = fcntl(sfd, F_SETFL, flags);
    if (flags == -1) {
        perror("ERROR : fcntl F_SETFL\n");
        return -1;
    }
    
    return 0;
}


static int
process_work_process(int sfd)
{
    int res, fd, efd, nfds, i;
    struct epoll_event event, events[MAX_EVENTS];
    struct sockaddr addr;
    char buf[BUF_SIZE];

 		    
    /* param is ignored by kernel, but must be bigger than zero */
    efd = epoll_create(1);
    if (efd == -1) {
        perror("ERROR : epoll_create\n");
        return -1;
    }
    
    memset(&event, 0, sizeof(struct epoll_event));
    event.events |= EPOLLIN;
    event.data.fd = sfd;
    
    /* add listening sfd */
    res = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event);
    if (res == -1) {
        perror("ERROR : epoll_ctl EPOLL_CTL_ADD\n");
        return -1;
    }
    for ( ; ; ) {
        nfds = epoll_wait(efd, events, MAX_EVENTS, -1);
        for ( i = 0; i < nfds; i++) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) {
                close(events[i].data.fd);
                printf("socket error, fd = %d\n", events[i].data.fd);
                continue;
            } else if (events[i].data.fd == sfd) {
				         printf("process wake up, pid = %d\n", getpid());
                while (1) {
                  socklen_t addrlen = sizeof(struct sockaddr);
                  fd = accept(sfd, &addr, &addrlen);
         					
         				  if (fd == -1) {
	                    //Resource temporarily unavailable,if fd have accepted.
                        if (errno == EAGAIN) 
                            break;
                        else
                            continue;
                    }
                    printf("accepted, pid = %d\n", getpid());
                    res = make_socket_non_binding(fd);
                    if (res == -1) {
                        perror("ERROR : make_non_binding fd");
                        exit(1);
                    }
                       
                    event.data.fd = fd;
                    event.events |= EPOLLIN;
                    res = epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event);
                    if (res == -1) {
                        perror("ERROR : epoll_ctl\n");
                        exit(1);
                    }
                    break;// attempt to accept fd that have accepted. that will incur EAGAIN error.
                }
                
            } else { 
                while (1) {
                    memset(buf, 0, BUF_SIZE); 
                   res = read(events[i].data.fd, buf, BUF_SIZE);
                    if (res == -1) {
                        if (errno == EAGAIN) {
                            break;
                        } else {
                            perror("ERROR : read\n");
                            exit(1);
                        }

                    } else if (res == 0) {
                        printf("\nclient closed the socket\n");
                        close(events[i].data.fd);
                        break;
                        
                    } else {
                        write(STDOUT_FILENO, buf, res);
												printf("\n");
                    }
                }
            }
        }
    }
    
    return 0;
}

