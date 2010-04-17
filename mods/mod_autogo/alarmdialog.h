#if !defined(AFX_ALARMDIALOG_H__6708BC0F_CCB4_42DA_8932_B4F1C9AED01D__INCLUDED_)
#define AFX_ALARMDIALOG_H__6708BC0F_CCB4_42DA_8932_B4F1C9AED01D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlarmDialog.h : header file
//
#include "MyDialog.h"
#include "GroupBoxEx.h"
#include "ComboBoxSuper.h"
#include "Alarm.h"
#include <list>

using namespace std;

struct SpellInfo {
	int manaCost;
	int spellDelay;
};

/////////////////////////////////////////////////////////////////////////////
// CAlarmDialog dialog

class CAlarmDialog : public MyDialog
{
// Construction
public:
	bool addToList(Alarm *);
	void controlsToConfig(CConfigData *);
	void configToControls(CConfigData *);
	void enableControls();
	void disableControls();
	CAlarmDialog(CWnd* pParent = NULL);   // standard constructor
	~CAlarmDialog();   // standard destructor

// Dialog Data
	//{{AFX_DATA(CAlarmDialog)
	enum { IDD = IDD_ALARM_DIALOG };
	CButtonST	m_actionStopWalking;
	CListBox	m_modules2;
	CStatic	m_instructionText;
	CListBox	m_modules;
	CEdit	m_trigger;
	CComboBox	m_spellList;
	CComboBox	m_screenshotOptions;
	CComboBox	m_audioFile;
	CComboBoxSuper	m_condition;
	CComboBoxSuper	m_attribute;
	CComboBoxSuper	m_alarmType;
	CListCtrl	m_alarmList;
	CGroupBoxEx	m_actionFrame;
	CGroupBoxEx	m_alarmFrame;
	CButtonST	m_windowAction;
	CComboBox	m_windowActionList;
	CButtonST	m_actionSound;
	CButtonST	m_actionEnable;
	CButtonST	m_actionLogEvents;
	CButtonST	m_alarmEdit;
	CButtonST	m_alarmDelete;
	CButtonST	m_alarmAdd;
	CButtonST	m_actionSuspend;
	CButtonST	m_actionStart;
	CButtonST	m_actionSpell;
	CButtonST	m_actionShutdown;
	CButtonST	m_actionScreenshot;
	CButtonST	m_actionRunaway;
	CButtonST	m_actionLogout;
	CButtonST	m_actionKill;
	CButtonST	m_actionDepot;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	list<Alarm> memAlarmList;
	CImageList m_attributeImg; 
	CImageList m_columnImg; 
	CString instructionText;
	// Generated message map functions
	//{{AFX_MSG(CAlarmDialog)
	afx_msg void OnActionKill();
	afx_msg void OnActionShutdown();
	afx_msg void OnActionRunaway();
	afx_msg void OnActionStart();
	afx_msg void OnActionDepot();
	afx_msg void OnActionSpell();
	afx_msg void OnActionScreenshot();
	afx_msg void OnActionSuspendModules();
	afx_msg void OnSelchangeAlarmType();
	virtual BOOL OnInitDialog();
	afx_msg void OnActionSound();
	afx_msg void OnSelchangeAttribute();
	afx_msg void OnSelchangeCondition();
	afx_msg void OnActionStartModules();
	afx_msg void OnSetfocusTrigger();
	afx_msg void OnActionLogout();
	afx_msg void OnWindowAction();
	afx_msg void OnActionLogEvents();
	afx_msg void OnAlarmAdd();
	afx_msg void OnAlarmDelete();
	afx_msg void OnAlarmEdit();
	afx_msg void OnSelchangeSpellList();
	afx_msg void OnActionStopWalking();
	afx_msg void OnSelchangeModulesList();
	afx_msg void OnSelchangeModulesList2();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	SpellInfo * m_spellInfo[50];
	bool triggerMessage;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMDIALOG_H__6708BC0F_CCB4_42DA_8932_B4F1C9AED01D__INCLUDED_)
