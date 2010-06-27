// ItemAdd.cpp : implementation file
//

#include "stdafx.h"
#include "mod_itemconfig.h"
#include "ItemConfig.h"
#include "TibiaTile.h"
#include "memReaderProxy.h"
#include "TibiaItemProxy.h"
#include <math.h>
#include "TibiaStructures.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CItemAdd dialog

CItemAdd::CItemAdd(CTreeCtrl* treeIn, HTREEITEM itemIn)
: MyDialog(CItemAdd::IDD) {
	tree=treeIn;
	item=itemIn;
	
	//{{AFX_DATA_INIT(CItemAdd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CItemAdd::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemAdd)
    DDX_Control(pDX, IDC_TYPE, m_ItemType);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_ITEM_ADD_NAME, m_ItemName);
	DDX_Control(pDX, IDC_ITEM_ADD_ID, m_ItemID);
	DDX_Control(pDX, IDC_ITEM_ADD_BRANCH, m_ItemBranch);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CItemAdd, CDialog)
	//{{AFX_MSG_MAP(CItemAdd)
	ON_BN_CLICKED(IDC_ITEM_ADD_BRANCH, OnAddBranch)
	ON_BN_CLICKED(IDOK, OnCommit)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemAdd message handlers

void CItemAdd::OnCommit() {

	//Add as child if item is a branch, otherwise add as sibling
	HTREEITEM parent;
	if (item==TVI_ROOT || tree->ItemHasChildren(item) || tree->GetItemData(item)==0){
		parent=item;
	} else {
		parent = tree->GetParentItem(item);
		if (parent==NULL) parent=TVI_ROOT;
	}
	HTREEITEM newItem;
	if(!m_ItemBranch.GetCheck()){//Item node
		char buf[64];
		m_ItemID.GetWindowText(buf,63);
		itemId=0;
		sscanf(buf,"0x%x",&itemId);
		if (!itemId) sscanf(buf,"%d",&itemId);

		itemType=pow(2, m_ItemType.GetCurSel());

		m_ItemName.GetWindowText(name,1023);
		char* txt=(char*)malloc(strlen(name)+10);
		sprintf(txt,"%s[%d]",name,itemId);
		CGUITreeItemData *itemData=new CGUITreeItemData (itemId,itemType);
		newItem=tree->InsertItem(TVIF_STATE|TVIF_PARAM|TVIF_TEXT|TVIF_HANDLE,txt,0,0,0,0,(long)itemData,parent,TVI_LAST);
		free(txt);
	}else{// Branch node
		m_ItemName.GetWindowText(name,1023);
		newItem=tree->InsertItem(name,parent,TVI_LAST);
		tree->SetItemState(newItem,TVIS_BOLD,TVIS_BOLD);
	}

	tree->SelectItem(newItem);//ToolConfign function looks at selected item to see what was added
	
	this->EndDialog(IDOK);
}

