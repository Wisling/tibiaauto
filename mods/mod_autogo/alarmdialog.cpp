// AlarmDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_autogo.h"
#include "AlarmDialog.h"
#include "TibiaItemProxy.h"
#include "MemReaderProxy.h"
#include "ModuleUtil.h"
#include "CustomSpellDialog.h"
#include <Tlhelp32.h>
#include <MMSystem.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlarmDialog dialog


CAlarmDialog::CAlarmDialog(CWnd* pParent /*=NULL*/)
	: MyDialog(CAlarmDialog::IDD, pParent) {
	//{{AFX_DATA_INIT(CAlarmDialog)
	//}}AFX_DATA_INIT
}

CAlarmDialog::~CAlarmDialog() {
	int count = m_spellList.GetCount() - 1;
	SpellInfo *spellData;
	for (int loop = 1; loop < count; loop++) {
		spellData = (SpellInfo *)m_spellList.GetItemData(loop);
		delete spellData;
	}
}

void CAlarmDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlarmDialog)
	DDX_Control(pDX, IDC_ACTION_STOP_WALKING, m_actionStopWalking);
	DDX_Control(pDX, IDC_MODULES_LIST2, m_modules2);
	DDX_Control(pDX, IDC_INSTRUCTION_TEXT, m_instructionText);
	DDX_Control(pDX, IDC_MODULES_LIST, m_modules);
	DDX_Control(pDX, IDC_TRIGGER, m_trigger);
	DDX_Control(pDX, IDC_SPELL_LIST, m_spellList);
	DDX_Control(pDX, IDC_SCREENSHOT_OPTIONS, m_screenshotOptions);
	DDX_Control(pDX, IDC_AUDIO_FILE, m_audioFile);
	DDX_Control(pDX, IDC_CONDITION, m_condition);
	DDX_Control(pDX, IDC_ATTRIBUTE, m_attribute);
	DDX_Control(pDX, IDC_ALARM_TYPE, m_alarmType);
	DDX_Control(pDX, IDC_ALARM_LIST, m_alarmList);
	DDX_Control(pDX, IDC_ACTION_FRAME, m_actionFrame);
	DDX_Control(pDX, IDC_ALARM_FRAME, m_alarmFrame);
	DDX_Control(pDX, IDC_ACTION_WINDOW, m_windowAction);
	DDX_Control(pDX, IDC_ACTION_WINDOW_LIST, m_windowActionList);
	DDX_Control(pDX, IDC_ACTION_SOUND, m_actionSound);
	DDX_Control(pDX, IDC_ACTION_START_MODULES, m_actionEnable);
	DDX_Control(pDX, IDC_ACTION_LOG_EVENTS, m_actionLogEvents);
	DDX_Control(pDX, IDC_ALARM_EDIT, m_alarmEdit);
	DDX_Control(pDX, IDC_ALARM_DELETE, m_alarmDelete);
	DDX_Control(pDX, IDC_ALARM_ADD, m_alarmAdd);
	DDX_Control(pDX, IDC_ACTION_SUSPEND_MODULES, m_actionSuspend);
	DDX_Control(pDX, IDC_ACTION_START, m_actionStart);
	DDX_Control(pDX, IDC_ACTION_SPELL, m_actionSpell);
	DDX_Control(pDX, IDC_ACTION_SHUTDOWN, m_actionShutdown);
	DDX_Control(pDX, IDC_ACTION_SCREENSHOT, m_actionScreenshot);
	DDX_Control(pDX, IDC_ACTION_RUNAWAY, m_actionRunaway);
	DDX_Control(pDX, IDC_ACTION_LOGOUT, m_actionLogout);
	DDX_Control(pDX, IDC_ACTION_KILL, m_actionKill);
	DDX_Control(pDX, IDC_ACTION_DEPOT, m_actionDepot);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlarmDialog, CDialog)
	//{{AFX_MSG_MAP(CAlarmDialog)
	ON_BN_CLICKED(IDC_ACTION_KILL, OnActionKill)
	ON_BN_CLICKED(IDC_ACTION_SHUTDOWN, OnActionShutdown)
	ON_BN_CLICKED(IDC_ACTION_RUNAWAY, OnActionRunaway)
	ON_BN_CLICKED(IDC_ACTION_START, OnActionStart)
	ON_BN_CLICKED(IDC_ACTION_DEPOT, OnActionDepot)
	ON_BN_CLICKED(IDC_ACTION_SPELL, OnActionSpell)
	ON_BN_CLICKED(IDC_ACTION_SCREENSHOT, OnActionScreenshot)
	ON_BN_CLICKED(IDC_ACTION_SUSPEND_MODULES, OnActionSuspendModules)
	ON_CBN_SELCHANGE(IDC_ALARM_TYPE, OnSelchangeAlarmType)
	ON_BN_CLICKED(IDC_ACTION_SOUND, OnActionSound)
	ON_CBN_SELCHANGE(IDC_ATTRIBUTE, OnSelchangeAttribute)
	ON_CBN_SELCHANGE(IDC_CONDITION, OnSelchangeCondition)
	ON_BN_CLICKED(IDC_ACTION_START_MODULES, OnActionStartModules)
	ON_EN_SETFOCUS(IDC_TRIGGER, OnSetfocusTrigger)
	ON_BN_CLICKED(IDC_ACTION_LOGOUT, OnActionLogout)
	ON_BN_CLICKED(IDC_ACTION_WINDOW, OnWindowAction)
	ON_BN_CLICKED(IDC_ACTION_LOG_EVENTS, OnActionLogEvents)
	ON_BN_CLICKED(IDC_ALARM_ADD, OnAlarmAdd)
	ON_BN_CLICKED(IDC_ALARM_DELETE, OnAlarmDelete)
	ON_BN_CLICKED(IDC_ALARM_EDIT, OnAlarmEdit)
	ON_CBN_SELCHANGE(IDC_SPELL_LIST, OnSelchangeSpellList)
	ON_BN_CLICKED(IDC_ACTION_STOP_WALKING, OnActionStopWalking)
	ON_LBN_SELCHANGE(IDC_MODULES_LIST, OnSelchangeModulesList)
	ON_LBN_SELCHANGE(IDC_MODULES_LIST2, OnSelchangeModulesList2)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmDialog message handlers

void CAlarmDialog::OnActionKill() {
	VERIFY(instructionText.LoadString(IDS_KILL)); 
	m_instructionText.SetWindowText(instructionText); 
	m_actionShutdown.EnableWindow(!m_actionKill.GetCheck());
}

void CAlarmDialog::OnActionShutdown() {
	VERIFY(instructionText.LoadString(IDS_SHUTDOWN)); 
	m_instructionText.SetWindowText(instructionText); 
	m_actionKill.EnableWindow(!m_actionShutdown.GetCheck());
}

void CAlarmDialog::OnActionRunaway() {
	if (m_actionStart.GetCheck() && m_actionRunaway.GetCheck()) VERIFY(instructionText.LoadString(IDS_START_RUNAWAY));
	else VERIFY(instructionText.LoadString(IDS_RUNAWAY));
	m_instructionText.SetWindowText(instructionText); 
	m_actionDepot.EnableWindow(!m_actionRunaway.GetCheck() && !m_actionStart.GetCheck());
	//m_actionStart.EnableWindow(!m_actionRunaway.GetCheck());
}

void CAlarmDialog::OnActionStart() {
	if (m_actionStart.GetCheck() && m_actionRunaway.GetCheck()) VERIFY(instructionText.LoadString(IDS_START_RUNAWAY));
	else VERIFY(instructionText.LoadString(IDS_START));
	m_instructionText.SetWindowText(instructionText); 
	m_actionDepot.EnableWindow(!m_actionStart.GetCheck() && !m_actionRunaway.GetCheck());
	//m_actionRunaway.EnableWindow(!m_actionStart.GetCheck());
}

void CAlarmDialog::OnActionDepot() {
	VERIFY(instructionText.LoadString(IDS_DEPOT)); 
	m_instructionText.SetWindowText(instructionText); 
	m_actionStart.EnableWindow(!m_actionDepot.GetCheck()); 
	m_actionRunaway.EnableWindow(!m_actionDepot.GetCheck());
}

void CAlarmDialog::OnActionSpell() {
	VERIFY(instructionText.LoadString(IDS_SPELL)); 
	m_instructionText.SetWindowText(instructionText); 
	m_spellList.EnableWindow(m_actionSpell.GetCheck());
	m_spellList.SetCurSel(m_actionSpell.GetCheck()?0:-1);
}

void CAlarmDialog::OnActionScreenshot() {
	VERIFY(instructionText.LoadString(IDS_SCREENSHOT)); 
	m_instructionText.SetWindowText(instructionText); 
	m_screenshotOptions.EnableWindow(m_actionScreenshot.GetCheck());
	m_screenshotOptions.SetCurSel(m_actionScreenshot.GetCheck()?0:-1);
}

