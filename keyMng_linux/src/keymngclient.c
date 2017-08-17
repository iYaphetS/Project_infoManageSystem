#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <pthread.h>
#include "keymnglog.h"

#include "poolsocket.h"
#include "keymngclientop.h"
#include "keymng_msg.h"

int Usage()
{
    int nSel = -1;
    
    system("clear");    
    printf("\n  /*************************************************************/");
    printf("\n  /*************************************************************/");
    printf("\n  /*     1.密钥协商                                            */");
    printf("\n  /*     2.密钥校验                                            */");
    printf("\n  /*     3.密钥注销                                            */");
    printf("\n  /*     4.密钥查看                                            */");
    printf("\n  /*     0.退出系统                                            */");
    printf("\n  /*************************************************************/");
    printf("\n  /*************************************************************/");
    printf("\n\n  选择:");
    scanf("%d", &nSel);
    while(getchar() != '\n'); //把应用程序io缓冲器的所有的数据 都读走,避免影响下一次 输入
    
    return nSel;
}

int main()
{
	int 				ret = 0;
	int 				nSel = 0;
	
	MngClient_Info		mngClientInfo;
	memset(&mngClientInfo, 0, sizeof(MngClient_Info));

    //客户端基础信息初始化
	ret = MngClient_InitInfo(&mngClientInfo);
	if (ret != 0)
	{
        KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "客户端基础信息初始化失败");
        printf("客户端基础信息初始化失败...\n");
	}
	
	while (1)
	{
		nSel = Usage();
		
		switch (nSel)
		{
		case KeyMng_NEWorUPDATE:	
			//密钥协商
			ret =  MngClient_Agree(&mngClientInfo);
            if(ret != 0)
            {
                KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥协商失败");
                printf("密钥协商失败...\n");
            }
			break;
        case KeyMng_Check:
            //密钥校验
			ret = MngClient_Check(&mngClientInfo);
            if(ret != 0)
            {
                KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥校验失败");
                printf("密钥校验失败...\n");
            }
			break;
		case KeyMng_Revoke:	
            //密钥注销
            ret = MngClient_Revoke(&mngClientInfo);
            if(ret != 0)
            {
                KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥注销失败");
                printf("密钥注销失败...\n");
            }
			break;
        case 4:
            //密钥查看
            ret = MngClient_view(&mngClientInfo);
            if(ret != 0)
            {
                KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "密钥查看失败");
                printf("密钥查看失败...\n");
            }
            break;
		case 0:	
            //退出系统
            ret = MngClient_Quit(&mngClientInfo);
            if(ret != 0)
            {
                KeyMng_Log(__FILE__, __LINE__, KeyMngLevel[4], ret, "退出系统失败");
                printf("退出系统失败...\n");
            }
		default :
			printf("选项不支持\n");
			break;
		}
		
		if (ret)
		{
			printf("\n!!!!!!!!!!!!!!!!!!!!ERROR!!!!!!!!!!!!!!!!!!!!");
			printf("\n错误码是：%x\n", ret);
		}
		else
		{
			printf("\n!!!!!!!!!!!!!!!!!!!!SUCCESS!!!!!!!!!!!!!!!!!!!!\n");
		}	
		getchar();	
	}
	
	printf("keymngclient hello...\n");
	return 0;
}

int mainxxxx()
{
	
	KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[0], 0,"func 00000000000000");
	KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[1], 1,"func 11111111111");
	KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[2], 2,"func 2222222222222222");
	KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[3], 3,"func 33333333333333");
	KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], 4,"func 4444444444444444444)");
	printf("keymngclient hello...\n");

	return 0;
}

