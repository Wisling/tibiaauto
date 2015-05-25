// TimedDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_spellcaster.h"
#include "TimedDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// TimedDialog dialog


TimedDialog::TimedDialog(std::vector<TimedSpell> timedSpellList, CWnd* pParent /*=NULL*/)
	: MyDialog(TimedDialog::IDD, pParent)
{
	spellList = timedSpellList;
	//{{AFX_DATA_INIT(TimedDialog)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

TimedDialog::TimedDialog(CWnd* pParent /*=NULL*/)
	: MyDialog(TimedDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(TimedDialog)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void TimedDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TimedDialog)
	DDX_Control(pDX, IDC_TIMED_SPELL_LIST, m_timedSpellList);
	DDX_Control(pDX, IDC_TIMED_SPELL_DELETE, m_timedSpellDelete);
	DDX_Control(pDX, IDC_TIMED_SPELL_ADD, m_timedSpellAdd);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_USE_POTIONS, m_UsePotions);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_TIMED_SPELL_MANA, m_timedSpellMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_TIMED_SPELL_DELAY, m_timedSpellDelay);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_TIMED_SPELL, m_timedSpellWords);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_TIMED, m_timedSpell);
	DDX_Control(pDX, IDC_FRAM_TIMED_CASTING, m_timedSpellFrame);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TimedDialog, CDialog)
//{{AFX_MSG_MAP(TimedDialog)
ON_BN_CLICKED(IDC_TIMED_SPELL_ADD, OnTimedSpellAdd)
ON_BN_CLICKED(IDC_TIMED_SPELL_DELETE, OnTimedSpellDelete)
ON_BN_DOUBLECLICKED(IDC_TIMED_SPELL_DELETE, OnTimedSpellDelete)
ON_WM_ERASEBKGND()
ON_WM_CTLCOLOR()
ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_TIMED, OnToolSpellcasterTimed)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TimedDialog message handlers

void TimedDialog::disableControls()
{
	m_timedSpellWords.EnableWindow(false);
	m_timedSpellDelay.EnableWindow(false);
	m_UsePotions.EnableWindow(false);
	m_timedSpell.EnableWindow(false);
	m_timedSpellMana.EnableWindow(false);
}

void TimedDialog::enableControls()
{
	m_timedSpell.EnableWindow(true);
	if (m_timedSpell.GetCheck())
	{
		m_timedSpellWords.EnableWindow(true);
		m_timedSpellMana.EnableWindow(true);
		m_timedSpellDelay.EnableWindow(true);
		m_UsePotions.EnableWindow(true);
	}
}

void TimedDialog::configToControls(CConfigData *configData)
{
	m_timedSpell.SetCheck(configData->timedSpell);
	spellList = configData->timedSpellList;
	m_timedSpellList.DeleteAllItems();
	char lpBuffer[32];
	for (size_t i = 0; i < spellList.size(); i++)
	{
		TimedSpell temp = spellList[i];
		m_timedSpellList.InsertItem(i, "");
		m_timedSpellList.SetItemText(0, 0, temp.spell);
		sprintf(lpBuffer, "%d", temp.mana);
		m_timedSpellList.SetItemText(0, 1, lpBuffer);
		sprintf(lpBuffer, "%d", temp.delay);
		m_timedSpellList.SetItemText(0, 2, lpBuffer);
		m_timedSpellList.SetItemText(0, 3, temp.usePotions ? "Yes" : "No");
	}
	OnToolSpellcasterTimed();
}

void TimedDialog::controlsToConfig(CConfigData *newConfigData)
{
	newConfigData->timedSpell     = m_timedSpell.GetCheck();
	newConfigData->timedSpellList = spellList;
}

void TimedDialog::DoSetButtonSkin()
{
	skin.SetButtonSkin(m_timedSpellDelete);
	skin.SetButtonSkin(m_timedSpellAdd);
}

BOOL TimedDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_timedSpellList.InsertColumn(0, "Spell", LVCFMT_LEFT, 78);
	m_timedSpellList.InsertColumn(1, "Mana", LVCFMT_LEFT, 47);
	m_timedSpellList.InsertColumn(2, "Delay (ms)", LVCFMT_LEFT, 65);
	m_timedSpellList.InsertColumn(3, "Potions", LVCFMT_LEFT, 48);

	m_timedSpellList.SetExtendedStyle(m_timedSpellList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	OnToolSpellcasterTimed();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TimedDialog::OnTimedSpellAdd()
{
	char lpSpellBuffer[64];
	char lpManaBuffer[8];
	char lpDelayBuffer[8];
	int spellLen = m_timedSpellWords.GetWindowText(lpSpellBuffer, 64);
	int manaLen  = m_timedSpellMana.GetWindowText(lpManaBuffer, 8);
	int delayLen = m_timedSpellDelay.GetWindowText(lpDelayBuffer, 8);
	if (spellLen && manaLen && delayLen)
	{
		m_timedSpellList.InsertItem(spellList.size(), "");
		m_timedSpellList.SetItemText(0, 0, lpSpellBuffer);
		m_timedSpellList.SetItemText(0, 1, lpManaBuffer);
		m_timedSpellList.SetItemText(0, 2, lpDelayBuffer);
		m_timedSpellList.SetItemText(0, 3, m_UsePotions.GetCheck() ? "Yes" : "No");
		m_timedSpellWords.SetWindowText("");
		m_timedSpellMana.SetWindowText("");
		m_timedSpellDelay.SetWindowText("");
		m_UsePotions.SetCheck(false);
		TimedSpell temp;
		temp.randMana    = 0;
		temp.triggerTime = 0;
		strcpy(temp.spell, lpSpellBuffer);
		temp.mana       = atoi(lpManaBuffer);
		temp.delay      = atoi(lpDelayBuffer);
		temp.usePotions = m_UsePotions.GetCheck() ? true : false;
		spellList.push_back(temp);
	}
	else
	{
		AfxMessageBox("Some entries are blank, please try again!");
	}
}

void TimedDialog::OnTimedSpellDelete()
{
	int nSelRows = m_timedSpellList.GetSelectedCount();
	if (!nSelRows)
		return;

	int* pnArrayOfSelRows = new int[nSelRows];
	if (!pnArrayOfSelRows)
		return;

	int nTemp    = nSelRows;
	POSITION pos = m_timedSpellList.GetFirstSelectedItemPosition();

	int i = m_timedSpellList.GetNextSelectedItem(pos);

	while (i != -1)
	{
		int find = i;
		nTemp                   = nTemp - 1;
		pnArrayOfSelRows[nTemp] = i;
		i                       = m_timedSpellList.GetNextSelectedItem(pos);
		for (size_t k = 0; k < spellList.size(); k++)
		{
			if (spellList[k].spell == m_timedSpellList.GetItemText(find, 0))
				spellList.erase(spellList.begin() + k);
		}
	}

	for (int j = 0; j < nSelRows; j++)
		m_timedSpellList.DeleteItem(pnArrayOfSelRows[j]);

	delete(pnArrayOfSelRows);
	pnArrayOfSelRows = NULL;
}

void TimedDialog::OnToolSpellcasterTimed()
{
	int val = m_timedSpell.GetCheck();
	m_timedSpellWords.EnableWindow(val);
	m_timedSpellMana.EnableWindow(val);
	m_timedSpellDelay.EnableWindow(val);
	m_UsePotions.EnableWindow(val);
	m_timedSpellAdd.EnableWindow(val);
	m_timedSpellDelete.EnableWindow(val);
}
