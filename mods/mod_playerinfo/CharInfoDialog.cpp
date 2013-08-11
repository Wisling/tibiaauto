// CharInfoDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_playerinfo.h"
#include "CharInfoDialog.h"
#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaCharacter.h"
#include "IPCBackPipeProxy.h"
#include <math.h>

#include "playerInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WARNING1	20
#define WARNING2	5



/////////////////////////////////////////////////////////////////////////////
// CCharInfoDialog dialog


CCharInfoDialog::CCharInfoDialog(CConfigData *configIn, CWnd* pParent /*=NULL*/)
	: MyDialog(CCharInfoDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCharInfoDialog)
	//}}AFX_DATA_INIT
	config = configIn;
}


void CCharInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharInfoDialog)
	DDX_Control(pDX, IDC_MAGIC_SHIELD_TIMER_TEXT, m_magicShieldText);
	DDX_Control(pDX, IDC_STRONG_HASTE_TIMER_TEXT, m_strongHasteText);
	DDX_Control(pDX, IDC_INVIS_TIMER_TEXT, m_invisText);
	DDX_Control(pDX, IDC_HASTE_TIMER_TEXT, m_hasteText);
	DDX_Control(pDX, IDC_ENABLETIMERS, m_EnableTimer);
	DDX_Control(pDX, IDC_RESET_COUNTERS, m_ResetCounters);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_SPELL_STATS, m_SpellStats);
	DDX_Control(pDX, IDC_CHARACTER_STATS, m_CharStats);
	DDX_Control(pDX, IDC_CHARINFO_EXP_UPIN, m_expUpin);
	DDX_Control(pDX, IDC_CHARINFO_LVL_UPIN, m_lvlUpin);
	DDX_Control(pDX, IDC_CHARINFO_LVL_SPEED, m_lvlSpeed);
	DDX_Control(pDX, IDC_CHARINFO_MLVL_UPIN, m_mLvlUpin);
	DDX_Control(pDX, IDC_CHARINFO_MLVL_SPEED, m_mLvlSpeed);
	DDX_Control(pDX, IDC_CHARINFO_MLVL, m_mLvl);
	DDX_Control(pDX, IDC_CHARINFO_FIST_UPIN, m_fistUpin);
	DDX_Control(pDX, IDC_CHARINFO_FIST_SPEED, m_fistSpeed);
	DDX_Control(pDX, IDC_CHARINFO_FIST, m_fist);
	DDX_Control(pDX, IDC_TIME, m_time);
	DDX_Control(pDX, IDC_CHARINFO_SWORD_UPIN, m_swordUpin);
	DDX_Control(pDX, IDC_CHARINFO_SHIELD_UPIN, m_shieldUpin);
	DDX_Control(pDX, IDC_CHARINFO_FISH_UPIN, m_fishUpin);
	DDX_Control(pDX, IDC_CHARINFO_FISH_SPEED, m_fishSpeed);
	DDX_Control(pDX, IDC_CHARINFO_FISH, m_fish);
	DDX_Control(pDX, IDC_CHARINFO_DIST_UPIN, m_distUpin);
	DDX_Control(pDX, IDC_CHARINFO_DIST_SPEED, m_distSpeed);
	DDX_Control(pDX, IDC_CHARINFO_AXE_UPIN, m_axeUpin);
	DDX_Control(pDX, IDC_CHARINFO_CLUB_UPIN, m_clubUpin);
	DDX_Control(pDX, IDC_CHARINFO_DIST, m_dist);
	DDX_Control(pDX, IDC_CHARINFO_SHIELD, m_shield);
	DDX_Control(pDX, IDC_CHARINFO_SWORD, m_sword);
	DDX_Control(pDX, IDC_CHARINFO_SWORD_SPEED, m_swordSpeed);
	DDX_Control(pDX, IDC_CHARINFO_SHIELD_SPEED, m_shieldSpeed);
	DDX_Control(pDX, IDC_CHARINFO_EXP_SPEED, m_expSpeed);
	DDX_Control(pDX, IDC_CHARINFO_CLUB_SPEED, m_clubSpeed);
	DDX_Control(pDX, IDC_CHARINFO_AXE_SPEED, m_axeSpeed);
	DDX_Control(pDX, IDC_CHARINFO_AXE, m_axe);
	DDX_Control(pDX, IDC_CHARINFO_CLUB, m_club);
	DDX_Control(pDX, IDC_CHARINFO_EXP, m_exp);
	DDX_Control(pDX, IDC_CHARINFO_LVL, m_lvl);
	DDX_Control(pDX, IDC_CHARINFO_INVIS_REMAINING, m_invisRemaining);
	DDX_Control(pDX, IDC_CHARINFO_HASTE_REMAINING, m_hasteRemaining);
	DDX_Control(pDX, IDC_CHARINFO_STRONGHASTE_REMAINING, m_stronghasteRemaining);
	DDX_Control(pDX, IDC_CHARINFO_MAGICSHIELD_REMAINING, m_magicshieldRemaining);
	DDX_Control(pDX, IDC_CHARINFO_MANA, m_mana);
	DDX_Control(pDX, IDC_CHARINFO_HP, m_hp);
	DDX_Control(pDX, IDC_CHARINFO_SP, m_sp);
	DDX_Control(pDX, IDC_CHARINFO_CAP, m_cap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCharInfoDialog, MyDialog)
	//{{AFX_MSG_MAP(CCharInfoDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RESET_COUNTERS, OnResetCounters)
	ON_BN_CLICKED(IDC_ENABLETIMERS, OnEnabletimers)
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharInfoDialog message handlers

void CCharInfoDialog::resetCounters()
{
	CMemReaderProxy reader;	
	CTibiaCharacter *ch = reader.readSelfCharacter();

	playerInfo.timeStart	= time(NULL);

	long lvl;
	long expForLvl=0;	
	long expLeft=0;
	long percForLvl=0;
	long expForLvlDiff=0;

	//=((1+A1)*(2+A1)/2-4*A1+3)*100
	for (lvl=1;lvl<250;lvl++){
		expForLvlDiff	=  ((2+lvl)*(3+lvl)/2-4*(lvl+1)+3)*100;
		expForLvl		+= expForLvlDiff;

		if (expForLvl>ch->exp){
			expLeft		= expForLvl-ch->exp;
			percForLvl	= MulDiv(expLeft,100,expForLvlDiff);
			break;
		}
	}

	playerInfo.expStart	= ch->exp;
	playerInfo.skill[SKILL_LEVEL].start		= ch->lvl*100+(100-percForLvl);
	playerInfo.skill[SKILL_MAGICLEVEL].start= ch->mlvl*100+(100-ch->mlvlPercLeft);
	playerInfo.skill[SKILL_FIST].start		= ch->skillFist*100+(100-ch->skillFistPercLeft);
	playerInfo.skill[SKILL_CLUB].start		= ch->skillClub*100+(100-ch->skillClubPercLeft);
	playerInfo.skill[SKILL_SWORD].start		= ch->skillSword*100+(100-ch->skillSwordPercLeft);
	playerInfo.skill[SKILL_AXE].start		= ch->skillAxe*100+(100-ch->skillAxePercLeft);
	playerInfo.skill[SKILL_DIST].start		= ch->skillDist*100+(100-ch->skillDistPercLeft);
	playerInfo.skill[SKILL_SHIELD].start	= ch->skillShield*100+(100-ch->skillShieldPercLeft);
	playerInfo.skill[SKILL_FISH].start		= ch->skillFish*100+(100-ch->skillFishPercLeft);

	playerInfo.spell[SPELL_INVISIBLE].lasts = 200;
	playerInfo.spell[SPELL_HASTE].lasts		= 30;
	playerInfo.spell[SPELL_STRONGHASTE].lasts= 20;
	playerInfo.spell[SPELL_SHIELD].lasts	= 200;

	delete ch;
}

void CCharInfoDialog::DoSetButtonSkin(){
	skin.SetButtonSkin(	m_ResetCounters);
	skin.SetButtonSkin(	m_OK);
}

BOOL CCharInfoDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	DoSetButtonSkin();
	
	configToControls(config);
	resetCounters();
	m_hasteRemaining.EnableWindow(config->enableTimers);
	m_stronghasteRemaining.EnableWindow(config->enableTimers);
	m_invisRemaining.EnableWindow(config->enableTimers);
	m_magicshieldRemaining.EnableWindow(config->enableTimers);
	m_magicShieldText.EnableWindow(config->enableTimers);
	m_strongHasteText.EnableWindow(config->enableTimers);
	m_invisText.EnableWindow(config->enableTimers);
	m_hasteText.EnableWindow(config->enableTimers);

	SetTimer(1001,500,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCharInfoDialog::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent==1001){
		dataCalc();
		dataShow();
	}
	//CDialog::OnTimer(nIDEvent);
	
}

