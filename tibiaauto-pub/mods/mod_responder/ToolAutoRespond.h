#if !defined(AFX_TOOLAUTORESPOND_H__4610782C_BCAB_4BD5_8837_0285276C0CAD__INCLUDED_)
#define AFX_TOOLAUTORESPOND_H__4610782C_BCAB_4BD5_8837_0285276C0CAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolAutoRespond.h : header file
//
#include "AutoResponderParserContext.h"
#include "ToolAutoResponderThreadConfig.h"
#include "MyDialog.h"
#include "GroupBoxEx.h"
#include "BtnST.h"

#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>


XERCES_CPP_NAMESPACE_USE

#define MAX_THREADS 1000

/////////////////////////////////////////////////////////////////////////////
// CToolAutoRespond dialog

class CToolAutoRespond : public MyDialog
{
// Construction

public:
	void stop();
	void start();
	void DoSetButtonSkin();
	~CToolAutoRespond();
	CToolAutoRespond(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToolAutoRespond)
	enum { IDD = IDD_TOOL_AUTORESPOND };
	CGroupBoxEx	m_ResponderLogsFrame;
	CButton	m_localEcho;
	CButton	m_debug;
	CListCtrl	m_threadStatus;
	CEdit	m_script;
	CButtonST	m_enable;
	CButtonST	m_ClearLogs;
	CButtonST	m_OK;
	CListCtrl	m_actionLog;
	CListCtrl	m_msgHistory;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolAutoRespond)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CToolAutoRespond)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnAutorespondEnable();
	afx_msg void OnAutorespondClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int toolThreadAutoResponderShouldStopWaiting;
	void readInfo();
	void refreshThreadStatus();

	int threadCount;
	CToolAutoResponderThreadConfig threadConfig[MAX_THREADS];
	
	DOMNode  *doc;
	XercesDOMParser *parser;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLAUTORESPOND_H__4610782C_BCAB_4BD5_8837_0285276C0CAD__INCLUDED_)
