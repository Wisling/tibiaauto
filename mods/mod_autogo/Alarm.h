// Alarm.h: interface for the Alarm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARM_H__514125AB_2BAC_490F_9367_E72C0A92DAF0__INCLUDED_)
#define AFX_ALARM_H__514125AB_2BAC_490F_9367_E72C0A92DAF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TibiaMapPoint.h"
#include "Enumerations.h"
#include <list>

using namespace std;

class Alarm  
{
public:
	int getSpellDelay();
	void setSpellDelay(int);
	struct point currentLoc;
	int movedTime;
	bool halfSleep;
	bool stopWalk;
	bool flashed;
	int windowActed;
	bool eventLogged;
	void setDescriptor(CString);
	CString getDescriptor();
	bool modulesSuspended;
	bool modulesStarted;
	int timeLastSS;
	int screenshotsTaken;
	int spellCast;
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
	void setWindowAction(int);
	void setShutdownComputer(bool);
	void setKillClient(bool);
	void setLogout(bool);
	void setTakeScreenshot(int);
	void setCastSpell(CString);
	void setGoToDepot(bool);
	void setGoToStart(bool);
	void setGoToRunaway(bool);
	void setStopWalking(bool);

	// Actions/Accessors
	list<CString> doStopModules();
	list<CString> doStartModules();
	bool doLogEvents();
	CString doAlarm();
	int doWindowAction();
	bool doShutdownComputer();
	bool doKillClient();
	bool doLogout();
	CString doCastSpell();
	bool doGoToDepot();
	bool doGoToStart();
	bool doGoToRunaway();
	int doTakeScreenshot();
	bool doStopWalking();

	// Methods
	void initializeCharacter();
	bool checkAlarm(char whiteList[100][32], int);

	// Construction/Destruction
	Alarm(int, int, int, int, CString, bool, bool, bool, CString, int, int, int, bool, bool, bool, bool, int, CString, bool, list<CString>, list<CString>);
	Alarm();
	virtual ~Alarm();

private:
	int spellDelay;
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
	int windowAction;
	bool shutdown;
	bool killTibia;
	bool logout;
	bool depot;
	bool start;
	bool runaway;
	CString strTrigger;
	int intTrigger;
	int condition;
	int attribute;
	int alarmType;
	bool stopWalking;

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
	int healthInit;

	// Specific Needs Variables
	int onScreenAt;
};

#endif // !defined(AFX_ALARM_H__514125AB_2BAC_490F_9367_E72C0A92DAF0__INCLUDED_)
