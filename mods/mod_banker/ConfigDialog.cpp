// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_banker.h"
#include "ConfigDialog.h"
#include "ModuleUtil.h"
#include "MemReaderProxy.h"
#include <fstream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

banker bankersInfo[MAX_BANKER_NPCS];
int bankerNum;

extern CToolBankerState globalBankerState;
extern int GUIx,GUIy,GUIz;

int initalizeBankers();
/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_bankerApp *app,CWnd* pParent /*=NULL*/)
	: MyDialog(CConfigDialog::IDD, pParent) {
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app=app;
}


void CConfigDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_FRAME_BANKER, m_BankerFrame);
	DDX_Control(pDX, IDC_BANKER_BANKER, m_Banker);
	DDX_Control(pDX, IDC_BANKER_MIN_GOLD, m_MinGold);
	DDX_Control(pDX, IDC_BANKER_ON_HAND, m_OnHand);
	DDX_Control(pDX, IDC_BANKER_MODPRIORITY,m_modPriority);
	DDX_Control(pDX, IDC_BANKER_CHANGEGOLD,m_changeGold);
	DDX_Control(pDX, IDC_BANKER_CAPSLIMIT,m_capsLimit);
	DDX_Control(pDX, IDC_BANKER_STOPBYBANKER,m_stopByBanker);
	DDX_Control(pDX, IDC_BANKER_STATE,m_stateBanker);
	DDX_Control(pDX, IDC_BANKER_DRAWUPTO,m_drawUpTo);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BANKER_CHANGEGOLD, OnChangeGold)
	ON_BN_CLICKED(IDC_BANKER_STOPBYBANKER,OnStopBy)
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog message handlers

void CConfigDialog::OnOK() {
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnClose() {
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnEnable() {
	if (m_enable.GetCheck()) {
		m_app->controlsToConfig();
		if (m_app->validateConfig(1))
			m_app->start();
		else
			m_enable.SetCheck(0);
	}
	else
		m_app->stop();
}

void CConfigDialog::disableControls() {
	m_MinGold.EnableWindow(false);
	m_OnHand.EnableWindow(false);
	m_Banker.EnableWindow(false);
	m_modPriority.EnableWindow(false);
	m_changeGold.EnableWindow(false);
	m_capsLimit.EnableWindow(false);
	m_stopByBanker.EnableWindow(false);
	m_drawUpTo.EnableWindow(false);
}

void CConfigDialog::enableControls() {
	m_OnHand.EnableWindow(true);
	m_MinGold.EnableWindow(true);
	m_Banker.EnableWindow(true);
	m_modPriority.EnableWindow(true);
	m_changeGold.EnableWindow(true);
	m_capsLimit.EnableWindow(true);
	m_stopByBanker.EnableWindow(true);
	m_drawUpTo.EnableWindow(true);

	OnChangeGold();
	OnStopBy();
}

void CConfigDialog::configToControls(CConfigData *configData) {
	char buf[128];
	sprintf(buf, "%d", configData->minimumGoldToBank); m_MinGold.SetWindowText(buf);
	sprintf(buf, "%d", configData->cashOnHand); m_OnHand.SetWindowText(buf);
	reloadBankers();
	m_Banker.SetCurSel(m_Banker.FindStringExact(-1,configData->banker.bankerName));
	if (m_Banker.GetCurSel()==-1) m_Banker.SetCurSel(0);
	m_modPriority.SetCurSel(atoi(configData->modPriorityStr) - 1);
	m_changeGold.SetCheck(configData->changeGold);
	sprintf(buf, "%d", configData->capsLimit); m_capsLimit.SetWindowText(buf);
	m_stopByBanker.SetCheck(configData->stopByBanker);
	m_drawUpTo.SetCheck(configData->drawUpTo);

}

CConfigData * CConfigDialog::controlsToConfig() {
	char buf[128];
	CConfigData *newConfigData = new CConfigData();
	m_MinGold.GetWindowText(buf,127);newConfigData->minimumGoldToBank=atoi(buf);
	m_OnHand.GetWindowText(buf,127);newConfigData->cashOnHand=atoi(buf);
	int index = m_Banker.GetCurSel();
	m_Banker.GetLBText(index,buf);
	for (index=0;index<bankerNum && strcmp(bankersInfo[index].name,buf);index++){}

	strcpy(newConfigData->banker.bankerName,bankersInfo[index].name);
	newConfigData->banker.bankerX = bankersInfo[index].xPos;
	newConfigData->banker.bankerY = bankersInfo[index].yPos;
	newConfigData->banker.bankerZ = bankersInfo[index].zPos;
	sprintf(newConfigData->modPriorityStr,"%d",m_modPriority.GetCurSel()+1);
	newConfigData->changeGold = m_changeGold.GetCheck();
	m_capsLimit.GetWindowText(buf,127);newConfigData->capsLimit=atoi(buf);
	newConfigData->stopByBanker=m_stopByBanker.GetCheck();
	newConfigData->drawUpTo=m_drawUpTo.GetCheck();

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) {
	if (nIDEvent==1001) {
		KillTimer(1001);
		CMemReaderProxy reader;

		char buf[256];
		switch(globalBankerState){
		case CToolBankerState_notRunning:
			m_stateBanker.SetWindowText("Not running");
			break;
		case CToolBankerState_halfSleep:
			sprintf(buf,"Module sleep by %s:%s",reader.getGlobalVariable("walking_control"),reader.getGlobalVariable("walking_priority"));
			m_stateBanker.SetWindowText(buf);
			break;
		case CToolBankerState_noPathFound:
			sprintf(buf,"Path not found (%d,%d,%d)",GUIx,GUIy,GUIz);
			m_stateBanker.SetWindowText(buf);
			break;
		case CToolBankerState_walking:
			m_stateBanker.SetWindowText("Walking to bank");
			break;
		case CToolBankerState_talking:
			m_stateBanker.SetWindowText("Talking with banker");
			break;
		default:
			m_stateBanker.SetWindowText("Unknown state");
			break;
		}
		SetTimer(1001,250,NULL);
	}
	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin(){
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_enable);
}

BOOL CConfigDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	initalizeBankers();
	SetTimer(1001,250,NULL);
	m_Banker.LimitText(127);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg) {
	return CDialog::PreTranslateMessage(pMsg);
}

void CConfigDialog::activateEnableButton(int enable) {
	m_enable.SetCheck(enable);
}

void CConfigDialog::reloadBankers() {
	while (m_Banker.GetCount()>0) m_Banker.DeleteString(0);
	for (int count = 0; count < bankerNum; count++) {
		m_Banker.AddString(bankersInfo[count].name);
	}
	m_Banker.SetCurSel(0);
}

void CConfigDialog::OnChangeGold(){
	int val=m_changeGold.GetCheck();
	m_MinGold.EnableWindow(!val);
	m_OnHand.EnableWindow(!val);
}

void CConfigDialog::OnStopBy(){
	int val=m_stopByBanker.GetCheck();
	m_drawUpTo.EnableWindow(val);
}

int initalizeBankers() {
	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);
	
	char pathBuf[2048];
	
	sprintf(pathBuf,"%s\\mods\\tibiaauto-bankers.csv",installPath);
	
	ifstream bankerFile (pathBuf, ios::in);
	if (!bankerFile.is_open()) {	AfxMessageBox("File TibiaAuto-Bankers.csv Not found!"); bankerFile.close(); return 0;}
	char buf[129] = {0};

	bankerNum = 0;
	while (!bankerFile.eof() && bankerNum<MAX_BANKER_NPCS) {
		bankerFile.getline(buf, 128, ',');
		if (strlen(buf)==0) break;
		strcpy(bankersInfo[bankerNum].name,buf);
		bankerFile.getline(buf, 128, ',');
		bankersInfo[bankerNum].xPos = atoi(buf);
		bankerFile.getline(buf, 128, ',');
		bankersInfo[bankerNum].yPos = atoi(buf);
		bankerFile.getline(buf, 128, '\n');
		bankersInfo[bankerNum++].zPos = atoi(buf);
	}
	bankerFile.close();
	
	return 1;
}


