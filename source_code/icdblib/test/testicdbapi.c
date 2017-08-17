

#include <unistd.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>


#include "icdbapi.h"


/*
[2015.12.15 14:38:30] [DEBUG] [SUCCESS] Begin Sql_FindWhere [src/icdbsqlcheck.c] [87]
[2015.12.15 14:38:30] [DEBUG] [SUCCESS] Leave Sql_Check [src/icdbsqlcheck.c] [108]
[2015.12.15 14:38:30] [ERROR] [ERRNO is -1] IC_DBApi_ExecNSelSql EXECUTE IMMEDIATE Error [./src/icdbapi.c] [1669]
[2015.12.15 14:38:30] [ERROR] [ERRNO is -1] Sql is [insert into dept(deptno, dname, loc) values(91, '91name', '91loc')] [./src/icdbapi.c] [1670]
*/


// oerr ora 1
                         


//向数据库中插入数据
// sqlplus scott/aa@orcl 

// insert into dept(deptno, dname, loc) values(91, '91name', '91loc') ;


// 
int main02_1()
{
	int 		ret = 0;
	int 		bounds = 10;
	char		*dbName = "orcl";
	char		*dbUser = "scott";
	char		*dbPswd = "aa";
	
	char	 	mysqlbuf[2048] = {0};
	
	ICDBHandle 		handle = NULL;
	
	ret = IC_DBApi_PoolInit(bounds, dbName, dbUser, dbPswd);
	if (ret != 0)
	{
		printf("func IC_DBApi_PoolInit() err: %d \n", ret);
		return ret;
	}
	
	
	ret = IC_DBApi_ConnGet(&handle, 0, 0);
	if (ret != 0)
	{
		printf("func IC_DBApi_ConnGet() err: %d \n", ret);
		return ret;
	}
	
	
	ret = IC_DBApi_BeginTran(handle);
	if (ret != 0)
	{
		printf("func IC_DBApi_BeginTran() err: %d \n", ret);
		goto End;
	}
	
	
	strcpy(mysqlbuf, "insert into dept(deptno, dname, loc) values(92, '92name', '92loc')");
	ret = IC_DBApi_ExecNSelSql(handle, mysqlbuf);
	if (ret != 0)
	{
		printf("func IC_DBApi_ExecNSelSql() err: %d \n", ret);
		goto End;
	}
	
End:
	if (ret == 0) //若ok . 
	{
		IC_DBApi_Commit(handle);
	}
	else
	{
		IC_DBApi_Rollback(handle);
	}
		
	if (ret != 0)
	{
		if (ret == IC_DB_CONNECTION_ERR)
		{
			ret = IC_DBApi_ConnFree(handle, 0);//告诉底层库,进行锻炼修复
		}
		else
		{
			ret = IC_DBApi_ConnFree(handle, 1);
		}
	}
	else
	{
		IC_DBApi_ConnFree(handle, 1); //有效连接
	}

	IC_DBApi_PoolFree();

	printf("hello....\n");
	return 0;	
}




// 
int main()
{
	int 		ret = 0;
	int 		bounds = 10;
	char		*dbName = "orcl";
	char		*dbUser = "scott";
	char		*dbPswd = "aa";
	
	ICDBCursor 	cursor;
	
	char	 	mysqlbuf[2048] = {0};
	
	ICDBHandle 	handle = NULL;
	
	int 		deptno;
	char 		dname[24];
	char  		loc[24];

	//准备数据的内存空间
	ICDBRow   		row;
	ICDBField		field[4];
	field[0].cont = (char *)&deptno;
	field[0].contLen = 4;
	
	field[1].cont = (char *)dname;
	field[1].contLen = 24;
	
	field[2].cont = (char *)loc;
	field[2].contLen = 24;
	row.field = field; //建立关联
	row.fieldCount = 3;

	
	ret = IC_DBApi_PoolInit(bounds, dbName, dbUser, dbPswd);
	if (ret != 0)
	{
		printf("func IC_DBApi_PoolInit() err: %d \n", ret);
		return ret;
	}
	
	
	ret = IC_DBApi_ConnGet(&handle, 0, 0);
	if (ret != 0)
	{
		printf("func IC_DBApi_ConnGet() err: %d \n", ret);
		return ret;
	}
	
	strcpy(mysqlbuf, "select deptno, dname, loc from dept");
	
	ret= IC_DBApi_OpenCursor(handle, mysqlbuf, 3, &cursor);
	if (ret != 0)
	{
		printf("func IC_DBApi_OpenCursor() err: %d \n", ret);
		return ret;
	}	
	printf("\n");
	while (1)
	{
		deptno = 0;
		memset(dname, 0, sizeof(dname));
		memset(loc, 0, sizeof(loc));		
		ret = IC_DBApi_FetchByCursor(handle, cursor, &row);
		if (ret == 100)
		{
			break;
		}
		else if (ret == 0)
		{
			printf("%d\t%s\t%s\t \n", deptno, dname, loc);
		}
		else
		{
			printf("fun IC_DBApi_FetchByCursor() err:%d \n", ret);
			break;
		}
	}
	
	IC_DBApi_CloseCursor(handle, cursor);	
	printf("\n");


	if (ret != 0)
	{
		if (ret == IC_DB_CONNECTION_ERR)
		{
			ret = IC_DBApi_ConnFree(handle, 0);//告诉底层库,进行锻炼修复
		}
		else
		{
			ret = IC_DBApi_ConnFree(handle, 1);
		}
	}
	else
	{
		IC_DBApi_ConnFree(handle, 1); //有效连接
	}

	IC_DBApi_PoolFree();

	printf("hello....\n");
	return 0;	
}

