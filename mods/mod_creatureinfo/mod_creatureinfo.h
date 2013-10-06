// mod_creatureinfo.h : main header file for the MOD_CREATUREINFO DLL
//

#if !defined(AFX_MOD_CREATUREINFO_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
#define AFX_MOD_CREATUREINFO_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <IModuleInterface.h>
#include "ConfigData.h"
#include "ConfigDialog.h"

//T4
#define MAX_PLAYERS			5000
#define MAX_MONSTERS		500
#define MAX_LINE_TOKENS		15
#define MAX_LINE_LEN		256

#define VOCID_NONE		0x0000
#define VOCID_KNIGHT	0x0001
#define VOCID_PALLADIN	0x0002
#define VOCID_DRUID		0x0004
#define VOCID_SORCERER	0x0008
#define VOCID_PREMIUM	0x1000

#define TYPE_UNKNOWN	0
#define TYPE_PLAYER		1
#define TYPE_MONSTER	2
#define TYPE_NPC		3
#define TYPE_SELF		4

#define MON_IMMUNE		0
#define MON_STRONG		1
#define MON_NORMAL		2
#define MON_WEAK		5

struct creature{
	char name[40];
	int type;
	int maxHp;
	creature() {
		type=maxHp=0;
		memset(name,0,40);
	}
};

struct monster{
	char name[40];
	int type;
	int maxHp;
	int exp;
	int physical;
	int earth;
	int fire;
	int energy;
	int ice;
	int holy;
	int death;
	int seeInvisible;
	int paralysisImmune;
	char description[MAX_LINE_LEN];
	monster() {
		type=maxHp=exp=seeInvisible=paralysisImmune=0;
		physical=earth=fire=energy=ice=holy=death=MON_NORMAL;
		memset(description,0,MAX_LINE_LEN);
		memset(name,0,40);
	}
};

struct player{
	char name[40];
	int type;
	int maxHp;
	int vocId;
	int level;
	char guildName[32];
	char guildRank[32];
	char guildDescription[32];
	player() {
		type=maxHp=vocId=level=0;
		memset(name,0,40);
		memset(guildName,0,32);
		memset(guildRank,0,32);
		memset(guildDescription,0,32);
	}
};

struct expressionTag{
	char name[40];
	char prefix[40];
	char suffix[40];
	char value[MAX_LINE_LEN];
	expressionTag() {
		memset(name,0,40);
		memset(prefix,0,40);
		memset(suffix,0,40);
		memset(value,0,MAX_LINE_LEN);
	}
};

struct expressionInfo{
	int tagCount;
	char mask[MAX_LINE_LEN];
	expressionTag tag[MAX_LINE_TOKENS];
	expressionInfo() {
		tagCount=0;
		memset(mask,0,MAX_LINE_LEN);
	}
};

extern int playersCount;	//T4: First is reserved for self
extern player playersInfo[MAX_PLAYERS];
extern int monstersCount;
extern monster monstersInfo[MAX_MONSTERS];

void Creature_TypeToText(int,char*);
int Player_VocID2Vocation(int,char*,int);

/////////////////////////////////////////////////////////////////////////////
// CMod_creatureinfoApp
// See mod_creatureinfo.cpp for the implementation of this class
//
 
class CMod_creatureinfoApp : public CWinApp, public IModuleInterface
{
public:
	CMod_creatureinfoApp();
	~CMod_creatureinfoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMod_creatureinfoApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMod_creatureinfoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:		
	void getNewSkin(CSkin);
	int validateConfig(int showAlerts);
	// functions from IModuleInterface
	char * getName();
	int isStarted();
	void start();
	void stop();
	void showConfigDialog();
	void configToControls();
	void controlsToConfig();
	void disableControls();
	void enableControls();		
	char *getVersion();
	void resetConfig();
	void loadConfigParam(char *paramName,char *paramValue);
	char *saveConfigParam(char *paramName);
	char *getConfigParamName(int nr);
private:	
	int m_started;
	CConfigDialog * m_configDialog;
	CConfigData *m_configData;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOD_CREATUREINFO_H__D1220A19_D7FC_4B51_900D_224C2CAC1CB8__INCLUDED_)
