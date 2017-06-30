### 1.测试惊群现象
- 测试: 当一个客户端连进来，是否唤醒所有同时accept的进程,进行争抢FD.
- 测试: 增加SO_REUSEPORT,是否会将连接均衡地分配到各个进程,原文如下:

Linux Kernel 3.9中合并了TCP/UDP的SO_REUSEPORT特性。
允许多个进程(或者线程)绑定同一个端口：

```
int sfd = socket(domain, socktype, 0);
int optval = 1;
setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
 
bind(sfd, (struct sockaddr *) &addr, addrlen);
...
while (1) {
    new_fd = accept(...);
    process_connection(new_fd);
} 

```
>? 一般地，当连接进来时，进程会被不均衡的唤醒。SO_REUSEPORT特性，能够实现将连接均衡地分配到各个进程。?
>
> (答案是否定的)

> #uname -r
>
> #4.10.16-200.fc25.x86_64

### 2.测试keepalive/发送与接收缓存
- 通过setsockopt修改接收缓存与发送缓存大小,当发送及接收缓存满之时,c端与s端理论论会停止发送与接收。（的确如此）
- 当停止发送与接收时，这个会话能保持多久，是否跟keepalive有关。（发现会话一直保持着，难道是keepalive设置得不够短，或者设置无效？下次继续）
