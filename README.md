## 一：生成CA证书

> 目前不使用第三方权威机构的CA来认证，自己充当CA的角色。
> 网上下载一个openssl软件

#### 1.创建私钥：

 `openssl genrsa -out ca/ca-key.pem 1024` 

#### 2.创建证书请求:

 `openssl req -new -out ca/ca-req.csr -key ca/ca-key.pem`

> Country Name (2 letter code) [AU]:cn 
> 
>  State or Province Name (full name) [Some-State]:guangdong
> 
>  Locality Name (eg, city) []:hangzhou
> 
>  Organization Name (eg, company) [Internet Widgits Pty Ltd]:chinabit 
> 
> Organizational Unit Name (eg, section) []:iot
> 
> Common Name (eg, YOUR name) []:beefly

Email Address []:sky 

#### 3.自签署证书：

`openssl x509 -req -in ca/ca-req.csr -out ca/ca-cert.pem -signkey ca/ca-key.pem -days 3650` 

### 4.将证书导出成浏览器支持的.p12格式 ：

`openssl pkcs12 -export -clcerts -in ca/ca-cert.pem -inkey ca/ca-key.pem -out ca/ca.p12` 
密码：xxxxxxxx       

## 二.生成server证书。

### 1.创建私钥 ：

`openssl genrsa -out server/server-key.pem 1024` 

### 2.创建证书请求 ：

`openssl req -new -out server/server-req.csr -key server/server-key.pem` 

* * *

> Country Name (2 letter code) [AU]:cn 
> State or Province Name (full name) [Some-State]:guangdong
> 
> Locality Name (eg, city) []:hangzhou
> 
> Organization Name (eg, company) [Internet Widgits Pty Ltd]:chinabit
> 
> Organizational Unit Name (eg, section) []:iot
> 
> Common Name (eg, YOUR name) []:192.168.1.246  //一定要写服务器所在的ip地址 
> Email Address []:sky 
> 
> ### 3.自签署证书 ：
> 
> openssl x509 -req -in server/server-req.csr -out server/server-cert.pem -signkey server/server-key.pem -CA ca/ca-cert.pem -CAkey ca/ca-key.pem -CAcreateserial -days 3650  
> 
> ### 4.将证书导出成浏览器支持的.p12格式 ：
> 
> `openssl pkcs12 -export -clcerts -in server/server-cert.pem -inkey server/server-key.pem -out server/server.p12`
> 密码：xxxxxxxx
> 
> ## 三.生成client证书
> 
> ### 1.创建私钥 ：
> 
> `openssl genrsa -out client/client-key.pem 1024`  
> 
> ### 2.创建证书请求：
> 
> `openssl req -new -out client/client-req.csr -key client/client-key.pem`

* * *

> Country Name (2 letter code) [AU]:cn
> 
> State or Province Name (full name) [Some-State]:guangdong
> 
> Locality Name (eg, city) []:hangzhou
> 
> Organization Name (eg, company) [Internet Widgits Pty Ltd]:chinabit
> 
> Organizational Unit Name (eg, section) []:iot
> 
> Common Name (eg, YOUR name) []:sky
> 
> Email Address []:sky //就是登入中心的用户（本来用户名应该是Common Name，但是中山公安的不知道为什么使用的Email Address，其他版本没有测试）
> 
> Please enter the following 'extra' attributes
> 
> to be sent with your certificate request 
> A challenge password []:123456
> 
> An optional company name []:tsing  
> 
> ### 3.自签署证书 ：
> 
> `openssl x509 -req -in client/client-req.csr -out client/client-cert.pem -signkey client/client-key.pem -CA ca/ca-cert.pem -CAkey ca/ca-key.pem -CAcreateserial -days 3650`  
> 
> ### 4.将证书导出成浏览器支持的.p12格式 ：
> 
> `openssl pkcs12 -export -clcerts -in client/client-cert.pem -inkey client/client-key.pem -out client/client.p12`
> 
> 密码：xxxxxxxx
