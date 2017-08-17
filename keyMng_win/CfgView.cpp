// CfgView.cpp : 实现文件
//

#include "stdafx.h"
#include "MyAdmin.h"
#include "CfgView.h"
#include "SECMNGSRVCFG.h"


// CCfgView

IMPLEMENT_DYNCREATE(CCfgView, CFormView)

CCfgView::CCfgView()
	: CFormView(CCfgView::IDD)
	, m_strsrvip(_T(""))
	, m_strsrvnodenum(_T(""))
	, m_strsrvport(_T(""))
{

}

CCfgView::~CCfgView()
{
}

void CCfgView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_IP, m_strsrvip);
	DDX_Text(pDX, IDC_EDIT_MAXNODE, m_strsrvnodenum);
	DDX_Text(pDX, IDC_EDIT_PORT, m_strsrvport);
}

BEGIN_MESSAGE_MAP(CCfgView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_SERVERCFGSAVE, &CCfgView::OnBnClickedButtonServercfgsave)
END_MESSAGE_MAP()


// CCfgView 诊断

#ifdef _DEBUG
void CCfgView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCfgView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCfgView 消息处理程序
extern CDatabase	*g_pDB ;

void CCfgView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	int  dbTag = 0; //0 正常
	CSECMNGSRVCFG    srvCfgSet(g_pDB);
	//select * from secmng.srvcvg where key = 'secmng_server_ip'
	TRY 
	{
		srvCfgSet.m_strFilter.Format("key  = '%s'", "secmng_server_ip");
		if ( !srvCfgSet.Open(CRecordset::snapshot, NULL, CRecordset::none) )
		{
			AfxMessageBox("打开表 srvcfg失败 ");
			return;
		}      
		
		srvCfgSet.m_KEY.TrimLeft();			srvCfgSet.m_KEY.TrimRight();
		srvCfgSet.m_VALUDE.TrimLeft();		srvCfgSet.m_VALUDE.TrimRight();

		m_strsrvip = srvCfgSet.m_VALUDE;

		//继续查询 //反复查询
		srvCfgSet.m_strFilter.Format("key  = '%s'", "secmng_server_port");
		srvCfgSet.Requery();
		if (!srvCfgSet.IsEOF())
		{
			srvCfgSet.m_KEY.TrimRight(); srvCfgSet.m_KEY.TrimLeft();
			srvCfgSet.m_VALUDE.TrimRight(); srvCfgSet.m_VALUDE.TrimLeft();
			m_strsrvport = srvCfgSet.m_VALUDE;
		}

		//反复查询 最大网点个数
		srvCfgSet.m_strFilter.Format("key  = '%s'", "secmng_server_maxnetnum");
		srvCfgSet.Requery();
		if (!srvCfgSet.IsEOF())
		{
			srvCfgSet.m_KEY.TrimRight(); srvCfgSet.m_KEY.TrimLeft();
			srvCfgSet.m_VALUDE.TrimRight(); srvCfgSet.m_VALUDE.TrimLeft();
			m_strsrvnodenum = srvCfgSet.m_VALUDE;
		}

	}
	CATCH_ALL (e)
	{
		e->ReportError();
		dbTag = 1;// 异常
		return;
	}
	END_CATCH_ALL

	UpdateData(FALSE);
	
	if (srvCfgSet.IsOpen())
	{
		srvCfgSet.Close();
	}

	// TODO:  在此添加专用代码和/或调用基类
}


void CCfgView::OnBnClickedButtonServercfgsave()
{
	// TODO:  在此添加控件通知处理程序代码
	int		dbtag = 0;
	UpdateData(TRUE); //界面的值传给变量

	if (m_strsrvip.IsEmpty())
	{
		MessageBox("IP地址为空", "服务器配置错误", MB_MODEMASK);
		return;
	}
	if (m_strsrvport.IsEmpty())
	{
		MessageBox("端口为空", "服务器配置错误", MB_MODEMASK);
		return;
	}

	if (m_strsrvnodenum.IsEmpty())
	{
		MessageBox("最大网点数为空", "服务器配置错误", MB_MODEMASK);
		return;
	}

	//
	g_pDB->BeginTrans();
	CSECMNGSRVCFG    srvCfgSet(g_pDB);
	TRY 
	{
		srvCfgSet.m_strFilter.Format("key  = '%s'", "secmng_server_ip");
		if (!srvCfgSet.Open(CRecordset::snapshot, NULL, CRecordset::none))
		{
			AfxMessageBox("打开表 srvcfg失败 ");
			g_pDB->Rollback();
			return;
		}

		if (!srvCfgSet.IsEOF())
		{
			srvCfgSet.Edit(); //有数据则修改
			srvCfgSet.m_VALUDE = m_strsrvip;
			srvCfgSet.Update();
		}
		else
		{
			srvCfgSet.AddNew(); //没有数据 新增
			srvCfgSet.m_KEY = "secmng_server_ip";
			srvCfgSet.m_VALUDE = m_strsrvip;
			srvCfgSet.Update();
		}

		// 修改port
		srvCfgSet.m_strFilter.Format("key  = '%s'", "secmng_server_port");
		srvCfgSet.Requery();
		if (!srvCfgSet.IsEOF())
		{
			srvCfgSet.Edit(); //有数据则修改
			srvCfgSet.m_VALUDE = m_strsrvport;
			srvCfgSet.Update();
		}
		else
		{
			srvCfgSet.AddNew(); //没有数据 新增
			srvCfgSet.m_KEY = "secmng_server_port";
			srvCfgSet.m_VALUDE = m_strsrvport;
			srvCfgSet.Update();
		}

		//// 修改 最大网点个数
		srvCfgSet.m_strFilter.Format("key  = '%s'", "secmng_server_maxnetnum");
		srvCfgSet.Requery();
		if (!srvCfgSet.IsEOF())
		{
			srvCfgSet.Edit(); //有数据则修改
			srvCfgSet.m_VALUDE = m_strsrvnodenum;
			srvCfgSet.Update();
		}
		else
		{
			srvCfgSet.AddNew(); //没有数据 新增
			srvCfgSet.m_KEY = "secmng_server_maxnetnum";
			srvCfgSet.m_VALUDE = m_strsrvnodenum;
			srvCfgSet.Update();
		}
	}
	CATCH_ALL (e)
	{
		e->ReportError();
		dbtag = 1; //有异常
	}
	END_CATCH_ALL

	if (dbtag == 0)
	{
		g_pDB->CommitTrans();
	}
	else
	{
		g_pDB->Rollback();
	}

	if (srvCfgSet.IsOpen())
	{
		srvCfgSet.Close();
	}
	
	AfxMessageBox("保存数据 ok");
}