void CCharInfoDialog::OnResetCounters() 
{
	resetCounters();	
}

void CCharInfoDialog::dataCalc(){
	CMemReaderProxy reader;	
	CTibiaCharacter *ch = reader.readSelfCharacter();

	//T4: Exp calculation
	long lvl;
	long expForLvl=0;	
	long expLeft=0;
	long percForLvl=0;
	long expForLvlDiff=0;

	//=((1+A1)*(2+A1)/2-4*A1+3)*100
	for (lvl=1;lvl<250;lvl++){
		expForLvlDiff	=  ((2+lvl)*(3+lvl)/2-4*(lvl+1)+3)*100;
		expForLvl		+= expForLvlDiff;

		if (expForLvl>ch->exp){
			expLeft		= expForLvl-ch->exp;
			percForLvl	= MulDiv(expLeft,100,expForLvlDiff);
			break;
		}
	}

	playerInfo.timeCurrent = time(NULL);
	unsigned long timeDiff = playerInfo.timeCurrent-playerInfo.timeStart;

	playerInfo.hp			= ch->hp;
	playerInfo.maxHp		= ch->maxHp;
	playerInfo.mana			= ch->mana;
	playerInfo.maxMana		= ch->maxMana;
	playerInfo.soulPoints	= ch->soulPoints;
	playerInfo.stamina		= ch->stamina;
	playerInfo.capacity		= ch->cap;
	strncpy(playerInfo.voc,ch->voc,3);

	if (!strcmp(ch->voc,"k") || !strcmp(ch->voc,"ek")){ //Knight
		playerInfo.maxCapacity	= 270+ch->lvl*25;
	}else if (!strcmp(ch->voc,"p") || !strcmp(ch->voc,"rp")){ //Pall
		playerInfo.maxCapacity	= 310+ch->lvl*20;
	}else{ //Rest
		playerInfo.maxCapacity	= 390+ch->lvl*10;
	}

	//Exp stats
	playerInfo.expCurrent	= ch->exp;
	playerInfo.expLvl		= expForLvlDiff;
	playerInfo.expLeft		= expLeft;
	if (timeDiff){
		playerInfo.expPerHour	= MulDiv(playerInfo.expCurrent-playerInfo.expStart,3600,timeDiff);
	}
	
	//Skills
	unsigned long skillLvl[9]		= {ch->lvl,ch->mlvl,ch->skillFist,ch->skillClub,ch->skillSword,ch->skillAxe,ch->skillDist,ch->skillShield,ch->skillFish};
	unsigned long skillPercLeft[9]	= {percForLvl,ch->mlvlPercLeft,ch->skillFistPercLeft,ch->skillClubPercLeft,ch->skillSwordPercLeft,ch->skillAxePercLeft,ch->skillDistPercLeft,ch->skillShieldPercLeft,ch->skillFishPercLeft};

	//Lvl
	playerInfo.skill[SKILL_LEVEL].lvl		= skillLvl[SKILL_LEVEL];
	playerInfo.skill[SKILL_LEVEL].percLeft	= skillPercLeft[SKILL_LEVEL];
	playerInfo.skill[SKILL_LEVEL].current	= skillLvl[SKILL_LEVEL]*100+(100-skillPercLeft[SKILL_LEVEL]);
	if (timeDiff){
		playerInfo.skill[SKILL_LEVEL].percPerHour = float(float(playerInfo.expPerHour)*100./float(playerInfo.expLvl));
	}

	//Mlvl & skills stats
	for (int i=1;i<9;i++){
		playerInfo.skill[i].lvl		= skillLvl[i];
		playerInfo.skill[i].percLeft= skillPercLeft[i];
		playerInfo.skill[i].current = skillLvl[i]*100+(100-skillPercLeft[i]);
		if (timeDiff){
			playerInfo.skill[i].percPerHour = float(float(playerInfo.skill[i].current-playerInfo.skill[i].start)*3600./float(timeDiff));
		}
	}

	int warn1Time = 20;
	int warn2Time = 5;

	int secRemainingInvisible = 0;
	int secRemainingHaste = 0;
	int secRemainingStrongHaste = 0;
	int secRemainingMagicShield = 0;

	//detect spell spell casting and record time
	CIPCBackPipeProxy backPipe;
	struct ipcMessage mess;	

	if (backPipe.readFromPipe(&mess,1004))
	{				
		int infoType;
		int nickLen;
		int msgLen;
		char nickBuf[16384];
		char msgBuf[16384];

		memset(nickBuf,0,16384);
		memset(msgBuf,0,16384);
		memcpy(&infoType,mess.payload,sizeof(int));
		memcpy(&nickLen,mess.payload+4,sizeof(int));
		memcpy(&msgLen,mess.payload+8,sizeof(int));
		memcpy(nickBuf,mess.payload+12,nickLen);
		memcpy(msgBuf,mess.payload+12+nickLen,msgLen);


		if ((infoType == 1) && (strcmp(nickBuf, ch->name) == 0) && (strcmp(nickBuf,"Tibia Auto") != 0)) {
//			sprintf(buf,"nick: '%s', msg: '%s', name: '%s', type: '%d'", nickBuf, msgBuf, ch->name, infoType);
//			sender.sendTAMessage(buf);

			if ((strcmpi(msgBuf, "utana vid ") == 0) || (strcmp(msgBuf,"test invis") == 0)) { //invisible
				playerInfo.spell[SPELL_INVISIBLE].start		= time(NULL);
				playerInfo.spell[SPELL_INVISIBLE].warning	= 0;
			} else if ((strcmpi(msgBuf, "utani hur ") == 0) || (strcmp(msgBuf,"test haste") == 0)) { //haste
				playerInfo.spell[SPELL_HASTE].start			= time(NULL);
				playerInfo.spell[SPELL_HASTE].warning		= 0;
			} else if ((strcmpi(msgBuf, "utani gran hur ") == 0) || (strcmp(msgBuf,"test strong haste") == 0)) { //strong haste
				playerInfo.spell[SPELL_STRONGHASTE].start	= time(NULL);
				playerInfo.spell[SPELL_STRONGHASTE].warning	= 0x01; //0x01 cuz it lasts less then warnin1
			} else if ((lstrcmpi(msgBuf, "utamo vita ") == 0) || (strcmp(msgBuf,"test magic shield") == 0)) { //magic shield
				playerInfo.spell[SPELL_SHIELD].start		= time(NULL);
				playerInfo.spell[SPELL_SHIELD].warning		= 0;
			}
		}

	}	

	CPackSenderProxy sender;
	char spellName[4][32]={"Invisible","Haste","Strong Haste","Magic Shield"};
	char buffer[260];

	for (i=0;i<4;i++){
		playerInfo.spell[i].remaining = max(playerInfo.spell[i].lasts - (time(NULL) - playerInfo.spell[i].start),0);
		if (playerInfo.spell[i].remaining > 0 && ((playerInfo.spell[i].remaining <= WARNING1 && !(playerInfo.spell[i].warning&0x01)) || (playerInfo.spell[i].remaining <= WARNING2 && !(playerInfo.spell[i].warning&0x02)))){
			sprintf(buffer,"%s will wear off in %d seconds.",spellName[i],playerInfo.spell[i].remaining);
			if (config->enableTimers)
				sender.sendTAMessage(buffer);
			if (playerInfo.spell[i].remaining <= WARNING1)
				playerInfo.spell[i].warning |= 0x01;
			if (playerInfo.spell[i].remaining <= WARNING2)
				playerInfo.spell[i].warning |= 0x02;
		}
	}
	delete ch;
}

