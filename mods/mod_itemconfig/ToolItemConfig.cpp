// ToolItemConfig.cpp : implementation file
//

#include "stdafx.h"
#include "mod_itemconfig.h"
#include "ToolItemConfig.h"
#include "TibiaItem.h"
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
// CToolItemConfig dialog


CToolItemConfig::CToolItemConfig(CWnd* pParent /*=NULL*/)
: MyDialog(CToolItemConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolItemConfig)
	//}}AFX_DATA_INIT
}


void CToolItemConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolItemConfig)
	DDX_Control(pDX, IDC_FRAME_LOOT, m_LootFrame);
	DDX_Control(pDX, IDC_FRAME_ITEMS, m_ItemsFrame);
	DDX_Control(pDX, IDC_FRAME_FOOD, m_FoodFrame);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG_REFRESH, m_RefreshItems);
	DDX_Control(pDX, IDC_EDIT_LOOT, m_EditLoot);
	DDX_Control(pDX, IDC_EDIT_ITEM, m_EditItem);
	DDX_Control(pDX, IDC_EDIT_FOOD, m_EditFood);
	DDX_Control(pDX, IDC_DELETE_LOOT, m_DeleteLoot);
	DDX_Control(pDX, IDC_DELETE_ITEM, m_DeleteItem);
	DDX_Control(pDX, IDC_DELETE_FOOD, m_DeleteFood);
	DDX_Control(pDX, IDC_ADD_LOOT, m_AddLoot);
	DDX_Control(pDX, IDC_ADD_ITEM, m_AddItem);
	DDX_Control(pDX, IDC_ADD_FOOD, m_AddFood);
	DDX_Control(pDX, IDC_TREE1, m_testTree);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG_LOOTED_LIST, m_lootedList);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG_ITEMS_LIST, m_itemsList);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG_FOODLIST, m_foodList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolItemConfig, CDialog)
	//{{AFX_MSG_MAP(CToolItemConfig)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_TOOL_ITEMCONFIG_REFRESH, OnToolItemconfigRefresh)
	ON_BN_CLICKED(IDC_ADD_ITEM, OnItemAdd)
	ON_BN_CLICKED(IDC_EDIT_ITEM, OnItemEdit)
	ON_BN_CLICKED(IDC_DELETE_ITEM, OnItemDelete)
	ON_BN_CLICKED(IDC_ADD_FOOD, OnFoodAdd)
	ON_BN_CLICKED(IDC_EDIT_FOOD, OnFoodEdit)
	ON_BN_CLICKED(IDC_DELETE_FOOD, OnFoodDelete)
	ON_BN_CLICKED(IDC_ADD_LOOT, OnLootAdd)
	ON_BN_CLICKED(IDC_EDIT_LOOT, OnLootEdit)
	ON_BN_CLICKED(IDC_DELETE_LOOT, OnLootDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolItemConfig message handlers

void CToolItemConfig::OnOK() 
{
	ShowWindow(SW_HIDE);
}

void CToolItemConfig::OnClose() 
{
	ShowWindow(SW_HIDE);
}

void CToolItemConfig::OnToolItemconfigRefresh() 
{
	int i;
	char buf[16384];
	CTibiaItemProxy itemProxy;
	itemProxy.refreshItemLists();
	
	
	while (m_itemsList.GetCount()) m_itemsList.DeleteString(0);
	for (i=0;i<itemProxy.getItemsItemsCount();i++)
	{
		sprintf(buf,"(0x%x) %s",itemProxy.getItemsItemsId(i),itemProxy.getItemsItems(i));
		m_itemsList.AddString(buf);
	}
	
	while (m_foodList.GetCount()) m_foodList.DeleteString(0);
	for (i=0;i<itemProxy.getItemsFoodCount();i++)
	{
		sprintf(buf,"(0x%x) %s",itemProxy.getItemsFoodId(i),itemProxy.getItemsFood(i));
		m_foodList.AddString(buf);
	}
	
	while (m_lootedList.GetCount()) m_lootedList.DeleteString(0);
	for (i=0;i<itemProxy.getItemsLootedCount();i++)
	{
		sprintf(buf,"(0x%x) %s",itemProxy.getItemsLootedId(i),itemProxy.getItemsLooted(i));
		m_lootedList.AddString(buf);
	}

	HTREEITEM item1=m_testTree.InsertItem("hi");;
	while (m_lootedList.GetCount()) m_lootedList.DeleteString(0);
	for (i=0;i<itemProxy.getItemsLootedCount();i++)
	{
		sprintf(buf,"(0x%x) %s",itemProxy.getItemsLootedId(i),itemProxy.getItemsLooted(i));
		m_testTree.InsertItem(buf,item1);
	}
	
	
}

BOOL CToolItemConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_RefreshItems);
	skin.SetButtonSkin(	m_EditLoot);
	skin.SetButtonSkin(	m_EditItem);
	skin.SetButtonSkin(	m_EditFood);
	skin.SetButtonSkin(	m_DeleteLoot);
	skin.SetButtonSkin(	m_DeleteItem);
	skin.SetButtonSkin(	m_DeleteFood);
	skin.SetButtonSkin(	m_AddLoot);
	skin.SetButtonSkin(	m_AddItem);
	skin.SetButtonSkin(	m_AddFood);
	
	OnToolItemconfigRefresh();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CToolItemConfig::OnItemAdd() {
	CItemAdd *dialog = new CItemAdd();
	if (dialog->DoModal() == IDOK) OnToolItemconfigRefresh();
	delete dialog;
}
void CToolItemConfig::OnItemEdit() {
	CTibiaItemProxy item;
	CItemEdit *dialog = new CItemEdit(item.getObjectId(parseNameFromItemSelected(ITEM)), parseNameFromItemSelected(ITEM));
	if (dialog->DoModal() == IDOK) OnToolItemconfigRefresh();
	delete dialog;
}
void CToolItemConfig::OnItemDelete() {
	CTibiaItemProxy itemProxy;
	int loop = itemProxy.getIndex(itemProxy.getObjectId(parseNameFromItemSelected(ITEM)), 1);
	for (; loop < itemProxy.getItemsItemsCount() - 1; loop++) {
		if (loop < itemProxy.getItemsItemsCount()) {
			itemProxy.setItemId(loop, itemProxy.getItemsItemsId(loop + 1));
			itemProxy.setItemName(loop, itemProxy.getItemsItems(loop + 1));
		}
	}
	itemProxy.setItemsItemsCount(itemProxy.getItemsItemsCount() - 1);
	itemProxy.saveItemLists();
	OnToolItemconfigRefresh();
}

