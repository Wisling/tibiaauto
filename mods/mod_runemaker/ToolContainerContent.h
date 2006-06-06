#if !defined(AFX_TOOLCONTAINERCONTENT_H__5C46010F_8D80_4C27_859D_85E6414787D0__INCLUDED_)
#define AFX_TOOLCONTAINERCONTENT_H__5C46010F_8D80_4C27_859D_85E6414787D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolContainerContent.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolContainerContent dialog

class CToolContainerContent : public CDialog
{
private:
	int m_containerNr;
// Construction
public:
	CToolContainerContent(int containerNr,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToolContainerContent)
	enum { IDD = IDD_TOOL_CONTAINERCONTENT };
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolContainerContent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CToolContainerContent)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLCONTAINERCONTENT_H__5C46010F_8D80_4C27_859D_85E6414787D0__INCLUDED_)
