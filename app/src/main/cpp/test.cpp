//
// Created by admin on 2016/8/26.
//

#include "test.h"
#include "pthread.h"
#include "stdio.h"
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <android/log.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define  TAG    "native_log"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

int test::func() {
    return 128;
}

struct  parameter {
    char * servname;
};

/**
 * 执行命令
 */
void ExecuteCommandWithPopen(char* command, char* out_result,
                             int resultBufferSize) {
    FILE * fp;
    out_result[resultBufferSize - 1] = '\0';
    fp = popen(command, "r");
    if (fp) {
        fgets(out_result, resultBufferSize - 1, fp);
        out_result[resultBufferSize - 1] = '\0';
        pclose(fp);
    } else {
//        LOGI("popen null,so exit");
        exit(0);
    }
}

/**
 * 检测服务，如果不存在服务则启动.
 * 通过am命令启动一个laucher服务,由laucher服务负责进行主服务的检测,laucher服务在检测后自动退出
 */
void check_and_restart_service(char* service) {
//    LOGI("当前所在的进程pid=",getpid());
    char cmdline[200];
    sprintf(cmdline, "am startservice --user 0 -n %s", service);
    char tmp[200];
    sprintf(tmp, "cmd=%s", cmdline);
    ExecuteCommandWithPopen(cmdline, tmp, 200);
}


void* thread(void* arg) {
    while(1){
        struct parameter *pstru;
        pstru = ( struct parameter *) arg;
        check_and_restart_service(pstru->servname);
        sleep(4);
    }
}


int start(int argc, char* srvname, char* sd) {
    pthread_t id;
    int ret;
    struct rlimit r;

    int pid = fork();
//    LOGI("fork pid: %d", pid);
    if (pid < 0) {
//        LOGI("first fork() error pid %d,so exit", pid);
        exit(0);
    } else if (pid != 0) {
//        LOGI("first fork(): I'am father pid=%d", getpid());
        //exit(0);
    } else { //  第一个子进程
//        LOGI("first fork(): I'am child pid=%d", getpid());
        setsid();
//        LOGI("first fork(): setsid=%d", setsid());
        umask(0); //为文件赋予更多的权限，因为继承来的文件可能某些权限被屏蔽

        int pid = fork();
        if (pid == 0) { // 第二个子进程
            FILE  *fp;
            int fd;
            sprintf(sd,"%s/pid",sd);
            if((fd = open(sd, O_CREAT))== -1) {//打开文件 没有就创建
//                LOGI("%s文件还未创建!",sd);
                ftruncate(fd, 0);
                lseek(fd, 0, SEEK_SET);
            }
            close(fd);

            fp=fopen(sd,"rw");
            if(fp>0){
                char buff1[6];
                int p = 0;
                memset(buff1,0,sizeof(buff1));
                fseek(fp,0,SEEK_SET);
                fgets(buff1,6,fp);  //读取一行（pid）
//                LOGI("读取的进程号：%s",buff1);
                if(strlen(buff1)>1){ // 有值
                    kill(atoi(buff1), SIGTERM); // 把上一次的进程干掉，防止重复执行
//                    LOGI("杀死进程，pid=%d",atoi(buff1));
                }
            }
            fclose(fp);
            fp=fopen(sd,"w");
            char buff[100];
            int k = 3;
            if(fp>0){
                sprintf(buff,"%d",getpid());
                fprintf(fp,"%s\n",buff); // 把进程号写入文件
            }
            fclose(fp);
            fflush(fp);

//            LOGI("I'am child-child pid=%d", getpid());
            chdir("/"); //<span style="font-family: Arial, Helvetica, sans-serif;">修改进程工作目录为根目录，chdir(“/”)</span>
            //关闭不需要的从父进程继承过来的文件描述符。
            if (r.rlim_max == RLIM_INFINITY) {
                r.rlim_max = 1024;
            }
            int i;
            for (i = 0; i < r.rlim_max; i++) {
                close(i);
            }

            umask(0);
            parameter arg;
            arg.servname = srvname;
            ret = pthread_create(&id, NULL, &thread, &arg); // 开启线程，轮询去监听启动服务
            if (ret != 0) {
                printf("Create pthread error!\n");
                exit(1);
            }
            int stdfd = open ("/dev/null", O_RDWR);
            dup2(stdfd, STDOUT_FILENO);
            dup2(stdfd, STDERR_FILENO);
        } else {
            exit(0);
        }
    }
    return 0;
}
