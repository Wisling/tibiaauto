// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_addressfinder.h"
#include "ConfigDialog.h"
#include "ModuleUtil.h"
#include <MemReader.h>
#include <MemUtil.h>
#include <TibiaItem.h>
#include <fstream>
#include <List>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_addressfinderApp *app, CWnd* pParent /*=NULL*/)
	: MyDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	m_outputStartByte = 0;
	m_outputEndByte   = 0;
	//}}AFX_DATA_INIT
	m_app = app;
}

void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_STAMINA_ADDRESS, m_staminaAddress);
	DDX_Control(pDX, IDC_STAMINA, m_stamina);
	DDX_Control(pDX, IDC_CAPACITY_ADDRESS, m_capacityAddress);
	DDX_Control(pDX, IDC_CAPACITY, m_capacity);
	DDX_Control(pDX, IDC_BATTLE_LIST_MAX, m_battleListMax);
	DDX_Control(pDX, IDC_TILES_LEFT_ADDRESS, m_tilesLeftAddress);
	DDX_Control(pDX, IDC_TILES_LEFT, m_tilesLeft);
	DDX_Control(pDX, IDC_SOUL_POINTS_ADDRESS, m_soulPointsAddress);
	DDX_Control(pDX, IDC_SOUL_POINTS, m_soulPoints);
	DDX_Control(pDX, IDC_SELF_ID, m_selfID);
	DDX_Control(pDX, IDC_SELF_ID_ADDRESS, m_selfIDAddress);
	DDX_Control(pDX, IDC_MANA_MAX_ADDRESS, m_manaMaxAddress);
	DDX_Control(pDX, IDC_MANA_MAX, m_manaMax);
	DDX_Control(pDX, IDC_MANA_ADDRESS, m_manaAddress);
	DDX_Control(pDX, IDC_MANA, m_mana);
	DDX_Control(pDX, IDC_MAGIC_LEVEL_PERCENT_LEFT, m_magicLevelPercentLeft);
	DDX_Control(pDX, IDC_MAGIC_LEVEL_PERCENT_LEFT_ADDRESS, m_magicLevelPrecentLeftAddress);
	DDX_Control(pDX, IDC_MAGIC_LEVEL_ADDRESS, m_magicLevelAddress);
	DDX_Control(pDX, IDC_MAGIC_LEVEL, m_magicLevel);
	DDX_Control(pDX, IDC_LEVEL_ADDRESS, m_levelAddress);
	DDX_Control(pDX, IDC_LEVEL, m_level);
	DDX_Control(pDX, IDC_HP_MAX_ADDRESS, m_HPMaxAddress);
	DDX_Control(pDX, IDC_HP_ADDRESS, m_HPAddress);
	DDX_Control(pDX, IDC_HP, m_HP);
	DDX_Control(pDX, IDC_HP_MAX, m_HPMax);
	DDX_Control(pDX, IDC_GO_Z_ADDRESS, m_goZAddress);
	DDX_Control(pDX, IDC_GO_Z, m_goZ);
	DDX_Control(pDX, IDC_GO_Y_ADDRESS, m_goYAddress);
	DDX_Control(pDX, IDC_GO_Y, m_goY);
	DDX_Control(pDX, IDC_GO_X, m_goX);
	DDX_Control(pDX, IDC_GO_X_ADDRESS, m_goXAddress);
	DDX_Control(pDX, IDC_GO_TO_TILE_ADDRESS, m_goToTileAddress);
	DDX_Control(pDX, IDC_GO_TO_TILE, m_goToTile);
	DDX_Control(pDX, IDC_FLAGS_ADDRESS, m_flagsAddress);
	DDX_Control(pDX, IDC_FLAGS, m_flags);
	DDX_Control(pDX, IDC_FIST_SKILL_ADDRESS, m_fistSkillAddress);
	DDX_Control(pDX, IDC_FIST_SKILL, m_fistSkill);
	DDX_Control(pDX, IDC_FIST_PERCENT_LEFT_ADDRESS, m_fistPercentLeftAddress);
	DDX_Control(pDX, IDC_FIST_PERCENT_LEFT, m_fistPercentLeft);
	DDX_Control(pDX, IDC_FISH_SKILL_ADDRESS, m_fishSkillAddress);
	DDX_Control(pDX, IDC_FISH_SKILL, m_fishSkill);
	DDX_Control(pDX, IDC_FIRST_CREATURE_ADDRESS, m_firstCreatureAddress);
	DDX_Control(pDX, IDC_EXPERIENCE_ADDRESS, m_experienceAddress);
	DDX_Control(pDX, IDC_FIRST_CREATURE, m_firstCreature);
	DDX_Control(pDX, IDC_EXPERIENCE, m_experience);
	DDX_Control(pDX, IDC_BATTLE_LIST_MIN_ADDRESS, m_battleListMinAddress);
	DDX_Control(pDX, IDC_BATTLE_LIST_MIN, m_battleListMin);
	DDX_Control(pDX, IDC_BATTLE_LIST_MAX_ADDRESS, m_battleListMaxAddress);
	DDX_Control(pDX, IDC_ATTACKED_CREATURE_ADDRESS, m_attackedCreatureAddress);
	DDX_Control(pDX, IDC_ATTACKED_CREATURE, m_attackedCreature);
	DDX_Control(pDX, IDC_BEGIN_OUTPUT, m_beginOutput);
	DDX_Control(pDX, IDC_FRAME_2, m_addressOutput);
	DDX_Control(pDX, IDC_FRAME_ADDRESSFINDER, m_AddressFinderFrame);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDC_SEARCHSTRING, m_searchString);
	DDX_Text(pDX, IDC_START_BYTE, m_outputStartByte);
	DDX_Text(pDX, IDC_END_BYTE, m_outputEndByte);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
