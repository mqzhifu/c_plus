/*socket tcp服务器端*/

//引入相关函数文件
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdarg.h>
//For Windows
#ifdef _WIN32
int betriebssystem = 1;
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <ws2def.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <io.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
int betriebssystem = 2;
#endif

#include "util.c"

void main(){
     int serverSocket = create_socket(5555);
     int maxfd = lfd;

     // 初始化检测的读集合
     fd_set rdset;
     fd_set rdtemp;
     // 清零
     FD_ZERO(&rdset);
     // 将监听的lfd设置到检测的读集合中
     FD_SET(lfd, &rdset);

     while(1){
         // 默认阻塞
         // rdset 中是委托内核检测的所有的文件描述符
         rdtemp = rdset;
         int num = select(maxfd+1, &rdtemp, NULL, NULL, NULL);
         // rdset中的数据被内核改写了, 只保留了发生变化的文件描述的标志位上的1, 没变化的改为0
         // 只要rdset中的fd对应的标志位为1 -> 缓冲区有数据了
         // 判断
         // 有没有新连接
         if(FD_ISSET(lfd, &rdtemp))
         {
             // 接受连接请求, 这个调用不阻塞
             struct sockaddr_in cliaddr;
             int cliLen = sizeof(cliaddr);
             int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &cliLen);

             // 得到了有效的文件描述符
             // 通信的文件描述符添加到读集合
             // 在下一轮select检测的时候, 就能得到缓冲区的状态
             FD_SET(cfd, &rdset);
             // 重置最大的文件描述符
             maxfd = cfd > maxfd ? cfd : maxfd;
         }

         // 没有新连接, 通信
         for(int i=0; i<maxfd+1; ++i)
         {
            // 判断从监听的文件描述符之后到maxfd这个范围内的文件描述符是否读缓冲区有数据
             if(i != lfd && FD_ISSET(i, &rdtemp))
             {
                 // 接收数据
                 char buf[10] = {0};
                 // 一次只能接收10个字节, 客户端一次发送100个字节
                 // 一次是接收不完的, 文件描述符对应的读缓冲区中还有数据
                 // 下一轮select检测的时候, 内核还会标记这个文件描述符缓冲区有数据 -> 再读一次
                 // 	循环会一直持续, 知道缓冲区数据被读完位置
                 int len = read(i, buf, sizeof(buf));
                 if(len == 0)
                 {
                     printf("客户端关闭了连接...\n");
                     // 将检测的文件描述符从读集合中删除
                     FD_CLR(i, &rdset);
                     close(i);
                 }
                 else if(len > 0)
                 {
                     // 收到了数据
                     // 发送数据
                     write(i, buf, strlen(buf)+1);
                 }
                 else
                 {
                     // 异常
                     perror("read");
                 }
             }
         }
     }



}