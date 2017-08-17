#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "keymng_dbop.h"
#include "keymngserverop.h"
#include "keymnglog.h"
#include "icdbapi.h"

//读keysn表 更新ikeysn列 +1 ---> seckeyid 
int KeyMngsvr_Agree_DBOp_GetKeyID(void *dbhdl, int *keyid)
{
	int 			ret = 0;
	int 			ikeysn;
	
	ICDBRow     	row;
    ICDBField   	field[7];
    
    char			mysql1[1024];
    char			mysql2[1024];

    memset(field, 0, sizeof(field));
    memset(mysql1, 0, sizeof(mysql1));   
   	memset(mysql2, 0, sizeof(mysql2));   

	if (dbhdl== NULL || keyid==NULL)
	{
		ret = -1;
		KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"func KeyMngsvr_DBOp_GenKeyID() err");
       	goto END;
	}
	
	sprintf(mysql1, "select ikeysn from SECMNG.KEYSN for update ");//使用sql锁 for update  
	
	// 读取序列号累加器
    field[0].cont = (char *)&ikeysn;
                
    row.field = field;
    row.fieldCount = 1;	
    
    // 累加器加1 
	sprintf(mysql2, "update SECMNG.KEYSN set ikeysn = ikeysn + 1");  
	
    ret = IC_DBApi_ExecSelSql(dbhdl, mysql1, &row);    
	if (ret != 0)
	{
    	KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"func IC_DBApi_ExecSelSql() err,\n sql:%s", mysql1);
    	goto END;	
	}
		
    ret =  IC_DBApi_ExecNSelSql(dbhdl, mysql2); //执行单个非select语言
    if (ret != 0)
    {
    	KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"func IC_DBApi_ExecNSelSql() err, sql:%s", mysql2);
    	goto END;
    }
    
 	*keyid = ikeysn;
  
END:
	
	return ret;
}

//写密钥  插入共享内存结构体 到数据库   seckinof
int KeyMngsvr_Agree_DBOp_WriteSecKey(void *dbhdl, NodeSHMInfo *pNodeInfo)
{
	int         ret = 0;
    char        mysql_insert[2048] = {0};
    char        mysql_update[2048] = {0};
    
    char		optime[24] = {0};
    char		tmpseckey2[1024];
    int			tmplen = 1024;
    //char 		buf2[1024];
    
    memset(tmpseckey2, 0, sizeof(tmpseckey2));
    memset(mysql_insert, 0, sizeof(mysql_insert));
    //memset(mysql_update, 0, sizeof(mysql_update));

    
    // 获取当前操作时间
    ret = IC_DBApi_GetDBTime(dbhdl, optime);
    if (ret != 0)
    {
    	KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"func IC_DBApi_GetDBTime() err");
       	goto END;
    }
    
    // base64编码 Oracle 9i
	ret = IC_DBApi_Der2Pem( pNodeInfo->seckey, sizeof(pNodeInfo->seckey) , tmpseckey2, &tmplen );
    if (ret != 0)
    {
    	KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"func IC_DBApi_Der2Pem() err\n");
       	goto END;
    }

    //组织sql语句
    sprintf(mysql_insert, "Insert Into SECMNG.SECKYEINFO(clientid, serverid, keyid, createtime, state, seckey) \
                    values ('%s', '%s', %d, '%s', %d, '%s') ", pNodeInfo->clientId,  pNodeInfo->serverId, pNodeInfo->seckeyid, optime, 0, tmpseckey2 );
    //sprintf(mysql_update, "update SECMNG.SECKYEINFO set keyid = %d where seckey = '%s'", pNodeInfo->seckeyid, tmpseckey2 );
   

    //printf("---------1---------------");
    //执行非select sql语句
    ret = IC_DBApi_ExecNSelSql(dbhdl, mysql_insert);
    if (ret != 0)
    {
        //printf("---------2---------------");

        KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret, "func IC_DBApi_ExecNSelSql() err \n sql===>%s\n", mysql_update);
#if 0
        ret = IC_DBApi_ExecNSelSql(dbhdl, mysql_insert);
        if(ret != 0)
        {
            printf("---------3---------------");

            KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret, "func IC_DBApi_ExecNSelSql() err \n sql===>%s\n", mysql_insert);
            goto END;
        }
#endif
        goto END;
    }
  
 END:	
	return ret;
}

//测试PEM和DER互转
int IC_DBApi_Pem2Der_Test2()
{
	int	 			rv = -1;
	char 			str[1024];
	char 			strPem[1024];
	unsigned char 	strDer[1024];
	int	 			strPemLen = 1024;
	int	 			strDerLen = 1024;
	
	memset(str, 0, 1024);
	memset(strPem, 0, 1024);
	memset(strDer, 0, 1024);
	
	printf("请输入要从DER转化为PEM的字符串：");
	//fgets(str, 256, stdin);
//	scanf("%s", str);
//	while(getchar() != '\n');

	strcpy(str, "abcdefg");
    
    //DER2PEM
    rv = IC_DBApi_Der2Pem((unsigned char*)str, strlen(str), strPem, &strPemLen);
    if(rv)
    {
    	printf("DER2PEM ERROR!\n");
    	return rv;
    }
    else
    {
    	printf("DER2PEM OK!PEM is %s\n", strPem);
    }
    
    //PEM2DER
    rv = IC_DBApi_Pem2Der(strPem, strPemLen, strDer, &strDerLen);
    if(rv)
    {
    	printf("PEM2DER ERROR!\n");
    	return rv;
    }
    else
    {
    	printf("PEM2DER OK!DER is %s\n", strDer);
    }
	
	return 0;
}

int KeyMngsvr_Revoke_DBOp_UpdateSecKey(void *dbhdl, NodeSHMInfo *pNodeInfo)
{
    int ret = 0;
    char        mysql_update[2048] = {0};
    memset(mysql_update, 0, sizeof(mysql_update));

    char		tmpseckey2[1024];
    int			tmplen = 1024;
    //char 		buf2[1024];

    memset(tmpseckey2, 0, sizeof(tmpseckey2));
    // base64编码 Oracle 9i
    ret = IC_DBApi_Der2Pem( pNodeInfo->seckey, sizeof(pNodeInfo->seckey) , tmpseckey2, &tmplen );
    if (ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret,"func IC_DBApi_Der2Pem() err\n");
        return ret;
    }

    sprintf(mysql_update, "update SECMNG.SECKYEINFO set state = 1 where seckey = '%s'", tmpseckey2);
    ret = IC_DBApi_ExecNSelSql(dbhdl, mysql_update);
    if(ret != 0)
    {
        KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[4], ret, "func IC_DBApi_ExecNSelSql() err \n sql===>%s\n", mysql_update);
        return ret;
    }
    return ret;
}
