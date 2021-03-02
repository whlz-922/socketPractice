/************server*********/
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <thread>
#include <iostream>

#define PORT 7000 //端口
#define QUEUE 20  //连接请求队列

int conn;

void thread_task()
{
}

int main(){
	fd_set rfds;
	struct timeval tv;
	int retval, maxfd;
	//成功则返回一个套接字描述符 sockfd AF_INET为ipv4协议
	int ss = socket(AF_INET, SOCK_STREAM, 0);
	//存储地址和端口 用于信息的显示及存储使用
	struct sockaddr_in server_sockaddr;
	//设置sockaddr_in 结构体相关参数
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);//将一个无符号短整型数值转换为网络字节序
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(ss, (struct sockaddr*) & server_sockaddr, sizeof(server_sockaddr)) == -1){
		perror("bind");
		exit(1);
	}
	if(listen(ss, QUEUE) == -1){
		perror("listen");
		exit(1);
	}
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
	//成功返回非负描述字,出错返回-1
	conn = accept(ss, (struct sockaddr*)&client_addr, &length);
	//如果accept成功，那么其返回值是由内核自动生成的一个全新描述符，代表与所返回客户的tcp连接
	//accept之后就会用新的套接字conn
	if( conn < 0){
		perror("connect");
		exit(1);
	}

	char buffer[1024];
	//创建另一个线程
	//std::thread t(thread_task);
	//t.join();
	//char buf[1024]
	//主线程
	while(1){
		/*把可读文件描述符的集合清空*/
		FD_ZERO(&rfds);
		/*把标准输入的文件描述符加入到集合中*/
		FD_SET(0, &rfds);
		maxfd = 0;
		/*把当前的文件描述符加入到集合中*/
		FD_SET(conn, &rfds);
		/*找出文件描述符集合中最大的文件描述符*/
		if(maxfd < conn){
			maxfd = conn;
		}
		//设置超时间
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		//等待聊天
		retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
		if(retval == -1){
			break;
		}
		else if(retval == 0){
			continue;
		}
		else{
			//客户端发来了消息
			if(FD_ISSET(conn,&rfds)){
				char buffer[1024];
				memset(buffer, 0, sizeof(buffer));
				int len = recv(conn, buffer, sizeof(buffer), 0);
			        if(strcmp(buffer, "exit\n") == 0){
					break;
				}	
				printf("client: %s", buffer);
			}
			/*用户输入了信息，开始处理信息并发送*/
			if(FD_ISSET(0, &rfds)){
				char buf[1024];
				fgets(buf, sizeof(buf), stdin);
				send(conn, buf, sizeof(buf), 0);
			}
		}
	}
	close(conn);
	close(ss);
	return 0;
}
