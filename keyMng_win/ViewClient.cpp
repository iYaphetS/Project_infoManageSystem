// ViewClient.cpp : 实现文件

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

// CViewClient 诊断

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


// CViewClient 消息处理程序

int CViewClient::DbInitListSecNode(CString &clientid, CString &serverid, int keyid, int state, CTime &time)
{

	// TODO:  在此添加控件通知处理程序代
	LVITEM   lvi;
	lvi.mask = LVIF_IMAGE | LVIF_TEXT;
	lvi.iItem = 0;	//在第几行上插入数据 始终头插法
	lvi.iImage = 4;

	//插入第0列数据
	lvi.iSubItem = 0;		// Set subitem 0
	lvi.pszText = (LPTSTR)(LPCTSTR)clientid;
	m_listSecNode.InsertItem(&lvi);

	////插入第1列数据
	lvi.iSubItem = 1;		// Set subitem 1
	lvi.pszText = (LPTSTR)(LPCTSTR)serverid;
	m_listSecNode.SetItem(&lvi);

	char buf[128] = { 0 };
	sprintf(buf, "%d", keyid);
	////插入第2列数据
	lvi.iSubItem = 2;		// Set subitem 1
	lvi.pszText = (LPTSTR)(LPCTSTR)buf;
	m_listSecNode.SetItem(&lvi);

	//插入第3列数据
	lvi.iSubItem = 3;		// Set subitem 3
	if (state == 1)
	{
		lvi.pszText = "禁用";

	}
	else {
		lvi.pszText = "正常";
	}
	m_listSecNode.SetItem(&lvi);


	//插入第4列数据
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
	m_listSecNode.InsertColumn(0, "Client编号", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(1, "Server编号", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(2, "当前密钥KeyId", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(3, "密钥状态", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(4, "请求时间", LVCFMT_LEFT, rect.Width() - 4 * nColInterval);
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
	// TODO: 在此添加控件通知处理程序代码
	int ret = 0;
	NodeSHMInfo nodeSHMInfo;

	ret = MngClient_Agree(&pCltInfo, &nodeSHMInfo); // in
	if (ret != 0) {
		AfxMessageBox("协商失败");
	}
	CTime time = CTime::GetCurrentTime();
	//CTime time = COleDateTime::GetCurrentTime();
	DbInitListSecNode((CString)nodeSHMInfo.clientId,(CString)nodeSHMInfo.serverId, nodeSHMInfo.seckeyid, nodeSHMInfo.status, time);

	AfxMessageBox("客户端密钥协商完成");
}

void CViewClient::OnBnClickedButtonInit()
{
	// TODO: 在此添加控件通知处理程序代码
	int ret = 0;

	ret = MngClient_InitInfo(&pCltInfo);
	if (ret != 0) {
		AfxMessageBox("客户端初始化失败");
		return;
	}
	AfxMessageBox("客户端初始化成功");
}


void CViewClient::OnBnClickedButtonCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	int ret = 0;
	ret = MngClient_Check(&pCltInfo); // in
	if (ret != 0) {
		AfxMessageBox("客户端密钥校验失败");
		return;
	}
	AfxMessageBox("客户端密钥校验完成");
}
