// DlgInitCfg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyAdmin.h"
#include "DlgInitCfg.h"
#include "afxdialogex.h"
#include "cfg_op.h"


// CDlgInitCfg �Ի���

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


// CDlgInitCfg ��Ϣ�������

extern CString		g_SecMngIniName;

void CDlgInitCfg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE); //�ѿؼ��ϵ�ֵ(�����ϵ�ֵ) ���ݸ� C++����
	if (m_dbdsn.IsEmpty() || m_dbpw.IsEmpty() || m_dbuid.IsEmpty())
	{
		AfxMessageBox("����ֵ���벻��Ϊ��");
	}

	//д�����ļ�
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
	ret = WriteCfgItem((LPTSTR)(LPCTSTR)g_strINIPath, "DSN", (LPTSTR)(LPCTSTR)m_dbdsn, m_dbdsn.GetLength() );
	if (ret != 0)
	{
		AfxMessageBox("д������ DSN ʧ��");
		return ;
	}
	// д������ UID
	ret = WriteCfgItem((LPTSTR)(LPCTSTR)g_strINIPath, "UID", (LPTSTR)(LPCTSTR)m_dbuid, m_dbuid.GetLength()  );
	if (ret != 0)
	{
		AfxMessageBox("д������ UID ʧ��");
		return ;
	}

	//д������ PWD
	ret = WriteCfgItem((LPTSTR)(LPCTSTR)g_strINIPath, "PWD", (LPTSTR)(LPCTSTR)m_dbpw, m_dbpw.GetLength() );
	if (ret != 0)
	{
		AfxMessageBox("д������ PWD ʧ��");
		return ;
	}

	CDialog::OnOK();
}


void CDlgInitCfg::OnBnClickedCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnCancel();
}

extern CString g_dbSource, g_dbUse, g_dbpw;

void CDlgInitCfg::OnBnClickedButtonDbtest()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int   iTag = 0;
	UpdateData(TRUE); //�ѿؼ��ϵ�ֵ(�����ϵ�ֵ) ���ݸ� C++����
	if (m_dbdsn.IsEmpty() || m_dbpw.IsEmpty() || m_dbuid.IsEmpty())
	{
		AfxMessageBox("����ֵ���벻��Ϊ��");
		return;
	}

	CDatabase db;
	CString		strCon;
	TRY 
	{
		strCon.Format("DSN=%s;UID=%s;PWD=%s", m_dbdsn, m_dbuid, m_dbpw);
		if (!db.OpenEx(strCon, CDatabase::noOdbcDialog))
		{
			AfxMessageBox("�������ݿ�ʧ�� ");
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
		AfxMessageBox("�������� ok ");
	}
}