void CAlarmDialog::OnActionSuspendModules() {
	VERIFY(instructionText.LoadString(IDS_SUSPEND_MODULES)); 
	m_instructionText.SetWindowText(instructionText); 
	m_modules.EnableWindow(m_actionSuspend.GetCheck());
}

void CAlarmDialog::OnActionStartModules() {
	VERIFY(instructionText.LoadString(IDS_START_MODULES)); 
	m_instructionText.SetWindowText(instructionText); 
	m_modules2.EnableWindow(m_actionEnable.GetCheck());
}

void CAlarmDialog::OnActionSound() {
	VERIFY(instructionText.LoadString(IDS_SOUND)); 
	m_instructionText.SetWindowText(instructionText); 
	m_audioFile.EnableWindow(m_actionSound.GetCheck());	
	m_audioFile.SetCurSel(m_actionSound.GetCheck()?0:-1);
}

void CAlarmDialog::OnSelchangeAlarmType() {
	CString text;
	int selected = m_alarmType.GetCurSel();
	char itemText[128];
	m_attribute.GetLBText(0, itemText);
	switch (selected) {
	case SKILL:
		if(strcmp(itemText, "Any")) {
			m_attribute.ResetContent();
			m_attribute.AddString("Any");
			m_attribute.AddString("Experience Level");
			m_attribute.AddString("Magic Level");
			m_attribute.AddString("Fishing");
			m_attribute.AddString("Fist");
			m_attribute.AddString("Club");
			m_attribute.AddString("Sword");
			m_attribute.AddString("Axe");
			m_attribute.AddString("Distance");
			m_attribute.AddString("Shielding");

			m_condition.ResetContent();
			m_condition.AddString("Level Up");
			m_condition.AddString("% of Level Achieved");
			m_condition.AddString("% of Level Remains");

			m_condition.EnableWindow(true);
			m_trigger.SetWindowText("");
			m_trigger.EnableWindow(true);
		}
		VERIFY(instructionText.LoadString(IDS_SKILL));
		m_instructionText.SetWindowText(instructionText); 
		break;
	case RESOURCE:
		if(strcmp(itemText, "Hit Points")) {
			m_attribute.ResetContent();
			m_attribute.AddString("Hit Points");
			m_attribute.AddString("Mana Points");
			m_attribute.AddString("Experience");
			m_attribute.AddString("Soul Points");
			m_attribute.AddString("Stamina");
			m_attribute.AddString("Capacity");
			m_attribute.AddString("Item Space");

			m_condition.ResetContent();
			m_condition.AddString("Equal To...");
			m_condition.AddString("Less Than...");
			m_condition.AddString("More Than...");

			m_condition.EnableWindow(true);
			m_trigger.SetWindowText("");
			m_trigger.EnableWindow(true);
		}
		VERIFY(instructionText.LoadString(IDS_RESOURCE)); 
		m_instructionText.SetWindowText(instructionText); 
		break;
	case PROXIMITY:
		if(strcmp(itemText, "Player")) {
			m_attribute.ResetContent();
			m_attribute.SetItemImage(m_attribute.AddString("Player"), 0);
			m_attribute.SetItemImage(m_attribute.AddString("Monster/NPC"), 18);
			m_attribute.SetItemImage(m_attribute.AddString("GM/CM"), 17);
			m_attribute.SetItemImage(m_attribute.AddString("Creature on Battlelist"), 19);
			m_attribute.SetItemImage(m_attribute.AddString("Black Skull"), 12);
			m_attribute.SetItemImage(m_attribute.AddString("Red Skull"), 13);
			m_attribute.SetItemImage(m_attribute.AddString("Green Skull"), 14);
			m_attribute.SetItemImage(m_attribute.AddString("Yellow Skull"), 15);
			m_attribute.SetItemImage(m_attribute.AddString("White Skull"), 16);

			m_condition.ResetContent();
			m_condition.AddString("Nearby");
			m_condition.AddString("Out of Area");
			m_condition.AddString("Is Onscreen For...");

			m_condition.EnableWindow(true);
			m_trigger.SetWindowText("Not Applicable");
			m_trigger.EnableWindow(false);
		}
		VERIFY(instructionText.LoadString(IDS_PROXIMITY)); 
		m_instructionText.SetWindowText(instructionText); 
		break;
	case ONLINE:
		if(strcmp(itemText, "Current Player")) {
			m_attribute.ResetContent();
			m_attribute.SetItemImage(m_attribute.AddString("Current Player"), 0);
			m_attribute.SetItemImage(m_attribute.AddString("VIP (Blank)"), 1);
			m_attribute.SetItemImage(m_attribute.AddString("VIP (Heart)"), 2);
			m_attribute.SetItemImage(m_attribute.AddString("VIP (Skull)"), 3);
			m_attribute.SetItemImage(m_attribute.AddString("VIP (Lightning)"), 4);
			m_attribute.SetItemImage(m_attribute.AddString("VIP (Reticle)"), 5);
			m_attribute.SetItemImage(m_attribute.AddString("VIP (Star)"), 6);
			m_attribute.SetItemImage(m_attribute.AddString("VIP (Yin Yang)"), 7);
			m_attribute.SetItemImage(m_attribute.AddString("VIP (Tripoint)"), 8);
			m_attribute.SetItemImage(m_attribute.AddString("VIP (X)"), 9);
			m_attribute.SetItemImage(m_attribute.AddString("VIP (Dollar Sign)"), 10);
			m_attribute.SetItemImage(m_attribute.AddString("VIP (Iron Cross)"), 11);

			m_condition.ResetContent();
			m_condition.AddString("Online");
			m_condition.AddString("Offline");

			m_condition.EnableWindow(true);
			m_trigger.SetWindowText("Not Applicable");
			m_trigger.EnableWindow(false);
		}
		VERIFY(instructionText.LoadString(IDS_ONLINE)); 
		m_instructionText.SetWindowText(instructionText); 
		break;
	case ITEMS:
		if(strcmp(itemText, "Food")) {
			m_attribute.ResetContent();
			m_attribute.SetItemImage(m_attribute.AddString("Food"), 39);
			CTibiaItemProxy itemProxy;
			CMemReaderProxy reader;
			for (int j = 0; j < 26; j++) {
				itemProxy.fillTypedItemIdArray(pow(2, j));
				int count = itemProxy.getTypedItemCount();
				int index;
				for (int i = 0; i < count; i++) {
					int objectId = itemProxy.getTypedItemIdAtIndex(i);
					CTibiaTile *tile = reader.getTibiaTile(objectId);
					if (tile) {
						text = itemProxy.getTypedItemNameAtIndex(i);
						index = m_attribute.AddString(text);
						switch ((int)pow(2, j)) {
						case 1:
							m_attribute.SetItemImage(index, 48);
							break;
						case 2:
							m_attribute.SetItemImage(index, 47);
							break;
						case 4:
							m_attribute.SetItemImage(index, 44);
							break;
						case 8:
							m_attribute.SetItemImage(index, 45);
							break;
						case 16:
							m_attribute.SetItemImage(index, 54);
							break;
						case 32:
							m_attribute.SetItemImage(index, 52);
							break;
						case 64:
							m_attribute.SetItemImage(index, 53);
							break;
						case 128:
							m_attribute.SetItemImage(index, 42);
							break;
						case 256:
							m_attribute.SetItemImage(index, 51);
							break;
						case 512:
							m_attribute.SetItemImage(index, 38);
							break;
						case 1024:
							m_attribute.SetItemImage(index, 50);
							break;
						case 2048:
							m_attribute.SetItemImage(index, 49);
							break;
						case 4096:
							m_attribute.SetItemImage(index, 46);
							break;
						case 8192:
							m_attribute.SetItemImage(index, 41);
							break;
						case 16384:
							m_attribute.SetItemImage(index, 40);
							break;
						case 32768:
							m_attribute.SetItemImage(index, 43);
							break;
						default:
							m_attribute.SetItemImage(index, 38);
							break;
						}
						m_attribute.SetItemData(index, objectId);
					}
				}
			}
			m_condition.ResetContent();
			m_condition.AddString("Equal To...");
			m_condition.AddString("Less Than...");
			m_condition.AddString("More Than...");

			m_condition.EnableWindow(true);
			m_trigger.SetWindowText("");
			m_trigger.EnableWindow(true);
		}
		VERIFY(instructionText.LoadString(IDS_ITEM)); 
		m_instructionText.SetWindowText(instructionText); 
		break;
	case CHARACTER_STATUS:
		if(strcmp(itemText, "Poisoned")) {
			m_attribute.ResetContent();
			m_attribute.SetItemImage(m_attribute.AddString("Poisoned"), 24);
			m_attribute.SetItemImage(m_attribute.AddString("Burning"), 25);
			m_attribute.SetItemImage(m_attribute.AddString("Electrified"), 26);
			m_attribute.SetItemImage(m_attribute.AddString("Drunk"), 27);
			m_attribute.SetItemImage(m_attribute.AddString("Mana Shield"), 28);
			m_attribute.SetItemImage(m_attribute.AddString("Slowed"), 29);
			m_attribute.SetItemImage(m_attribute.AddString("Hasted"), 30);
			m_attribute.SetItemImage(m_attribute.AddString("Log-out Block"), 19);
			m_attribute.SetItemImage(m_attribute.AddString("Drowning"), 31);
			m_attribute.SetItemImage(m_attribute.AddString("Freezing"), 32);
			m_attribute.SetItemImage(m_attribute.AddString("Dazzled"), 33);
			m_attribute.SetItemImage(m_attribute.AddString("Cursed"), 34);
			m_attribute.SetItemImage(m_attribute.AddString("Strengthened"), 35);
			m_attribute.SetItemImage(m_attribute.AddString("PZ Block"), 36);
			m_attribute.SetItemImage(m_attribute.AddString("In PZ"), 37);

			m_condition.EnableWindow(false);
			m_trigger.SetWindowText("Not Applicable");
			m_trigger.EnableWindow(false);
		}
		VERIFY(instructionText.LoadString(IDS_STATUS)); 
		m_instructionText.SetWindowText(instructionText); 
		break;
	case EVENT:
		if(strcmp(itemText, "Character Moved")) {
			m_attribute.ResetContent();
			m_attribute.SetItemImage(m_attribute.AddString("Character Moved"), 0);
			m_attribute.SetItemImage(m_attribute.AddString("Character Not Moved"), 0);
			m_attribute.SetItemImage(m_attribute.AddString("Character Hit"), 0);
			m_attribute.SetItemImage(m_attribute.AddString("Waypoint Reached"), 0);

			m_condition.ResetContent();
			m_condition.AddString("From Anyone");
			m_condition.AddString("From...");
			m_condition.AddString("Containing...");
			m_condition.EnableWindow(false);
			m_trigger.SetWindowText("Not Applicable");
			m_trigger.EnableWindow(false);
		}
		VERIFY(instructionText.LoadString(IDS_GENERAL)); 
		m_instructionText.SetWindowText(instructionText); 
		break;
	case MESSAGE:
		if(strcmp(itemText, "All Messages")) {
			m_attribute.ResetContent();
			m_attribute.SetItemImage(m_attribute.AddString("All Messages"), 21);
			m_attribute.SetItemImage(m_attribute.AddString("Public Messages"), 22);
			m_attribute.SetItemImage(m_attribute.AddString("Private Messages"), 23);

			m_condition.ResetContent();
			m_condition.AddString("From Anyone");
			m_condition.AddString("From...");
			m_condition.AddString("Containing...");
			m_condition.EnableWindow(false);
			m_trigger.SetWindowText("Not Applicable");
			m_trigger.EnableWindow(false);
		}
		VERIFY(instructionText.LoadString(IDS_MESSAGE)); 
		m_instructionText.SetWindowText(instructionText); 
		break;
	default:
		VERIFY(instructionText.LoadString(IDS_BEGIN)); 
		m_instructionText.SetWindowText(instructionText); 

	}
}

