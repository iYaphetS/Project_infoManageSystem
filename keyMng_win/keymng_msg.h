#ifndef _KEYMNG_MSG_H_
#define _KEYMNG_MSG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define		KeyMng_ParamErr			200		//�������ʧ��
#define		KeyMng_TypeErr			201		//��������ʧ��
#define		KeyMng_MallocErr		202		//�����ڴ�ʧ��

#define		KeyMng_NEWorUPDATE		1		//1 ��Կ���� 
#define		KeyMng_Check			2		//2 ��ԿУ��
#define		KeyMng_Revoke			3		//3 ��Կע��
		 	; 	
#define  ID_MsgKey_Req  60

//��Կ������
typedef struct _MsgKey_Req
{
	//1 ��Կ����  	//2 ��ԿУ��; 	//3 ��Կע��
	int				cmdType;		//���������� 
	char			clientId[12];	//�ͻ��˱��
	char			AuthCode[16];	//��֤��
	char			serverId[12];	//��������I���
	char			r1[64];		//�ͻ��������
	
}MsgKey_Req;


//��ԿӦ����
#define  ID_MsgKey_Res  61
typedef struct  _MsgKey_Res
{
	int					rv;				//����ֵ
	char				clientId[12];	//�ͻ��˱��
	char				serverId[12];	//���������
	unsigned char		r2[64];			//�������������
	int					seckeyid;		//�Գ���Կ��� //modfy 2015.07.20
}MsgKey_Res;


/*
 pstruct :	����ı������� ; (ָ����Ӧ�ṹ���ָ��) 
 type :		��������ͱ�ʶ(�����ڲ�ͨ��type �õ� pstruct ��ָ��ı�������)
 poutData:	����ı����ı��� ; 
 outlen :	��������ݳ���;
*/

int MsgEncode(
	void			*pStruct , /*in*/
	int				type,
	unsigned char	**outData, /*out*/
	int				*outLen );

/*
 inData		: ����ı���������;
 inLen		: ��������ݳ��� ;
 pstruct	: ����Ľ���������; (��ռ������ڲ����ٵģ�Ҳ��Ҫ���ڲ������free���������ͷ�)
 type		: �ṹ�����ͱ�ʶ(�������ͱ�ʶ��ʹ�õ�����ͨ��flag�����жϣ���pstruct ת��Ϊ��Ӧ�Ľṹ)
*/
int MsgDecode( 
	unsigned char *inData,/*in*/
	int           inLen,
	void          **pStruct /*out*/,
	int           *type /*out*/);


/*
�ͷ� MsgEncode( )�����е�outData; ������MsgMemFree((void **)outData, 0); 
�ͷ�MsgDecode( )�����е�pstruct�ṹ�壬MsgMemFree((void **)outData, type);
type : �������,���ں����жϵ����ĸ��ṹ���free����
*/ 

int MsgMemFree(void **point,int type);

#ifdef __cplusplus
}
#endif

#endif