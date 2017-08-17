// DlgNetInfo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyAdmin.h"
#include "DlgNetInfo.h"
#include "SECMNGSECNODE.h"


// CDlgNetInfo

IMPLEMENT_DYNCREATE(CDlgNetInfo, CFormView)

CDlgNetInfo::CDlgNetInfo()
	: CFormView(CDlgNetInfo::IDD)
	, m_dateBegin(COleDateTime::GetCurrentTime())
	, m_dateEnd(COleDateTime::GetCurrentTime())
{

}

CDlgNetInfo::~CDlgNetInfo()
{
}

void CDlgNetInfo::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SECNODE, m_listSecNode);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_dateBegin);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_dateEnd);
}

BEGIN_MESSAGE_MAP(CDlgNetInfo, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgNetInfo::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CDlgNetInfo::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgNetInfo::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlgNetInfo::OnBnClickedButton4)
END_MESSAGE_MAP()


///CDlgNetInfo ���

#ifdef _DEBUG
void CDlgNetInfo::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDlgNetInfo::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDlgNetInfo ��Ϣ�������

//int InsertItem(_In_ const LVITEM* pItem);

/* 
typedef struct tagLVITEMW
{
	UINT mask;			//��ʾ��ʽ ���֡�ͼƬ��
	int iItem;			//����λ�ã��ڵڼ��в���
	int iSubItem;		//���������
	UINT state;			//״̬
	UINT stateMask;
	LPWSTR pszText;		//��ʾ���������� 
	int cchTextMax;
	int iImage;			//ʹ�õ�ͼƬ����� 
	LPARAM lParam;		//������ÿһ���� ����һЩ����
	int iIndent;
#if (NTDDI_VERSION >= NTDDI_WINXP)
	int iGroupId;
	UINT cColumns;		// tile view columns
	PUINT puColumns;
#endif
#if (NTDDI_VERSION >= NTDDI_VISTA)
	int* piColFmt;
	int iGroup; // readonly. only valid for owner data.
#endif
} LVITEMW, *LPLVITEMW;
*/

// ����һ�����ݵ� listContrl�ؼ���
int CDlgNetInfo::DbInitListSecNode(CString &ID, CString &Name, 
								   CTime &time, int state, int authcode)
{
	LVITEM   lvi;			//�ṹ�����

	lvi.mask = LVIF_IMAGE | LVIF_TEXT;	//��ͼƬ+�ı���ʽ��ʾ����
	lvi.iItem = 0;			//�ڵ�0���ϲ��������� ͷ�巨
	lvi.iImage = 4;			//ʹ��ͼƬ�б��еĵ�5��ͼƬ	

	//��listContrl�У�����һ�У�Я����0������
	lvi.iSubItem = 0;		
	lvi.pszText = (LPTSTR)(LPCTSTR)ID;
	m_listSecNode.InsertItem(&lvi);

	//���õ�1������
	lvi.iSubItem = 1;		
	lvi.pszText = (LPTSTR)(LPCTSTR)Name;
	m_listSecNode.SetItem(&lvi);

	//���õ�2������
	CString strTime = time.Format("%Y-%m-%d %H:%M:%S");
	lvi.iSubItem = 2;		
	lvi.pszText = (LPTSTR)(LPCTSTR)strTime;
	m_listSecNode.SetItem(&lvi);

	//���õ�3������
	lvi.iSubItem = 3;		
	if (state == 1) {
		lvi.pszText = "����";
	} else {
		lvi.pszText = "����";
	}
	m_listSecNode.SetItem(&lvi);

	//���õ�4������
	lvi.iSubItem = 4;		
	//CString strAuthcode(authcode) ;
	char buf[100];
	sprintf(buf, "%d", authcode);
	lvi.pszText = buf;
	m_listSecNode.SetItem(&lvi);

	return 0;
}

// ��������һ����¼
void CDlgNetInfo::OnBnClickedButton1()
{
	CString ID = "0001";
	CString Name = "����������������";
	CTime time = CTime::GetCurrentTime();
	int state = 0;
	int authcode = 1;

	DbInitListSecNode(ID, Name, time, state, authcode);
}


