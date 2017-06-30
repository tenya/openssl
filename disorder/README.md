###1.测试惊群现象
>测试: 当一个客户端连进来，是否唤醒所有同时accept的进程,进行争抢FD.
>
>:测试: 增加SO_REUSEPORT,是否会将连接均衡地分配到各个进程,原文如下:
>Linux Kernel 3.9中合并了TCP/UDP的SO_REUSEPORT特性。
允许多个进程(或者线程)绑定同一个端口：
`int sfd = socket(domain, socktype, 0);  
  
int optval = 1;  
setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));  
  
bind(sfd, (struct sockaddr *) &addr, addrlen);  
...  
while (1) {  
    new_fd = accept(...);  
    process_connection(new_fd);  
}  `
>一般地，当连接进来时，进程会被不均衡的唤醒。SO_REUSEPORT特性，能够实现将连接均衡地分配到各个进程。

