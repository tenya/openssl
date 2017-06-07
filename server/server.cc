#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void EXIT_IF_TRUE(bool x)  
{  
    if (x) {  
        exit(2);  
    }  
}  
// 
int Test_openssl_server_v2()  
{  
  
    SSL_CTX     *ctx;  
    SSL         *ssl;  
    X509        *client_cert;  
  
    char szBuffer[1024];  
    int nLen;  
  
    struct sockaddr_in addr;  
    int len;  
    int nListenFd, nAcceptFd;  
  
    // 初始化  
    SSLeay_add_ssl_algorithms();  
    OpenSSL_add_all_algorithms();  
    SSL_load_error_strings();  
    ERR_load_BIO_strings();  
  
    // 我们使用SSL V3,V2  
    EXIT_IF_TRUE((ctx = SSL_CTX_new(SSLv23_method())) == NULL);  
  
    // 要求校验对方证书  
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);  
  
    // 加载CA的证书  
    EXIT_IF_TRUE(!SSL_CTX_load_verify_locations(ctx, "../ca/ca-cert.pem", NULL));  
  
    // 加载自己的证书  
    EXIT_IF_TRUE(SSL_CTX_use_certificate_file(ctx, "server-cert.pem", SSL_FILETYPE_PEM) <= 0);  
  
    // 加载自己的私钥    
    EXIT_IF_TRUE(SSL_CTX_use_PrivateKey_file(ctx, "server-key.pem", SSL_FILETYPE_PEM) <= 0);  
  
    // 判定私钥是否正确    
    EXIT_IF_TRUE(!SSL_CTX_check_private_key(ctx));  
  
    // 创建并等待连接  
    nListenFd = socket(PF_INET, SOCK_STREAM, 0);  
    struct sockaddr_in my_addr;  
    memset(&my_addr, 0, sizeof(my_addr));  
    my_addr.sin_family = PF_INET;  
    my_addr.sin_port = htons(8812);  
    //my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//  INADDR_ANY;  
    my_addr.sin_addr.s_addr = inet_addr("0.0.0.0");//  INADDR_ANY;  
    if (bind(nListenFd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {  
        int a = 2;  
        int b = a;  
    }  
      
    //  
    listen(nListenFd, 10);  
  
    memset(&addr, 0, sizeof(addr));  
    len = sizeof(addr);  
    nAcceptFd = accept(nListenFd, (struct sockaddr *)&addr, (socklen_t*)&len);  
    fprintf(stdout, "Accept a connect from [%s:%d]\n",  
        inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));  
  
    // 将连接付给SSL    
    EXIT_IF_TRUE((ssl = SSL_new(ctx)) == NULL);  
    SSL_set_fd(ssl, nAcceptFd);  
    int n1 = SSL_accept(ssl);  
    if (n1 == -1) {  
        const char* p1 = SSL_state_string_long(ssl);  
        int a = 2;  
        int b = a;  
    }  
    // 进行操作    
    memset(szBuffer, 0, sizeof(szBuffer));  
    nLen = SSL_read(ssl, szBuffer, sizeof(szBuffer));  
    fprintf(stdout, "Get Len %d %s ok\n", nLen, szBuffer);  
  
    strcat(szBuffer, " this is from server");  
    SSL_write(ssl, szBuffer, strlen(szBuffer));  
  
    // 释放资源    
    SSL_free(ssl);  
    SSL_CTX_free(ctx);  
    close(nAcceptFd);  
  
    return 0;  
}  

int main(int argc, char **argv)
{
	return Test_openssl_server_v2();
}
