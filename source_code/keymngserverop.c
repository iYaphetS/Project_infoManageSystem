#include <stdio.h>
#include <string.h>
#include "keymngserverop.h"

#include "keymnglog.h"
#include "keymng_shmop.h"
#include "icdbapi.h"
#include "keymng_dbop.h"

int MngServer_InitInfo(MngServer_Info *svrInfo)
{
    strcpy(svrInfo->serverId, "0001");
    strcpy(svrInfo->dbuse, "SECMNG");
    strcpy(svrInfo->dbpasswd, "SECMNG");
    strcpy(svrInfo->dbsid, "orcl");
    strcpy(svrInfo->serverip, "127.0.0.1");
    svrInfo->dbpoolnum = 5;
    svrInfo->serverport = 9003;
    svrInfo->maxnode = 10;
    svrInfo->shmkey = 0x0022;
    svrInfo->shmhdl = 0;//?

    //初始化共享内存
    int ret = KeyMng_ShmInit(svrInfo->shmkey, svrInfo->maxnode, &svrInfo->shmhdl);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "共享内存初始化失败");
        printf("共享内存初始化失败...\n");
        return ret;
    }
    //初始化数据库连接池
    ret = IC_DBApi_PoolInit(svrInfo->dbpoolnum, svrInfo->dbsid, svrInfo->dbuse, svrInfo->dbpasswd);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "数据库连接池初始化失败");
        printf("数据库连接池初始化失败...\n");
        return ret;
    }
    return 0;
}
//static int SecKeyid = 0;
int MngServer_Agree(MngServer_Info *svrInfo, MsgKey_Req *req, unsigned char **outData, int *datalen)
{
    int ret = 0;
    MsgKey_Res res;
    unsigned char	*res_TLV = NULL;
    int				res_TLV_len  = 0;
    ICDBHandle handle = NULL;

    //生成随机数 //组织回应报文
    int i;
    for(i = 0; i < 64; i++)
    {
        res.r2[i] = 'A' + i;
    }
    strcpy(res.clientId, req->clientId);
    res.rv = 0;//0代表成功，1代表失败
    strcpy(res.serverId, svrInfo->serverId);
    res.seckeyid = 0;

    //得到数据库连接池的句柄
    ret = IC_DBApi_ConnGet(&handle, 0, 0);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥协商获取连接池句柄失败");
        printf("密钥协商获取连接池句柄失败...\n");
        goto END;
    }
    //开启事务----保证数据的一致性（原子性，一致性，隔离性，）
    ret = IC_DBApi_BeginTran(handle);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥协商开启事务失败");
        printf("密钥协商开启事务失败...\n");
        goto END;
    }
    //查询数据库表得到seckeyid,并更新seckeyid
    KeyMngsvr_Agree_DBOp_GetKeyID((void *)handle, &res.seckeyid);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥协商，查询密钥ID并更新失败");
        printf("密钥协商，查询密钥ID并更新失败...\n");
        goto END;
    }

    //编码回应报文
    ret = MsgEncode((void *)&res, ID_MsgKey_Res, &res_TLV, &res_TLV_len);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥协商编码回应报文失败");
        printf("密钥协商编码回应报文失败...\n");
        goto END;
    }

    *outData = res_TLV;
    *datalen = res_TLV_len;

    //组织网点密钥信息共享内存信息结构体
    NodeSHMInfo shmInfo;
    shmInfo.status = 0; //密钥有效
    strcpy(shmInfo.clientId, req->clientId);
    strcpy(shmInfo.serverId, res.serverId);
    shmInfo.seckeyid = res.seckeyid;


    for(i = 0; i < 64; i++)
    {
        shmInfo.seckey[2*i] = req->r1[i];
        shmInfo.seckey[2*i + 1] = res.r2[i];
    }

    //写入共享内存
    ret = KeyMng_ShmWrite(svrInfo->shmhdl, svrInfo->maxnode, &shmInfo);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥协商，密钥写入共享内存失败");
        printf("密钥协商，密钥写入共享内存失败...\n");
        goto END;
    }

    //将协商的密钥永久性备份存储到数据库
    ret = KeyMngsvr_Agree_DBOp_WriteSecKey((void *)handle, &shmInfo);
    if(ret != 0)
    {
        IC_DBApi_Rollback(handle);//事务回滚
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥协商写密钥到数据库失败");
        printf("密钥协商写密钥到数据库失败...\n");
        goto END;
    }

    //关闭事务
    IC_DBApi_Commit((void *)handle);//事务提交
    IC_DBApi_ConnFree(handle, 1);//可修复
END:
    if(res_TLV != NULL && ret != 0)
        MsgMemFree((void **)&res_TLV, 0);
    if(handle != NULL)
    {
        //IC_DBApi_ConnFree(handle, 0);
        if(ret == IC_DB_CONNECT_ERR)
        {
            IC_DBApi_ConnFree(handle, 0);
        }
        else
            IC_DBApi_ConnFree(handle, 1);
    }
    return ret;
}

