// ScriptConfigDialg.cpp : implementation file
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "ScriptConfigDialg.h"
#include "PythonScript.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScriptConfigDialg dialog


CScriptConfigDialg::CScriptConfigDialg(int scriptNr, CWnd* pParent /*=NULL*/)
	: MyDialog(CScriptConfigDialg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScriptConfigDialg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	this->scriptNr = scriptNr;
	lastParamNr    = -1;
}

void CScriptConfigDialg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptConfigDialg)
	DDX_Control(pDX, IDC_SCRIPT_CONFIG_PARAM_LIST, m_list);
	DDX_Control(pDX, IDC_SCRIPT_CONFIG_PARAM_EDIT, m_edit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScriptConfigDialg, CDialog)
//{{AFX_MSG_MAP(CScriptConfigDialg)
ON_NOTIFY(NM_CLICK, IDC_SCRIPT_CONFIG_PARAM_LIST, OnClickScriptConfigParamList)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptConfigDialg message handlers

BOOL CScriptConfigDialg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_list.InsertColumn(0, "Name", LVCFMT_LEFT, 70);
	m_list.InsertColumn(1, "Description", LVCFMT_LEFT, 200);
	m_list.InsertColumn(2, "Value", LVCFMT_LEFT, 100);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
	if (pythonScript)
	{
		int nr = 0;
		for (;; )
		{
			struct paramType *param = pythonScript->getParamDef(nr++);
			if (!param)
				break;

			m_list.InsertItem(nr, "");
			m_list.SetItemText(nr - 1, 0, param->name);
			m_list.SetItemText(nr - 1, 1, param->description);
			m_list.SetItemText(nr - 1, 2, param->value);
			m_list.SetItemData(nr - 1, nr - 1);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScriptConfigDialg::OnClickScriptConfigParamList(NMHDR* pNMHDR, LRESULT* pResult)
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos)
	{
		CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
		int paramNr                 = m_list.GetItemData(m_list.GetNextSelectedItem(pos));
		struct paramType *param     = pythonScript->getParamDef(paramNr);

		if (lastParamNr != paramNr && lastParamNr != -1)
			if (lastParamNr != -1)
				saveParamValue(lastParamNr);

		if (param)
		{
			m_edit.SetWindowText(param->value);
			lastParamNr = paramNr;
		}
		else
		{
			m_edit.SetWindowText("");
			lastParamNr = -1;
		}
	}
	else
	{
		if (lastParamNr != -1)
			saveParamValue(lastParamNr);
		m_edit.SetWindowText("");
		lastParamNr = -1;
	}


	*pResult = 0;
}

void CScriptConfigDialg::saveParamValue(int paramNr)
{
	CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);

	char buf[1024];
	m_edit.GetWindowText(buf, 1023);
	// save last param value

	m_list.SetItemText(paramNr, 2, buf);
	struct paramType *oldParam = pythonScript->getParamDef(paramNr);
	if (oldParam)
		strncpy(oldParam->value, buf, 1023);
}

void CScriptConfigDialg::OnOK()
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos)
	{
		CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
		int paramNr                 = m_list.GetItemData(m_list.GetNextSelectedItem(pos));
		saveParamValue(paramNr);
	}
	CDialog::OnOK();
}