BOOL CItemAdd::OnInitDialog() {
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_Cancel);

    CBitmap items; 
    items.LoadBitmap(IDB_ITEMS);
    m_itemImg.Create(14, 14, ILC_COLOR32 | ILC_MASK, 0, 0);
    m_itemImg.Add(&items, RGB(255, 0, 255));
    // Set type dropdown settings
    m_ItemType.SetColumnCount(0);
    m_ItemType.SetUseImage(true);
    m_ItemType.SetImageList(&m_itemImg);

    //m_ItemType.AddString("Select a type");
    m_ItemType.SetItemImage(m_ItemType.AddString("Axes"), 10);
    m_ItemType.SetItemImage(m_ItemType.AddString("Clubs"), 9);
    m_ItemType.SetItemImage(m_ItemType.AddString("Swords"), 6);
    m_ItemType.SetItemImage(m_ItemType.AddString("Distance Weapons"), 7);
    m_ItemType.SetItemImage(m_ItemType.AddString("Wands and Rods"), 16);
    m_ItemType.SetItemImage(m_ItemType.AddString("Throwables"), 15);
    m_ItemType.SetItemImage(m_ItemType.AddString("Ammunition"), 14);
    m_ItemType.SetItemImage(m_ItemType.AddString("Amulets and Necklaces"), 4);
    m_ItemType.SetItemImage(m_ItemType.AddString("Armor"), 13);
    m_ItemType.SetItemImage(m_ItemType.AddString("Boots"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Helmets"), 12);
    m_ItemType.SetItemImage(m_ItemType.AddString("Legs"), 11);
    m_ItemType.SetItemImage(m_ItemType.AddString("Shields"), 8);
    m_ItemType.SetItemImage(m_ItemType.AddString("Rings"), 3);
    m_ItemType.SetItemImage(m_ItemType.AddString("Runes"), 2);
    m_ItemType.SetItemImage(m_ItemType.AddString("Flasks"), 5);
    m_ItemType.SetItemImage(m_ItemType.AddString("Containers"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Creature Products"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Gamepieces"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Instruments"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Keys"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Paperwork"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Quest Items"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Tools"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Trophies"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Valuables"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Foods"), 1);
    m_ItemType.SetItemImage(m_ItemType.AddString("Misc. Loots"), 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CItemAdd::OnAddBranch()
{
	m_ItemID.EnableWindow(!m_ItemBranch.GetCheck());
	itemId=0;
	m_ItemID.SetWindowText("");
}

/////////////////////////////////////////////////////////////////////////////
// CItemEdit dialog

CItemEdit::CItemEdit(CTreeCtrl* treeIn, HTREEITEM itemIn)
: MyDialog(CItemEdit::IDD) {
	tree=treeIn;
	item=itemIn;
	if (!tree->ItemHasChildren(item) && tree->GetItemData(item)!=0){//Item node
		CGUITreeItemData* itemData = (CGUITreeItemData*)tree->GetItemData(item);
		itemId=itemData->id;
		itemType=itemData->type;
		CString cText=tree->GetItemText(item);
		int ind=min(cText.GetLength(),1023,(unsigned int)cText.ReverseFind('['));
		memcpy(name,cText,ind);
		name[ind]=0;
	} else {//Branch node
		CString cText=tree->GetItemText(item);
		int ind=min(cText.GetLength(),1023,(unsigned int)cText.ReverseFind('['));
		memcpy(name,cText,ind);
		name[ind]=0;
		itemId=0;
		itemType=0;
	}

	//{{AFX_DATA_INIT(CItemEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CItemEdit::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemEdit)
    DDX_Control(pDX, IDC_TYPE, m_ItemType);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_ITEM_EDIT_NAME, m_ItemName);
	DDX_Control(pDX, IDC_ITEM_EDIT_ID, m_ItemID);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CItemEdit, CDialog)
	//{{AFX_MSG_MAP(CItemEdit)
	ON_BN_CLICKED(IDOK, OnCommit)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemEdit message handlers

void CItemEdit::OnCommit() {

	if(m_ItemID.IsWindowEnabled()){//Item node

		char buf[64];
		m_ItemID.GetWindowText(buf,63);
		itemId=0;
		sscanf(buf,"0x%x",&itemId);
		if (!itemId) itemId=atoi(buf);

		m_ItemName.GetWindowText(name,1023);
		char* txt=(char*)malloc(strlen(name)+10);
		sprintf(txt,"%s[%d]",name,itemId);

		itemType=pow(2, m_ItemType.GetCurSel());

		CGUITreeItemData* itemData = (CGUITreeItemData*)tree->GetItemData(item);
		itemData->id=itemId;
		itemData->type=itemType;
		tree->SetItemText(item,txt);
		free(txt);
	}else{// Branch node
		m_ItemName.GetWindowText(name,1023);
		tree->SetItemText(item,name);
	}


	this->EndDialog(IDOK);
}

BOOL CItemEdit::OnInitDialog() {
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_Cancel);

    m_itemImg.Create(14, 14, ILC_COLOR32 | ILC_MASK, 0, 0);

    CBitmap items; //Images 38 & 39
    items.LoadBitmap(IDB_ITEMS);
    m_itemImg.Add(&items, RGB(255, 0, 255));
    // Set type dropdown settings
    m_ItemType.SetColumnCount(0);
    m_ItemType.SetUseImage(true);
    m_ItemType.SetImageList(&m_itemImg);

    //m_ItemType.AddString("Select a type");
    m_ItemType.SetItemImage(m_ItemType.AddString("Axes"), 10);
    m_ItemType.SetItemImage(m_ItemType.AddString("Clubs"), 9);
    m_ItemType.SetItemImage(m_ItemType.AddString("Swords"), 6);
    m_ItemType.SetItemImage(m_ItemType.AddString("Distance Weapons"), 7);
    m_ItemType.SetItemImage(m_ItemType.AddString("Wands and Rods"), 16);
    m_ItemType.SetItemImage(m_ItemType.AddString("Throwables"), 15);
    m_ItemType.SetItemImage(m_ItemType.AddString("Ammunition"), 14);
    m_ItemType.SetItemImage(m_ItemType.AddString("Amulets and Necklaces"), 4);
    m_ItemType.SetItemImage(m_ItemType.AddString("Armor"), 13);
    m_ItemType.SetItemImage(m_ItemType.AddString("Boots"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Helmets"), 12);
    m_ItemType.SetItemImage(m_ItemType.AddString("Legs"), 11);
    m_ItemType.SetItemImage(m_ItemType.AddString("Shields"), 8);
    m_ItemType.SetItemImage(m_ItemType.AddString("Rings"), 3);
    m_ItemType.SetItemImage(m_ItemType.AddString("Runes"), 2);
    m_ItemType.SetItemImage(m_ItemType.AddString("Flasks"), 5);
    m_ItemType.SetItemImage(m_ItemType.AddString("Containers"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Creature Products"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Gamepieces"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Instruments"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Keys"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Paperwork"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Quest Items"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Tools"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Trophies"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Valuables"), 0);
    m_ItemType.SetItemImage(m_ItemType.AddString("Foods"), 1);
    m_ItemType.SetItemImage(m_ItemType.AddString("Misc. Loots"), 0);

	if (itemId){//Item node
		char buf[64];
		sprintf(buf, "%d", itemId);
		m_ItemID.SetWindowText(buf);
        int index = log10((double)itemType)/log10(2);
        m_ItemType.SetCurSel(index);
	} else{ //Branch node
		m_ItemID.EnableWindow(FALSE);
		m_ItemType.EnableWindow(FALSE);
	}
	m_ItemName.SetWindowText(name);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CFoodAdd dialog

CFoodAdd::CFoodAdd(CListBox* listIn) : MyDialog(CFoodAdd::IDD) {
	list=listIn;
	//{{AFX_DATA_INIT(CFoodAdd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CFoodAdd::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFoodAdd)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_FOOD_ADD_NAME, m_ItemName);
	DDX_Control(pDX, IDC_FOOD_ADD_ID, m_ItemID);
	DDX_Control(pDX, IDC_FOOD_ADD_TIME, m_EatTime);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFoodAdd, CDialog)
	//{{AFX_MSG_MAP(CFoodAdd)
	ON_BN_CLICKED(IDOK, OnCommit)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFoodAdd message handlers

void CFoodAdd::OnCommit() {
	char buf[64];

	m_ItemID.GetWindowText(buf,63);
	itemId=0;
	sscanf(buf,"0x%x",&itemId);
	if (!itemId) sscanf(buf,"%d",&itemId);

	m_ItemName.GetWindowText(name,511);

	m_EatTime.GetWindowText(buf,63); eatTime=atoi(buf);

	char* txt=(char*)malloc(strlen(name)+10);
	sprintf(txt,"%s[%d]",name,itemId);
	int ind=list->AddString(txt);
	list->SetItemData(ind,eatTime);
	free(txt);
	list->SetCurSel(ind);

	this->EndDialog(IDOK);
}

BOOL CFoodAdd::OnInitDialog() {
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_Cancel);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
};

/////////////////////////////////////////////////////////////////////////////
// CFoodEdit dialog

CFoodEdit::CFoodEdit(int idIn, char* nameIn, int timeIn, CListBox* listIn,int indexIn)
: MyDialog(CFoodEdit::IDD) {
	itemId = idIn;
	name[511]='\0';
	memcpy(name, nameIn, 511);
	eatTime = timeIn;
	list=listIn;
	index=indexIn;
	//{{AFX_DATA_INIT(CFoodEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CFoodEdit::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFoodEdit)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_FOOD_EDIT_NAME, m_ItemName);
	DDX_Control(pDX, IDC_FOOD_EDIT_ID, m_ItemID);
	DDX_Control(pDX, IDC_FOOD_EDIT_TIME, m_EatTime);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFoodEdit, CDialog)
	//{{AFX_MSG_MAP(CFoodEdit)
	ON_BN_CLICKED(IDOK, OnCommit)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFoodEdit message handlers

void CFoodEdit::OnCommit() {
	char buf[64];
	m_ItemID.GetWindowText(buf,63);
	itemId=0;
	sscanf(buf,"0x%x",&itemId);
	if (!itemId) sscanf(buf,"%d",&itemId);

	m_ItemName.GetWindowText(name,511);
	char* txt=(char*)malloc(strlen(name)+10);
	sprintf(txt,"%s[%d]",name,itemId);

	m_EatTime.GetWindowText(buf,63); eatTime=atoi(buf);

	list->DeleteString(index);
	list->InsertString(index,txt);
	list->SetItemData(index,eatTime);
	free(txt);

	this->EndDialog(IDOK);
}

BOOL CFoodEdit::OnInitDialog() {
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_Cancel);
	char buf[64];
	sprintf(buf, "%d", itemId);
	m_ItemID.SetWindowText(buf);
	m_ItemName.SetWindowText(name);
	sprintf(buf, "%d", eatTime);
	m_EatTime.SetWindowText(buf);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
};
