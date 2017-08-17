// CfgView.cpp : ʵ���ļ�
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


// CCfgView ���

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


// CCfgView ��Ϣ�������
extern CDatabase	*g_pDB ;

void CCfgView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	int  dbTag = 0; //0 ����
	CSECMNGSRVCFG    srvCfgSet(g_pDB);
	//select * from secmng.srvcvg where key = 'secmng_server_ip'
	TRY 
	{
		srvCfgSet.m_strFilter.Format("key  = '%s'", "secmng_server_ip");
		if ( !srvCfgSet.Open(CRecordset::snapshot, NULL, CRecordset::none) )
		{
			AfxMessageBox("�򿪱� srvcfgʧ�� ");
			return;
		}      
		
		srvCfgSet.m_KEY.TrimLeft();			srvCfgSet.m_KEY.TrimRight();
		srvCfgSet.m_VALUDE.TrimLeft();		srvCfgSet.m_VALUDE.TrimRight();

		m_strsrvip = srvCfgSet.m_VALUDE;

		//������ѯ //������ѯ
		srvCfgSet.m_strFilter.Format("key  = '%s'", "secmng_server_port");
		srvCfgSet.Requery();
		if (!srvCfgSet.IsEOF())
		{
			srvCfgSet.m_KEY.TrimRight(); srvCfgSet.m_KEY.TrimLeft();
			srvCfgSet.m_VALUDE.TrimRight(); srvCfgSet.m_VALUDE.TrimLeft();
			m_strsrvport = srvCfgSet.m_VALUDE;
		}

		//������ѯ ����������
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
		dbTag = 1;// �쳣
		return;
	}
	END_CATCH_ALL

	UpdateData(FALSE);
	
	if (srvCfgSet.IsOpen())
	{
		srvCfgSet.Close();
	}

	// TODO:  �ڴ����ר�ô����/����û���
}


void CCfgView::OnBnClickedButtonServercfgsave()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int		dbtag = 0;
	UpdateData(TRUE); //�����ֵ��������

	if (m_strsrvip.IsEmpty())
	{
		MessageBox("IP��ַΪ��", "���������ô���", MB_MODEMASK);
		return;
	}
	if (m_strsrvport.IsEmpty())
	{
		MessageBox("�˿�Ϊ��", "���������ô���", MB_MODEMASK);
		return;
	}

	if (m_strsrvnodenum.IsEmpty())
	{
		MessageBox("���������Ϊ��", "���������ô���", MB_MODEMASK);
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
			AfxMessageBox("�򿪱� srvcfgʧ�� ");
			g_pDB->Rollback();
			return;
		}

		if (!srvCfgSet.IsEOF())
		{
			srvCfgSet.Edit(); //���������޸�
			srvCfgSet.m_VALUDE = m_strsrvip;
			srvCfgSet.Update();
		}
		else
		{
			srvCfgSet.AddNew(); //û������ ����
			srvCfgSet.m_KEY = "secmng_server_ip";
			srvCfgSet.m_VALUDE = m_strsrvip;
			srvCfgSet.Update();
		}

		// �޸�port
		srvCfgSet.m_strFilter.Format("key  = '%s'", "secmng_server_port");
		srvCfgSet.Requery();
		if (!srvCfgSet.IsEOF())
		{
			srvCfgSet.Edit(); //���������޸�
			srvCfgSet.m_VALUDE = m_strsrvport;
			srvCfgSet.Update();
		}
		else
		{
			srvCfgSet.AddNew(); //û������ ����
			srvCfgSet.m_KEY = "secmng_server_port";
			srvCfgSet.m_VALUDE = m_strsrvport;
			srvCfgSet.Update();
		}

		//// �޸� ����������
		srvCfgSet.m_strFilter.Format("key  = '%s'", "secmng_server_maxnetnum");
		srvCfgSet.Requery();
		if (!srvCfgSet.IsEOF())
		{
			srvCfgSet.Edit(); //���������޸�
			srvCfgSet.m_VALUDE = m_strsrvnodenum;
			srvCfgSet.Update();
		}
		else
		{
			srvCfgSet.AddNew(); //û������ ����
			srvCfgSet.m_KEY = "secmng_server_maxnetnum";
			srvCfgSet.m_VALUDE = m_strsrvnodenum;
			srvCfgSet.Update();
		}
	}
	CATCH_ALL (e)
	{
		e->ReportError();
		dbtag = 1; //���쳣
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
	
	AfxMessageBox("�������� ok");
}
