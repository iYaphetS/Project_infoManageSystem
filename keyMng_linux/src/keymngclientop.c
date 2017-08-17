#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "keymngclientop.h"
#include "keymng_msg.h"
#include "keymnglog.h"
#include "poolsocket.h"
#include "keymng_shmop.h"


int MngClient_InitInfo(MngClient_Info *pCltInfo)
{
    strcpy(pCltInfo->AuthCode, "1111");
    strcpy(pCltInfo->clientId, "1111");
    strcpy(pCltInfo->serverId, "0001");
    strcpy(pCltInfo->serverip, "127.0.0.1");

    pCltInfo->maxnode = 10;
    pCltInfo->serverport = 9003;
    pCltInfo->shmkey = 0x0011;
    pCltInfo->shmhdl = 0;//共享内存句柄，操作共享内存依据

    int ret = KeyMng_ShmInit(pCltInfo->shmkey, pCltInfo->maxnode, &(pCltInfo->shmhdl));
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端 共享内存初始化失败");
        printf("客户端 共享内存初始化失败...\n");
        return ret;
    }
}


int MngClient_ConnAndhandleData(MngClient_Info *pCltInfo, MsgKey_Req *preq, MsgKey_Res **ppres)
{

    int ret;

    unsigned char	*req_TLV = NULL;
    int req_TLV_len;

    int time = 3;
    int connfd = -1;

    unsigned char	*res_TLV = NULL;
    int res_TLV_len;
    int type;

    //3. 编码请求报文
    ret = MsgEncode((void *)preq, ID_MsgKey_Req, &req_TLV, &req_TLV_len);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端 编码请求报文失败");
        printf("客户端 编码请求报文失败...\n");
        goto END;
    }

    /*--------与服务器建立连接，发报文，接受报文--------*/
    //4. 初始化，建立连接
    sckClient_init();
    
    while(1)
    {
        ret = sckClient_connect(pCltInfo->serverip,
                                pCltInfo->serverport,
                                time, &connfd);
        if(ret == Sck_ErrTimeOut)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "客户端 连接服务器超时");
            printf("客户端 连接服务器超时...\n");
            //客户端 关闭和服务端的连接
            sckClient_closeconn(connfd);
            continue;
        }
        else if(ret != 0)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端连接错误");
            printf("客户端连接错误...\n");
            goto END;
        }
        else if(ret == 0)//成功
            break;
    }
    //5. 发送报文  //6. 接受报文
    while(1)
    {
        //客户端 发送报文
        ret = sckClient_send(connfd, time, req_TLV, req_TLV_len);
        if(ret == Sck_ErrTimeOut)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "客户端发送数据超时");
            printf("客户端发送数据超时...\n");
            continue;
        }
        else if(ret == Sck_ErrPeerClosed)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端发送数据，检测到服务器关闭");
            printf("客户端发送数据，检测到服务器关闭...\n");
            goto END;
        }
        else if(ret != 0)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端发送数据失败");
            printf("客户端发送数据失败...\n");
            goto END;
        }

        //客户端 接受报文
        ret =  sckClient_rev(connfd, time, &res_TLV, &res_TLV_len); //1
        if(ret == Sck_ErrTimeOut)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "客户端接受数据超时");
            printf("客户端接受数据超时...\n");
            goto END;
        }
        else if(ret == Sck_ErrPeerClosed)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端接受数据，检测到服务器关闭");
            printf("客户端接受数据，检测到服务器关闭...\n");
            goto END;
        }
        else if(ret != 0)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端接受数据失败");
            printf("客户端接受数据失败...\n");
            goto END;
        }
        else if(ret == 0)
            break;

    }
    /*---------处理服务器返回的数据---------*/
    //7. 解码回应报文
    ret = MsgDecode( res_TLV, res_TLV_len, (void **)ppres, &type);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端 解码应答报文失败");
        printf("客户端 解码应答报文失败\n");
        goto END;
    }


    //资源释放
END:
    sckClient_closeconn(connfd);
    if(req_TLV != NULL)
    {
        MsgMemFree((void **)&req_TLV,0);
    }

    if(res_TLV != NULL)
    {
        sck_FreeMem((void **)&res_TLV);
    }
    if(ppres != NULL && ret != 0)//?????????????
    {
        MsgMemFree((void **)ppres, ID_MsgKey_Res);
    }
    return ret;
}


int MngClient_Agree(MngClient_Info *pCltInfo)
{
    int ret;

    MsgKey_Res *res = NULL;

    /*----------------------------客户端准备密钥协商的数据-----------------------------------*/
    MsgKey_Req req;

    //1. 生成随机数 //2. 组织请求报文
    int i;
    for(i = 0; i < 64; i++)
    {
        req.r1[i] = 'A' + i;
    }
    strcpy(req.AuthCode, pCltInfo->AuthCode);
    strcpy(req.clientId, pCltInfo->clientId);
    strcpy(req.serverId, pCltInfo->serverId);
    req.cmdType = KeyMng_NEWorUPDATE;

    //3-7 编码 连接 发送数据 接受数据 解码
    ret = MngClient_ConnAndhandleData(pCltInfo, &req, &res);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端处理密钥协商数据业务出错");
        printf("客户端处理密钥协商数据业务出错...");
        return ret;
    }


    //8. 根据 rv 判断是否生成密钥	rv = 0 成功、 rv = 1 失败
    if (res->rv == 0){
        printf("客户端服务器密钥协商成功。生成密钥编号为：%d\n", res->seckeyid);
    } else {
        printf("客户端服务器密钥协商失败\n");
        return -1;
    }

    //9.生成密钥 组织网点密钥信息共享内存信息结构体
    NodeSHMInfo shmInfo;
    shmInfo.status = 0; //密钥有效
    strcpy(shmInfo.clientId, req.clientId);
    strcpy(shmInfo.serverId, res->serverId);
    shmInfo.seckeyid = res->seckeyid;

    for(i = 0; i < 64; i++)
    {
        shmInfo.seckey[2*i] = req.r1[i];
        shmInfo.seckey[2*i + 1] = res->r2[i];
    }

    //printf("req.clientId: %s res->serverId: %s\n", req.clientId, res->serverId);
    //10. 密钥写入共享内存
    ret = KeyMng_ShmWrite(pCltInfo->shmhdl, pCltInfo->maxnode, &shmInfo);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端 密钥写入共享内存失败");
        printf("客户端 密钥写入共享内存失败...\n");
        return ret;
    }

    //释放
    if(res != NULL)
    {
        MsgMemFree((void **)&res, ID_MsgKey_Res);
    }

    return 0;
}

