#pragma once
#include "afxcmn.h"



// CViewClient 窗体视图

class CViewClient : public CFormView
{
	DECLARE_DYNCREATE(CViewClient)

protected:
	CViewClient();           // 动态创建所使用的受保护的构造函数
	virtual ~CViewClient();

public:
	enum { IDD = IDD_DIALOG_CLIENT };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	CListCtrl m_listSecNode;

public:

	CImageList		m_imageList;
	int CViewClient::DbInitListSecNode(CString &clientid, CString &serverid, int keyid, int state, CTime &time);
	afx_msg void OnBnClickedButtonAgree();
	afx_msg void OnBnClickedButtonInit();
	afx_msg void OnBnClickedButtonCheck();
};


