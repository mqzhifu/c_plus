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

#define max(x,y) ((x) > (y) ? (x) : (y))

void  itoa(int num, char* str, int radix)
{
    int i = 0;
    int sum;
    unsigned int num1 = num;  //如果是负数求补码，必须将他的绝对值放在无符号位中在进行求反码
    char str1[33] = { 0 };
    if (num<0) {              //求出负数的补码
        num = -num;
        num1 = ~num;
        num1 += 1;
    }
    if (num == 0) {
        str1[i] = '0';

        i++;
    }
    while(num1 !=0) {                      //进行进制运算
        sum = num1 % radix;
        str1[i] = (sum > 9) ? (sum - 10) + 'a' : sum + '0';
        num1 = num1 / radix;
        i++;
    }
    i--;
    int j = 0;
    for (i; i >= 0; i--) {               //逆序输出
        str[i] = str1[j];
        j++;
    }

}

//错误处理函数
void error(char *msg , int code) {
    fprintf(stderr, "Error: %s  %s \n", msg, strerror(errno));
    exit(1);
}

//int printByPid(char msg[]){
//      pid_t pid ;
//      pid = getpid();
//
//      printf("(%d)%s \n",pid,msg);
//}

//方式二
void myPrint(const char *cmd, ...)
{
      pid_t pid ;
      pid = getpid();
//    printf("%s %s ", __DATE__, __TIME__);
    printf("(%d)",pid);
    va_list args;       //定义一个va_list类型的变量，用来储存单个参数
    va_start(args,cmd); //使args指向可变参数的第一个参数
    vprintf(cmd,args);  //必须用vprintf等带V的
    va_end(args);       //结束可变参数的获取
    printf("\n");
}

// 向客户端发消息
int send_data(int socket, char *s) {
    int result = (int)send(socket, s, strlen(s), 0);
    if (result == -1) {
        fprintf(stderr, "%s: %s \n","和客户端通信发生错误",strerror(errno));
    }

    myPrint("send_data:%s",s);
    return result;
}

char * recv_data(int client ,char* recvDataBuffer,int recvDataBufferNum ){
//    int     recvDataBufferMax = 1024;//一次最多读取 1024 字节 数据
//    char    recvDataBuffer[recvDataBufferMax];
//    int     recvDataLen;

//    一但 accept 函数成功 返回一个 client socketFD，就要立刻做 SOCKET IO 处理，要创建一个新的进程，避免阻塞
//    pid_t pid = fork();
//    myPrint("accept fork new process:");
//    if(pid < 0){
//        error("accept fork error",-9);
//    }else if(pid == 0){
//        myPrint("accept child process,start recv data......");
        //接收数据缓冲区
//            char final_recv_data[255];
//            while(1){//这里不能循环，recv 会造成阻塞
            int recvDataLen = recv(client, recvDataBuffer,recvDataBufferNum, 0);//阻塞接收客户端的数据
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

//            if(recvDataLen > recvDataBufferMax ){//客户端一次发送过来的数据，大于 buffer
//                error("recvDataLen > recvDataBufferMax",-8);
//            }
//                printf("strcat data \n");
//                strcat(final_recv_data,buffer);
//            }
            myPrint("recv  data:%s",recvDataBuffer);
//            int sleepSecond = 10;
//            myPrint("sleep:%d",sleepSecond);
//            sleep(sleepSecond);
        //printf("recv_str_num:%d,recv data is: %s,send_data:%s\n", strlen(final_recv_data), final_recv_data,"yes!");
//    }else{
//        myPrint("fork , im father ,nothing to do...");//父进程，不做任何操作，返回

//    }

    return recvDataBuffer;
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

int accept_client(int serverSocket ){
    struct  sockaddr_in clientAddr;//accept 中返回的参数
    int     addr_len = sizeof(clientAddr);
    int     client;//clientAddr 对应的 socketFD Id

    //调用 accept，进入阻塞, 返回一个新的套接字FD(client)
    //clientAddr 是一个传出参数，accept返回时，传出客户端的地址和端口号
    //addr_len 是一个传入 传出参数，传入的是调用者提供的缓冲区的 clientAddr 的长度，以避免缓冲区溢出
    myPrint("accept_client accept and block...");
    client = accept(serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
    if(client < 0){
        error("accept",-5);
        return client;
    }

    myPrint("accept client:%d , %s:%d",client,inet_ntoa(clientAddr.sin_addr), htons(clientAddr.sin_port));

    return client;
}

int create_socket(int port){
    int     serverSocket;//socketFD
    //创建socket
    serverSocket = open_listener_socket();
    myPrint("create socket ok.");
    //绑定IP端口
    bind_to_port(serverSocket,port);
    myPrint("bind socket ok.");

    //设置服务器上的socket为监听状态.backlog=5
    if(listen(serverSocket, 5) < 0)
    {
        error("listen",-3);
    }

    myPrint("Listening on port: %d", port);

    return serverSocket;
}
