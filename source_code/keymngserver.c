#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "keymnglog.h"

#include "poolsocket.h"
#include "keymngserverop.h"
#include "keymng_msg.h"
#include "icdbapi.h"

#define CREATE_DEAMON if(fork()>0)exit(1);setsid();

MngServer_Info svrInfo;
int flag = 0;

void cathSignal(int signum)
{
    flag = 1;
    return;
}

void *thread_callback(void *arg)
{
    int connfd = (int)arg;

    int ret = 0;
    int time = 3;
    unsigned char *req_TLV = NULL;
    int req_TLV_len = 0;

    MsgKey_Req *req = NULL;
    int type;

    unsigned char *res_TLV = NULL;
    int res_TLV_len = 0;

    while(1)
    {
        if(flag == 1)
            break;


        //服务器端端接受报文
        ret = sckServer_rev(connfd, time, &req_TLV, &req_TLV_len); //1
        if(ret == Sck_ErrPeerClosed)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器接受数据,检测到客户端关闭");
            printf("服务器接受数据,检测到客户端关闭...\n");
            goto END;
        }
        else if(ret == Sck_ErrTimeOut)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "服务器接受数据,服务器接受数据超时");
            printf("服务器接受数据,服务器接受数据超时...\n");
            sckServer_close(connfd);
            continue;
        }
        else if(ret != 0)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器接受数据失败");
            printf("服务器接受数据失败...\n");
            goto END;
        }

        /*------------------数据业务处理------------------*/
        //解码收到的报文
        ret = MsgDecode(req_TLV, req_TLV_len, (void **)&req, &type);
        if(ret != 0)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器解码报文失败");
            printf("服务器解码报文失败...\n");
            goto END;
        }

        //根据请求类型，进行不同的业务处理
        switch (req->cmdType) {
        case KeyMng_NEWorUPDATE:
            ret = MngServer_Agree(&svrInfo, req, &res_TLV, &res_TLV_len);
            if(ret != 0)
            {
                KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥协商失败");
                printf("服务器密钥协商失败...\n");
                goto END;
            }
            break;
        case KeyMng_Check:
            ret = MngServer_Check(&svrInfo, req, &res_TLV, &res_TLV_len);
            if(ret != 0)
            {
                KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器密钥校验失败");
                printf("服务器密钥校验失败...\n");
                goto END;
            }
            break;
        case KeyMng_Revoke:
            ret = MngServer_Revoke(&svrInfo, req, &res_TLV, &res_TLV_len);
            if(ret != 0)
            {
                KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器密钥注销失败");
                printf("服务器密钥注销失败...\n");
                goto END;
            }
            break;
        default:
            break;
        }
        /*---------------------------------------------------*/
        //服务器端发送报文
        ret = sckServer_send(connfd, time, res_TLV, res_TLV_len);
        if(ret == Sck_ErrPeerClosed)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器发送数据,检测到客户端关闭");
            printf("服务器发送数据,检测到客户端关闭...\n");
            goto END;
        }
        else if(ret == Sck_ErrTimeOut)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "服务器发送数据,服务器发送数据超时");
            printf("服务器发送数据,服务器发送数据超时...\n");
            goto END;
        }
        else if(ret != 0)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器发送数据失败");
            printf("服务器发送数据失败...\n");
            goto END;
        }
    }

END:
    sckServer_close(connfd);

    if(req_TLV != NULL)
    {
        sck_FreeMem((void **)&req_TLV);
    }
    if(req != NULL)
    {
        MsgMemFree((void **)&req, type);
    }
    if(res_TLV != NULL)
    {
        MsgMemFree((void **)&res_TLV, 0);
    }

    return NULL;
}


int main()
{
    int ret;
    int listenfd = -1;

    int time = 3;
    int connfd = -1;
    pthread_t 	tid;

    //创建守护进程
    CREATE_DEAMON;

#if 0
    //3个文件描述符重定向
    close(STDIN_FILENO);
    open("/dev/null", O_RDWR);
    dup2(0, STDOUT_FILENO);
    dup2(0, STDERR_FILENO);
#endif

    //通过信号管理服务器内存资源和生命周期
    //信号注册
    signal(SIGUSR2, cathSignal);


    //服务器基础信息初始化
    ret = MngServer_InitInfo(&svrInfo);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器基础信息初始化失败");
        printf("服务器基础信息初始化失败\n");
        return ret;
    }

    //服务器初始化,获得监听的fd
    ret = sckServer_init(svrInfo.serverport, &listenfd);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器初始化失败");
        printf("服务器初始化失败\n");
        return ret;
    }

    while(1)
    {
        if(flag == 1)
            break;


        //接受客户端连接
        ret = sckServer_accept(listenfd, time, &connfd);
        if(ret == Sck_ErrTimeOut)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "服务器等待客户端连接超时");
            printf("服务器等待客户端连接超时...\n");
            sckServer_close(connfd);
            continue;
        }
        else if(ret != 0)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端连接失败");
            printf("客户端连接失败...\n");
            break;
        }

        ret = pthread_create(&tid, NULL, thread_callback, (void *)connfd);
        if(ret != 0)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器线程创建失败");
            printf("服务器线程创建失败...\n");
            break;
        }
        pthread_join(tid, NULL);
    }

    sckServer_close(connfd);
    IC_DBApi_PoolFree();
    printf("-----服务器正在退出---------\n");
    return 0;
}
