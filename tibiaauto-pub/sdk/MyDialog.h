#if !defined(AFX_MYDIALOG_H__9BE7D398_041C_49F7_B9AF_D10CAA6A3250__INCLUDED_)
#define AFX_MYDIALOG_H__9BE7D398_041C_49F7_B9AF_D10CAA6A3250__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyDialog.h : header file
//
#include "Skin.h"
#include "BtnST.h"
class CConfigData;
/////////////////////////////////////////////////////////////////////////////
// MyDialog dialog

class MyDialog : public CDialog
{
// Construction
public:
	virtual void configToControls(CConfigData *){
	};
	virtual void controlsToConfig(CConfigData *){
	};
	virtual void enableControls(){
	};
	virtual void disableControls(){
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

#endif // !defined(AFX_MYDIALOG_H__9BE7D398_041C_49F7_B9AF_D10CAA6A3250__INCLUDED_)