void CDlgNetInfo::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// �� 16 x 16 ���У�һ����8��ͼ�꣬���һ��ͼƬ�б�
	HIMAGELIST hList = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, 8, 1);

	m_imageList.Attach(hList);		//���� CImageList ���󸽼�����

	CBitmap cBmp;			// ����CBitmap����
	cBmp.LoadBitmap(IDB_BITMAP_SECNODE);
	m_imageList.Add(&cBmp, RGB(255, 0, 255));	// �� CImageList ������� ͼƬ��Դ
	cBmp.DeleteObject();	// ����CBitmap����

	// ��listContrl�ؼ����ͼƬ�б�
	m_listSecNode.SetImageList(&m_imageList, LVSIL_SMALL);

	//��ȡ�ؼ�����ʾ״̬
	DWORD dwExStyle = ListView_GetExtendedListViewStyle(m_listSecNode.m_hWnd);
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;	//�޸� ״̬
	ListView_SetExtendedListViewStyle(m_listSecNode.m_hWnd, dwExStyle); //����
	
	CRect rect; 
	m_listSecNode.GetClientRect(&rect);				// ��ȡ listContrl �ؼ���С

	int nColInterval = rect.Width() / 5;		// ���ݴ�Сȡ�п�

	m_listSecNode.SetRedraw(FALSE);
	m_listSecNode.InsertColumn(0, "������", LVCFMT_LEFT, nColInterval);		// ����������
	m_listSecNode.InsertColumn(1, "��������", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(2, "���㴴��ʱ��", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(3, "����״̬", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(4, "������Ȩ��", LVCFMT_LEFT, rect.Width() - 4 * nColInterval);
	m_listSecNode.SetRedraw(TRUE);
}

extern CDatabase	*g_pDB;

void CDlgNetInfo::OnBnClickedButtonSearch()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int			dbtag = 0;
	CWnd		*myWnd = NULL;
	CButton		*But = NULL;
	int			rv = 0, tag = 0;	//��ʾû�м�������¼
	int			dbTag = 0;			//���ݿ�����Ƿ�ʧ��0�ɹ�
	CString		strFilter;

	//���ݿؼ���Դ����ȡ�����������С���򵥵Ŀؼ��� 
	myWnd = (CWnd *)GetDlgItem(IDC_CHECK_TIME);
	But = (CButton *)myWnd;

	UpdateData(TRUE); // �ѽ����ֵ ���ݸ�C++����

	if (But->GetCheck() == BST_CHECKED)
	{
		// select * from secmng.secnode;
		CSECMNGSECNODE	rsetMngSecNode(g_pDB);
		TRY 
		{
			if (!rsetMngSecNode.Open(CRecordset::snapshot, NULL, CRecordset::none))
			{
				MessageBox("��CSECMNGSECNODE��ʧ�ܣ�", "���ݿ����", MB_MODEMASK);
				return;
			}
			CTime sqlTime1(m_dateBegin.GetYear(), m_dateBegin.GetMonth(), m_dateBegin.GetDay(), 0, 0, 0);
			CTime sqlTime2(m_dateEnd.GetYear(), m_dateEnd.GetMonth(), m_dateEnd.GetDay(), 23, 59, 59);

			if (sqlTime1 >= sqlTime2)
			{
				MessageBox("��ʼʱ�䲻�ܴ��ڽ���ʱ�䣡", "ʱ���ѯ", MB_MODEMASK);
				return;
			}
			m_listSecNode.DeleteAllItems();

			while (!rsetMngSecNode.IsEOF())
			{			
				// ��������ʱ�䷶Χ��
				if (rsetMngSecNode.m_CREATETIME < sqlTime1 || 
					rsetMngSecNode.m_CREATETIME > sqlTime2)
				{
					rsetMngSecNode.MoveNext();
					continue;
				}  
				rsetMngSecNode.m_ID.TrimLeft(); rsetMngSecNode.m_ID.TrimRight();
				rsetMngSecNode.m_NAME.TrimLeft(); rsetMngSecNode.m_NAME.TrimRight();
				rsetMngSecNode.m_NODEDESC.TrimLeft(); rsetMngSecNode.m_NODEDESC.TrimRight();
				//rsetMngSecNode.m_CREATETIME;
				//rsetMngSecNode.m_AUTHCODE;  
				//rsetMngSecNode.m_STATE;

				//��������������
				DbInitListSecNode(rsetMngSecNode.m_ID, rsetMngSecNode.m_NAME, rsetMngSecNode.m_CREATETIME,
					rsetMngSecNode.m_STATE, rsetMngSecNode.m_AUTHCODE);

				rsetMngSecNode.MoveNext();
			}
		}
		CATCH_ALL (e)
		{
			e->ReportError();
			tag = 1;	//���쳣 ״̬	
		}
		END_CATCH_ALL
	
		if (rsetMngSecNode.IsOpen())
		{
			rsetMngSecNode.Close();
		}
	}

	if (tag == 1)
	{
		AfxMessageBox("�������ݱ�ʧ��");
	}
}

void CDlgNetInfo::OnBnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int dbTag = 0;
	CString strTmp, strID, strFilter;

	POSITION pos = m_listSecNode.GetFirstSelectedItemPosition();	//��ȡ��ѡ���е�λ�á�

	int nItem = m_listSecNode.GetNextSelectedItem(pos);  //�õ�ѡ���е��кţ���0��ʼ��

	strID = m_listSecNode.GetItemText(nItem, 0);
	CString strname = m_listSecNode.GetItemText(nItem, 1);

	strFilter.Format("id = '%s' ", strID);

	strTmp.Format("�Ƿ�Ҫɾ�����Ϊ��%s����������Ϣ��", strID);
	if (AfxMessageBox(strTmp, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDNO)
	{
		return;
	}

	g_pDB->BeginTrans();
	// select * from secmng.secnode where id='ID';

	CSECMNGSECNODE	rsetMngSecNode(g_pDB);
	TRY
	{
		rsetMngSecNode.m_strFilter = strFilter;
		if (!rsetMngSecNode.Open(CRecordset::snapshot, NULL, CRecordset::none))
		{
			g_pDB->Rollback();
			MessageBox("��������Ϣ��ʧ�ܣ�", "���ݿ����", MB_MODEMASK);
			return;
		}
		//ɾ����¼
		if (!rsetMngSecNode.IsEOF())
		{
			rsetMngSecNode.Delete();
		}
		else
		{
			MessageBox("û�м��������������ļ�¼��", "���ݿ����", MB_MODEMASK);
			dbTag = 1;
		}
		rsetMngSecNode.Close();
	}
	CATCH_ALL(e)
	{
		dbTag = 1;
		e->ReportError();
		if (rsetMngSecNode.IsOpen())
		{
			rsetMngSecNode.Close();
		}
	}
	END_CATCH_ALL

	if (dbTag == 1)
	{
		g_pDB->Rollback();
		MessageBox("�������ݿ�ʧ�ܣ�", "���ݿ����", MB_MODEMASK);
		return;
	}
	else
	{
		g_pDB->CommitTrans();
	}

	m_listSecNode.DeleteItem(nItem);
}

void CDlgNetInfo::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	// 1. ��ȡ pos  POSITION pos = m_listSecNode.GetFirstSelectedItemPosition();
	// 2. ��ȡ item int nItem = m_listSecNode.GetNextSelectedItem(pos);	
	// 3. ��ȡ�ı� strID = m_listSecNode.GetItemText(nItem, 0);
			/*
			name = m_listSecNode.GetItemText(nItem, 1);
			time = m_listSecNode.GetItemText(nItem, 2);
			node = m_listSecNode.GetItemText(nItem, 3);
			state = m_listSecNode.GetItemText(nItem,4);
			*/
	// 4. ��Dialog��չʾ  iD/name/time/node/state
	// 5. �û����±༭ iD/name/time/node/state ���档

	// 6. �����水ť��ӻص���{
		// 1. updatedate(T)   ---�� dialog �ĳ�Ա������
		// 2. ������¼�����	format  open  isEoF()	 Edit()-update();  �������ݿ�
		// 3. д��listcontrl����

	// 7. ������ɡ�
}
