#if !defined(AFX_COLORCHOOSER_H__A63A6809_8E64_4A97_BB3C_EF70031CA74D__INCLUDED_)
#define AFX_COLORCHOOSER_H__A63A6809_8E64_4A97_BB3C_EF70031CA74D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorChooser.h : header file
//
#include "SliderCtrlEx.h"
#include "RepeatButton.h"
#include "Skin.h"
#include "GroupBoxEx.h"
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

XERCES_CPP_NAMESPACE_USE
/////////////////////////////////////////////////////////////////////////////
// CColorChooser dialog

class CColorChooser : public CDialog
{
// Construction
public:
	CColorChooser(CWnd* pParent = NULL);   // standard constructor
	CString pathBuf;

// Dialog Data
	//{{AFX_DATA(CColorChooser)
	enum { IDD = IDD_COLORCHOOSER_DIALOG };
	CEdit	m_ExampleEditbox;
	CButton	m_ExampleCheckbox;
	CGroupBoxEx	m_SecondaryBackground;
	CComboBox	m_DetailPicker;
	CRepeatButton	m_GreenSliderUp;
	CRepeatButton	m_RedSliderUp;
	CRepeatButton	m_GreenSliderDown;
	CRepeatButton	m_BlueSliderUp;
	CRepeatButton	m_BlueSliderDown;
	CButtonST		m_Example;
	CRepeatButton	m_RedSliderDown;
	CSliderCtrlEx	m_GreenSlider;
	CSliderCtrlEx	m_BlueSlider;
	CSliderCtrlEx	m_RedSlider;
	int		m_DetailRedValue;
	int		m_DetailGreenValue;
	int		m_DetailBlueValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorChooser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	CSkin loadedSkin;
	void	UpdateDisplay(int, int, int, int);
	CRect	m_ExampleRect;
	CSkin	originalSkin;

	// Generated message map functions
	//{{AFX_MSG(CColorChooser)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRedSliderDown();
	afx_msg void OnGreenSliderDown();
	afx_msg void OnBlueSliderDown();
	afx_msg void OnRedSliderUp();
	afx_msg void OnGreenSliderUp();
	afx_msg void OnBlueSliderUp();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSelchangeDetailPicker();
	virtual void OnOK();
	afx_msg void OnLoad();
	virtual void OnCancel();
	afx_msg void OnSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCHOOSER_H__A63A6809_8E64_4A97_BB3C_EF70031CA74D__INCLUDED_)
