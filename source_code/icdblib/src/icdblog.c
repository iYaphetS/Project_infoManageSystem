#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


#include "icmemory.h"
#include "icdblog.h"


#ifdef HAVE_GCCVISIBILITYPATCH
	#define DLLEXPORT __attribute__ ((visibility("default")))
	#define DLLLOCAL __attribute__ ((visibility("hidden")))
#else
	#define DLLEXPORT
	#define DLLLOCAL
#endif


#define ICORADB_DEBUG_FILE_	"icdbapi.log"
#define ICORADB_MAX_STRING_LEN 		10240
//Level���
#define IC_NO_LOG_LEVEL			0
#define IC_DEBUG_LEVEL			1
#define IC_INFO_LEVEL			2
#define IC_WARNING_LEVEL		3
#define IC_ERROR_LEVEL			4


//int	pf = -1;
//Level������
char ICORADBLevelName[5][10] = {"NOLOG", "DEBUG", "INFO", "WARNING", "ERROR"};

//ʵ��ʹ�õ�Level
DLLEXPORT int ICORADBLevel[5] = {IC_NO_LOG_LEVEL, IC_DEBUG_LEVEL, IC_INFO_LEVEL, IC_WARNING_LEVEL, IC_ERROR_LEVEL};

static int ICORADB_Error_GetCurTime(char* strTime)
{
	struct tm*		tmTime = NULL;
	size_t			timeLen = 0;
	time_t			tTime = 0;	
	
	tTime = time(NULL);
	tmTime = localtime(&tTime);
	//timeLen = strftime(strTime, 33, "%Y(Y)%m(M)%d(D)%H(H)%M(M)%S(S)", tmTime);
	timeLen = strftime(strTime, 33, "%Y.%m.%d %H:%M:%S", tmTime);
	
	return timeLen;
}

//wangbaomng 20080711 modify 
static int ICORADB_Error_OpenFile(int* pf)
{
	int 		iLen = 0;
	char		fileName[512];
	char		tmpfileName[512];
	char		suffix[64];
	
	struct tm 	*aclock;
	time_t		ctime;

	memset(fileName, 0, sizeof(fileName));
	memset(tmpfileName, 0, sizeof(tmpfileName));
	memset(suffix, 0, sizeof(suffix));
	
	time(&ctime);
	aclock = localtime(&ctime);
	
	sprintf(suffix, "_%04d%02d%02d.log",1900+aclock->tm_year, 1+aclock->tm_mon, aclock->tm_mday);
	
		
#ifdef	ICORADB_DEBUG_FILE_
	iLen = strlen(ICORADB_DEBUG_FILE_);
	if (iLen>4)
	{
		strcpy(tmpfileName, ICORADB_DEBUG_FILE_);
		tmpfileName[iLen-4] = '\0';
		strcat(tmpfileName, suffix);
	}
	else
	{
		strcat(tmpfileName, suffix);
	}
	sprintf(fileName, "%s/log/%s", getenv("HOME"), tmpfileName);
#else
	sprintf(fileName, "%s/log/%s%s", getenv("HOME"), "DebugMessage", suffix);
#endif
    
    *pf = open(fileName, O_WRONLY|O_CREAT|O_APPEND, 0666);
    if(*pf < 0)
    {
        return-1;
    }
	
	return 0;
}


/*static void IC_Log_Error_WriteFile(const char *str)
{
	int rv = -1;
	
	//�ж��ļ��Ƿ��Ѿ���
	if(pf < 0)
	{
		if(IC_Log_Error_OpenFile())
		{
			return ;
		}
	}
	
	//��LOG����д���ļ���
	rv = write(pf, str, strlen(str));
	if(rv <= 0)
	{
		if(IC_Log_Error_OpenFile())
		{
			return ;
		}
		write(pf, str, strlen(str));
	}
	
	return ;
}*/


static void ICORADB_Error_Core(const char *file, int line, int level, int status, const char *fmt, va_list args)
{
    char str[ICORADB_MAX_STRING_LEN ];
    int	 strLen = 0;
    char tmpStr[64];
    int	 tmpStrLen = 0;
    int  pf = 0;
    
    //��ʼ��
    memset(str, 0, ICORADB_MAX_STRING_LEN );
    memset(tmpStr, 0, 64);
    
    //����LOGʱ��
    tmpStrLen = ICORADB_Error_GetCurTime(tmpStr);
    tmpStrLen = sprintf(str, "[%s] ", tmpStr);
    strLen = tmpStrLen;

    //����LOG�ȼ�
    tmpStrLen = sprintf(str+strLen, "[%s] ", ICORADBLevelName[level]);
    strLen += tmpStrLen;
    
    //����LOG״̬
    if (status != 0) 
    {
        tmpStrLen = sprintf(str+strLen, "[ERRNO is %d] ", status);
    }
    else
    {
    	tmpStrLen = sprintf(str+strLen, "[SUCCESS] ");
    }
    strLen += tmpStrLen;

    //����LOG��Ϣ
    tmpStrLen = vsprintf(str+strLen, fmt, args);
    strLen += tmpStrLen;

    //����LOG�����ļ�
    tmpStrLen = sprintf(str+strLen, " [%s]", file);
    strLen += tmpStrLen;

    //����LOG��������
    tmpStrLen = sprintf(str+strLen, " [%d]\n", line);
    strLen += tmpStrLen;
    
    //��LOG�ļ�
    if(ICORADB_Error_OpenFile(&pf))
	{
		return ;
	}
	
    //д��LOG�ļ�
    write(pf, str, strLen);
    //IC_Log_Error_WriteFile(str);
    
    //�ر��ļ�
    close(pf);
    
    return ;
}


void IC_DBLOG(const char *file, int line, int level, int status, const char *fmt, ...)
{
    va_list args;
	
	//�ж��Ƿ���ҪдLOG
//	if(level!=IC_DEBUG_LEVEL && level!=IC_INFO_LEVEL && level!=IC_WARNING_LEVEL && level!=IC_WARNING_LEVEL)
	if(level == IC_NO_LOG_LEVEL)
	{
		return ;
	}
	
	//���ú��ĵ�дLOG����
    va_start(args, fmt);
    ICORADB_Error_Core(file, line, level, status, fmt, args);
    va_end(args);
    
    return ;
}
