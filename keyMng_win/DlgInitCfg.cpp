// DlgInitCfg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyAdmin.h"
#include "DlgInitCfg.h"
#include "afxdialogex.h"
#include "cfg_op.h"


// CDlgInitCfg 对话框

IMPLEMENT_DYNAMIC(CDlgInitCfg, CDialog)

CDlgInitCfg::CDlgInitCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInitCfg::IDD, pParent)
	, m_dbdsn(_T(""))
	, m_dbpw(_T(""))
	, m_dbuid(_T(""))
{

}

CDlgInitCfg::~CDlgInitCfg()
{
}

void CDlgInitCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DSN, m_dbdsn);
	DDX_Text(pDX, IDC_EDIT_PASSWD, m_dbpw);
	DDX_Text(pDX, IDC_EDIT_USER, m_dbuid);
}


BEGIN_MESSAGE_MAP(CDlgInitCfg, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgInitCfg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgInitCfg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_DBTEST, &CDlgInitCfg::OnBnClickedButtonDbtest)
END_MESSAGE_MAP()


// CDlgInitCfg 消息处理程序

extern CString		g_SecMngIniName;

void CDlgInitCfg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE); //把控件上的值(界面上的值) 传递给 C++变量
	if (m_dbdsn.IsEmpty() || m_dbpw.IsEmpty() || m_dbuid.IsEmpty())
	{
		AfxMessageBox("变量值输入不能为空");
	}

	//写配置文件
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
	ret = WriteCfgItem((LPTSTR)(LPCTSTR)g_strINIPath, "DSN", (LPTSTR)(LPCTSTR)m_dbdsn, m_dbdsn.GetLength() );
	if (ret != 0)
	{
		AfxMessageBox("写配置项 DSN 失败");
		return ;
	}
	// 写配置项 UID
	ret = WriteCfgItem((LPTSTR)(LPCTSTR)g_strINIPath, "UID", (LPTSTR)(LPCTSTR)m_dbuid, m_dbuid.GetLength()  );
	if (ret != 0)
	{
		AfxMessageBox("写配置项 UID 失败");
		return ;
	}

	//写配置项 PWD
	ret = WriteCfgItem((LPTSTR)(LPCTSTR)g_strINIPath, "PWD", (LPTSTR)(LPCTSTR)m_dbpw, m_dbpw.GetLength() );
	if (ret != 0)
	{
		AfxMessageBox("写配置项 PWD 失败");
		return ;
	}

	CDialog::OnOK();
}


void CDlgInitCfg::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialog::OnCancel();
}

extern CString g_dbSource, g_dbUse, g_dbpw;

void CDlgInitCfg::OnBnClickedButtonDbtest()
{
	// TODO:  在此添加控件通知处理程序代码
	int   iTag = 0;
	UpdateData(TRUE); //把控件上的值(界面上的值) 传递给 C++变量
	if (m_dbdsn.IsEmpty() || m_dbpw.IsEmpty() || m_dbuid.IsEmpty())
	{
		AfxMessageBox("变量值输入不能为空");
		return;
	}

	CDatabase db;
	CString		strCon;
	TRY 
	{
		strCon.Format("DSN=%s;UID=%s;PWD=%s", m_dbdsn, m_dbuid, m_dbpw);
		if (!db.OpenEx(strCon, CDatabase::noOdbcDialog))
		{
			AfxMessageBox("连接数据库失败 ");
			return ;
		}
	}
	CATCH_ALL (e)
	{
		e->ReportError();
		iTag = 1;
	}
	END_CATCH_ALL
	

	if (db.IsOpen())
	{
		db.Close();
	}

	if (iTag == 0)
	{
		AfxMessageBox("测试连接 ok ");
	}
}
