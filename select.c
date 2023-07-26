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
    int max_fd = serverSocket;
    myPrint("serverSocketFD:%d",serverSocket);

    int select_rs, nfds = 0;
    // 设置变量：读/写/异常 集合
    fd_set read_set, write_set, err_set;
    // 初始化检测的读/写/异常 集合 ：清零
    FD_ZERO(&read_set);
    FD_ZERO(&write_set);
    FD_ZERO(&err_set);
    // 把 socketFD 加入到 集合中
    FD_SET(serverSocket, &read_set);
    //设置最大值
    maxFD = serverSocket;
    //开始循环
    while(1){
        select_rs = select(maxFD + 1, &read_set, &write_set, &err_set, NULL);
        myPrint("select wake up~");
        if (r == -1 && errno == EINTR){
            error("select_rs err",-400);
        }

        if (select_rs == -1) {
            error("select_rs err",-401);
        }

        if (FD_ISSET(serverSocket, &read_set)) {
            int client = accept_client(serverSocket);
            //maxFD = client
            maxFD++;
            FD_SET(client, &read_set);
            FD_SET(client, &write_set);
            FD_SET(client, &err_set);
        }
        //0 1 2 标准输入输出异常
        for(int i=3; i<maxFD+1; i++){
            if(i == serverSocket ){
                continue;
            }

            if ( FD_ISSET(i, &read_set ) ){
                int  recvDataBufferMax = 1024;//一次最多读取 1024 字节 数据
                char recvDataBuffer[recvDataBufferMax];
                char* recv_data_p = recv_data(i, recvDataBuffer,recvDataBufferMax);
            }
        }

        //0 1 2 标准输入输出异常
//        for(int i=3; i<maxFD+1; i++){
//            if(i != serverSocket && FD_ISSET(i, &write_set)){
//                char send_data_arr[] = "yes,im z!";
//                send_data(client,send_data_arr);
//            }
//        }



         // rdset 中是委托内核检测的所有的文件描述符
//         rdtemp = rdset;
//         int num = select(maxfd+1, &rdtemp, NULL, NULL, NULL);
//         // rdset 中的数据被内核改写了, 只保留了发生变化的文件描述的标志位上的1, 没变化的改为0
//         // 只要rdset中的fd对应的标志位为1 -> 缓冲区有数据了
//
//         // 判断，有没有新连接
//         if( FD_ISSET( serverSocke t, &rdtemp) )
//         {
//             // 接受连接请求, 这个调用不阻塞
//             struct sockaddr_in cliaddr;
//             int cliLen = sizeof(cliaddr);
//             int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &cliLen);
//
//             // 得到了有效的文件描述符
//             // 通信的文件描述符添加到读集合
//             // 在下一轮select检测的时候, 就能得到缓冲区的状态
//             FD_SET(cfd, &rdset);
//             // 重置最大的文件描述符
//             maxfd = cfd > maxfd ? cfd : maxfd;
//         }
//
//         // 没有新连接, 通信
//         for(int i=0; i<maxfd+1; ++i)
//         {
//            // 判断从监听的文件描述符之后到maxfd这个范围内的文件描述符是否读缓冲区有数据
//             if(i != lfd && FD_ISSET(i, &rdtemp))
//             {
//                 // 接收数据
//                 char buf[10] = {0};
//                 // 一次只能接收10个字节, 客户端一次发送100个字节
//                 // 一次是接收不完的, 文件描述符对应的读缓冲区中还有数据
//                 // 下一轮select检测的时候, 内核还会标记这个文件描述符缓冲区有数据 -> 再读一次
//                 // 	循环会一直持续, 知道缓冲区数据被读完位置
//                 int len = read(i, buf, sizeof(buf));
//                 if(len == 0)
//                 {
//                     printf("客户端关闭了连接...\n");
//                     // 将检测的文件描述符从读集合中删除
//                     FD_CLR(i, &rdset);
//                     close(i);
//                 }
//                 else if(len > 0)
//                 {
//                     // 收到了数据
//                     // 发送数据
//                     write(i, buf, strlen(buf)+1);
//                 }
//                 else
//                 {
//                     // 异常
//                     perror("read");
//                 }
//             }
//         }
        break;
     }



}