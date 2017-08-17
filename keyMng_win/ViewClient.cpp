// ViewClient.cpp : ʵ���ļ�

#include "stdafx.h"
#include "MyAdmin.h"
#include "ViewClient.h"
#include "keymngclientop.h"
#include "keymng_shmop.h"

// CViewClient

IMPLEMENT_DYNCREATE(CViewClient, CFormView)

CViewClient::CViewClient()
	: CFormView(CViewClient::IDD)
{

}

CViewClient::~CViewClient()
{
}

void CViewClient::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SECNODE, m_listSecNode);
}

BEGIN_MESSAGE_MAP(CViewClient, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_AGREE, &CViewClient::OnBnClickedButtonAgree)
	ON_BN_CLICKED(IDC_BUTTON_INIT, &CViewClient::OnBnClickedButtonInit)
	ON_BN_CLICKED(IDC_BUTTON_CHECK, &CViewClient::OnBnClickedButtonCheck)
END_MESSAGE_MAP()

// CViewClient ���

#ifdef _DEBUG
void CViewClient::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewClient::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewClient ��Ϣ�������

int CViewClient::DbInitListSecNode(CString &clientid, CString &serverid, int keyid, int state, CTime &time)
{

	// TODO:  �ڴ���ӿؼ�֪ͨ��������
	LVITEM   lvi;
	lvi.mask = LVIF_IMAGE | LVIF_TEXT;
	lvi.iItem = 0;	//�ڵڼ����ϲ������� ʼ��ͷ�巨
	lvi.iImage = 4;

	//�����0������
	lvi.iSubItem = 0;		// Set subitem 0
	lvi.pszText = (LPTSTR)(LPCTSTR)clientid;
	m_listSecNode.InsertItem(&lvi);

	////�����1������
	lvi.iSubItem = 1;		// Set subitem 1
	lvi.pszText = (LPTSTR)(LPCTSTR)serverid;
	m_listSecNode.SetItem(&lvi);

	char buf[128] = { 0 };
	sprintf(buf, "%d", keyid);
	////�����2������
	lvi.iSubItem = 2;		// Set subitem 1
	lvi.pszText = (LPTSTR)(LPCTSTR)buf;
	m_listSecNode.SetItem(&lvi);

	//�����3������
	lvi.iSubItem = 3;		// Set subitem 3
	if (state == 1)
	{
		lvi.pszText = "����";

	}
	else {
		lvi.pszText = "����";
	}
	m_listSecNode.SetItem(&lvi);


	//�����4������
	CString strTime = time.Format("%Y-%m-%d %H:%M:%S");
	lvi.iSubItem = 4;		// Set subitem 4
	//CString strAuthcode(authcode) ;
	lvi.pszText = (LPTSTR)(LPCTSTR)strTime;
	m_listSecNode.SetItem(&lvi);

	return 0;
}

void CViewClient::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	HIMAGELIST hList = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, 8, 1);
	m_imageList.Attach(hList);

	CBitmap cBmp;
	cBmp.LoadBitmap(IDB_BITMAP_SECNODE);
	m_imageList.Add(&cBmp, RGB(255, 0, 255));
	cBmp.DeleteObject();

	m_listSecNode.SetImageList(&m_imageList, LVSIL_SMALL);

	DWORD dwExStyle = ListView_GetExtendedListViewStyle(m_listSecNode.m_hWnd);
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_listSecNode.m_hWnd, dwExStyle);

	CRect rect; 
	m_listSecNode.GetClientRect(&rect);
	int nColInterval = rect.Width() / 5;

	m_listSecNode.SetRedraw(FALSE);
	m_listSecNode.InsertColumn(0, "Client���", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(1, "Server���", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(2, "��ǰ��ԿKeyId", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(3, "��Կ״̬", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(4, "����ʱ��", LVCFMT_LEFT, rect.Width() - 4 * nColInterval);
	m_listSecNode.SetRedraw(TRUE);
/*
	CString clientid = "1111";
	CString serverid = "0001";
	int keyid = 105;
	int state = 0;
	CTime time = COleDateTime::GetCurrentTime();

	DbInitListSecNode(clientid, serverid, keyid, state, time);
	*/
}

MngClient_Info pCltInfo;

void CViewClient::OnBnClickedButtonAgree()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int ret = 0;
	NodeSHMInfo nodeSHMInfo;

	ret = MngClient_Agree(&pCltInfo, &nodeSHMInfo); // in
	if (ret != 0) {
		AfxMessageBox("Э��ʧ��");
	}
	CTime time = CTime::GetCurrentTime();
	//CTime time = COleDateTime::GetCurrentTime();
	DbInitListSecNode((CString)nodeSHMInfo.clientId,(CString)nodeSHMInfo.serverId, nodeSHMInfo.seckeyid, nodeSHMInfo.status, time);

	AfxMessageBox("�ͻ�����ԿЭ�����");
}

void CViewClient::OnBnClickedButtonInit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int ret = 0;

	ret = MngClient_InitInfo(&pCltInfo);
	if (ret != 0) {
		AfxMessageBox("�ͻ��˳�ʼ��ʧ��");
		return;
	}
	AfxMessageBox("�ͻ��˳�ʼ���ɹ�");
}


void CViewClient::OnBnClickedButtonCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int ret = 0;
	ret = MngClient_Check(&pCltInfo); // in
	if (ret != 0) {
		AfxMessageBox("�ͻ�����ԿУ��ʧ��");
		return;
	}
	AfxMessageBox("�ͻ�����ԿУ�����");
}
