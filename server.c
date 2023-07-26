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



void worker_cycle(int serverSocket){
    prctl(PR_SET_PDEATHSIG,SIGKILL);//父进程退出，子进程也结束
    while(1){
        int client = accept_client(serverSocket);
        int     recvDataBufferMax = 1024;//一次最多读取 1024 字节 数据
        char recvDataBuffer[recvDataBufferMax];
        char* recv_data_p = recv_data(client, recvDataBuffer,recvDataBufferMax);

        char send_data_arr[] = "yes,im z!";
        send_data(client,send_data_arr);
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


