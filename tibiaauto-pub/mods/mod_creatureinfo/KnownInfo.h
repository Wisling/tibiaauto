#if !defined(AFX_KNOWNINFO_H__C1156AD3_72D1_48D5_BE62_75389319EB38__INCLUDED_)
#define AFX_KNOWNINFO_H__C1156AD3_72D1_48D5_BE62_75389319EB38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KnownInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKnownInfo dialog

class CKnownInfo : public CDialog
{
// Construction
public:
	CKnownInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKnownInfo)
	enum { IDD = IDD_INFO };
	CStatic m_txtLevel;
	CEdit m_voc;
	CEdit m_type;
	CStatic m_txtVoc;
	CEdit m_name;
	CEdit m_level;
	CEdit m_lastSeen;
	CEdit m_hp;
	CEdit m_guildRank;
	CEdit m_guildName;
	CEdit m_guildDesc;
	CComboBox m_combo;
	CButton m_players;
	CButton m_monsters;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKnownInfo)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKnownInfo)
	afx_msg void OnToolcreaturinfoInfoPlayers();
	afx_msg void OnToolcreaturinfoInfoMonsters();
	virtual BOOL OnInitDialog();
	afx_msg void OnEditchangeToolcreaturinfoInfoCombo();
	afx_msg void OnSelendokToolcreaturinfoInfoCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Fill_Monster();
	void Fill_Player();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KNOWNINFO_H__C1156AD3_72D1_48D5_BE62_75389319EB38__INCLUDED_)
