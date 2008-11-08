// ItemAdd.cpp : implementation file
//

#include "stdafx.h"
#include "mod_itemconfig.h"
#include "ItemConfig.h"
#include "TibiaTile.h"
#include "memReaderProxy.h"
#include "TibiaItemProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ITEM	1
#define FOOD	2
#define LOOT	3	

/////////////////////////////////////////////////////////////////////////////
// CItemAdd dialog

CItemAdd::CItemAdd() : CDialog(CItemAdd::IDD) {
	//{{AFX_DATA_INIT(CItemAdd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CItemAdd::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemAdd)
	DDX_Control(pDX, IDC_ITEM_ADD_NAME, m_ItemName);
	DDX_Control(pDX, IDC_ITEM_ADD_ID, m_ItemID);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CItemAdd, CDialog)
	//{{AFX_MSG_MAP(CItemAdd)
	ON_BN_CLICKED(IDOK, OnCommit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemAdd message handlers

void CItemAdd::OnCommit() {
	char buf[64];
	CTibiaItemProxy itemProxy;
	m_ItemID.GetWindowText(buf,63); itemId=atoi(buf);
	m_ItemName.GetWindowText(name,60);
	if (itemProxy.getItemsItemsCount() < 200)
		itemProxy.addItemItem(name, itemId);

	itemProxy.saveItemLists();
	this->EndDialog(IDOK);
}

BOOL CItemAdd::OnInitDialog() {
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
};

/////////////////////////////////////////////////////////////////////////////
// CItemEdit dialog

CItemEdit::CItemEdit(int idIn, char* nameIn)
: CDialog(CItemEdit::IDD) {
	itemId = idIn;
	memcpy(name, nameIn, 59);
	//{{AFX_DATA_INIT(CItemEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CItemEdit::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemEdit)
	DDX_Control(pDX, IDC_ITEM_EDIT_NAME, m_ItemName);
	DDX_Control(pDX, IDC_ITEM_EDIT_ID, m_ItemID);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CItemEdit, CDialog)
	//{{AFX_MSG_MAP(CItemEdit)
	ON_BN_CLICKED(IDOK, OnCommit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemEdit message handlers

void CItemEdit::OnCommit() {
	char buf[64];
	CTibiaItemProxy itemProxy;
	int index = itemProxy.getIndex(itemId, ITEM);
	m_ItemID.GetWindowText(buf,63); itemId=atoi(buf);
	m_ItemName.GetWindowText(name,60);
	itemProxy.setItemId(index, itemId);
	itemProxy.setItemName(index, name);
	itemProxy.saveItemLists();
	this->EndDialog(IDOK);
}

BOOL CItemEdit::OnInitDialog() {
	CDialog::OnInitDialog();
	char buf[64];
	sprintf(buf, "%d", itemId);
	m_ItemID.SetWindowText(buf);
	m_ItemName.SetWindowText(name);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CFoodAdd dialog

CFoodAdd::CFoodAdd() : CDialog(CFoodAdd::IDD) {
	//{{AFX_DATA_INIT(CFoodAdd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CFoodAdd::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFoodAdd)
	DDX_Control(pDX, IDC_FOOD_ADD_NAME, m_ItemName);
	DDX_Control(pDX, IDC_FOOD_ADD_ID, m_ItemID);
	DDX_Control(pDX, IDC_FOOD_ADD_TIME, m_EatTime);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFoodAdd, CDialog)
	//{{AFX_MSG_MAP(CFoodAdd)
	ON_BN_CLICKED(IDOK, OnCommit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFoodAdd message handlers

void CFoodAdd::OnCommit() {
	char buf[64];
	CTibiaItemProxy itemProxy;
	m_ItemID.GetWindowText(buf,63); itemId=atoi(buf);
	m_ItemName.GetWindowText(name,60);
	m_EatTime.GetWindowText(buf,63); eatTime=atoi(buf);
	if (itemProxy.getItemsFoodCount() < 200)
		itemProxy.addItemFood(name, itemId, eatTime);

	itemProxy.saveItemLists();
	this->EndDialog(IDOK);
}

BOOL CFoodAdd::OnInitDialog() {
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
};

/////////////////////////////////////////////////////////////////////////////
// CFoodEdit dialog

CFoodEdit::CFoodEdit(int idIn, char* nameIn, int timeIn)
: CDialog(CFoodEdit::IDD) {
	itemId = idIn;
	memcpy(name, nameIn, 59);
	eatTime = timeIn;
	//{{AFX_DATA_INIT(CFoodEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CFoodEdit::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFoodEdit)
	DDX_Control(pDX, IDC_FOOD_EDIT_NAME, m_ItemName);
	DDX_Control(pDX, IDC_FOOD_EDIT_ID, m_ItemID);
	DDX_Control(pDX, IDC_FOOD_EDIT_TIME, m_EatTime);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFoodEdit, CDialog)
	//{{AFX_MSG_MAP(CFoodEdit)
	ON_BN_CLICKED(IDOK, OnCommit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFoodEdit message handlers

void CFoodEdit::OnCommit() {
	char buf[64];
	CTibiaItemProxy itemProxy;
	int index = itemProxy.getIndex(itemId, FOOD);
	m_ItemID.GetWindowText(buf,63); itemId=atoi(buf);
	m_ItemName.GetWindowText(name,60);
	m_EatTime.GetWindowText(buf,63); eatTime=atoi(buf);
	itemProxy.setFoodId(index, itemId);
	itemProxy.setFoodName(index, name);
	itemProxy.setExtraInfo(index, eatTime, FOOD);
	itemProxy.saveItemLists();
	this->EndDialog(IDOK);
}

BOOL CFoodEdit::OnInitDialog() {
	CDialog::OnInitDialog();
	char buf[64];
	sprintf(buf, "%d", itemId);
	m_ItemID.SetWindowText(buf);
	m_ItemName.SetWindowText(name);
	sprintf(buf, "%d", eatTime);
	m_EatTime.SetWindowText(buf);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CLootAdd dialog

CLootAdd::CLootAdd() : CDialog(CLootAdd::IDD) {
	//{{AFX_DATA_INIT(CLootAdd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CLootAdd::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLootAdd)
	DDX_Control(pDX, IDC_LOOT_ADD_NAME, m_ItemName);
	DDX_Control(pDX, IDC_LOOT_ADD_ID, m_ItemID);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLootAdd, CDialog)
	//{{AFX_MSG_MAP(CLootAdd)
	ON_BN_CLICKED(IDOK, OnCommit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLootAdd message handlers

void CLootAdd::OnCommit() {
	char buf[64];
	CTibiaItemProxy itemProxy;
	m_ItemID.GetWindowText(buf,63); itemId=atoi(buf);
	m_ItemName.GetWindowText(name,60);
	if (itemProxy.getItemsLootedCount() < 200)
		itemProxy.addItemLoot(name, itemId);

	itemProxy.saveItemLists();
	this->EndDialog(IDOK);
}

BOOL CLootAdd::OnInitDialog() {
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
};

/////////////////////////////////////////////////////////////////////////////
// CLootEdit dialog

CLootEdit::CLootEdit(int idIn, char* nameIn)
: CDialog(CLootEdit::IDD) {
	itemId = idIn;
	memcpy(name, nameIn, 59);
	//{{AFX_DATA_INIT(CLootEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CLootEdit::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLootEdit)
	DDX_Control(pDX, IDC_LOOT_EDIT_NAME, m_ItemName);
	DDX_Control(pDX, IDC_LOOT_EDIT_ID, m_ItemID);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLootEdit, CDialog)
	//{{AFX_MSG_MAP(CLootEdit)
	ON_BN_CLICKED(IDOK, OnCommit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLootEdit message handlers

void CLootEdit::OnCommit() {
	char buf[64];
	CTibiaItemProxy itemProxy;
	int index = itemProxy.getIndex(itemId, LOOT);
	m_ItemID.GetWindowText(buf,63); itemId=atoi(buf);
	m_ItemName.GetWindowText(name,60);
	itemProxy.setLootItemId(index, itemId);
	itemProxy.setLootItemName(index, name);
	itemProxy.saveItemLists();
	this->EndDialog(IDOK);
}

BOOL CLootEdit::OnInitDialog() {
	CDialog::OnInitDialog();
	char buf[64];
	sprintf(buf, "%d", itemId);
	m_ItemID.SetWindowText(buf);
	m_ItemName.SetWindowText(name);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



