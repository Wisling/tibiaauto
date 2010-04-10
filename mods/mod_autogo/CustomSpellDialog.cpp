// CustomSpellDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_autogo.h"
#include "CustomSpellDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomSpellDialog dialog


CCustomSpellDialog::CCustomSpellDialog(CWnd* pParent /*=NULL*/)
	: MyDialog(CCustomSpellDialog::IDD, pParent) {
	//{{AFX_DATA_INIT(CCustomSpellDialog)
	m_spellWords = _T("");
	m_castingDelay = 0;
	m_manaCost = 0;
	//}}AFX_DATA_INIT
}


void CCustomSpellDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomSpellDialog)
	DDX_Control(pDX, IDC_SPELL_CONFIGURATION_FRAME, m_spellConfigFrame);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_cancel);
	DDX_Text(pDX, IDC_SPELLWORDS, m_spellWords);
	DDX_Text(pDX, IDC_CASTING_DELAY, m_castingDelay);
	DDV_MinMaxInt(pDX, m_castingDelay, 0, 2000000000);
	DDX_Text(pDX, IDC_REQUIRED_MANA, m_manaCost);
	DDV_MinMaxInt(pDX, m_manaCost, 0, 2000000000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomSpellDialog, CDialog)
	//{{AFX_MSG_MAP(CCustomSpellDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomSpellDialog message handlers

BOOL CCustomSpellDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	
	skin.SetButtonSkin(m_OK);
	skin.SetButtonSkin(m_cancel);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
