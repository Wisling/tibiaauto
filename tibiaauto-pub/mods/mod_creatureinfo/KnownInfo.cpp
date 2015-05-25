// KnownInfo.cpp : implementation file
//

#include "stdafx.h"
#include "mod_creatureinfo.h"
#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "KnownInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

int iLastLen = 0;

/////////////////////////////////////////////////////////////////////////////
// CKnownInfo dialog


CKnownInfo::CKnownInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CKnownInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKnownInfo)
	//}}AFX_DATA_INIT
}


void CKnownInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKnownInfo)
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_TXTLEVEL, m_txtLevel);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_VOC, m_voc);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_TYPE, m_type);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_TXTVOC, m_txtVoc);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_NAME, m_name);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_LEVEL, m_level);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_LAST, m_lastSeen);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_HP, m_hp);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_GUILDRANK, m_guildRank);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_GUILDNAME, m_guildName);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_GUILDDESCRIPTION, m_guildDesc);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_COMBO, m_combo);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_PLAYERS, m_players);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_INFO_MONSTERS, m_monsters);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKnownInfo, CDialog)
//{{AFX_MSG_MAP(CKnownInfo)
ON_BN_CLICKED(IDC_TOOLCREATURINFO_INFO_PLAYERS, OnToolcreaturinfoInfoPlayers)
ON_BN_CLICKED(IDC_TOOLCREATURINFO_INFO_MONSTERS, OnToolcreaturinfoInfoMonsters)
ON_CBN_EDITCHANGE(IDC_TOOLCREATURINFO_INFO_COMBO, OnEditchangeToolcreaturinfoInfoCombo)
ON_CBN_SELENDOK(IDC_TOOLCREATURINFO_INFO_COMBO, OnSelendokToolcreaturinfoInfoCombo)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKnownInfo message handlers

void CKnownInfo::OnToolcreaturinfoInfoPlayers()
{
	m_monsters.SetCheck(0);

	m_txtLevel.SetWindowText("Level:");
	m_txtVoc.SetWindowText("Vocation:");

	Fill_Player();

	m_combo.SetCurSel(0);
	OnSelendokToolcreaturinfoInfoCombo();
}

void CKnownInfo::OnToolcreaturinfoInfoMonsters()
{
	m_players.SetCheck(0);

	m_txtLevel.SetWindowText("Exp:");
	m_txtVoc.SetWindowText("Description:");

	Fill_Monster();

	m_combo.SetCurSel(0);
	OnSelendokToolcreaturinfoInfoCombo();
}

void CKnownInfo::Fill_Player(){
	int i;

	m_combo.ResetContent();
	for (i = 0; i < playersCount; i++)
	{
		m_combo.InsertString(i, playersInfo[i].name);
	}
}

void CKnownInfo::Fill_Monster(){
	int i;

	m_combo.ResetContent();
	for (i = 0; i < monstersCount; i++)
	{
		m_combo.InsertString(i, monstersInfo[i].name);
	}
}

BOOL CKnownInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_players.SetCheck(1);
	OnToolcreaturinfoInfoPlayers();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CKnownInfo::OnEditchangeToolcreaturinfoInfoCombo()
{
//T4: Someone changed Combo to DropList and now my briliant code is useless ;(

/*	char buffer[128];
        char iLen = m_combo.GetWindowText(buffer,127);

        int iItem = m_combo.FindString(-1,buffer);
        if (iItem ==CB_ERR){
                MessageBeep(MB_OK);
        }else{
                if (iLastLen<iLen){
                        m_combo.SetCurSel(iItem);
                        m_combo.SetEditSel(iLen,128);
                        OnSelendokToolcreaturinfoInfoCombo();
                }
        }
        iLastLen = iLen;*/
}

void CKnownInfo::OnSelendokToolcreaturinfoInfoCombo() {
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	char buffer[260];
	int iSel  = m_combo.GetCurSel();
	int found = -1;

	if (m_players.GetCheck())
	{
		//T4: Try to find player in BattleList
		for (int chNr = 0; chNr < memConstData.m_memMaxCreatures; chNr++)
		{
			CTibiaCharacter *ch = reader.readVisibleCreature(chNr);
			if (ch->tibiaId == 0)
			{
				delete ch;
				break;
			}
			if (!_strcmpi(ch->name, playersInfo[iSel].name))
			{
				found = chNr;
				delete ch;
				break;
			}
			delete ch;
		}
		if (found != -1)
		{
			CTibiaCharacter *ch = reader.readVisibleCreature(found);

			sprintf(buffer, "%d/%d", MulDiv(ch->hpPercLeft, playersInfo[iSel].maxHp, 100), playersInfo[iSel].maxHp);
			m_hp.SetWindowText(buffer);
			sprintf(buffer, "%d,%d,%d", ch->x, ch->y, ch->z);
			m_lastSeen.SetWindowText(buffer);

			delete ch;
		}
		else
		{
			sprintf(buffer, "%d", playersInfo[iSel].maxHp);
			m_hp.SetWindowText(buffer);
			m_lastSeen.SetWindowText("N/A");
		}
		m_name.SetWindowText(playersInfo[iSel].name);
		Creature_TypeToText(playersInfo[iSel].type, buffer);
		m_type.SetWindowText(buffer);
		Player_VocID2Vocation(playersInfo[iSel].vocId, buffer, false);
		m_voc.SetWindowText(buffer);
		sprintf(buffer, "%d", playersInfo[iSel].level);
		m_level.SetWindowText(buffer);

		m_guildName.SetWindowText(playersInfo[iSel].guildName);
		m_guildRank.SetWindowText(playersInfo[iSel].guildRank);
		m_guildDesc.SetWindowText(playersInfo[iSel].guildDescription);
	}
	else
	{
		m_name.SetWindowText(monstersInfo[iSel].name);
		Creature_TypeToText(monstersInfo[iSel].type, buffer);
		m_type.SetWindowText(buffer);
		sprintf(buffer, "%d", monstersInfo[iSel].maxHp);
		m_hp.SetWindowText(buffer);
		m_voc.SetWindowText(monstersInfo[iSel].description);
		sprintf(buffer, "%d", monstersInfo[iSel].exp);
		m_level.SetWindowText(buffer);
		m_lastSeen.SetWindowText("N/A");

		m_guildName.SetWindowText("N/A");
		m_guildRank.SetWindowText("N/A");
		m_guildDesc.SetWindowText("N/A");
	}
}
