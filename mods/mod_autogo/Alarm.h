// Alarm.h: interface for the Alarm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARM_H__514125AB_2BAC_490F_9367_E72C0A92DAF0__INCLUDED_)
#define AFX_ALARM_H__514125AB_2BAC_490F_9367_E72C0A92DAF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Enumerations.h"
#include <list>

using namespace std;

class Alarm  
{
public:
	bool halfSleep;
	bool fullSleep;
	bool cavebotForced;
	bool flashed;
	bool maximized;
	bool eventLogged;
	void setDescriptor(CString);
	CString getDescriptor();
	bool modulesSuspended;
	bool modulesStarted;
	int timeLastSS;
	int screenshotsTaken;
	bool spellCast;
	void setManaCost(int);
	int getManaCost();
	int getIntTrigger();
	CString getStrTrigger();
	int getCondition();
	int getAttribute();
	int getAlarmType();
	// Setters
	void setTrigger(CString);
	void setTrigger(int);
	void setCondition(int);
	void setAttribute(int);
	void setType(int);
	void setStartModules(list<CString>);
	void setStopModules(list<CString>);
	void setLogEvents(bool);
	void setAudioAlarm(CString);
	void setMaximizeTibia(bool);
	void setShutdownComputer(bool);
	void setKillClient(bool);
	void setLogout(bool);
	void setAttack(bool);
	void setTakeScreenshot(int);
	void setCastSpell(CString);
	void setGoToDepot(bool);
	void setGoToStart(bool);
	void setGoToRunaway(bool);

	// Actions/Accessors
	list<CString> doStopModules();
	list<CString> doStartModules();
	bool doLogEvents();
	CString doAlarm();
	bool doMaximizeClient();
	bool doShutdownComputer();
	bool doKillClient();
	bool doLogout();
	bool doAttack();
	CString doCastSpell();
	bool doGoToDepot();
	bool doGoToStart();
	bool doGoToRunaway();
	int doTakeScreenshot();

	// Methods
	void initializeCharacter();
	bool checkAlarm(char whiteList[100][32], int);

	// Construction/Destruction
	Alarm(int, int, int, int, bool, bool, bool, CString, int, bool, bool, bool, bool, bool, CString, bool, list<CString>, list<CString>);
	Alarm();
	virtual ~Alarm();

private:
	CString alarmDescriptor;
	int manaCost;
	// Trigger Functions
	bool vipOnline(int);
	bool battleListActive();
	bool monsterOnScreen(char whiteList[100][32], int options);
	bool skullOnScreen(int);
	bool gmOnScreen(char whiteList[100][32],int);
	bool playerOnScreen(char whiteList[100][32], int);
	
	// Helper Functions
	int spaceAvailable();
	bool isSpellMessage(char *);
	bool OnList(char whiteList[100][32], char name[]);
	struct tibiaMessage * Alarm::triggerMessage();
	int countAllFood();
	int countAllItemsOfType(int);

	// Alarm Parameters
	list <CString> startModules;
	list <CString> stopModules;
	int screenshot;
	CString spell;
	CString sound;
	bool logEvents;
	bool maximize;
	bool shutdown;
	bool killTibia;
	bool logout;
	bool attack;
	bool depot;
	bool start;
	bool runaway;
	CString strTrigger;
	int intTrigger;
	int condition;
	int attribute;
	int alarmType;

	// Initial Character State
	int lvlInit;
	int mlvlInit;
	int skillFishInit;
	int skillFistInit;
	int skillClubInit;
	int skillSwordInit;
	int skillAxeInit;
	int skillDistInit;
	int skillShieldInit;
	int positionXInit;
	int positionYInit;
	int positionZInit;

	// Specific Needs Variables
	int onScreenAt;
};

#endif // !defined(AFX_ALARM_H__514125AB_2BAC_490F_9367_E72C0A92DAF0__INCLUDED_)
