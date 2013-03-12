#if !defined(AFX_TOOLMAPSHOW_H__38871855_61A8_44B3_BF65_3D8172E4314F__INCLUDED_)
#define AFX_TOOLMAPSHOW_H__38871855_61A8_44B3_BF65_3D8172E4314F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolMapShow.h : header file
//
#include "resource.h"
#include "MapButton.h"
#include "ConfigWindow.h"
#include "tibiatile.h"
#include "MyDialog.h"
#include "BtnST.h"
#include "GroupBoxEx.h"
/////////////////////////////////////////////////////////////////////////////
// CToolMapShow dialog

class CToolMapShow : public MyDialog
{
// Construction
public:
	void setTileDetails(int, int, int);
	void showTileDetails(int x,int y);
	void mapPointClicked(int posX, int posY, int pos);
	void ShowMapConfig(int, int);
	void refreshVisibleMap();
	void DoSetButtonSkin();
	void RefreshExtendedResearchMap();
	CToolMapShow(CWnd* pParent = NULL);   // standard constructor
	~CToolMapShow();

// Dialog Data
	//{{AFX_DATA(CToolMapShow)
	enum { IDD = IDD_TOOL_MAPSHOW };
	CButtonST	m_ClearMap;
	CButtonST	m_OK;
	CGroupBoxEx	m_MappingOptionFrame;
	CButton	m_research;
	CButton m_extendedResearch;
	CStatic m_tileInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolMapShow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CToolMapShow)
	afx_msg void OnClose();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnToolMapshowClear();
	afx_msg void OnToolMapshowResearch();
	afx_msg void OnToolMapshowExtendedResearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	
	CMapButton * m_mapButtons[21][21];
	int m_mapButtonImage[21][21];
	CTibiaTile *memTilesForConfig[10];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLMAPSHOW_H__38871855_61A8_44B3_BF65_3D8172E4314F__INCLUDED_)