void CToolItemConfig::OnFoodAdd() {
	CFoodAdd *dialog = new CFoodAdd();
	if (dialog->DoModal() == IDOK) OnToolItemconfigRefresh();
	delete dialog;
}
void CToolItemConfig::OnFoodEdit() {
	CTibiaItemProxy item;
	int idOut = item.getFoodId(parseNameFromItemSelected(FOOD));
	char *nameOut = parseNameFromItemSelected(FOOD);
	int timeOut = item.getExtraInfo(item.getIndex(idOut, FOOD), FOOD);
	CFoodEdit *dialog = new CFoodEdit(idOut, nameOut, timeOut);
	if (dialog->DoModal() == IDOK) OnToolItemconfigRefresh();
	delete dialog;
}
void CToolItemConfig::OnFoodDelete() {
	CTibiaItemProxy itemProxy;
	int loop = itemProxy.getIndex(itemProxy.getFoodId(parseNameFromItemSelected(FOOD)), 2);
	for (; loop < itemProxy.getItemsFoodCount() - 1; loop++) {
		if (loop < itemProxy.getItemsFoodCount()) {
			itemProxy.setFoodId(loop, itemProxy.getItemsFoodId(loop + 1));
			itemProxy.setFoodName(loop, itemProxy.getItemsFood(loop + 1));
			itemProxy.setExtraInfo(loop, itemProxy.getExtraInfo(loop + 1, FOOD), FOOD);
		}
	}
	itemProxy.setItemsFoodCount(itemProxy.getItemsFoodCount() - 1);
	itemProxy.saveItemLists();
	OnToolItemconfigRefresh();
}

void CToolItemConfig::OnLootAdd() {
	CLootAdd *dialog = new CLootAdd();
	if (dialog->DoModal() == IDOK) OnToolItemconfigRefresh();
	delete dialog;
}
void CToolItemConfig::OnLootEdit() {
	CTibiaItemProxy item;
	CLootEdit *dialog = new CLootEdit(item.getLootItemId(parseNameFromItemSelected(LOOT)), parseNameFromItemSelected(LOOT));
	if (dialog->DoModal() == IDOK) OnToolItemconfigRefresh();
	delete dialog;
}
void CToolItemConfig::OnLootDelete() {
	CTibiaItemProxy itemProxy;
	int loop = itemProxy.getIndex(itemProxy.getObjectId(parseNameFromItemSelected(LOOT)), 3);
	for (; loop < itemProxy.getItemsLootedCount() - 1; loop++) {
		if (loop < itemProxy.getItemsLootedCount()) {
			itemProxy.setLootItemId(loop, itemProxy.getItemsLootedId(loop + 1));
			itemProxy.setLootItemName(loop, itemProxy.getItemsLooted(loop + 1));
		}
	}
	itemProxy.setItemsLootCount(itemProxy.getItemsLootedCount() - 1);
	itemProxy.saveItemLists();
	OnToolItemconfigRefresh();
}

char *CToolItemConfig::parseNameFromItemSelected(int type) {
	if (type == ITEM) {
		int index = m_itemsList.GetCurSel();
		char buf[128];
		m_itemsList.GetText(index, buf);
		for (int k = strlen(buf)-1; k > 0; k--)	{
			if (buf[k] == ')' && buf[k + 1] == ' ') {
				memcpy(outbuf, buf + k + 2, strlen(buf)-k);
			}
		}
		return outbuf;
	}
	if (type == FOOD) {
		int index = m_foodList.GetCurSel();
		char buf[128];
		m_foodList.GetText(index, buf);
		for (int k = strlen(buf)-1; k > 0; k--)	{
			if (buf[k] == ')' && buf[k + 1] == ' ') {
				memcpy(outbuf, buf + k + 2, strlen(buf)-k);
			}
		}
		return outbuf;
	}
	if (type == LOOT) {
		int index = m_lootedList.GetCurSel();
		char buf[128];
		m_lootedList.GetText(index, buf);
		for (int k = strlen(buf)-1; k > 0; k--)	{
			if (buf[k] == ')' && buf[k + 1] == ' ') {
				memcpy(outbuf, buf + k + 2, strlen(buf)-k);
			}
		}
		return outbuf;
	}
	return NULL;
}
