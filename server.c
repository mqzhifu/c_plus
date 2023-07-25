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

//#define SERVER_PORT 5555




// 向客户端发消息
int send_data(int socket, char *s) {
    int result = (int)send(socket, s, strlen(s), 0);
    if (result == -1) {
        fprintf(stderr, "%s: %s \n","和客户端通信发生错误",strerror(errno));
    }

    myPrint("send_data:%s",s);
    return result;
}

int recv_data(int client ){
    int     recvDataBufferMax = 1024;//一次最多读取 1024 字节 数据
    char    recvDataBuffer[recvDataBufferMax];
    int     recvDataLen;
    //计数器，无用
    int     recvBuffDataCnt = 0;

    //一但 accept 函数成功 返回一个 client socketFD，就要立刻做 SOCKET IO 处理，要创建一个新的进程，避免阻塞
//    pid_t pid = fork();
//    myPrint("accept fork new process:");
//    if(pid < 0){
//        error("accept fork error",-9);
//    }else if(pid == 0){
//        myPrint("accept child process,start recv data......");
        //接收数据缓冲区
//            char final_recv_data[255];
//            while(1){//这里不能循环，recv 会造成阻塞
            recvDataLen = recv(client, recvDataBuffer, sizeof(recvDataBuffer), 0);//阻塞接收客户端的数据
            myPrint("recvDataLen:%d",recvDataLen);
            if(recvDataLen < 0)
            {
                error("recv error",-6);
//                    break;
            }

            if(recvDataLen == 0){
                myPrint("recv client data end");
                error("recvDataLen == 0",-7);
//                    break;
            }

            if(recvDataLen > recvDataBufferMax ){//客户端一次发送过来的数据，大于 buffer
                error("recvDataLen > recvDataBufferMax",-8);
            }
//                printf("strcat data \n");
//                strcat(final_recv_data,buffer);
//                //这里防止死循环，也是防止C端恶意攻击
//                recvBuffDataCnt++;
//                if(recvBuffDataCnt > 10){
//                    printf(" err,cnt > 10 exec!\n");
//                    break;
//                }
//            }
            myPrint("recv  data:%s",recvDataBuffer);
//            int sleepSecond = 10;
//            myPrint("sleep:%d",sleepSecond);
//            sleep(sleepSecond);
        //printf("recv_str_num:%d,recv data is: %s,send_data:%s\n", strlen(final_recv_data), final_recv_data,"yes!");
        char send_data_arr[] = "yes,im z!";
        send_data(client,send_data_arr);
//    }else{
//        myPrint("fork , im father ,nothing to do...");//父进程，不做任何操作，返回

//    }

    return client;
}

int spawn(){//zygote

}

void worker_cycle(int serverSocket){
    prctl(PR_SET_PDEATHSIG,SIGKILL);//父进程退出，子进程也结束
    while(1){
        int client = accept_client(serverSocket);
        recv_data(client);
    }
}

void main(){
    int serverSocket = create_socket(5555);

    int worker_process_num = 5;//开启几个子进程，用于 accept
//    int pipe_fd_1[worker_process_num],pipe_fd_2[worker_process_num];//创建 2个 管道，用于父子进程通信
//    if( pipe(pipe_fd_1) < 0 ){
//        error("pipe pipe_fd_1 err",-11);
//    }
//
//    if( pipe(pipe_fd_2) < 0 ){
//        error("pipe pipe_fd_2 err",-12);
//    }

    //开始创建子进程 accept
    for(int i=0 ; i < worker_process_num ; i++){
        pid_t pid = fork();
        if(pid < 0 ){
            myPrint("main fork process err.");
        }else if(pid == 0 ){
//            if( i == 0 ){
//                int pid = (int)getpid();
//                char pidBuff[100] = {0};
//
//                sprintf(pidBuff," im child 0 ,pid:%d ",pid);
//                printf("%s\n", pidBuff);
//
//                close ( pipe_fd_1[0] );
//                write( pipe_fd_1[1],pidBuff,strlen(pidBuff) );
//            }
//
//            if ( i == 1){
//                close ( pipe_fd_2[0] );
//                write( pipe_fd_2[1],"im child 1",strlen("im child 1") );
//            }

            myPrint("main create child worker process ok.");
            worker_cycle(serverSocket);
            break;
        }else{

//            char buff[100];
//            if(i == 0){
//                close ( pipe_fd_1[1] );
//
//                int readPipeNum = read(pipe_fd_1[0],buff,sizeof(buff));
//                printf("read over, n==[%d], buf==[%s]",readPipeNum,buff);
//            }
        }
    }
//    int childPidList[2] = {0};
//    int len = sizeof(childPidList)/sizeof(childPidList[0]);
//    for(int i=0;i<worker_process_num;i++){
//        myPrint("child pid %d:",childPidList[i]);
//    }

    int status;
    pid_t wait_pid = wait(&status);
    myPrint("wait_pid:%d",wait_pid);

}


