#pragma once


// CDlgInitCfg �Ի���

class CDlgInitCfg : public CDialog
{
	DECLARE_DYNAMIC(CDlgInitCfg)

public:
	CDlgInitCfg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgInitCfg();

// �Ի�������
	enum { IDD = IDD_DIALOG_IINTDB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_dbdsn;
	CString m_dbpw;
	CString m_dbuid;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonDbtest();
};