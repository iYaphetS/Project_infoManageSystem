// DlgNetInfo.cpp : 实现文件
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


///CDlgNetInfo 诊断

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


// CDlgNetInfo 消息处理程序

//int InsertItem(_In_ const LVITEM* pItem);

/* 
typedef struct tagLVITEMW
{
	UINT mask;			//显示方式 文字、图片？
	int iItem;			//插入位置，在第几行插入
	int iSubItem;		//插入的列数
	UINT state;			//状态
	UINT stateMask;
	LPWSTR pszText;		//显示的数据内容 
	int cchTextMax;
	int iImage;			//使用的图片的序号 
	LPARAM lParam;		//可以在每一行上 隐藏一些数据
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

// 插入一行数据到 listContrl控件中
int CDlgNetInfo::DbInitListSecNode(CString &ID, CString &Name, 
								   CTime &time, int state, int authcode)
{
	LVITEM   lvi;			//结构体变量

	lvi.mask = LVIF_IMAGE | LVIF_TEXT;	//按图片+文本方式显示数据
	lvi.iItem = 0;			//在第0行上插入新数据 头插法
	lvi.iImage = 4;			//使用图片列表中的第5个图片	

	//向listContrl中，插入一行，携带第0列数据
	lvi.iSubItem = 0;		
	lvi.pszText = (LPTSTR)(LPCTSTR)ID;
	m_listSecNode.InsertItem(&lvi);

	//设置第1列数据
	lvi.iSubItem = 1;		
	lvi.pszText = (LPTSTR)(LPCTSTR)Name;
	m_listSecNode.SetItem(&lvi);

	//设置第2列数据
	CString strTime = time.Format("%Y-%m-%d %H:%M:%S");
	lvi.iSubItem = 2;		
	lvi.pszText = (LPTSTR)(LPCTSTR)strTime;
	m_listSecNode.SetItem(&lvi);

	//设置第3列数据
	lvi.iSubItem = 3;		
	if (state == 1) {
		lvi.pszText = "禁用";
	} else {
		lvi.pszText = "正常";
	}
	m_listSecNode.SetItem(&lvi);

	//设置第4列数据
	lvi.iSubItem = 4;		
	//CString strAuthcode(authcode) ;
	char buf[100];
	sprintf(buf, "%d", authcode);
	lvi.pszText = buf;
	m_listSecNode.SetItem(&lvi);

	return 0;
}

// “创建”一条记录
void CDlgNetInfo::OnBnClickedButton1()
{
	CString ID = "0001";
	CString Name = "北京建设总行网点";
	CTime time = CTime::GetCurrentTime();
	int state = 0;
	int authcode = 1;

	DbInitListSecNode(ID, Name, time, state, authcode);
}


void CDlgNetInfo::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// 按 16 x 16 排列，一共有8个图标，组成一个图片列表。
	HIMAGELIST hList = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, 8, 1);

	m_imageList.Attach(hList);		//设置 CImageList 对象附加属性

	CBitmap cBmp;			// 创建CBitmap对象。
	cBmp.LoadBitmap(IDB_BITMAP_SECNODE);
	m_imageList.Add(&cBmp, RGB(255, 0, 255));	// 给 CImageList 对象添加 图片资源
	cBmp.DeleteObject();	// 销毁CBitmap对象。

	// 给listContrl控件添加图片列表
	m_listSecNode.SetImageList(&m_imageList, LVSIL_SMALL);

	//获取控件的显示状态
	DWORD dwExStyle = ListView_GetExtendedListViewStyle(m_listSecNode.m_hWnd);
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;	//修改 状态
	ListView_SetExtendedListViewStyle(m_listSecNode.m_hWnd, dwExStyle); //设置
	
	CRect rect; 
	m_listSecNode.GetClientRect(&rect);				// 获取 listContrl 控件大小

	int nColInterval = rect.Width() / 5;		// 根据大小取列宽

	m_listSecNode.SetRedraw(FALSE);
	m_listSecNode.InsertColumn(0, "网点编号", LVCFMT_LEFT, nColInterval);		// 设置列属性
	m_listSecNode.InsertColumn(1, "网点名称", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(2, "网点创建时间", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(3, "网点状态", LVCFMT_LEFT, nColInterval);
	m_listSecNode.InsertColumn(4, "网点授权码", LVCFMT_LEFT, rect.Width() - 4 * nColInterval);
	m_listSecNode.SetRedraw(TRUE);
}

extern CDatabase	*g_pDB;

void CDlgNetInfo::OnBnClickedButtonSearch()
{
	// TODO:  在此添加控件通知处理程序代码
	int			dbtag = 0;
	CWnd		*myWnd = NULL;
	CButton		*But = NULL;
	int			rv = 0, tag = 0;	//表示没有检索到记录
	int			dbTag = 0;			//数据库操作是否失败0成功
	CString		strFilter;

	//根据控件资源，获取类对象。适用于小而简单的控件。 
	myWnd = (CWnd *)GetDlgItem(IDC_CHECK_TIME);
	But = (CButton *)myWnd;

	UpdateData(TRUE); // 把界面的值 传递给C++变量

	if (But->GetCheck() == BST_CHECKED)
	{
		// select * from secmng.secnode;
		CSECMNGSECNODE	rsetMngSecNode(g_pDB);
		TRY 
		{
			if (!rsetMngSecNode.Open(CRecordset::snapshot, NULL, CRecordset::none))
			{
				MessageBox("打开CSECMNGSECNODE表失败！", "数据库操作", MB_MODEMASK);
				return;
			}
			CTime sqlTime1(m_dateBegin.GetYear(), m_dateBegin.GetMonth(), m_dateBegin.GetDay(), 0, 0, 0);
			CTime sqlTime2(m_dateEnd.GetYear(), m_dateEnd.GetMonth(), m_dateEnd.GetDay(), 23, 59, 59);

			if (sqlTime1 >= sqlTime2)
			{
				MessageBox("开始时间不能大于结束时间！", "时间查询", MB_MODEMASK);
				return;
			}
			m_listSecNode.DeleteAllItems();

			while (!rsetMngSecNode.IsEOF())
			{			
				// 不在搜索时间范围内
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

				//逐行向界面挂数据
				DbInitListSecNode(rsetMngSecNode.m_ID, rsetMngSecNode.m_NAME, rsetMngSecNode.m_CREATETIME,
					rsetMngSecNode.m_STATE, rsetMngSecNode.m_AUTHCODE);

				rsetMngSecNode.MoveNext();
			}
		}
		CATCH_ALL (e)
		{
			e->ReportError();
			tag = 1;	//有异常 状态	
		}
		END_CATCH_ALL
	
		if (rsetMngSecNode.IsOpen())
		{
			rsetMngSecNode.Close();
		}
	}

	if (tag == 1)
	{
		AfxMessageBox("检索数据表失败");
	}
}

void CDlgNetInfo::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	int dbTag = 0;
	CString strTmp, strID, strFilter;

	POSITION pos = m_listSecNode.GetFirstSelectedItemPosition();	//获取被选中行的位置。

	int nItem = m_listSecNode.GetNextSelectedItem(pos);  //得到选中行的行号，从0开始。

	strID = m_listSecNode.GetItemText(nItem, 0);
	CString strname = m_listSecNode.GetItemText(nItem, 1);

	strFilter.Format("id = '%s' ", strID);

	strTmp.Format("是否要删除编号为【%s】的网点信息吗？", strID);
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
			MessageBox("打开网点信息表失败！", "数据库操作", MB_MODEMASK);
			return;
		}
		//删除记录
		if (!rsetMngSecNode.IsEOF())
		{
			rsetMngSecNode.Delete();
		}
		else
		{
			MessageBox("没有检索到符合条件的记录！", "数据库操作", MB_MODEMASK);
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
		MessageBox("检索数据库失败！", "数据库操作", MB_MODEMASK);
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
	// TODO: 在此添加控件通知处理程序代码

	// 1. 获取 pos  POSITION pos = m_listSecNode.GetFirstSelectedItemPosition();
	// 2. 获取 item int nItem = m_listSecNode.GetNextSelectedItem(pos);	
	// 3. 获取文本 strID = m_listSecNode.GetItemText(nItem, 0);
			/*
			name = m_listSecNode.GetItemText(nItem, 1);
			time = m_listSecNode.GetItemText(nItem, 2);
			node = m_listSecNode.GetItemText(nItem, 3);
			state = m_listSecNode.GetItemText(nItem,4);
			*/
	// 4. 在Dialog中展示  iD/name/time/node/state
	// 5. 用户重新编辑 iD/name/time/node/state 保存。

	// 6. 给保存按钮添加回调：{
		// 1. updatedate(T)   ---》 dialog 的成员变量中
		// 2. 创建记录类对象	format  open  isEoF()	 Edit()-update();  更新数据库
		// 3. 写入listcontrl界面

	// 7. 更新完成。
}
