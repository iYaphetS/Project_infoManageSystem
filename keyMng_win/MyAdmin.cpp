
// MyAdmin.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MyAdmin.h"
#include "MainFrm.h"

#include "MyAdminDoc.h"
#include "MyAdminView.h"
#include "cfg_op.h"
#include "DlgInitCfg.h"

#include "secmng_globvar.h" //�����ҽ�����һ��

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyAdminApp

BEGIN_MESSAGE_MAP(CMyAdminApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMyAdminApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CMyAdminApp ����

CMyAdminApp::CMyAdminApp()
{
	// TODO:  ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("MyAdmin.AppID.NoVersion"));

	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CMyAdminApp ����

CMyAdminApp theApp;


// CMyAdminApp ��ʼ��
//CString  g_SecMngIniName = "secmngadmin.ini";

//������ api
//int  readCfg(char *filepath, char *mykey, char *myvalude);
//int	WriteCfg(char *filePath, char *mykey, char *myvalue);


int  CMyAdminApp::ReadSecMngCfg()
{
	//�����ļ�Ӧ���ڵ�ǰexe���ڵ�Ŀ¼֮��  //��ǰӦ�ó�������·��
	int			ret = 0;
	char strFileName[2048] = { 0 };
	CString  g_strINIPath = "";//
	//�뵱ǰӦ�ó������ڵ�·��
	GetModuleFileName(AfxGetInstanceHandle(), strFileName, sizeof(strFileName)); ////������г�������
	//E:\01_work\23_��Ŀ\MyAdmin\Debug\MyAdmin.exe ===>
	//E:\01_work\23_��Ŀ\MyAdmin\Debug\secmngadmin.ini ===>
	//AfxMessageBox(strFileName);

	g_strINIPath.Format("%s", strFileName);
	int i = g_strINIPath.ReverseFind('\\');
	g_strINIPath = g_strINIPath.Left(i);
	g_strINIPath = g_strINIPath + "\\" + g_SecMngIniName;

	char	tmpbuf[4096] = { 0 };
	int		tmpBufLen = 0;
	//�������� DSN
	ret = GetCfgItem((LPTSTR)(LPCTSTR)g_strINIPath /*in*/, "DSN" /*in*/, tmpbuf/*in out*/, &tmpBufLen /*out*/);
	if (ret != 0)
	{
		AfxMessageBox("�������� DSN ʧ��");
		return ret;
	}
	g_dbSource = tmpbuf;
//	CString	g_dbSource, g_dbUse, g_dbpw;

	//�������� UID
	memset(tmpbuf, 0, sizeof(tmpbuf));
	ret = GetCfgItem((LPTSTR)(LPCTSTR)g_strINIPath /*in*/, "UID" /*in*/, tmpbuf/*in out*/, &tmpBufLen /*out*/);
	if (ret != 0)
	{
		AfxMessageBox("�������� DSN ʧ��");
		return ret;
	}
	g_dbUse = tmpbuf;

	//�������� PWD
	memset(tmpbuf, 0, sizeof(tmpbuf));
	ret = GetCfgItem((LPTSTR)(LPCTSTR)g_strINIPath /*in*/, "PWD" /*in*/, tmpbuf/*in out*/, &tmpBufLen /*out*/);
	if (ret != 0)
	{
		AfxMessageBox("�������� DSN ʧ��");
		return ret;
	}
	g_dbpw = tmpbuf;

	return ret;
}

//CDatabase	myDB;
//CDatabase	*g_pDB = NULL;
int CMyAdminApp::ConnectByodbc()
{
	int				ret = 0;
	int			iTag = 0; //0 û���쳣    1���쳣
	CString		strCon;
	TRY
	{
		strCon.Format("DSN=%s;UID=%s;PWD=%s", g_dbSource, g_dbUse, g_dbpw);
		if (!myDB.OpenEx(strCon, CDatabase::noOdbcDialog))
		{
			ret = -1;
			AfxMessageBox("�������ݿ�ʧ�� ");
			return ret;
		}
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		iTag = 1;
	}
	END_CATCH_ALL

	/*   //bug 
	if (myDB.IsOpen())
	{
		myDB.Close();
	}  
	*/

	if (iTag == 0)
	{
		g_pDB = &myDB;
	}
	else
	{
		ret = 2;
	}
	
	return ret;
}

BOOL CMyAdminApp::InitInstance()
{
	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO:  Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)

	int			ret = 0;
	CDlgInitCfg dlgInitCfg;
	//�������ļ�
	ret = ReadSecMngCfg();
	if (ret != 0)
	{
		if (dlgInitCfg.DoModal() == IDCANCEL)
		{
			return FALSE;
		}
		else
		{
			//��ȫ�ֱ�����ֵ
			g_dbSource = dlgInitCfg.m_dbdsn;
			g_dbpw = dlgInitCfg.m_dbpw;
			g_dbUse = dlgInitCfg.m_dbuid;
		}
	}

	//�������ݿ� ����һ������
	ret = CMyAdminApp::ConnectByodbc();
	if (ret != 0)
	{
		AfxMessageBox("�������ݿ�ʧ��");
		return FALSE;
	}

	// ע��Ӧ�ó�����ĵ�ģ�塣  �ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMyAdminDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CMyAdminView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ��������������ָ�������  ���
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);  //SW_SHOW
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

// CMyAdminApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CMyAdminApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMyAdminApp ��Ϣ�������



