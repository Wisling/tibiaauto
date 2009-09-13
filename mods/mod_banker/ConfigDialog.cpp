// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_banker.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"
#include <fstream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

banker bankersInfo[50];

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
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_CLOSE()
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
}

void CConfigDialog::enableControls() {
	m_OnHand.EnableWindow(true);
	m_MinGold.EnableWindow(true);
	m_Banker.EnableWindow(true);

}

void CConfigDialog::configToControls(CConfigData *configData) {
	char buf[128];
	sprintf(buf, "%d", configData->minimumGoldToBank); m_MinGold.SetWindowText(buf);
	sprintf(buf, "%d", configData->cashOnHand); m_OnHand.SetWindowText(buf);
	reloadBankers();
	m_Banker.SetCurSel(m_Banker.FindString(-1,configData->banker.bankerName));
	if (m_Banker.GetCurSel()==-1) m_Banker.SetCurSel(0);
}

CConfigData * CConfigDialog::controlsToConfig() {	
	char buf[128];
	CConfigData *newConfigData = new CConfigData();
	m_MinGold.GetWindowText(buf,127);newConfigData->minimumGoldToBank=atoi(buf);
	m_OnHand.GetWindowText(buf,127);newConfigData->cashOnHand=atoi(buf);
	int index = m_Banker.GetCurSel();
	strcpy(newConfigData->banker.bankerName, bankersInfo[index].name);
	for (int loopPos = 0; loopPos < 10; loopPos++) {
		newConfigData->banker.position[loopPos].bankerX = bankersInfo[index].xPos[loopPos];
		newConfigData->banker.position[loopPos].bankerY = bankersInfo[index].yPos[loopPos];
		newConfigData->banker.position[loopPos].bankerZ = bankersInfo[index].zPos[loopPos];
		
	}


	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) {
	if (nIDEvent==1001) {
		KillTimer(1001);
		SetTimer(1001,250,NULL);
	}	
	CDialog::OnTimer(nIDEvent);
}

BOOL CConfigDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_enable);

	initalizeBankers();
	SetTimer(1001,250,NULL);	
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
	CTibiaItemProxy itemProxy;
	while (m_Banker.GetCount()>0) m_Banker.DeleteString(0);
	for (int count = 0; count < 50; count++) {
		if (strcmp(bankersInfo[count].name, "End of Bankers"))			
			m_Banker.AddString(bankersInfo[count].name);
		else
			break;
	}
	m_Banker.SetCurSel(0);
}

int initalizeBankers() {
	char installPath[1024];
	unsigned long installPathLen=1023;
	installPath[0]='\0';
	HKEY hkey=NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey)) {
		RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
		RegCloseKey(hkey);
	}
	if (!strlen(installPath)) {
		AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
		exit(1);
	}
	
	char pathBuf[2048];    
	
	sprintf(pathBuf,"%s\\mods\\tibiaauto-bankers.csv",installPath);
	
	ifstream bankerFile (pathBuf, ios::in);
	if (!bankerFile.is_open()) {	AfxMessageBox("File TibiaAuto-Bankers.csv Not found!"); bankerFile.close(); return 0;}
	char buf[128] = {0};
	int bankerNum = 0;
	int posNum = 0;
	
	while (!bankerFile.eof()) {
		bankerFile.getline(bankersInfo[bankerNum].name, 128, ',');
		for (int posNum = 0; posNum < 9; posNum++) {
			bankerFile.getline(buf, 128, ',');
			bankersInfo[bankerNum].xPos[posNum] = atoi(buf);
			bankerFile.getline(buf, 128, ',');
			bankersInfo[bankerNum].yPos[posNum] = atoi(buf);
			bankerFile.getline(buf, 128, ',');
			bankersInfo[bankerNum].zPos[posNum] = atoi(buf);
		}
		bankerFile.getline(buf, 128, ',');
		bankersInfo[bankerNum].xPos[9] = atoi(buf);
		bankerFile.getline(buf, 128, ',');
		bankersInfo[bankerNum].yPos[9] = atoi(buf);
		bankerFile.getline(buf, 128, '\n');
		bankersInfo[bankerNum++].zPos[9] = atoi(buf);
	}
	bankerFile.close();
	
	return 1;
}


