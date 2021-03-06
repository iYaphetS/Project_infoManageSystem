#pragma once


// CDlgInitCfg 对话框

class CDlgInitCfg : public CDialog
{
	DECLARE_DYNAMIC(CDlgInitCfg)

public:
	CDlgInitCfg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgInitCfg();

// 对话框数据
	enum { IDD = IDD_DIALOG_IINTDB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_dbdsn;
	CString m_dbpw;
	CString m_dbuid;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonDbtest();
};
