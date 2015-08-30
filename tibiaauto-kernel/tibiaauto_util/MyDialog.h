#pragma once

#include "tibiaauto_util.h"
#include "Skin.h"
#include "BtnST.h"
class CConfigData;
/////////////////////////////////////////////////////////////////////////////
// MyDialog dialog

class TIBIAAUTOUTIL_API MyDialog : public CDialog
{
// Construction
public:
	virtual void configToControls(CConfigData *)
	{
	};
	virtual void controlsToConfig(CConfigData *)
	{
	};
	virtual void enableControls()
	{
	};
	virtual void disableControls()
	{
	};
	MyDialog(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor
	~MyDialog();   // standard constructor
// Dialog Data
	//{{AFX_DATA(MyDialog)
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MyDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush backBrush;
	CBrush foreBrush;
	CBrush editBrush;

	// Generated message map functions
	//{{AFX_MSG(MyDialog)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