int MngClient_Check(MngClient_Info *pCltInfo)
{
    int ret = 0;

    MsgKey_Res *res = NULL;

    //printf("pCltInfo.clientId: %s pCltInfo->serverId: %s\n", pCltInfo->clientId, pCltInfo->serverId);

    //1.获取客户端对应的共享内存的网点密钥信息
    NodeSHMInfo shmInfo;
    ret = KeyMng_ShmRead(pCltInfo->shmhdl, pCltInfo->clientId, pCltInfo->serverId,  pCltInfo->maxnode, &shmInfo);
    if(ret != 0)
    {
        printf("ret : %d\n", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "读共享内存数据失败");
        printf("读共享内存数据失败...\n");
        return ret;
    }
    /*-----------------------客户端准备密钥校验的数据--------------片段校验法----------------------*/
    MsgKey_Req req;

    //2.从共享内存中取出密钥的前10数据   组织请求报文
    int i;
    for(i = 0; i < 10; i++)
    {
        req.r1[i] = shmInfo.seckey[i];
    }
    strcpy(req.AuthCode, pCltInfo->AuthCode);
    strcpy(req.clientId, pCltInfo->clientId);
    strcpy(req.serverId, pCltInfo->serverId);
    req.cmdType = KeyMng_Check;

    //3-7 编码 连接 发送数据 接受数据 解码
    ret = MngClient_ConnAndhandleData(pCltInfo, &req, &res);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端处理密钥校验数据业务出错");
        printf("客户端处理密钥校验数据业务出错...");
        return ret;
    }

    //8. 根据 rv 判断是否校验成功	rv = 0 成功、 rv = 1 失败
    if (res->rv == 0){
        printf("客户端服务器密钥校验成功。密钥编号为：%d\n", res->seckeyid);
    } else {
        printf("客户端服务器密钥校验失败\n");
        return -1;
    }


    //释放
    if(res != NULL)
    {
        MsgMemFree((void **)&res, ID_MsgKey_Res);
    }
    return ret;
}

int MngClient_Revoke(MngClient_Info *pCltInfo)
{

    int ret = 0;
    MsgKey_Res *res = NULL;

    //1. 组织请求报文
    MsgKey_Req req;
    strcpy(req.AuthCode, pCltInfo->AuthCode);
    strcpy(req.clientId, pCltInfo->clientId);
    strcpy(req.serverId, pCltInfo->serverId);
    req.cmdType = KeyMng_Revoke;

    //3-7 编码 连接 发送数据 接受数据 解码
    ret = MngClient_ConnAndhandleData(pCltInfo, &req, &res);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端处理密钥注销数据业务出错");
        printf("客户端处理密钥注销数据业务出错...");
        return ret;
    }

    //8. 根据 rv 判断是否注销成功	rv = 0 成功、 rv = 1 失败
    if (res->rv == 0){
        printf("客户端服务器密钥注销成功。密钥编号为：%d\n", res->seckeyid);
    } else {
        printf("客户端服务器密钥注销失败\n");
        return -1;
    }

    //9. 注销成功后，修改共享内存中密钥状态
    NodeSHMInfo shmInfo;
    //先读取
    ret = KeyMng_ShmRead(pCltInfo->shmhdl, pCltInfo->clientId, pCltInfo->serverId,  pCltInfo->maxnode, &shmInfo);
    if(ret != 0)
    {
        printf("ret : %d\n", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "读共享内存数据失败");
        printf("读共享内存数据失败...\n");
        return ret;
    }

    //修改状态
    shmInfo.status = 1;//密钥失效

    //写入共享内存
    ret = KeyMng_ShmWrite(pCltInfo->shmhdl, pCltInfo->maxnode, &shmInfo);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端 密钥写入共享内存失败");
        printf("客户端 密钥写入共享内存失败...\n");
        return ret;
    }

    //释放
    if(res != NULL)
    {
        MsgMemFree((void **)&res, ID_MsgKey_Res);
    }
    return ret;
}

int MngClient_view(MngClient_Info *pCltInfo)
{
    int ret = 0;
    NodeSHMInfo shmInfo;
    ret = KeyMng_ShmRead(pCltInfo->shmhdl, pCltInfo->clientId, pCltInfo->serverId,  pCltInfo->maxnode, &shmInfo);
    if(ret != 0)
    {
        printf("ret : %d\n", ret);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "读共享内存数据失败");
        printf("读共享内存数据失败...\n");
        return ret;
    }

    printf("密钥状态\t客户端id\t服务器端id\t对称密钥id\n");
    printf("%d\t\t%s\t\t%s\t\t%d\n", shmInfo.status, shmInfo.clientId, shmInfo.serverId, shmInfo.seckeyid);
    return ret;
}

int MngClient_Quit(MngClient_Info *pCltInfo)
{
    exit(0);
}
