// Alarm.h: interface for the Alarm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARM_H__514125AB_2BAC_490F_9367_E72C0A92DAF0__INCLUDED_)
#define AFX_ALARM_H__514125AB_2BAC_490F_9367_E72C0A92DAF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TibiaMapPoint.h"
#include "Trigger.h"
#include "Enumerations.h"
#include <list>


using namespace std;

class Alarm
{
public:
	CTrigger getTrigger();
	int getSpellDelay();
	void setSpellDelay(int);
	struct point currentLoc;
	time_t movedTime;
	void setDescriptor(CString);
	CString getDescriptor();
	int screenshotsTaken;
	time_t spellCast;
	void setManaCost(int);
	int getManaCost();
	int getCondition();
	int getAttribute();
	int getAlarmType();
	// Setters
	void setTrigger(int, CString);
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
	void setPersistent(bool);
	void setPermanent(bool);

	// Accessors
	list<CString> getStopModules();
	list<CString> getStartModules();
	bool getLogEvents();
	CString getAlarm();
	int getWindowAction();
	bool getShutdownComputer();
	bool getKillClient();
	bool getLogout();
	CString getCastSpell();
	bool getGoToDepot();
	bool getGoToStart();
	bool getGoToRunaway();
	int getTakeScreenshot();
	bool getStopWalking();
	bool getPersistent();
	bool getPermanent();

	// Methods
	void initializeCharacter();
	bool checkAlarm(char whiteList[100][32], int, struct tibiaMessage*);
	bool keepPersistent(bool isDestinationReached, bool isLoggedOut);

	// Construction/Destruction
	Alarm(int type, int attr, int cond, int trigType, CString strTrig, bool run, bool sta, bool dep, CString spe, int cost, int delay, int scr, bool stopwalk, bool log, bool kill, bool shut, int winAct, CString audio, bool event, list<CString> beginModules, list<CString> endModules, bool pers, bool perm, CString descript);
	Alarm();
	virtual ~Alarm();

private:
	int spellDelay;
	CString alarmDescriptor;
	int manaCost;
	// Trigger Functions
	bool vipOnline(int, bool=true);
	bool vipNameOnline(CString,bool=true);
	int onScreenCheck(char whiteList[100][32], int);
	
	// Helper Functions
	int spaceAvailable();
	bool isSpellMessage(char *);
	bool OnList(char whiteList[100][32], char name[]);
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
	CTrigger trigger;
	int condition;
	int attribute;
	int alarmType;
	bool stopWalking;
	bool persistent;
	bool permanent;



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
	time_t onScreenAt;

public:
	//Alarm variables
	int alarmState;
	int runCycle;

};

#endif // !defined(AFX_ALARM_H__514125AB_2BAC_490F_9367_E72C0A92DAF0__INCLUDED_)
