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
int Test_openssl_client_v2()  
{  
  
    SSL_METHOD  *meth;  
    SSL_CTX     *ctx;  
    SSL         *ssl;  
  
    int nFd;  
    int nLen;  
    char szBuffer[1024];  
  
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
    EXIT_IF_TRUE(SSL_CTX_use_certificate_file(ctx, "client-cert.pem", SSL_FILETYPE_PEM) <= 0);  
  
    // 加载自己的私钥  
    EXIT_IF_TRUE(SSL_CTX_use_PrivateKey_file(ctx, "client-key.pem", SSL_FILETYPE_PEM) <= 0);  
  
    // 判定私钥是否正确  
    EXIT_IF_TRUE(!SSL_CTX_check_private_key(ctx));  
  
    // new  
    // 创建连接  
    nFd = socket(PF_INET, SOCK_STREAM, 0);  
    struct sockaddr_in dest;  
    memset(&dest, 0, sizeof(dest));  
    dest.sin_family = AF_INET;  
    dest.sin_port = htons(8812);  
    dest.sin_addr.s_addr = inet_addr("127.0.0.1");  
    if (connect(nFd, (struct sockaddr *) &dest, sizeof(dest)) != 0) {  
        perror("Connect ");  
        exit(errno);  
    }  
  
    // 将连接付给SSL  
    EXIT_IF_TRUE((ssl = SSL_new(ctx)) == NULL);  
    SSL_set_fd(ssl, nFd);  
    int n1 = SSL_connect(ssl);  
    if (n1 == -1) {  
        int n2 = SSL_get_error(ssl, n1);  
  
        const char* p1 = SSL_state_string(ssl);  
    }  
  
    // 进行操作  
    sprintf(szBuffer, "this is from client %d", getpid());  
    int nWriten = SSL_write(ssl, szBuffer, strlen(szBuffer));  
  
    // 释放资源  
    memset(szBuffer, 0, sizeof(szBuffer));  
    nLen = SSL_read(ssl, szBuffer, sizeof(szBuffer));  
    fprintf(stdout, "Get Len %d %s ok\n", nLen, szBuffer);  
  
    SSL_free(ssl);  
    SSL_CTX_free(ctx);  
    close(nFd);  
    return 0;  
}  

int main(int argc, char **argv)
{
  Test_openssl_client_v2();
}
