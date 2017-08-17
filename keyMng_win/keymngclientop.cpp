#include <stdafx.h>

#include <stdio.h>
#include <string.h>
#include "keymngclientop.h"
#include "keymng_msg.h"
#include "poolsocket.h"
#include "keymnglog.h"
#include "keymng_shmop.h"


int MngClient_InitInfo(MngClient_Info *pCltInfo /*out*/)
{
	int ret = 0; 
	strcpy(pCltInfo->clientId, "1111");
	strcpy(pCltInfo->AuthCode, "1111");
	strcpy(pCltInfo->serverId, "0001");
	
	strcpy(pCltInfo->serverip, "192.168.19.251");	
	
	pCltInfo->serverport = 8001;
	pCltInfo->maxnode = 10;
	pCltInfo->shmkey = 0x0011;
	pCltInfo->shmhdl = 0;
	
	ret = KeyMng_ShmInit(pCltInfo->shmkey, pCltInfo->maxnode, &(pCltInfo->shmhdl));
	if (ret != 0) {
		KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "KeyMng_ShmInit error:%d", ret);
		printf("创建/打开 共享内存失败\n");
		return ret;
	}
	
	return 0;
}

int MngClient_Agree(MngClient_Info *pCltInfo, NodeSHMInfo *nodeSHMInfo)
{
	int ret, i;
	int time = 3;

	int connfd = 0;
	
	unsigned char *outData = NULL;
	int outLen = 0;
	
	unsigned char *pDataMsgKey_Res = NULL;
		
	MsgKey_Res *pMsgKey_Res = NULL;
	int iType = 0;
	
	MsgKey_Req  msgkey_Req;
	
	//1. 组织情求报文	MsgKey_Req
	msgkey_Req.cmdType = KeyMng_NEWorUPDATE;
	strcpy(msgkey_Req.clientId, pCltInfo->clientId);
	strcpy(msgkey_Req.AuthCode, pCltInfo->AuthCode);
	strcpy(msgkey_Req.serverId, pCltInfo->serverId);
	
	//2. 产生随机数
	for (i = 0; i < 64; i++) {
		msgkey_Req.r1[i] = 'a' + i;			// abcdef....
	}
	
	//3. 编码情求报文		MsgKey_Req --> TLV
	ret = MsgEncode((void *)&msgkey_Req, ID_MsgKey_Req, &outData, &outLen);
	if (ret != 0) {
		if (outData != NULL) MsgMemFree((void **)&outData, ID_MsgKey_Req);
		KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgEncode error:%d", ret);
		printf("MsgEncode error:%d\n", ret);
		return ret;
	}
	
	//4. 根据服务器 IP、port 建立连接 (通信组件)
	sckClient_init();

	ret = sckClient_connect(pCltInfo->serverip, pCltInfo->serverport, time, &connfd);
	if (ret == Sck_ErrTimeOut) {
		printf("---客户端建立连接超时\n");
		return ret;
	} else if (ret != 0) {
		KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "sckClient_connect error:%d", ret);
		printf("sckClient_connect error:%d\n", ret);		
	}

	//5. 发送请求报文（TLV）
	ret = sckClient_send(connfd, time, outData, outLen);
	if (ret == Sck_ErrPeerClosed)
	{ 
//		KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "客户端检测到 服务器已经关闭 退出");
		printf("sckClient_send 客户端检测到 服务器已经关闭 退出\n");
		sckClient_closeconn(connfd);
		goto END;	
	}
	else if (ret == Sck_ErrTimeOut)
	{
	//	KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "客户端发送数据超时");
		printf(" sckClient_send timeout \n");
		goto END;
	}
	else if (ret != 0)
	{
//		KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户发送数据出错：%d", ret);
		printf("fun sckClient_send() err:%d \n", ret);
		goto END;
	}
	
	printf("--------------客户端发送报文成功---------\n");
	
	// ---- 等待 服务器 回发应答报文

	//6. 接收应答报文	MsgKey_Res（TLV）	
	ret = sckClient_rev(connfd, time, &pDataMsgKey_Res, &outLen); 
	if (ret == Sck_ErrPeerClosed)
	{ 
//		KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "客户端检测到 服务器已经关闭 退出");
		printf("sckClient_rev 客户端检测到 服务器已经关闭 退出\n");
		sckClient_closeconn(connfd);
		goto END;	
	}
	else if (ret == Sck_ErrTimeOut)
	{
//		KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[2], ret, "客户端接收数据超时");
		printf(" sckClient_rev timeout \n");
		goto END;	
	}
	else if (ret != 0)
	{
//		KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户接收数据出错：%d", ret);
		printf("fun sckClient_rev() err:%d \n", ret);
		goto END;	
	}
	
	//7. 解码应答报文 	TLV --> MsgKey_Res 
	ret = MsgDecode(pDataMsgKey_Res, outLen, (void **)&pMsgKey_Res, &iType);
	if (ret != 0) {
//		KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "MsgDecode error:%d", ret);
		printf("MsgDecode error:%d\n", ret);
		goto END;
	}
	
	//8. 根据 rv 判断是否生成密钥 rv = 0 成功、 rv = 1 失败
	if (pMsgKey_Res->rv == 0){
		printf("客户端服务器密钥协商成功。生成密钥编号为：%d\n", pMsgKey_Res->seckeyid);
	} else {
		printf("客户端服务器密钥协商失败\n");
		return -1;
	}
	
	// 组织共享内存密钥结构体
	
	// 利用 r2、 r1 生成密钥		abcdef....	abcdef....	 aabbccddeeff
	for (i = 0; i < 64; i++) {
		nodeSHMInfo->seckey[2*i] = msgkey_Req.r1[i];
		nodeSHMInfo->seckey[2*i+1] = pMsgKey_Res->r2[i];
	}
	nodeSHMInfo->status = 0;		//密钥生效
	strcpy(nodeSHMInfo->clientId, msgkey_Req.clientId);
	strcpy(nodeSHMInfo->serverId, pMsgKey_Res->serverId);
	nodeSHMInfo->seckeyid = pMsgKey_Res->seckeyid;

	//10. 写共享内存
	ret = KeyMng_ShmWrite(pCltInfo->shmhdl, pCltInfo->maxnode, nodeSHMInfo);
	if (ret != 0) {
		KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "KeyMng_ShmWrite error:%d", ret);
		printf("KeyMng_ShmWrite error: %d\n", ret);
		return ret;
	}

	printf("客户端 生成密钥，并存储共享内存完成。\n");
	
END:
	if (outData != NULL) MsgMemFree((void **)&outData, ID_MsgKey_Req);
	if (pDataMsgKey_Res != NULL) sck_FreeMem((void **)&pDataMsgKey_Res);
	if (pMsgKey_Res != NULL) MsgMemFree((void **)&pMsgKey_Res, iType);
			
	return ret;
}

int MngClient_Check(MngClient_Info *pCltInfo)
{
	return 0;	 
}
