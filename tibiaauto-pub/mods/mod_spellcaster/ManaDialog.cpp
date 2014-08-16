// ManaDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_spellcaster.h"
#include "ManaDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ManaDialog dialog


ManaDialog::ManaDialog(CWnd* pParent /*=NULL*/)
: MyDialog(ManaDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(ManaDialog)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ManaDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ManaDialog)
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_MANA_SPELL, m_manaSpell);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_MANA_MANA, m_manaMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_MANA, m_mana);
	DDX_Control(pDX, IDC_FRAME_MANA, m_ManaFrame);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ManaDialog, CDialog)
	//{{AFX_MSG_MAP(ManaDialog)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_MANA, OnToolSpellcasterMana)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ManaDialog message handlers

void ManaDialog::disableControls() {
	m_mana.EnableWindow(false);
	m_manaMana.EnableWindow(false);
	m_manaSpell.EnableWindow(false);
}

void ManaDialog::enableControls() {
	m_mana.EnableWindow(true);
	if (m_mana.GetCheck()){
		m_manaMana.EnableWindow(true);
		m_manaSpell.EnableWindow(true);
	}
}

void ManaDialog::configToControls(CConfigData *configData) {
	char buf[128];
	m_mana.SetCheck(configData->mana);
	sprintf(buf,"%d",configData->manaMana);			m_manaMana.SetWindowText(buf);
	sprintf(buf,"%s",configData->manaSpell);		m_manaSpell.SetWindowText(buf);
	OnToolSpellcasterMana();
}

void ManaDialog::controlsToConfig(CConfigData *newConfigData) {
	char buf[128];
	newConfigData->mana = m_mana.GetCheck();
	m_manaMana.GetWindowText(buf,127);newConfigData->manaMana=atoi(buf);
	m_manaSpell.GetWindowText(newConfigData->manaSpell,127);
}

void ManaDialog::OnToolSpellcasterMana()
{
	int val = m_mana.GetCheck();
	m_manaMana.EnableWindow(val);
	m_manaSpell.EnableWindow(val);
}

BOOL ManaDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	OnToolSpellcasterMana();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
