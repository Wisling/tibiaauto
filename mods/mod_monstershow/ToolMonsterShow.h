#if !defined(AFX_TOOLMONSTERSHOW_H__D4E339EA_5A93_4625_A0DF_FDBE51E626FB__INCLUDED_)
#define AFX_TOOLMONSTERSHOW_H__D4E339EA_5A93_4625_A0DF_FDBE51E626FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolMonsterShow.h : header file
//
#include "resource.h"
#include "MemConstData.h"
#include "MyDialog.h"
#include "GroupBoxEx.h"
#include "BtnST.h"

////////////////////
/////////////////////////////////////////////////////////
// CToolMonsterShow dialog

class CToolMonsterShow : public MyDialog
{
// Construction
public:
	void showMonsterDetails(int x,int y);
	CToolMonsterShow(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToolMonsterShow)
	enum { IDD = IDD_TOOL_MONSTERSHOW };
	CGroupBoxEx	m_OptionsFrame;
	CButtonST	m_OK;
	CGroupBoxEx	m_MonsterMapFrame;
	CButton	m_revealSlime;
	CButton	m_freeze;
	CButton	m_showOneRelLvl;
	CEdit	m_monsterInfo;
	CEdit   m_monsterInfoBelow;
	CEdit   m_monsterInfoSame;
	CEdit   m_monsterInfoAbove;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolMonsterShow)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CToolMonsterShow)
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMonstershowFreeze();
	afx_msg void OnClose();
	afx_msg void OnMonstershowRevealslime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void refreshVisibleCreatures();
	CButton * m_monsterButtons[21][17];
	CMemConstData memConstData;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLMONSTERSHOW_H__D4E339EA_5A93_4625_A0DF_FDBE51E626FB__INCLUDED_)