//{{AFX_MSG_MAP(CConfigDialog)
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_ENABLE, OnEnable)
ON_WM_TIMER()
ON_WM_ERASEBKGND()
ON_WM_CTLCOLOR()
ON_BN_CLICKED(IDC_BEGIN_OUTPUT, OnBeginOutput)
ON_BN_CLICKED(IDC_SEARCHSTRING, OnCreateSearchString)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog message handlers

void CConfigDialog::OnOK()
{
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnClose()
{
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnEnable()
{
	if (m_enable.GetCheck())
	{
		m_app->controlsToConfig();
		if (m_app->validateConfig(1))
			m_app->start();
		else
			m_enable.SetCheck(0);
	}
	else
	{
		m_app->stop();
	}
}

void CConfigDialog::disableControls()
{
	m_experience.EnableWindow(false);
	m_experienceAddress.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_experience.EnableWindow(true);
	m_experienceAddress.EnableWindow(true);
}

void CConfigDialog::configToControls(CConfigData *configData)
{
	char buf[128];
	sprintf(buf, "%d", configData->experience);
	m_experience.SetWindowText(buf);
	sprintf(buf, "0x%x", configData->experienceAddress);
	m_experienceAddress.SetWindowText(buf);
}

CConfigData * CConfigDialog::controlsToConfig()
{
	char buf[128];
	CConfigData *newConfigData = new CConfigData();
	m_experience.GetWindowText(buf, 127);
	newConfigData->experience = atoi(buf);
	m_experienceAddress.GetWindowText(buf, 127);
	newConfigData->experienceAddress = atoi(buf);

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	CMemReader& reader = CMemReader::getMemReader();
	KillTimer(nIDEvent);

	if (nIDEvent == 1001)
	{
		char buf[32];
		sprintf(buf, "0x%X", m_addressFinder.addresses.expAddress);
		m_experienceAddress.SetWindowText(buf);
		sprintf(buf, "%d", CMemUtil::GetMemIntValue(m_addressFinder.addresses.expAddress));
		m_experience.SetWindowText(buf);
		sprintf(buf, "0x%X", m_addressFinder.addresses.flagsAddress);
		m_flagsAddress.SetWindowText(buf);
		sprintf(buf, "%d", CMemUtil::GetMemIntValue(m_addressFinder.addresses.flagsAddress));
		m_flags.SetWindowText(buf);
		sprintf(buf, "0x%X", m_addressFinder.addresses.fistPercentLeftAddress);
		m_fistPercentLeftAddress.SetWindowText(buf);
		sprintf(buf, "%d", CMemUtil::GetMemIntValue(m_addressFinder.addresses.fistPercentLeftAddress));
		m_fistPercentLeft.SetWindowText(buf);
		sprintf(buf, "0x%X", m_addressFinder.addresses.fistSkillAddress);
		m_fistSkillAddress.SetWindowText(buf);
		sprintf(buf, "%d", CMemUtil::GetMemIntValue(m_addressFinder.addresses.fistSkillAddress));
		m_fistSkill.SetWindowText(buf);
		sprintf(buf, "%d", CMemUtil::GetMemIntValue(m_addressFinder.addresses.fishSkillAddress));
		m_fishSkill.SetWindowText(buf);
		sprintf(buf, "%d", CMemUtil::GetMemIntValue(m_addressFinder.addresses.fishSkillAddress));
		m_fishSkill.SetWindowText(buf);

		SetTimer(nIDEvent, 500, NULL);
	}
	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin()
{
	skin.SetButtonSkin(m_OK);
	skin.SetButtonSkin(m_enable);
	skin.SetButtonSkin(m_beginOutput);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();
	CMemReader& reader = CMemReader::getMemReader();

	m_addressFinder.addresses.flagsAddress                 = CMemUtil::GetMemIntValue(m_addressFinder.getFlagsAddress());
	m_addressFinder.addresses.fistPercentLeftAddress       = m_addressFinder.addresses.flagsAddress + 4;
	m_addressFinder.addresses.fistSkillAddress             = m_addressFinder.addresses.flagsAddress + 32;
	m_addressFinder.addresses.fishSkillAddress             = m_addressFinder.addresses.flagsAddress + 56;
	m_addressFinder.addresses.attackedCreatureAddress      = m_addressFinder.addresses.flagsAddress + 68;
	m_addressFinder.addresses.capacityAddress              = m_addressFinder.addresses.flagsAddress + 72;
	m_addressFinder.addresses.staminaAddress               = m_addressFinder.addresses.flagsAddress + 76;
	m_addressFinder.addresses.soulPointsAddress            = m_addressFinder.addresses.flagsAddress + 80;
	m_addressFinder.addresses.manaMaxAddress               = m_addressFinder.addresses.flagsAddress + 84;
	m_addressFinder.addresses.manaAddress                  = m_addressFinder.addresses.flagsAddress + 88;
	m_addressFinder.addresses.magicLevelPercentLeftAddress = m_addressFinder.addresses.flagsAddress + 92;
	m_addressFinder.addresses.magicLevelAddress            = m_addressFinder.addresses.flagsAddress + 100;
	m_addressFinder.addresses.levelAddress                 = m_addressFinder.addresses.flagsAddress + 104;
	m_addressFinder.addresses.expAddress                   = m_addressFinder.addresses.flagsAddress + 108;
	m_addressFinder.addresses.HPMaxAddress                 = m_addressFinder.addresses.flagsAddress + 112;
	m_addressFinder.addresses.HPAddress                    = m_addressFinder.addresses.flagsAddress + 116;
	m_addressFinder.addresses.selfIDAddress                = m_addressFinder.addresses.flagsAddress + 120;
	m_addressFinder.addresses.goToTileAddress              = m_addressFinder.addresses.flagsAddress + 128;
	m_addressFinder.addresses.tilesToGoAddress             = m_addressFinder.addresses.flagsAddress + 132;
	m_addressFinder.addresses.goZAddress                   = m_addressFinder.addresses.flagsAddress + 180;
	m_addressFinder.addresses.goYAddress                   = m_addressFinder.addresses.flagsAddress + 184;
	m_addressFinder.addresses.goXAddress                   = m_addressFinder.addresses.flagsAddress + 188;
	m_addressFinder.addresses.battleListMaxAddress         = m_addressFinder.addresses.flagsAddress + 208;
	m_addressFinder.addresses.battleListMinAddress         = m_addressFinder.addresses.flagsAddress + 212;
	m_addressFinder.addresses.firstCreatureAddress         = m_addressFinder.addresses.flagsAddress + 216;

	SetTimer(1001, 250, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CConfigDialog::activateEnableButton(int enable)
{
	m_enable.SetCheck(enable);
}

void CConfigDialog::OnBeginOutput()
{
	CMemReader& reader = CMemReader::getMemReader();
	char buf[128];
	UpdateData();
	FILE *outputFile = fopen("C:\\temp\\AddressFinderOutput.txt", "wb");
	fprintf(outputFile, "Start\r\n");
	fflush(outputFile);
	for (int loop = m_outputStartByte; loop < m_outputEndByte; loop++)
	{
		byte foundByte = (byte)CMemUtil::GetMemIntValue(loop);
		sprintf(buf, "\tComparison.push_back(0x%x);\r\n\tMask.push_back(1);\r\n", foundByte);
		fprintf(outputFile, buf);
		fflush(outputFile);
	}
	fprintf(outputFile, "End\r\n");
	fflush(outputFile);
	fclose(outputFile);
	AfxMessageBox("Done");
}

void CConfigDialog::OnCreateSearchString()
{
	char* filepath = "E:\\Tibia\\Tibia\\Previous Versions\\collection\\";
	//8.31	8.41	8.42	8.50	8.52
	const int NUM_ADDS                = 32;
	const int NUM_VERS                = 7;
	int addresses[NUM_ADDS][NUM_VERS] =
	{{0x4AE010, 0x4AE080, 0x4AE900, 0x4AED50, 0x4AF480, 0x4AF510, 0x4AF7B0},
	 {0x5432A0, 0x543360, 0x543C80, 0x5439A0, 0x5446F0, 0x5450C0, 0x545730},
	 {0x45E120, 0x45E190, 0x45EA00, 0x45EE80, 0x45F590, 0x45F640, 0x45F900},
	 {0x5497E0, 0x549950, 0x54A270, 0x54D9B0, 0x54E7A0, 0x54F180, 0x54FD30},
	 {0x549910, 0x549A80, 0x54A3A0, 0x54DAE0, 0x54E8D0, 0x54F2B0, 0x54FE60},
	 {0x546770, 0x5468C0, 0x5471E0, 0x546F20, 0x547C90, 0x548670, 0x548CE0},
	 {0x4AE821, 0x4AE891, 0x4AF111, 0x4AF561, 0x4AFC91, 0x4AFD21, 0x4AFFC1},
	 {0x4AEA1C, 0x4AEA8C, 0x4AF30C, 0x4AF75C, 0x4AFE8C, 0x4AFF1C, 0x4B01BC},
	 {0x4AEC12, 0x4AEC82, 0x4AF502, 0x4AF952, 0x4B0082, 0x4B0112, 0x4B03B2},
	 {0x4AEE12, 0x4AEE82, 0x4AF702, 0x4AFB52, 0x4B0282, 0x4B0312, 0x4B05B2},
	 {0x4AF00C, 0x4AF07C, 0x4AF8FC, 0x4AFD4C, 0x4B047C, 0x4B050C, 0x4B07AC},
	 {0x4AF201, 0x4AF271, 0x4AFAF1, 0x4AFF41, 0x4B0671, 0x4B0701, 0x4B09A1},
	 {0x413C63, 0x413C43, 0x413FD3, 0x413543, 0x413503, 0x41359F, 0x41359F},
	 {0x413518, 0x4134F8, 0x413888, 0x412DF8, 0x412DB8, 0x412E48, 0x412E48},
	 {0x4136E3, 0x4136C3, 0x413A53, 0x412FC3, 0x412F83, 0x413013, 0x413013},
	 {0x413A79, 0x413A59, 0x413DE9, 0x413189, 0x413319, 0x4133A9, 0x4133A9},
	 {0x427307, 0x4272D7, 0x4276B7, 0x4277B7, 0x427DD7, 0x427E57, 0x427E57},
	 {0x42A748, 0x42A738, 0x42AB18, 0x42AC18, 0x42B238, 0x42B2B8, 0x42B2B8},
	 {0x42A75F, 0x42A74F, 0x42AB2F, 0x42AC2F, 0x42B24F, 0x42B2CF, 0x42B2CF},
	 {0x44EC38, 0x44EC98, 0x44F1E8, 0x44F2C8, 0x44F978, 0x44FA18, 0x44FB68},
	 {0x49402E, 0x49407E, 0x49491E, 0x494D6E, 0x4954A0, 0x4954F0, 0x495774},
	 {0x4D5190, 0x4D5200, 0x4D5A90, 0x4D5EE0, 0x4D6632, 0x4D66C2, 0x4D6962},
	 {0x4D595D, 0x4D59CD, 0x4D625D, 0x4D66AD, 0x4D6E00, 0x4D6E90, 0x4D7130},
	 {0x4D59C3, 0x4D5A33, 0x4D62C3, 0x4D6713, 0x4D6E69, 0x4D6EF9, 0x4D7199},
	 {0x546EF8, 0x547055, 0x547975, 0x5476B5, 0x5484A2, 0x548E82, 0x5494F2},
	 {0x546F2B, 0x547088, 0x5479A8, 0x5476E8, 0x5484D5, 0x548EB5, 0x549525},
	 {0x547042, 0x54719F, 0x547ABF, 0x5477FF, 0x5485EC, 0x548FCC, 0x54963C},
	 {0x547270, 0x5473D0, 0x547CF0, 0x547A30, 0x548820, 0x549200, 0x549870},
	 {0x547976, 0x547AE6, 0x548406, 0x548146, 0x548F36, 0x549916, 0x549F86},
	 {0x549A7D, 0x549BED, 0x54A50D, 0x54DC4D, 0x54EA3D, 0x54F41D, 0x54FFCD},
	 {0x549A9D, 0x549C0D, 0x54A52D, 0x54DC6D, 0x54DC6D, 0x54F43D, 0x54FFED},
	 {0x4EC0EC, 0x4EC15C, 0x4EC9EC, 0x4ECA6C, 0x4ED21C, 0x4ED2AC, 0x4ED70C}};
	char allStrings[NUM_ADDS][NUM_VERS][2001];
	char filemask[1024];
	sprintf(filemask, "%s*.exe", filepath);
	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(filemask, &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		char fname[128];
		int lastfile = 1;
		int ver      = 0;
		while (lastfile)
		{
			sprintf(fname, "%s%s", filepath, data.cFileName);
			FILE* f = fopen(fname, "rb");
			if (f)
			{
				//AfxMessageBox(fname);
				for (int addnum = 0; addnum < NUM_ADDS; addnum++)
				{
					fseek(f, addresses[addnum][ver] - 1000 - 0x400000, SEEK_SET);
					fread(allStrings[addnum][ver], 2001, 1, f);
				}
				fclose(f);
			}
			lastfile = FindNextFile(hFind, &data);
			ver++;
		}
	}
	FILE* f = fopen("C:/addesses.txt", "w");
	for (int i = 0; i < NUM_ADDS; i++)
	{
		for (int j = 0; j < NUM_VERS; j++)
		{
			for (int k = 0; k < 2001; k++)
			{
				fprintf(f, "%x ", (unsigned char) (allStrings[i][j][k]));
			}
			fprintf(f, "\n");
		}
		fprintf(f, "\n\n\n");
	}
}
