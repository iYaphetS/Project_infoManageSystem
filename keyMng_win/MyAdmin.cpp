
// MyAdmin.cpp : 定义应用程序的类行为。
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

#include "secmng_globvar.h" //包含且仅包含一次

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyAdminApp

BEGIN_MESSAGE_MAP(CMyAdminApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMyAdminApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CMyAdminApp 构造

CMyAdminApp::CMyAdminApp()
{
	// TODO:  将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("MyAdmin.AppID.NoVersion"));

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CMyAdminApp 对象

CMyAdminApp theApp;


// CMyAdminApp 初始化
//CString  g_SecMngIniName = "secmngadmin.ini";

//读配置 api
//int  readCfg(char *filepath, char *mykey, char *myvalude);
//int	WriteCfg(char *filePath, char *mykey, char *myvalue);


int  CMyAdminApp::ReadSecMngCfg()
{
	//配置文件应放在当前exe所在的目录之下  //求当前应用程序所在路径
	int			ret = 0;
	char strFileName[2048] = { 0 };
	CString  g_strINIPath = "";//
	//请当前应用程序所在的路径
	GetModuleFileName(AfxGetInstanceHandle(), strFileName, sizeof(strFileName)); ////获得运行程序名字
	//E:\01_work\23_项目\MyAdmin\Debug\MyAdmin.exe ===>
	//E:\01_work\23_项目\MyAdmin\Debug\secmngadmin.ini ===>
	//AfxMessageBox(strFileName);

	g_strINIPath.Format("%s", strFileName);
	int i = g_strINIPath.ReverseFind('\\');
	g_strINIPath = g_strINIPath.Left(i);
	g_strINIPath = g_strINIPath + "\\" + g_SecMngIniName;

	char	tmpbuf[4096] = { 0 };
	int		tmpBufLen = 0;
	//读配置项 DSN
	ret = GetCfgItem((LPTSTR)(LPCTSTR)g_strINIPath /*in*/, "DSN" /*in*/, tmpbuf/*in out*/, &tmpBufLen /*out*/);
	if (ret != 0)
	{
		AfxMessageBox("读配置项 DSN 失败");
		return ret;
	}
	g_dbSource = tmpbuf;
//	CString	g_dbSource, g_dbUse, g_dbpw;

	//读配置项 UID
	memset(tmpbuf, 0, sizeof(tmpbuf));
	ret = GetCfgItem((LPTSTR)(LPCTSTR)g_strINIPath /*in*/, "UID" /*in*/, tmpbuf/*in out*/, &tmpBufLen /*out*/);
	if (ret != 0)
	{
		AfxMessageBox("读配置项 DSN 失败");
		return ret;
	}
	g_dbUse = tmpbuf;

	//读配置项 PWD
	memset(tmpbuf, 0, sizeof(tmpbuf));
	ret = GetCfgItem((LPTSTR)(LPCTSTR)g_strINIPath /*in*/, "PWD" /*in*/, tmpbuf/*in out*/, &tmpBufLen /*out*/);
	if (ret != 0)
	{
		AfxMessageBox("读配置项 DSN 失败");
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
	int			iTag = 0; //0 没有异常    1有异常
	CString		strCon;
	TRY
	{
		strCon.Format("DSN=%s;UID=%s;PWD=%s", g_dbSource, g_dbUse, g_dbpw);
		if (!myDB.OpenEx(strCon, CDatabase::noOdbcDialog))
		{
			ret = -1;
			AfxMessageBox("连接数据库失败 ");
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

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)

	int			ret = 0;
	CDlgInitCfg dlgInitCfg;
	//读配置文件
	ret = ReadSecMngCfg();
	if (ret != 0)
	{
		if (dlgInitCfg.DoModal() == IDCANCEL)
		{
			return FALSE;
		}
		else
		{
			//给全局变量赋值
			g_dbSource = dlgInitCfg.m_dbdsn;
			g_dbpw = dlgInitCfg.m_dbpw;
			g_dbUse = dlgInitCfg.m_dbuid;
		}
	}

	//连接数据库 建立一条连接
	ret = CMyAdminApp::ConnectByodbc();
	if (ret != 0)
	{
		AfxMessageBox("连接数据库失败");
		return FALSE;
	}

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMyAdminDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CMyAdminView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);  //SW_SHOW
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

// CMyAdminApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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

// 用于运行对话框的应用程序命令
void CMyAdminApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMyAdminApp 消息处理程序



