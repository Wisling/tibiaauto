#if !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
#define AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_seller.h"
#include "ConfigData.h"
#include "MyDialog.h"
#include "BtnST.h"
#include "GroupBoxEx.h"

struct seller{
	char name[64];
	int xPos;
	int yPos;
	int zPos;
};

enum CToolSellerState{
	CToolSellerState_notRunning,
	CToolSellerState_halfSleep,
	CToolSellerState_noPathFound,
	CToolSellerState_walking,
	CToolSellerState_talking
};
class CMod_SellerApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

class CConfigDialog : public MyDialog
{
// Construction
public:
	void activateEnableButton(int enable);
	CConfigData * controlsToConfig();
	void configToControls(CConfigData *configData);
	void enableControls();
	void disableControls();
	CConfigDialog(CMod_SellerApp *app,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CGroupBoxEx	m_BuySellControlsFrame;
	CGroupBoxEx	m_listFrame;
	CGroupBoxEx	m_BuyControlsFrame;
	CGroupBoxEx	m_Seller4Frame;
	CGroupBoxEx	m_Seller3Frame;
	CGroupBoxEx	m_Seller2Frame;
	CGroupBoxEx	m_Seller1Frame;
	CComboBox m_tradeItemList;
	CButton m_sellOnCap;
	CButton m_sellOnSpace;
	CEdit m_sellWhen;
	CEdit m_quantityBuySell;
	CEdit m_buyPrice;
	CEdit m_buyTriggerQuantity;
	CComboBox m_modPriority;
	CButtonST m_addSeller;
	CEdit m_addName;
	CButton m_stopBySeller;
	CButtonST	m_OK;
	CButtonST	m_enable;
	CStatic m_stateSeller;
	//}}AFX_DATA
	CButtonST m_BuyBoxAdd[MAX_SELLERS];
	CButtonST m_BuyBoxRemove[MAX_SELLERS];
	CButtonST m_SellBoxAdd[MAX_SELLERS];
	CButtonST m_SellBoxRemove[MAX_SELLERS];
	CComboBox m_Seller[MAX_SELLERS];
	CListBox m_SellBox[MAX_SELLERS];
	CListBox m_BuyBox[MAX_SELLERS];

	void reloadSaleItems();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDialog)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void sellBoxAdd0();
	afx_msg void sellBoxAdd1();
	afx_msg void sellBoxAdd2();
	afx_msg void sellBoxAdd3();
	afx_msg void buyBoxAdd0();
	afx_msg void buyBoxAdd1();
	afx_msg void buyBoxAdd2();
	afx_msg void buyBoxAdd3();
	afx_msg void sellBoxRemove0();
	afx_msg void sellBoxRemove1();
	afx_msg void sellBoxRemove2();
	afx_msg void sellBoxRemove3();
	afx_msg void buyBoxRemove0();
	afx_msg void buyBoxRemove1();
	afx_msg void buyBoxRemove2();
	afx_msg void buyBoxRemove3();
	afx_msg void onSellOnCap();
	afx_msg void OnAddSeller();
	afx_msg void OnEnable();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void reloadSellers();
	CMod_SellerApp * m_app;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
