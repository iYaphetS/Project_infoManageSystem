int dbtag = 0;
	CString sql;

	CSECMNGSRVCFG srvCfgSet(g_pDB);	//构造纪录类

	UpdateData(TRUE); //更新界面数据到变量中
	//空值判断
	m_srvIP.Trim();
	m_srvMAXNODE.Trim();
	m_srvPORT.Trim();

	if (m_srvIP.IsEmpty() || m_srvMAXNODE.IsEmpty() || m_srvPORT.IsEmpty())
	{
		AfxMessageBox("保存信息不能有空值");
		return;
	}

	//开启事务
	g_pDB->BeginTrans();

	TRY
	{
		//写IP
		sql.Format("select * from SECMNG.SRVCFG WHERE KEY = '%s'", "IP");
		srvCfgSet.Open(CRecordset::snapshot, sql);
		if (!srvCfgSet.IsEOF())
		{
			//修改数据
			sql.Format("UPDATE SECMNG.SRVCFG SET VALUDE = '%s' WHERE KEY = '%s'", m_srvIP, "IP");
		}
		else
		{
			//无该数据条，新增数据
			sql.Format("INSERT INTO SECMNG.SRVCFG(KEY, VALUDE) VALUES('%s', '%s')",  "IP", m_srvIP);
		}
		g_pDB->ExecuteSQL(sql);
		srvCfgSet.Close();

		//写MAXNODE
		sql.Format("select * from SECMNG.SRVCFG WHERE KEY = '%s'", "MAXNODE");
		srvCfgSet.Open(CRecordset::snapshot, sql);
		if (!srvCfgSet.IsEOF())
		{
			sql.Format("UPDATE SECMNG.SRVCFG SET VALUDE = '%s' WHERE KEY = 'MAXNODE'", m_srvMAXNODE, "MAXNODE");
		}
		else
		{
			sql.Format("INSERT INTO SECMNG.SRVCFG(KEY, VALUDE) VALUES('%s', '%s')", "MAXNODE", m_srvMAXNODE);
		}
		g_pDB->ExecuteSQL(sql);
		srvCfgSet.Close();

		//写PORT
		sql.Format("select * from SECMNG.SRVCFG WHERE KEY = '%s'", "PORT");
		srvCfgSet.Open(CRecordset::snapshot, sql);
		if (!srvCfgSet.IsEOF())
		{
			sql.Format("UPDATE SECMNG.SRVCFG SET VALUDE = '%s' WHERE KEY = 'PORT'", m_srvPORT, "PORT");
		}
		else
		{
			sql.Format("INSERT INTO SECMNG.SRVCFG(KEY, VALUDE) VALUES('%s', '%s')", "PORT", m_srvPORT);
		}

		g_pDB->ExecuteSQL(sql);
		srvCfgSet.Close();
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		g_pDB->Rollback();
		dbtag = 1;
	}
	END_CATCH_ALL	

	if (dbtag == 1)
	{
		g_pDB->Rollback();
		AfxMessageBox("数据库操作异常");
	}
	else
	{
		g_pDB->CommitTrans();
	}

	if (srvCfgSet.IsOpen())
	{
		srvCfgSet.Close();
	}

	AfxMessageBox("保存数据 ok");
	UpdateData(FALSE);
	return;