void CCharInfoDialog::dataShow(){
	CStatic *skillCtrl[9]			= {&m_lvl,&m_mLvl,&m_fist,&m_club,&m_sword,&m_axe,&m_dist,&m_shield,&m_fish};
	CStatic *skillPerHourCtrl[9]	= {&m_lvlSpeed,&m_mLvlSpeed,&m_fistSpeed,&m_clubSpeed,&m_swordSpeed,&m_axeSpeed,&m_distSpeed,&m_shieldSpeed,&m_fishSpeed};
	CStatic *skillUpInCtrl[9]		= {&m_lvlUpin,&m_mLvlUpin,&m_fistUpin,&m_clubUpin,&m_swordUpin,&m_axeUpin,&m_distUpin,&m_shieldUpin,&m_fishUpin};
	char buffer[128];

	unsigned long timeDiff = playerInfo.timeCurrent-playerInfo.timeStart;
	unsigned long sec	= timeDiff%60;
	timeDiff -= sec;
	unsigned long min	= (timeDiff%3600)/60;
	timeDiff -= min*60;
	unsigned long hour	= timeDiff/3600;
	sprintf(buffer,"Running for %dh%dm%ds",hour,min,sec);				m_time.SetWindowText(buffer);

	sprintf(buffer,"%d/%d",playerInfo.hp,playerInfo.maxHp);				m_hp.SetWindowText(buffer);
	sprintf(buffer,"%d/%d",playerInfo.mana,playerInfo.maxMana);			m_mana.SetWindowText(buffer);
	sprintf(buffer,"%d",playerInfo.soulPoints);							m_sp.SetWindowText(buffer);
	sprintf(buffer,"%d.%02d/%d",(int)playerInfo.capacity,(int)((playerInfo.capacity-(int)playerInfo.capacity)*100),playerInfo.maxCapacity);	m_cap.SetWindowText(buffer);

	sprintf(buffer,"%d",playerInfo.expCurrent);		m_exp.SetWindowText(buffer);
	sprintf(buffer,"%d/h",playerInfo.expPerHour);	m_expSpeed.SetWindowText(buffer);
	sprintf(buffer,"%d to go",playerInfo.expLeft);	m_expUpin.SetWindowText(buffer);

	for (int i=0;i<9;i++){
		//T4: Skill lvl and perc left
		sprintf(buffer,"%d [%d%%]",playerInfo.skill[i].lvl,playerInfo.skill[i].percLeft);
		skillCtrl[i]->SetWindowText(buffer);
		//T4: Skill per hour
		if (skillPerHourCtrl[i]){
			sprintf(buffer,"%.02f%%/h",playerInfo.skill[i].percPerHour);
			skillPerHourCtrl[i]->SetWindowText(buffer);

			if (playerInfo.skill[i].percPerHour){
				float fHour = float(float(playerInfo.skill[i].percLeft)/float(playerInfo.skill[i].percPerHour));
				unsigned long hour = long(floor(fHour));
				unsigned long min = long(60*(fHour-hour));

				sprintf(buffer,"%dh%dmin",hour,min);
			}else{
				sprintf(buffer,"n/a");

			}
			skillUpInCtrl[i]->SetWindowText(buffer);
		}
	}

	CStatic *spellCtrl[4] = {&m_invisRemaining,&m_hasteRemaining,&m_stronghasteRemaining,&m_magicshieldRemaining};

	for (i=0;i<4;i++){
		sprintf(buffer, "%d seconds remaining", playerInfo.spell[i].remaining);
		if (config->enableTimers)
			spellCtrl[i]->SetWindowText(buffer);
	}
}
void CCharInfoDialog::configToControls(CConfigData *configData) {
	//Something here
	m_EnableTimer.SetCheck(configData->enableTimers);
}

