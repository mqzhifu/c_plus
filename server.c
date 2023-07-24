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


#define SERVER_PORT 5555

//错误处理函数
void error(char *msg , int code) {
    fprintf(stderr, "Error: %s  %s \n", msg, strerror(errno));
    exit(1);
}


/*
    创建socket函数FD，失败返回-1
    int socket(int domain, int type, int protocol);
    参数1：使用的地址类型，一般都是ipv4，AF_INET
    参数1：套接字类型，tcp：面向连接的稳定数据传输SOCK_STREAM
    参数3：设置为0
*/
int open_listener_socket() {
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        error("Can't open socket",-4);
    }
    return s;
}


// 绑定端口
void bind_to_port(int socket, int port) {

    //对于bind，accept之类的函数，里面套接字参数都是需要强制转换成(struct sockaddr *)
    //bind三个参数：服务器端的套接字的文件描述符，



    struct sockaddr_in name;
    //初始化服务器端的套接字，并用 htons 和 htonl 将端口和地址转成网络字节序
    name.sin_family = PF_INET;
//    name.sin_family = AF_INET;
    name.sin_port = (in_port_t)htons(port);
//    name.sin_port = htons(SERVER_PORT);

    //ip本地ip或用宏INADDR_ANY，代表0.0.0.0，所有地址
    name.sin_addr.s_addr = htonl(INADDR_ANY);

     //正常一个程序绑定一个端口取消后，30秒内不允许再次绑定，防止ctrl+c无效
    int reuse = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1) {
        error("Can't set the reuse option on the socket",-2);
    }
    int c = bind(socket, (struct sockaddr*)&name, sizeof(name));
    if (c == -1) {
        error("Can't bind to socket",-1);
    }
}

// 向客户端发消息
int send_data(int socket, char *s) {
    int result = (int)send(socket, s, strlen(s), 0);
    if (result == -1) {
        fprintf(stderr, "%s: %s \n","和客户端通信发生错误",strerror(errno));
    }

    printf("send_data:%s \n",s);
    return result;
}

int printByPid(string msg){
      pid_t pid ;
      pid = gettid()

      printf("(%d)%s \n",pid,msg);
}

int accept_client(int serverSocket ){
    struct  sockaddr_in clientAddr;//accept 中返回的参数
    int     addr_len = sizeof(clientAddr);
    int     client;//clientAddr 对应的 socketFD Id
    int     recvDataBufferMax = 1024;//一次最多读取 1024 字节 数据
    char    recvDataBuffer[recvDataBufferMax];
    int     recvDataLen;
    //计数器，无用
    int     recvBuffDataCnt = 0;

    //调用accept，会进入阻塞状态,accept返回一个套接字FD，便有两个FD:serverSocket和client
    //serverSocket仍然继续在监听状态，client则负责接收和发送数据

    //clientAddr 是一个传出参数，accept返回时，传出客户端的地址和端口号
    //addr_len 是一个传入 传出参数，传入的是调用者提供的缓冲区的 clientAddr 的长度，以避免缓冲区溢出。
    //传出的是客户端地址结构体的实际长度。
    //出错返回-1

    printf("accept_client accept and block...\n");
    client = accept(serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
    if(client < 0){
        error("accept",-5);
        return client;
    }

    printf("accept client:%d.\n",client);


    //inet_ntoa   ip地址转换函数，将网络字节序IP转换为点分十进制IP
    //表达式：char *inet_ntoa (struct in_addr);
    printf("IP is %s\n", inet_ntoa(clientAddr.sin_addr));
    printf("Port is %d\n", htons(clientAddr.sin_port));

    //一但 accept 函数成功 返回一个 client socketFD，就要立刻做 SOCKET IO 处理，要创建一个新的进程，避免阻塞
    pid_t pid;
    pid = fork();
    printf("fork new process:\n");
    if(pid < 0){
        error("fork error",-9);
    }else if(pid == 0){
        printf("child process,start recv data......\n");
        //接收数据缓冲区
//            char final_recv_data[255];
//            while(1){//这里不能循环，recv 会造成阻塞
            recvDataLen = recv(client, recvDataBuffer, sizeof(recvDataBuffer), 0);//阻塞接收客户端的数据
            printf("recvDataLen:%d \n",recvDataLen);
            if(recvDataLen < 0)
            {
                error("recv error",-6);
//                    break;
            }

            if(recvDataLen == 0){
                printf("recv client data end.\n");
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
            printf("recv  data:%s \n",recvDataBuffer);
        //printf("recv_str_num:%d,recv data is: %s,send_data:%s\n", strlen(final_recv_data), final_recv_data,"yes!");
        char send_data_arr[] = "yes,im z!";
        send_data(client,send_data_arr);
    }else{
        printf("fork , im father ,nothing to do...\n");
        //父进程，不做任何操作，返回
    }

    return client;
}

int spawn(){//zygote

}

void main(){
    //调用socket函数返回的文件描述符
	int     serverSocket;
    //声明两个套接字 sockaddr_in 结构体变量，分别表示客户端和服务器
//    struct  sockaddr_in server_addr;
    //创建socket
    serverSocket = open_listener_socket();

	printByPid("create socket ok.");
    //绑定IP端口
    bind_to_port(serverSocket,SERVER_PORT);

    printf("bind socket ok.\n");


    //设置服务器上的socket为监听状态
    //第2个参数：backlog，半连接+已连接，之和
    if(listen(serverSocket, 5) < 0)
    {
        error("listen",-3);
    }


    printf("Listening on port: %d\n", SERVER_PORT);

//    while(1){
        for(int i=0;i<2;i++){
            pid_t pid;
            pid = fork();
            if(pid < 0 ){
                printf("main fork process err.\n");
            }else if(pid == 0 ){
                prctl(PR_SET_PDEATHSIG,SIGKILL);
                printf("main create child process ok.\n");
                accept_client(serverSocket);
            }else{
                printf("main process ,father nothing to do .\n");
            }

        }
//    }
    pid_t pid,wait_pid;
    int status;
    wait_pid = wait(&status);
    printf("wait_pid:%d",wait_pid);

}