BOOL CAlarmDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	m_actionDepot.EnableWindow(!m_actionRunaway.GetCheck() && !m_actionStart.GetCheck());
	m_actionRunaway.EnableWindow(!m_actionDepot.GetCheck());
	m_actionStart.EnableWindow(!m_actionDepot.GetCheck());
	m_actionKill.EnableWindow(!m_actionShutdown.GetCheck());
	m_actionShutdown.EnableWindow(!m_actionKill.GetCheck());
	m_modules.EnableWindow(m_actionSuspend.GetCheck());
	m_modules2.EnableWindow(m_actionEnable.GetCheck());
	m_screenshotOptions.EnableWindow(m_actionScreenshot.GetCheck());
	m_windowActionList.EnableWindow(m_windowAction.GetCheck());
	m_spellList.EnableWindow(m_actionSpell.GetCheck());
	m_audioFile.EnableWindow(m_actionSound.GetCheck());	
	
	char path[1024];
	CModuleUtil::getInstallPath(path);
	strcat(path, "\\mods\\sound\\*.wav");

	CFileFind finder;
	BOOL bWorking = finder.FindFile(path);
	while (bWorking) {
		bWorking = finder.FindNextFile();
		if(!finder.IsDirectory() && !finder.IsDots())
			m_audioFile.AddString(finder.GetFileName());
	}

	m_modules.AddString("<Select All>");
	m_modules.AddString("<Deselect All>");
	m_modules2.AddString("<Select All>");
	m_modules2.AddString("<Deselect All>");
	HANDLE hSnap;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
	if (hSnap){
		MODULEENTRY32 lpModule;
		lpModule.dwSize = sizeof(MODULEENTRY32);
		
		Module32First(hSnap,&lpModule);
		do {
			if (strcmpi(lpModule.szModule,"mod_autogo.dll")!=0 && strcmpi(lpModule.szModule,"mod_playerinfo.dll")!=0) {
				if (!strcmpi(strtok(lpModule.szModule, "_."), "mod")) {
					char *buf;
					buf = strtok(NULL, "_.");
					buf[0] = toupper(buf[0]);
					m_modules.AddString(buf);
					m_modules2.AddString(buf);
				}
			}
		}while (Module32Next(hSnap,&lpModule));		
		CloseHandle(hSnap);
	}
	// Spell List Mana Costs
	m_spellInfo[1]  = new SpellInfo();
		m_spellInfo[1]->manaCost = 60;
		m_spellInfo[1]->spellDelay = 33;
	m_spellList.SetItemData(1, (long)m_spellInfo[1]);
	m_spellInfo[2] = new SpellInfo();
		m_spellInfo[2]->manaCost = 100;
		m_spellInfo[2]->spellDelay = 22;
	m_spellList.SetItemData(2, (long)m_spellInfo[2]);
	m_spellInfo[3] = new SpellInfo();
		m_spellInfo[3]->manaCost = 100;
		m_spellInfo[3]->spellDelay = 200;
	m_spellList.SetItemData(3, (long)m_spellInfo[3]);

	// Attribute Combo Box...
	m_attributeImg.Create(14, 14, ILC_COLOR32 | ILC_MASK, 0, 0);

	CBitmap vip; //Images 0-11
	vip.LoadBitmap(IDB_VIP);

	CBitmap proximity; //Images 12-20
	proximity.LoadBitmap(IDB_PROXIMITY);

	CBitmap general; //Images 21-37
	general.LoadBitmap(IDB_GENERAL);

	CBitmap items; //Images 38 & 39
	items.LoadBitmap(IDB_ITEMS);

	m_attributeImg.Add(&vip, RGB(255, 0, 255));
	m_attributeImg.Add(&proximity, RGB(255, 0, 255));
	m_attributeImg.Add(&general, RGB(255, 0, 255));
	m_attributeImg.Add(&items, RGB(255, 0, 255));

	m_alarmType.SetColumnCount(0);
	m_attribute.SetColumnCount(0);
	m_condition.SetColumnCount(0);
	m_attribute.SetImageList(&m_attributeImg);

	m_columnImg.Create(14, 14, ILC_COLOR32 | ILC_MASK, 0, 0);

	int index = 0;
	CBitmap actionIcons[15];

	actionIcons[index].LoadBitmap(IDB_RUNAWAY);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionRunaway.SetBitmaps(IDB_RUNAWAY, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionRunaway);
	m_actionRunaway.SetFlat(true);
	m_actionRunaway.DrawFlatFocus(false);

	actionIcons[index].LoadBitmap(IDB_START);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionStart.SetBitmaps(IDB_START, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionStart);
	m_actionStart.SetFlat(true);
	m_actionStart.DrawFlatFocus(false);

	actionIcons[index].LoadBitmap(IDB_DEPOT);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionDepot.SetBitmaps(IDB_DEPOT, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionDepot);
	m_actionDepot.SetFlat(true);
	m_actionDepot.DrawFlatFocus(false);

	actionIcons[index].LoadBitmap(IDB_SPELL);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionSpell.SetBitmaps(IDB_SPELL, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionSpell);
	m_actionSpell.SetFlat(true);
	m_actionSpell.DrawFlatFocus(false);

	actionIcons[index].LoadBitmap(IDB_SCREENSHOT);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionScreenshot.SetBitmaps(IDB_SCREENSHOT, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionScreenshot);
	m_actionScreenshot.SetFlat(true);
	m_actionScreenshot.DrawFlatFocus(false);

	actionIcons[index].LoadBitmap(IDB_STOP);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionStopWalking.SetBitmaps(IDB_STOP, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionStopWalking);
	m_actionStopWalking.SetFlat(true);
	m_actionStopWalking.DrawFlatFocus(false);

	actionIcons[index].LoadBitmap(IDB_LOGOUT);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionLogout.SetBitmaps(IDB_LOGOUT, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionLogout);
	m_actionLogout.SetFlat(true);
	m_actionLogout.DrawFlatFocus(false);

	actionIcons[index].LoadBitmap(IDB_KILLTIBIA);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionKill.SetBitmaps(IDB_KILLTIBIA, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionKill);
	m_actionKill.SetFlat(true);
	m_actionKill.DrawFlatFocus(false);

	actionIcons[index].LoadBitmap(IDB_SHUTDOWN);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionShutdown.SetBitmaps(IDB_SHUTDOWN, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionShutdown);
	m_actionShutdown.SetFlat(true);
	m_actionShutdown.DrawFlatFocus(false);

	actionIcons[index].LoadBitmap(IDB_MAXIMIZE);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_windowAction.SetBitmaps(IDB_MAXIMIZE, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_windowAction);
	m_windowAction.SetFlat(true);
	m_windowAction.DrawFlatFocus(false);

	actionIcons[index].LoadBitmap(IDB_AUDIO);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionSound.SetBitmaps(IDB_AUDIO, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionSound);
	m_actionSound.SetFlat(true);
	m_actionSound.DrawFlatFocus(false);

	actionIcons[index].LoadBitmap(IDB_LOGEVENTS);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionLogEvents.SetBitmaps(IDB_LOGEVENTS, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionLogEvents);
	m_actionLogEvents.SetFlat(true);
	m_actionLogEvents.DrawFlatFocus(false);

	actionIcons[index].LoadBitmap(IDB_STARTMODULE);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionEnable.SetBitmaps(IDB_STARTMODULE, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionEnable);
	m_actionEnable.SetFlat(true);
	m_actionEnable.DrawFlatFocus(false);
	
	actionIcons[index].LoadBitmap(IDB_SUSPENDMODULE);
	m_columnImg.Add(&actionIcons[index++], RGB(255, 0, 255));
	m_actionSuspend.SetBitmaps(IDB_SUSPENDMODULE, RGB(255, 0, 255), IDB_BLANK);
	skin.SetButtonSkin(m_actionSuspend);
	m_actionSuspend.SetFlat(true);
	m_actionSuspend.DrawFlatFocus(false);




	//Alarm ListView container (column headers)...
	m_alarmList.SetImageList(&m_columnImg, LVSIL_SMALL);
	m_alarmList.InsertColumn(0, "Alarm",LVCFMT_LEFT,132);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_IMAGE | LVCF_WIDTH | LVCFMT_CENTER;
	lvColumn.cx = 26;
	lvColumn.iImage = 0;
	m_alarmList.InsertColumn(1, &lvColumn);
	lvColumn.iImage = 1;
	m_alarmList.InsertColumn(2, &lvColumn);
	lvColumn.iImage = 2;
	m_alarmList.InsertColumn(3, &lvColumn);
	lvColumn.iImage = 3;
	m_alarmList.InsertColumn(4, &lvColumn);
	lvColumn.iImage = 4;
	m_alarmList.InsertColumn(5, &lvColumn);
	lvColumn.iImage = 5;
	m_alarmList.InsertColumn(6, &lvColumn);
	lvColumn.iImage = 6;
	m_alarmList.InsertColumn(7, &lvColumn);
	lvColumn.iImage = 7;
	m_alarmList.InsertColumn(8, &lvColumn);
	lvColumn.iImage = 8;
	m_alarmList.InsertColumn(9, &lvColumn);
	lvColumn.iImage = 9;
	m_alarmList.InsertColumn(10, &lvColumn);
	lvColumn.iImage = 10;
	m_alarmList.InsertColumn(11, &lvColumn);
	lvColumn.iImage = 11;
	m_alarmList.InsertColumn(12, &lvColumn);
	lvColumn.iImage = 12;
	m_alarmList.InsertColumn(13, &lvColumn);
	lvColumn.iImage = 13;
	m_alarmList.InsertColumn(14, &lvColumn);
	lvColumn.iImage = 14;
	m_alarmList.InsertColumn(15, &lvColumn);
	CConfigData test;
	configToControls(&test);
	skin.SetButtonSkin(m_alarmEdit);
	skin.SetButtonSkin(m_alarmDelete);
	skin.SetButtonSkin(m_alarmAdd);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAlarmDialog::OnSelchangeAttribute(){
	CMemReaderProxy reader;

	int selected = m_alarmType.GetCurSel();
	switch (selected) {
	case SKILL:
		selected = m_attribute.GetCurSel();
		switch (selected) {
		case ALL:
			VERIFY(instructionText.LoadString(IDS_SKILL_ALL)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case FISHING:
			VERIFY(instructionText.LoadString(IDS_SKILL_FISHING)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case LEVEL:
			VERIFY(instructionText.LoadString(IDS_SKILL_LEVEL)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case MAGICLVL:
			VERIFY(instructionText.LoadString(IDS_SKILL_MAGICLVL)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case FIST:
			VERIFY(instructionText.LoadString(IDS_SKILL_FIST)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case CLUB:
			VERIFY(instructionText.LoadString(IDS_SKILL_CLUB)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case SWORDSKILL:
			VERIFY(instructionText.LoadString(IDS_SKILL_SWORD)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case AXE:
			VERIFY(instructionText.LoadString(IDS_SKILL_AXE)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case DISTANCE:
			VERIFY(instructionText.LoadString(IDS_SKILL_DISTANCE)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case SHIELD:
			VERIFY(instructionText.LoadString(IDS_SKILL_SHIELDING)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_SKILL)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
		break;
	case CHARACTER_STATUS:
		selected = m_attribute.GetCurSel();
		switch (selected) {
		case POISONED:
			VERIFY(instructionText.LoadString(IDS_STATUS_POISONED)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case BURNING:
			VERIFY(instructionText.LoadString(IDS_STATUS_BURNING)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case ELECTRIFIED:
			VERIFY(instructionText.LoadString(IDS_STATUS_ELECTRIFIED)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case DRUNK:
			VERIFY(instructionText.LoadString(IDS_STATUS_DRUNK)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case MANASHIELD:
			VERIFY(instructionText.LoadString(IDS_STATUS_MANASHIELD)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case SLOWED:
			VERIFY(instructionText.LoadString(IDS_STATUS_SLOWED)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case HASTED:
			VERIFY(instructionText.LoadString(IDS_STATUS_HASTED)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case DROWNING:
			VERIFY(instructionText.LoadString(IDS_STATUS_DROWNING)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case FREEZING:
			VERIFY(instructionText.LoadString(IDS_STATUS_FREEZING)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case DAZZLED:
			VERIFY(instructionText.LoadString(IDS_STATUS_DAZZLED)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case CURSED:
			VERIFY(instructionText.LoadString(IDS_STATUS_CURSED)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case STRENGTHENED:
			VERIFY(instructionText.LoadString(IDS_STATUS_STRENGTHENED)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case LOGOUTBLOCK:
			VERIFY(instructionText.LoadString(IDS_STATUS_LOGOUTBLOCK)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case PZBLOCK:
			VERIFY(instructionText.LoadString(IDS_STATUS_PZBLOCK)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case INPZ:
			VERIFY(instructionText.LoadString(IDS_STATUS_INPZ)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_STATUS)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
		break;
	case RESOURCE:
		selected = m_attribute.GetCurSel();
		switch (selected) {
		case HP:
			VERIFY(instructionText.LoadString(IDS_RESOURCE_HP)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case MP:
			VERIFY(instructionText.LoadString(IDS_RESOURCE_MP)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case XP:
			VERIFY(instructionText.LoadString(IDS_RESOURCE_XP)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case SP:
			VERIFY(instructionText.LoadString(IDS_RESOURCE_SP)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case STAMINA:
			VERIFY(instructionText.LoadString(IDS_RESOURCE_STAMINA)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case CAPACITY:
			VERIFY(instructionText.LoadString(IDS_RESOURCE_CAPACITY)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case SPACE:
			VERIFY(instructionText.LoadString(IDS_RESOURCE_SPACE)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_RESOURCE)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
		break;
	case PROXIMITY:
		selected = m_attribute.GetCurSel();
		switch (selected) {
		case PLAYER:
			VERIFY(instructionText.LoadString(IDS_PROXIMITY_PLAYER)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case MONSTER:
			VERIFY(instructionText.LoadString(IDS_PROXIMITY_MONSTER)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case GM:
			VERIFY(instructionText.LoadString(IDS_PROXIMITY_GM)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case BATTLELIST:
			VERIFY(instructionText.LoadString(IDS_PROXIMITY_BATTLELIST)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case BLACKSKULL:
			VERIFY(instructionText.LoadString(IDS_PROXIMITY_SKULL_BLACK)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case REDSKULL:
			VERIFY(instructionText.LoadString(IDS_PROXIMITY_SKULL_RED)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case GREENSKULL:
			VERIFY(instructionText.LoadString(IDS_PROXIMITY_SKULL_GREEN)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case YELLOWSKULL:
			VERIFY(instructionText.LoadString(IDS_PROXIMITY_SKULL_YELLOW)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case WHITESKULL:
			VERIFY(instructionText.LoadString(IDS_PROXIMITY_SKULL_WHITE)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_PROXIMITY)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
		break;
	case ONLINE:
		selected = m_attribute.GetCurSel();
		switch (selected) {
		case CURRENTPLAYERONLINE:
			VERIFY(instructionText.LoadString(IDS_ONLINE_CURRENT_PLAYER)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case NONEONLINE:
			VERIFY(instructionText.LoadString(IDS_ONLINE_NONE)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case HEARTONLINE:
			VERIFY(instructionText.LoadString(IDS_ONLINE_HEART)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case SKULLONLINE:
			VERIFY(instructionText.LoadString(IDS_ONLINE_SKULL)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case LIGHTNINGONLINE:
			VERIFY(instructionText.LoadString(IDS_ONLINE_LIGHTNING)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case RETICLEONLINE:
			VERIFY(instructionText.LoadString(IDS_ONLINE_RETICLE)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case STARONLINE:
			VERIFY(instructionText.LoadString(IDS_ONLINE_STAR)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case YINYANGONLINE:
			VERIFY(instructionText.LoadString(IDS_ONLINE_YINYANG)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case TRIPOINTONLINE:
			VERIFY(instructionText.LoadString(IDS_ONLINE_TRIPOINT)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case XONLINE:
			VERIFY(instructionText.LoadString(IDS_ONLINE_X)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case DOLLARSIGNONLINE:
			VERIFY(instructionText.LoadString(IDS_ONLINE_DOLLARSIGN)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case IRONCROSSONLINE:
			VERIFY(instructionText.LoadString(IDS_ONLINE_IRONCROSS)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_ONLINE)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
		break;
	case ITEMS:
		selected = m_attribute.GetCurSel();
		switch (selected) {
		case 0:
			VERIFY(instructionText.LoadString(IDS_ITEM_FOOD)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case -1:
			VERIFY(instructionText.LoadString(IDS_ITEM)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_ITEM_CUSTOM)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
		break;
	case EVENT:
		selected = m_attribute.GetCurSel();
		switch (selected) {
		case CHARACTERMOVED:
			m_condition.EnableWindow(false);
			m_condition.SetCurSel(-1);
			m_trigger.SetWindowText("Not Applicable");
			m_trigger.EnableWindow(false);
			VERIFY(instructionText.LoadString(IDS_GENERAL_CHARACTERMOVED)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case CHARACTERHIT:
			m_condition.EnableWindow(false);
			m_condition.SetCurSel(-1);
			m_trigger.SetWindowText("Not Applicable");
			m_trigger.EnableWindow(false);
			VERIFY(instructionText.LoadString(IDS_GENERAL_CHARACTERHIT)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case CHARACTERNOTMOVED:
			m_condition.ResetContent();
			m_condition.AddString("Equal To...");
			m_condition.AddString("Less Than...");
			m_condition.AddString("More Than...");
			m_condition.EnableWindow(true);

			m_condition.SetCurSel(-1);
			m_trigger.SetWindowText("Not Applicable");
			m_trigger.EnableWindow(false);
			VERIFY(instructionText.LoadString(IDS_GENERAL_CHARACTERNOTMOVED)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case WAYPOINTREACHED:
			{
			m_condition.EnableWindow(false);
			m_condition.SetCurSel(-1);
			CTibiaCharacter* self=reader.readSelfCharacter();
			char buf[256];
			sprintf(buf,"(%d,%d,%d)%d",self->x,self->y,self->z,2);
			delete self;
			m_trigger.SetWindowText(buf);
			m_trigger.EnableWindow(true);
			VERIFY(instructionText.LoadString(IDS_GENERAL_WAYPOINTREACHED)); 
			m_instructionText.SetWindowText(instructionText);
			}
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_GENERAL)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
		break;
	case MESSAGE:
		selected = m_attribute.GetCurSel();
		switch (selected) {
		case ALLMESSAGES:
			m_condition.EnableWindow(true);
			VERIFY(instructionText.LoadString(IDS_MESSAGE_ALLMESSAGES)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case PUBLICMESSAGES:
			m_condition.EnableWindow(true);
			VERIFY(instructionText.LoadString(IDS_MESSAGE_PUBLICMESSAGES)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case PRIVATEMESSAGES:
			m_condition.EnableWindow(true);
			VERIFY(instructionText.LoadString(IDS_MESSAGE_PRIVATEMESSAGES)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_MESSAGE)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
		break;
	default:
		VERIFY(instructionText.LoadString(IDS_BEGIN_ATTRIBUTE)); 
		m_instructionText.SetWindowText(instructionText);
	}
}

void CAlarmDialog::OnSelchangeCondition() {
	int selected = m_alarmType.GetCurSel();
	char itemText[128];
	m_attribute.GetLBText(0, itemText);
	switch (selected) {
	case SKILL:
		selected = m_condition.GetCurSel();
		switch (selected) {
		case LEVELUP:
			VERIFY(instructionText.LoadString(IDS_LEVELUP)); 
			m_instructionText.SetWindowText(instructionText);

			m_trigger.SetWindowText("Not Applicable");
			m_trigger.EnableWindow(false);
			break;
		case PERCLVLACHIEVED:
			VERIFY(instructionText.LoadString(IDS_PERCLVLACHIEVED)); 
			m_instructionText.SetWindowText(instructionText);

			m_trigger.SetWindowText("<Percentage>");
			m_trigger.EnableWindow(true);
			break;
		case PERCLVLREMAINING:
			VERIFY(instructionText.LoadString(IDS_PERCLVLREMAINING)); 
			m_instructionText.SetWindowText(instructionText);

			m_trigger.SetWindowText("<Percentage>");
			m_trigger.EnableWindow(true);
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_CONDITION)); 
			m_instructionText.SetWindowText(instructionText);

			m_trigger.SetWindowText("");
			m_trigger.EnableWindow(true);
			break;
		}
		break;
	case ITEMS:
	case RESOURCE:
		selected = m_condition.GetCurSel();
		switch (selected) {
		case EQUAL:
			VERIFY(instructionText.LoadString(IDS_EQUAL)); 
			m_instructionText.SetWindowText(instructionText);

			m_trigger.SetWindowText("<Quantity>");
			break;
		case LESS:
			VERIFY(instructionText.LoadString(IDS_LESS)); 
			m_instructionText.SetWindowText(instructionText);

			m_trigger.SetWindowText("<Quantity>");
			break;
		case MORE:
			VERIFY(instructionText.LoadString(IDS_MORE)); 
			m_instructionText.SetWindowText(instructionText);

			m_trigger.SetWindowText("<Quantity>");
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_CONDITION)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
		break;
	case PROXIMITY:
		selected = m_condition.GetCurSel();
		switch (selected) {
		case NEARBY:
			VERIFY(instructionText.LoadString(IDS_NEARBY)); 
			m_instructionText.SetWindowText(instructionText);
			m_trigger.SetWindowText("<Not Applicable>");
			m_trigger.EnableWindow(false);
			break;
		case DISAPPEARS:
			VERIFY(instructionText.LoadString(IDS_DISAPPEARS)); 
			m_instructionText.SetWindowText(instructionText);
			m_trigger.SetWindowText("<Not Applicable>");
			m_trigger.EnableWindow(false);
			break;
		case ISONSCREENFOR:
			VERIFY(instructionText.LoadString(IDS_ISONSCREENFOR)); 
			m_instructionText.SetWindowText(instructionText);
			m_trigger.SetWindowText("<Time in seconds>");
			m_trigger.EnableWindow(true);
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_CONDITION)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
		break;
	case ONLINE:
		selected = m_condition.GetCurSel();
		switch (selected) {
		case LOGON:
			VERIFY(instructionText.LoadString(IDS_LOGON)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case LOGOFF:
			VERIFY(instructionText.LoadString(IDS_LOGOFF)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_ONLINE)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
		m_trigger.SetWindowText("Not Applicable");
		m_trigger.EnableWindow(false);
		break;
	case EVENT:
		selected = m_attribute.GetCurSel();
		switch (selected) {
		case CHARACTERNOTMOVED:
			selected = m_condition.GetCurSel();
			switch (selected) {
			case EQUAL:
				VERIFY(instructionText.LoadString(IDS_EQUAL)); 
				m_instructionText.SetWindowText(instructionText);

				m_trigger.EnableWindow(true);
				m_trigger.SetWindowText("<Seconds>");
				break;
			case LESS:
				VERIFY(instructionText.LoadString(IDS_LESS)); 
				m_instructionText.SetWindowText(instructionText);

				m_trigger.EnableWindow(true);
				m_trigger.SetWindowText("<Seconds>");
				break;
			case MORE:
				VERIFY(instructionText.LoadString(IDS_MORE)); 
				m_instructionText.SetWindowText(instructionText);

				m_trigger.EnableWindow(true);
				m_trigger.SetWindowText("<Seconds>");
				break;
			default:
				VERIFY(instructionText.LoadString(IDS_CONDITION));
				m_instructionText.SetWindowText(instructionText);
				break;
			}
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_CONDITION)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
		break;
	case MESSAGE:
	selected = m_condition.GetCurSel();
		switch (selected) {
		case FROMALL:
			m_trigger.SetWindowText("Not Applicable");
			m_trigger.EnableWindow(false);
			VERIFY(instructionText.LoadString(IDS_FROMALL)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case FROM:
			m_trigger.SetWindowText("<Character Name>");
			m_trigger.EnableWindow(true);
			VERIFY(instructionText.LoadString(IDS_FROM)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		case CONTAINS:
			m_trigger.SetWindowText("<Message Part>");
			m_trigger.EnableWindow(true);
			VERIFY(instructionText.LoadString(IDS_CONTAINS)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		default:
			VERIFY(instructionText.LoadString(IDS_CONDITION)); 
			m_instructionText.SetWindowText(instructionText);
			break;
		}
	}
}

void CAlarmDialog::OnSetfocusTrigger() {
	CString text;
	m_trigger.GetWindowText(text);
	if (strncmp("Not ",text,4)==0)
		m_trigger.SetWindowText("");
	else if (strncmp("<",text,1)==0)
		m_trigger.SetWindowText("");
}

void CAlarmDialog::OnActionStopWalking() {
	VERIFY(instructionText.LoadString(IDS_STOPWALKING)); 
	m_instructionText.SetWindowText(instructionText); 
}

void CAlarmDialog::OnActionLogout() {
	VERIFY(instructionText.LoadString(IDS_LOGOUT)); 
	m_instructionText.SetWindowText(instructionText); 
}

void CAlarmDialog::OnWindowAction() {
	VERIFY(instructionText.LoadString(IDS_WINDOW_ACTION));
	m_instructionText.SetWindowText(instructionText);
	m_windowActionList.EnableWindow(m_windowAction.GetCheck());
	m_windowActionList.SetCurSel(m_windowAction.GetCheck()?0:-1);
}

void CAlarmDialog::OnActionLogEvents() {
	VERIFY(instructionText.LoadString(IDS_LOG_EVENTS));
	m_instructionText.SetWindowText(instructionText);
}

void CAlarmDialog::disableControls() {
	m_alarmAdd.EnableWindow(false);
	m_alarmEdit.EnableWindow(false);
	m_alarmDelete.EnableWindow(false);
}

void CAlarmDialog::enableControls() {
	m_alarmAdd.EnableWindow();
	m_alarmEdit.EnableWindow();
	m_alarmDelete.EnableWindow();
}

void CAlarmDialog::configToControls(CConfigData *config) {
	CString text;

	list<Alarm>::iterator listItr = config->alarmList.begin();
	memAlarmList.clear();
	m_alarmList.DeleteAllItems();
	while (listItr != config->alarmList.end()) {
		m_alarmType.SetCurSel(listItr->getAlarmType());
		OnSelchangeAlarmType();
		if (listItr->getAlarmType() == ITEMS) {
			for (int i = 0; i < m_attribute.GetCount(); i++){
				if (listItr->getAttribute() == m_attribute.GetItemData(i)) {
					m_attribute.SetCurSel(i);
					break;
				}
			}
		}
		else
			m_attribute.SetCurSel(listItr->getAttribute());
		OnSelchangeAttribute();
		m_condition.SetCurSel(listItr->getCondition());
		OnSelchangeCondition();
		if (listItr->getTrigger().getTriggerText().GetLength() > 0)
			m_trigger.SetWindowText(listItr->getTrigger().getTriggerText());
		m_actionRunaway.SetCheck(listItr->doGoToRunaway());
		m_actionStart.SetCheck(listItr->doGoToStart());
		m_actionDepot.SetCheck(listItr->doGoToDepot());
		m_actionSpell.SetCheck(listItr->doCastSpell().GetLength());
		int index = m_spellList.FindString(-1, listItr->doCastSpell());
		if (index == CB_ERR)
			m_spellList.SetCurSel(m_spellList.AddString(listItr->doCastSpell()));
		else
			m_spellList.SetCurSel(index);
		SpellInfo *info = new SpellInfo();
			info->manaCost = listItr->getManaCost();
			info->spellDelay = listItr->getSpellDelay();
		m_spellList.SetItemData(index, (long)info);
		m_actionScreenshot.SetCheck(listItr->doTakeScreenshot()+1);
			m_screenshotOptions.SetCurSel(listItr->doTakeScreenshot());
		m_actionLogout.SetCheck(listItr->doLogout());
		m_actionKill.SetCheck(listItr->doKillClient());
		m_actionShutdown.SetCheck(listItr->doShutdownComputer());
		m_windowAction.SetCheck(listItr->doWindowAction()!=-1);
			m_windowActionList.SetCurSel(listItr->doWindowAction());
		m_actionSound.SetCheck(listItr->doAlarm().GetLength());
			m_audioFile.SetCurSel(m_audioFile.FindString(-1, listItr->doAlarm()));
		m_actionLogEvents.SetCheck(listItr->doLogEvents());
		m_actionEnable.SetCheck(listItr->doStartModules().size());
		m_actionSuspend.SetCheck(listItr->doStopModules().size());
		m_actionStopWalking.SetCheck(listItr->doStopWalking());
		int size = memAlarmList.size();
		OnAlarmAdd();
		if(memAlarmList.size() == size + 1) {
			list<CString> tmpl=listItr->doStartModules();
			memAlarmList.rbegin()->setStartModules(tmpl);
			m_alarmList.SetItemText(memAlarmList.size() - 1, 13, memAlarmList.rbegin()->doStartModules().size() ? "X" : "");
			memAlarmList.rbegin()->setStopModules(listItr->doStopModules());
			m_alarmList.SetItemText(memAlarmList.size() - 1, 14, memAlarmList.rbegin()->doStopModules().size() ? "X" : "");
		}
		listItr++;
	}
}

void CAlarmDialog::controlsToConfig(CConfigData *config) {
	config->triggerMessage = triggerMessage;
	config->alarmList = memAlarmList;
}

void CAlarmDialog::OnAlarmAdd() {
	CString strBuffer;
	Alarm* temp = addToList();
	if (temp) {
		strBuffer = m_attribute.GetItemText(m_attribute.GetCurSel(), 0) + " (" + m_alarmType.GetItemText(m_alarmType.GetCurSel(), 0);
		strBuffer += strBuffer.Right(1) == 's' ? "es)" : "s)";
		if (strBuffer.Right(3) == "ys)")
			strBuffer.Replace("ys", "ies");
		strBuffer += m_condition.IsWindowEnabled() ? " " + m_condition.GetItemText(m_condition.GetCurSel(), 0) : "";
		strBuffer.Replace("Equal To", "=");
		strBuffer.Replace("Less Than", "<");
		strBuffer.Replace("More Than", ">");
		strBuffer.Replace("...", "");
		if (m_trigger.IsWindowEnabled()) {
			CString triggerBuffer;
			m_trigger.GetWindowText(triggerBuffer);
			int index = strBuffer.Find('%');
			if (index > 0)
				strBuffer.Insert(index, triggerBuffer);
			else {
				strBuffer += " ";
				if (m_alarmType.GetCurSel() == EVENT && m_attribute.GetCurSel() < CHARACTERMOVED && m_condition.GetCurSel() > FROMALL)
					strBuffer += "\"";
				strBuffer += triggerBuffer;
			}
		}
		if (m_alarmType.GetCurSel() == EVENT && m_attribute.GetCurSel() < CHARACTERMOVED && m_condition.GetCurSel() > FROMALL)
			strBuffer += "\"";
		temp->setDescriptor(strBuffer);
		memAlarmList.push_back(*temp);
		m_alarmList.InsertItem(memAlarmList.size(), strBuffer, -1);
		m_alarmList.SetItemText(memAlarmList.size() - 1, 1, m_actionRunaway.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 2, m_actionStart.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 3, m_actionDepot.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 4, m_actionSpell.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 5, m_actionScreenshot.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 6, m_actionStopWalking.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 7, m_actionLogout.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 8, m_actionKill.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 9, m_actionShutdown.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 10, m_windowAction.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 11, m_actionSound.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 12, m_actionLogEvents.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 13, m_actionEnable.GetCheck() ? "X" : "");
		m_alarmList.SetItemText(memAlarmList.size() - 1, 14, m_actionSuspend.GetCheck() ? "X" : "");

		m_alarmType.SetCurSel(-1);
		m_attribute.ResetContent();
		m_condition.EnableWindow();
		m_condition.ResetContent();
		m_trigger.EnableWindow();
		m_trigger.SetWindowText("");
		m_actionRunaway.SetCheck(false);
			m_actionRunaway.EnableWindow();
		m_actionStart.SetCheck(false);
			m_actionStart.EnableWindow();
		m_actionDepot.SetCheck(false);
			m_actionDepot.EnableWindow();
		m_actionSpell.SetCheck(false);
			m_spellList.SetCurSel(-1);
			m_spellList.EnableWindow(false);
		m_actionScreenshot.SetCheck(false);
			m_screenshotOptions.SetCurSel(-1);
			m_screenshotOptions.EnableWindow(false);
		m_actionLogout.SetCheck(false);
		m_actionKill.SetCheck(false);
			m_actionKill.EnableWindow();
		m_actionShutdown.SetCheck(false);
			m_actionShutdown.EnableWindow();
		m_windowAction.SetCheck(false);
			m_windowActionList.SetCurSel(-1);
			m_windowActionList.EnableWindow(false);
		m_actionSound.SetCheck(false);
			m_audioFile.SetCurSel(-1);
			m_audioFile.EnableWindow(false);
		m_actionLogEvents.SetCheck(false);
		m_actionEnable.SetCheck(false);
			m_modules2.SelItemRange(false, 0, m_modules2.GetCount());
			m_modules2.EnableWindow(false);
		m_actionSuspend.SetCheck(false);
			m_modules.SelItemRange(false, 0, m_modules.GetCount());
			m_modules.EnableWindow(false);
		m_actionStopWalking.SetCheck(false);
	}
}

Alarm* CAlarmDialog::addToList() {
	Alarm *temp = new Alarm();
	CString text;
	if (m_trigger.IsWindowEnabled()) {
		m_trigger.GetWindowText(text);
		if (text.GetLength() && text[0] != '<') {
			if (m_alarmType.GetCurSel() ==  MESSAGE) {
				temp->setTrigger(1, text);
			}
			else if (m_alarmType.GetCurSel() == EVENT && m_attribute.GetCurSel() == WAYPOINTREACHED) {
			temp->setTrigger(3, text);
			}
			else
				temp->setTrigger(2, text);
		}
		else {
			VERIFY(instructionText.LoadString(IDS_TRIGGER_ERROR)); 
			m_instructionText.SetWindowText(instructionText);
			PlaySound((LPCSTR)IDR_UHOH, AfxGetResourceHandle(), SND_RESOURCE | SND_ASYNC);
			return false;
		}
	}
	if (m_alarmType.GetCurSel() != -1) 
		temp->setType(m_alarmType.GetCurSel());
	else {
		VERIFY(instructionText.LoadString(IDS_ALARM_TYPE_ERROR)); 
		m_instructionText.SetWindowText(instructionText);
		PlaySound((LPCSTR)IDR_UHOH, AfxGetResourceHandle(), SND_RESOURCE | SND_ASYNC);
		return false;
	}

	if (m_attribute.GetCurSel() != -1)
		if (m_alarmType.GetCurSel() == ITEMS && m_attribute.GetCurSel() > 0)
			temp->setAttribute(m_attribute.GetItemData(m_attribute.GetCurSel()));
		else
			temp->setAttribute(m_attribute.GetCurSel());
	else {
		VERIFY(instructionText.LoadString(IDS_ATTRIBUTE_ERROR)); 
		m_instructionText.SetWindowText(instructionText);
		PlaySound((LPCSTR)IDR_UHOH, AfxGetResourceHandle(), SND_RESOURCE | SND_ASYNC);
		m_alarmType.GetCurSel();
		return false;
	}

	if (m_condition.IsWindowEnabled()) {
		if (m_condition.GetCurSel() != -1) 
			temp->setCondition(m_condition.GetCurSel());
		else {
			VERIFY(instructionText.LoadString(IDS_CONDITION_ERROR)); 
			m_instructionText.SetWindowText(instructionText);
			PlaySound((LPCSTR)IDR_UHOH, AfxGetResourceHandle(), SND_RESOURCE | SND_ASYNC);
			return false;
		}
	}

#pragma warning(disable: 4800)
	temp->setGoToRunaway(m_actionRunaway.GetCheck());
	temp->setGoToStart(m_actionStart.GetCheck());
	temp->setGoToDepot(m_actionDepot.GetCheck());
	if (m_actionSpell.GetCheck()) {
		if (m_spellList.GetCurSel() != -1) {
			SpellInfo *spellData = (SpellInfo *)m_spellList.GetItemData(m_spellList.GetCurSel());
			m_spellList.GetLBText(m_spellList.GetCurSel(), text);
			temp->setCastSpell(text);
			temp->setManaCost(spellData->manaCost);
			temp->setSpellDelay(spellData->spellDelay);
		}
		else {
			VERIFY(instructionText.LoadString(IDS_SPELLWORDS_ERROR)); 
			m_instructionText.SetWindowText(instructionText);
			PlaySound((LPCSTR)IDR_UHOH, AfxGetResourceHandle(), SND_RESOURCE | SND_ASYNC);
			return false;
		}
	}
	if (m_actionScreenshot.GetCheck()) {
		if (m_screenshotOptions.GetCurSel() != -1)
			temp->setTakeScreenshot(m_screenshotOptions.GetCurSel());
		else {
			VERIFY(instructionText.LoadString(IDS_SCREENSHOT_ERROR)); 
			m_instructionText.SetWindowText(instructionText);
			PlaySound((LPCSTR)IDR_UHOH, AfxGetResourceHandle(), SND_RESOURCE | SND_ASYNC);
			return false;
		}
	}
	else
		temp->setTakeScreenshot(-1);
	temp->setStopWalking(m_actionStopWalking.GetCheck());
	temp->setLogout(m_actionLogout.GetCheck());
	temp->setKillClient(m_actionKill.GetCheck());
	temp->setShutdownComputer(m_actionShutdown.GetCheck());
	if (m_windowAction.GetCheck()) {
		if (m_windowActionList.GetCurSel() != -1) {
			temp->setWindowAction(m_windowActionList.GetCurSel());
		}
		else {
			VERIFY(instructionText.LoadString(IDS_WINDOWACTION_ERROR)); 
			m_instructionText.SetWindowText(instructionText);
			PlaySound((LPCSTR)IDR_UHOH, AfxGetResourceHandle(), SND_RESOURCE | SND_ASYNC);
			return false;
		}
	} 
	else
		temp->setWindowAction(-1);
	if (m_actionSound.GetCheck()) {
		if (m_audioFile.GetCurSel() != -1) {
			m_audioFile.GetLBText(m_audioFile.GetCurSel(), text);
			temp->setAudioAlarm(text);
		}
		else {
			VERIFY(instructionText.LoadString(IDS_SOUNDFILE_ERROR)); 
			m_instructionText.SetWindowText(instructionText);
			PlaySound((LPCSTR)IDR_UHOH, AfxGetResourceHandle(), SND_RESOURCE | SND_ASYNC);
			return false;
		}
	}
	temp->setLogEvents(m_actionLogEvents.GetCheck());
	int nCount = m_modules2.GetSelCount();
	CArray<int,int> aryListBoxSel;
	if (m_actionEnable.GetCheck()){
		// Start Modules
		aryListBoxSel.SetSize(nCount);
		m_modules2.GetSelItems(nCount, aryListBoxSel.GetData()); 
		list<CString> beginModules;
		for (int loop = 0; loop < nCount; loop++) {
			int ind = aryListBoxSel.GetAt(loop);
			if (ind==0) continue; // 0 is special and need not be added
			m_modules2.GetText(ind, text);
			text.MakeLower();
			beginModules.push_back("mod_" + text + ".dll");
		}
		temp->setStartModules(beginModules);
	}
	if (m_actionSuspend.GetCheck()){
		// Suspend Modules
		nCount = m_modules.GetSelCount();
		aryListBoxSel.SetSize(nCount);
		m_modules.GetSelItems(nCount, aryListBoxSel.GetData()); 
		list<CString> endModules;
		for (int loop = 0; loop < nCount; loop++) {
			int ind = aryListBoxSel.GetAt(loop);
			if (ind==0) continue; // 0 is special and need not be added
			m_modules.GetText(ind, text);
			text.MakeLower();
			endModules.push_back("mod_" + text + ".dll");
		}
		temp->setStopModules(endModules);
	}
	//Special case variable
	if (m_alarmType.GetCurSel() == EVENT && m_attribute.GetCurSel() <= PRIVATEMESSAGES)
		triggerMessage |= true;
#pragma warning(default: 4800)
	return temp;
}

void CAlarmDialog::OnAlarmDelete() {
	list<Alarm>::iterator alarmItr = memAlarmList.begin();
	POSITION pos = m_alarmList.GetFirstSelectedItemPosition();
	if (pos) {
		int index =	m_alarmList.GetNextSelectedItem(pos);
		m_alarmList.DeleteItem(index);
		for (; index != 0; index--, alarmItr++);
		
		memAlarmList.erase(alarmItr);
	}
}

void CAlarmDialog::OnAlarmEdit() {
	CString text;
	list<Alarm>::iterator alarmItr = memAlarmList.begin();
	POSITION pos = m_alarmList.GetFirstSelectedItemPosition();
	if (pos) {
		int index =	m_alarmList.GetNextSelectedItem(pos);
		m_alarmList.DeleteItem(index);

		for (; index != 0; index--, alarmItr++);
		m_alarmType.SetCurSel(alarmItr->getAlarmType());
		OnSelchangeAlarmType();
		if (alarmItr->getAlarmType() == ITEMS) {
			for (int i = 0; i < m_attribute.GetCount(); i++){
				if (alarmItr->getAttribute() == m_attribute.GetItemData(i)) {
					m_attribute.SetCurSel(i);
					break;
				}
			}
		}
		else
			m_attribute.SetCurSel(alarmItr->getAttribute());
		OnSelchangeAttribute();
		m_condition.SetCurSel(alarmItr->getCondition());
		OnSelchangeCondition();
		//if (alarmItr->getTrigger().getTrigger().GetLength() > 0)
			m_trigger.SetWindowText(alarmItr->getTrigger().getTriggerText());
		//else if (alarmItr->getTrigger().getTrigger() != -1) {
		//	text.Format("%d", alarmItr->getTrigger().getTrigger());
		//	m_trigger.SetWindowText(text);
		//}
		m_actionRunaway.SetCheck(alarmItr->doGoToRunaway());			
		m_actionStart.SetCheck(alarmItr->doGoToStart());
		m_actionDepot.SetCheck(alarmItr->doGoToDepot());
		m_actionStart.EnableWindow(!m_actionDepot.GetCheck()); 
		m_actionRunaway.EnableWindow(!m_actionDepot.GetCheck());
		if (m_actionRunaway.GetCheck()) {
			m_actionDepot.EnableWindow(!m_actionRunaway.GetCheck());
		}
		else if (m_actionStart.GetCheck()) {
			m_actionDepot.EnableWindow(!m_actionStart.GetCheck());
		}
		else if (m_actionDepot.GetCheck()) {
			m_actionStart.EnableWindow(!m_actionDepot.GetCheck()); 
			m_actionRunaway.EnableWindow(!m_actionDepot.GetCheck());
		}
		m_actionSpell.SetCheck(alarmItr->doCastSpell().GetLength());
		m_spellList.EnableWindow(m_actionSpell.GetCheck());
		index = m_spellList.FindString(-1, alarmItr->doCastSpell());
		if (index == CB_ERR)
			m_spellList.SetCurSel(m_spellList.AddString(alarmItr->doCastSpell()));
		else
			m_spellList.SetCurSel(index);
		SpellInfo *info = new SpellInfo();
		info->manaCost = alarmItr->getManaCost();
		info->spellDelay = alarmItr->getSpellDelay();
		m_spellList.SetItemData(index, (long)info);
		m_actionScreenshot.SetCheck(alarmItr->doTakeScreenshot()+1);
		m_screenshotOptions.EnableWindow(m_actionScreenshot.GetCheck());
		m_screenshotOptions.SetCurSel(alarmItr->doTakeScreenshot());
		m_actionStopWalking.SetCheck(alarmItr->doStopWalking());
		m_actionLogout.SetCheck(alarmItr->doLogout());
		m_actionKill.SetCheck(alarmItr->doKillClient());
		m_actionShutdown.SetCheck(alarmItr->doShutdownComputer());
		if (m_actionKill.GetCheck())
			m_actionShutdown.EnableWindow(!m_actionKill.GetCheck());
		else if (m_actionShutdown.GetCheck())
			m_actionKill.EnableWindow(!m_actionShutdown.GetCheck());
		m_windowAction.SetCheck(alarmItr->doWindowAction()!=-1);
		m_windowActionList.SetCurSel(alarmItr->doWindowAction());
		m_windowActionList.EnableWindow(m_windowAction.GetCheck());
		m_actionSound.SetCheck(alarmItr->doAlarm().GetLength());
		m_audioFile.EnableWindow(m_actionSound.GetCheck());	
		m_audioFile.SetCurSel(m_audioFile.FindString(-1, alarmItr->doAlarm()));
		m_actionLogEvents.SetCheck(alarmItr->doLogEvents());
		m_actionEnable.SetCheck(alarmItr->doStartModules().size());

		m_modules2.EnableWindow(m_actionEnable.GetCheck());
		m_modules2.SelItemRange(0,0,m_modules2.GetCount());
        list<CString> temp = alarmItr->doStartModules();
        list<CString>::iterator modsItr = temp.begin();
		CString prefix("mod_");
		CString postfix(".dll");
        while (modsItr != temp.end()) {
            *modsItr=modsItr->Mid(prefix.GetLength(),modsItr->GetLength()-prefix.GetLength()-postfix.GetLength());//remove "mod_" and ".dll"
			CString a=*modsItr;
			int sel=m_modules2.FindStringExact(-1, *modsItr);
			if (sel!=-1) m_modules2.SetSel(sel);
			modsItr++;
		}
		m_actionSuspend.SetCheck(alarmItr->doStopModules().size());
		m_modules.SelItemRange(0,0,m_modules.GetCount());
		m_modules.EnableWindow(m_actionSuspend.GetCheck());
        temp =  alarmItr->doStopModules();
        modsItr = temp.begin();
        while (modsItr != temp.end()) {
			*modsItr=modsItr->Mid(prefix.GetLength(),modsItr->GetLength()-prefix.GetLength()-postfix.GetLength());//remove "mod_" and ".dll"//remove "mod_" and ".dll"
			int sel=m_modules.FindStringExact(-1, *modsItr);
			if (sel!=-1) m_modules.SetSel(sel);
			modsItr++;
		}
		memAlarmList.erase(alarmItr);
	}
}

void CAlarmDialog::OnSelchangeSpellList() {
	switch (m_spellList.GetCurSel()) {
	case 0:
		CCustomSpellDialog temp;
		if (temp.DoModal() == IDOK) {
			int index = m_spellList.AddString(temp.m_spellWords);
			m_spellInfo[index] = new SpellInfo();
				m_spellInfo[index]->manaCost = temp.m_manaCost;
				m_spellInfo[index]->spellDelay = temp.m_castingDelay;
			m_spellList.SetItemData(index, (long)m_spellInfo[index]);
			m_spellList.SetCurSel(-1);
		}
		break;
	}	
}

void CAlarmDialog::OnSelchangeModulesList() 
{
	if (m_modules.GetCurSel() == 1){//if <Select All>
		m_modules.SelItemRange(true, 2, m_modules.GetCount());
		m_modules.SetSel(1, false);
	}
	if (m_modules.GetCurSel() == 0)//if <Deselect All>
		m_modules.SelItemRange(false, 0, m_modules.GetCount());
}

void CAlarmDialog::OnSelchangeModulesList2() 
{
	if (m_modules2.GetCurSel() == 1){//if <Select All>
		m_modules2.SelItemRange(m_modules2.GetSel(1), 2, m_modules2.GetCount());
		m_modules2.SetSel(1, false);
	}	
	if (m_modules2.GetCurSel() == 0)//if <Deselect All>
		m_modules2.SelItemRange(false, 0, m_modules.GetCount());
}

void CAlarmDialog::OnDestroy() 
{
	MyDialog::OnDestroy();
	for (int loop = 1; loop < m_spellList.GetCount(); loop++) {
		delete(m_spellInfo[loop]);
	}
}

