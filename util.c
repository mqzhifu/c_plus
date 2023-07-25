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