CConfigData * CCharInfoDialog::controlsToConfig() {
	CConfigData *newConfigData = new CConfigData();

	newConfigData->enableTimers = m_EnableTimer.GetCheck() != 0;

	return newConfigData;
}

void CCharInfoDialog::OnEnabletimers() 
{
	m_hasteRemaining.EnableWindow(m_EnableTimer.GetCheck());
	m_stronghasteRemaining.EnableWindow(m_EnableTimer.GetCheck());
	m_invisRemaining.EnableWindow(m_EnableTimer.GetCheck());
	m_magicshieldRemaining.EnableWindow(m_EnableTimer.GetCheck());
	m_magicShieldText.EnableWindow(m_EnableTimer.GetCheck());
	m_strongHasteText.EnableWindow(m_EnableTimer.GetCheck());
	m_invisText.EnableWindow(m_EnableTimer.GetCheck());
	m_hasteText.EnableWindow(m_EnableTimer.GetCheck());
	if (!m_EnableTimer.GetCheck()) {
		m_hasteRemaining.SetWindowText("DISABLED");
		m_stronghasteRemaining.SetWindowText("DISABLED");
		m_invisRemaining.SetWindowText("DISABLED");
		m_magicshieldRemaining.SetWindowText("DISABLED");
	}
	else {
		m_hasteRemaining.SetWindowText("ENABLED");
		m_stronghasteRemaining.SetWindowText("ENABLED");
		m_invisRemaining.SetWindowText("ENABLED");
		m_magicshieldRemaining.SetWindowText("ENABLED");
	}
	config = controlsToConfig();
}