int MngServer_Check(MngServer_Info *svrInfo, MsgKey_Req *req, unsigned char **outData, int *datalen)
{
    int ret = 0;
    MsgKey_Res res;
    unsigned char	*res_TLV = NULL;
    int				res_TLV_len  = 0;


    //读共享内存数据
    NodeSHMInfo shmInfo;
    ret = KeyMng_ShmRead(svrInfo->shmhdl, req->clientId, req->serverId, svrInfo->maxnode,  &shmInfo);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器读共享内存失败");
        printf("服务器读共享内存失败...\n");
    }



    //组织回应报文
    //取出密钥与客户端传过来的部分密钥进行校验
    strcpy(res.clientId, req->clientId);
    strcpy(res.serverId, req->serverId);
    res.rv = 0;//默认密钥校验成功
    res.seckeyid = shmInfo.seckeyid;
    memset(res.r2, 0, sizeof(res.r2));
    int i;
    for(i = 0; i < 10; i++)
    {
        if(shmInfo.seckey[i] != req->r1[i])
        {
            res.rv = 1;
            break;
        }
    }
    //编码回应报文
    ret = MsgEncode((void *)&res, ID_MsgKey_Res, &res_TLV, &res_TLV_len);
    if(ret != 0)
    {
        if(res_TLV != NULL)
            MsgMemFree((void **)&res_TLV, 0);
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "编码回应报文失败");
        printf("编码回应报文失败...\n");
        return ret;
    }

    *outData = res_TLV;
    *datalen = res_TLV_len;


    return ret;
}

int MngServer_Revoke(MngServer_Info *svrInfo, MsgKey_Req *req, unsigned char **outData, int *datalen)
{
    int ret = 0;
    MsgKey_Res res;
    unsigned char	*res_TLV = NULL;
    int				res_TLV_len  = 0;
    ICDBHandle handle = NULL;


    //读共享内存数据
    NodeSHMInfo shmInfo;
    ret = KeyMng_ShmRead(svrInfo->shmhdl, req->clientId, req->serverId, svrInfo->maxnode,  &shmInfo);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "服务器读共享内存失败");
        printf("服务器读共享内存失败...\n");
        goto END;
    }

    //得到数据库连接池的句柄
    ret = IC_DBApi_ConnGet(&handle, 0, 0);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥注销获取连接池句柄失败");
        printf("密钥注销获取连接池句柄失败...\n");
        goto END;
    }
    //开启事务----保证数据的一致性（原子性，）
    ret = IC_DBApi_BeginTran(handle);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥注销开启事务失败");
        printf("密钥注销开启事务失败...\n");
        goto END;
    }



    //组织回应报文
    //取出密钥与客户端传过来的部分密钥进行校验
    strcpy(res.clientId, req->clientId);
    strcpy(res.serverId, req->serverId);
    res.rv = 1;//默认密钥注销失败
    res.seckeyid = shmInfo.seckeyid;
    memset(res.r2, 0, sizeof(res.r2));
    if(strcmp(req->clientId, shmInfo.clientId) == 0 && strcmp(req->serverId, shmInfo.serverId) == 0)
    {
        res.rv = 0;
        //组织网点密钥信息共享内存信息结构体
        shmInfo.status = 1;//注销成功
        //写入共享内存
        ret = KeyMng_ShmWrite(svrInfo->shmhdl, svrInfo->maxnode, &shmInfo);
        if(ret != 0)
        {
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥状态修改失败");
            printf("密钥状态修改失败...\n");
            goto END;
        }
        //更新数据库密钥存储信息表
        ret = KeyMngsvr_Revoke_DBOp_UpdateSecKey(handle, &shmInfo);
        if(ret != 0)
        {
            IC_DBApi_Rollback(handle);//事务回滚
            KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥注销写密钥到数据库失败");
            printf("密钥注销写密钥到数据库失败...\n");
            goto END;
        }

        //关闭事务
        IC_DBApi_Commit(handle);//事务提交
        IC_DBApi_ConnFree(handle, 1);//可修复

    }

    //编码回应报文
    ret = MsgEncode((void *)&res, ID_MsgKey_Res, &res_TLV, &res_TLV_len);
    if(ret != 0)
    {

        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "编码回应报文失败");
        printf("编码回应报文失败...\n");
        goto END;
    }

    *outData = res_TLV;
    *datalen = res_TLV_len;
END:
    if(res_TLV != NULL && ret != 0)
        MsgMemFree((void **)&res_TLV, 0);
    if(handle != NULL)
    {
        //IC_DBApi_ConnFree(handle, 0);
        if(ret == IC_DB_CONNECT_ERR)
        {
            IC_DBApi_ConnFree(handle, 0);
        }
        else
            IC_DBApi_ConnFree(handle, 1);
    }
    return ret;
}

