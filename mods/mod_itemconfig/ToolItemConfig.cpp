// ToolItemConfig.cpp : implementation file
//

#include "stdafx.h"
#include "mod_itemconfig.h"
#include "TibiaItem.h"
#include "TibiaItemProxy.h"
#include "ToolItemConfig.h"

#include "TibiaStructures.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int DEF_MASK=TVIF_STATE|TVIF_PARAM|TVIF_TEXT|TVIF_HANDLE;//TVIF_CHILDREN|TVIF_DI_SETITEM|TVIF_HANDLE|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE|TVIF_TEXT;
const int DEF_STATE_MASK=0;
/////////////////////////////////////////////////////////////////////////////
// CToolItemConfig dialog

static int CreateGUITree(CTreeCtrl* treeCtrl,HTREEITEM guiTree,CTibiaTree* dataTree);
static void CreateDataTree(CTibiaTree* dataTree,CTreeCtrl* treeCtrl,HTREEITEM guiTree);
static void SetParentsCheckBold(CTreeCtrl* treeCtrl,HTREEITEM treeItem,BOOL check);
static void SetChildrenCheckBold(CTreeCtrl* treeCtrl,HTREEITEM treeItem,BOOL check);

CToolItemConfig::CToolItemConfig(CWnd* pParent /*=NULL*/)
: MyDialog(CToolItemConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolItemConfig)
	//}}AFX_DATA_INIT
	hWnd=pParent->GetSafeHwnd();
	Dragging=FALSE;

}


void CToolItemConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolItemConfig)
	DDX_Control(pDX, IDC_ITEMCONFIG_ITEMSTREE, m_itemsTree);
	DDX_Control(pDX, IDC_ADD_ITEM, m_AddItem);
	DDX_Control(pDX, IDC_FRAME_ITEMS, m_ItemsFrame);
	DDX_Control(pDX, IDC_FRAME_FOOD, m_FoodFrame);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG_REFRESH, m_RefreshItems);
	DDX_Control(pDX, IDC_DELETE_FOOD, m_DeleteFood);
	DDX_Control(pDX, IDC_EDIT_FOOD, m_EditFood);
	DDX_Control(pDX, IDC_ADD_FOOD, m_AddFood);
	DDX_Control(pDX, IDC_DELETE_ITEM, m_DeleteItem);
	DDX_Control(pDX, IDC_EDIT_ITEM, m_EditItem);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG_FOODLIST, m_foodList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolItemConfig, CDialog)
	//{{AFX_MSG_MAP(CToolItemConfig)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_TOOL_ITEMCONFIG_REFRESH, OnToolItemconfigRefresh)
	ON_BN_CLICKED(IDC_EDIT_ITEM, OnItemEdit)
	ON_BN_CLICKED(IDC_DELETE_ITEM, OnItemDelete)
	ON_BN_CLICKED(IDC_ADD_FOOD, OnFoodAdd)
	ON_BN_CLICKED(IDC_EDIT_FOOD, OnFoodEdit)
	ON_BN_CLICKED(IDC_DELETE_FOOD, OnFoodDelete)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_ADD_ITEM, OnAddItem)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(TVN_SELCHANGED, IDC_ITEMCONFIG_ITEMSTREE, OnSelchangedTree)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_ITEMCONFIG_ITEMSTREE, OnBegindragTree)
	ON_NOTIFY(NM_CLICK, IDC_ITEMCONFIG_ITEMSTREE, OnClickTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolItemConfig message handlers

void CToolItemConfig::OnOK()
{
	// Applies changes to the lists only after OK is clicked.
	// Recreates entire list and tree from the GUI
	CTibiaItemProxy itemProxy;
	CTibiaTree* dataTree=new CTibiaTree(new CTibiaTreeBranchData("Root"));
	CreateDataTree(dataTree,&m_itemsTree,TVI_ROOT);

	itemProxy.setItemsTree(dataTree);
	
	itemProxy.clearFoodList();
	int count=m_foodList.GetCount();
	for (int i=0;i<count;i++){
		char text[30000];
		m_foodList.GetText(i,text);
		int id=parseId(text);
		char* name=parseName(text);
		int eatTime = m_foodList.GetItemData(i);
		itemProxy.addFood(name,id,eatTime);
		free(name);
	}

	itemProxy.saveItemLists();
	ShowWindow(SW_HIDE);
}

void CToolItemConfig::OnClose()
{
	ShowWindow(SW_HIDE);
}

static int CreateGUITree(CTreeCtrl* treeCtrl,HTREEITEM guiTree,CTibiaTree* dataTree){
	//recirsively ensures all children of dataTree are added as children of guiTree
	int size=dataTree->children.size();
	int ret=-1;
	for (int i=0;i<size;i++){
		CTibiaTree* child = dataTree->children[i];
		int val;
		if (child->children.size()==0 && child->data->GetType()==TT_ITEM_NODE){//Item node
			char* txt=NULL;
			CTibiaTreeItemData* data=(CTibiaTreeItemData*)child->data;
			txt=(char*)malloc(strlen(data->GetName())+10);
			sprintf(txt,"%s[%d]",data->GetName(),data->GetId());

			HTREEITEM newItem = treeCtrl->InsertItem(DEF_MASK,txt, 0,0,0,0,data->GetId(),guiTree, TVI_LAST);

			treeCtrl->SetCheck(newItem,data->IsLooted());
			val=data->IsLooted()?2:0;
		}else if(child->children.size()!=0 && child->data->GetType()==TT_BRANCH_NODE){//Branch node
			CTibiaTreeBranchData* data=(CTibiaTreeBranchData*)child->data;

			HTREEITEM newItem=treeCtrl->InsertItem(data->GetName(),guiTree, TVI_LAST);
			val=CreateGUITree(treeCtrl,newItem,child);//return value indicates if children are checked

			//replace when figured out pictures
			if (val==2){//all
				treeCtrl->SetItemState(newItem,TVIS_BOLD,TVIS_BOLD);
				treeCtrl->SetCheck(newItem,TRUE);
			} else if (val==1){//not all but some
				treeCtrl->SetItemState(newItem,NULL,TVIS_BOLD);
				treeCtrl->SetCheck(newItem,TRUE);
			}else if (val==0){//none
				treeCtrl->SetItemState(newItem,NULL,TVIS_BOLD);
				treeCtrl->SetCheck(newItem,FALSE);
			}
		} else { continue; } //Empty Branch
		if (val!=-1){
			if (ret==-1) ret=val;
			ret=(val==ret?ret:1);//change value to 1 if not everything either 0 or 2
		}
	}
	return ret;
}

static void CreateDataTree(CTibiaTree* dataTree,CTreeCtrl* treeCtrl,HTREEITEM guiTree){
	//recirsively ensures all children of guiTree are added as children of dataTree
	HTREEITEM child=treeCtrl->GetChildItem(guiTree);
	while (child!=NULL){
		if (treeCtrl->ItemHasChildren(child) && treeCtrl->GetItemData(child)==0){//Item node
			CString cText=treeCtrl->GetItemText(child);
			char* text=(char *)(LPCTSTR)cText;
			CTibiaTree* newTree=dataTree->AddChild(new CTibiaTreeBranchData(text));

			CreateDataTree(newTree,treeCtrl,child);
		} else if (!treeCtrl->ItemHasChildren(child) && treeCtrl->GetItemData(child)!=0) {//Branch node
			int id=treeCtrl->GetItemData(child);
			CString cText=treeCtrl->GetItemText(child);
			char* text=(char *)(LPCTSTR)cText;
			//Parse text
			for (int i=strlen(text)-1;i>=0;i--){
				if (text[i]=='['){
					text[i]='\0';
					break;
				}
			}
			dataTree->AddChild(new CTibiaTreeItemData(text,id,treeCtrl->GetCheck(child)!=0));
		} else{//empty branch case
		}

		child=treeCtrl->GetNextSiblingItem(child);//iterate
	}
}

void CToolItemConfig::OnToolItemconfigRefresh()
{
	char buf[16384];
	CTibiaItemProxy itemProxy;
	itemProxy.refreshItemLists();
	
	//Create Food List
	while (m_foodList.GetCount()) m_foodList.DeleteString(0);
	int size = itemProxy.getFoodCount();
	for (int i=0;i<size;i++)
	{
		sprintf(buf,"%s[%d]",itemProxy.getFoodNameAtIndex(i),itemProxy.getFoodIdAtIndex(i));
		m_foodList.AddString(buf);
		m_foodList.SetItemData(i,itemProxy.getFoodTimeAtIndex(i));
	}
	//m_itemsTree.ModifyStyle(NULL, TVS_CHECKBOXES);
	//m_itemsTree.ModifyStyle(NULL,TVS_HASBUTTONS | TVS_HASLINES | TVS_EDITLABELS | TVS_SHOWSELALWAYS | TVS_NOTOOLTIPS | TVS_TRACKSELECT | WS_BORDER | TVS_LINESATROOT | WS_TABSTOP);

	//Create Item Tree 
	m_itemsTree.ModifyStyle(TVS_DISABLEDRAGDROP,TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_CHECKBOXES | TVS_TRACKSELECT);

	CTibiaTree* itemsTree=(CTibiaTree*)itemProxy.getItemsTree();

	m_itemsTree.DeleteAllItems();
	CreateGUITree(&m_itemsTree,TVI_ROOT,itemsTree);
	

	//CImageList imgList;
	//imgList.Create(IDB_BITMAP1,16,1,RGB(0,0,255));
	/*
	imgList.Create(13,13,ILC_COLOR8|ILC_MASK,3,1);

	CBitmap bitmap1;
	bitmap1.LoadBitmap(IDB_UNCHECK);
	imgList.Add(&bitmap1,RGB(0,0,255));

	CBitmap bitmap2;
	bitmap2.LoadBitmap(IDB_HALFCHECK);
	imgList.Add(&bitmap2,RGB(0,0,255));

	CBitmap bitmap3;
	bitmap3.LoadBitmap(IDB_CHECK);
	imgList.Add(&bitmap3,RGB(0,0,255));
*/
	//m_itemsTree.SetImageList(&imgList,TVSIL_NORMAL);

	//HTREEITEM root=m_itemsTree.InsertItem(	DEF_MASK, "All Items", 1,0, 0, DEF_STATE_MASK, 0,TVI_ROOT, TVI_LAST);

/*

	while (m_lootedList.GetCount()) m_lootedList.DeleteString(0);
	for (i=0;i<itemProxy.getItemsLootedCount();i++)
	{
		sprintf(buf,"%s (#%d)",itemProxy.getItemsLooted(i),itemProxy.getItemsLootedId(i));
		m_lootedList.AddString(buf);
		if (i>10)
			m_itemsTree.InertItem(buf,1,0,m_itemsTree.GetChildItem(m_itemsTree.GetRootItem()));
			
		else if (i>3)
			m_itemsTree.InsertItem(buf,0,1,m_itemsTree.GetRootItem());
		else
			m_itemsTree.InsertItem(buf,0,1);
	}
	
	HTREEITEM item1=m_itemsTree.InsertItem("hi");;
	for (i=0;i<itemProxy.getItemsLootedCount();i++)
	{
		sprintf(buf,"(0x%x) %s",itemProxy.getItemsLootedId(i),itemProxy.getItemsLooted(i));
		m_itemsTree.InsertItem(buf,item1);
	}
*/	
	
}



BOOL CToolItemConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_RefreshItems);
	skin.SetButtonSkin(	m_EditFood);
	skin.SetButtonSkin(	m_DeleteFood);
	skin.SetButtonSkin(	m_AddFood);
	skin.SetButtonSkin(	m_AddItem);
	skin.SetButtonSkin(	m_EditItem);
	skin.SetButtonSkin(	m_DeleteItem);
	
	OnToolItemconfigRefresh();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CToolItemConfig::OnFoodAdd() {
	CFoodAdd *dialog = new CFoodAdd(&m_foodList);
	dialog->DoModal();
	delete dialog;
}
void CToolItemConfig::OnFoodEdit() {
	int index = m_foodList.GetCurSel();

	char text[30000];
	m_foodList.GetText(index,text);
	int id = parseId(text);
	char *name = parseName(text);

	int time = m_foodList.GetItemData(index);

	CFoodEdit *dialog = new CFoodEdit(id, name, time,&m_foodList,index);
	dialog->DoModal();
	delete dialog;

	free(name);
}
void CToolItemConfig::OnFoodDelete() {
	int index = m_foodList.GetCurSel();
	m_foodList.DeleteString(index);
	m_foodList.SetCurSel(index);
}

void CToolItemConfig::OnAddItem() 
{
	HTREEITEM item=m_itemsTree.GetSelectedItem();
	CItemAdd *dialog;
	dialog = new CItemAdd(&m_itemsTree,item);
	dialog->DoModal();
	delete dialog;
}

void CToolItemConfig::OnItemEdit() {
	HTREEITEM item=m_itemsTree.GetSelectedItem();
	CItemEdit *dialog;
	if (!m_itemsTree.ItemHasChildren(item) && m_itemsTree.GetItemData(item)!=0){//Branch node
		int id = m_itemsTree.GetItemData(item);
		CString cText=m_itemsTree.GetItemText(item);
		char* text=(char *)(LPCTSTR)cText;
		char* name = parseName(text);
		dialog = new CItemEdit(id,name,&m_itemsTree,item);
		dialog->DoModal();
		free(name);
	} else {//Item node
		CString cText=m_itemsTree.GetItemText(item);
		char* text=(char *)(LPCTSTR)cText;
		dialog = new CItemEdit(0,text,&m_itemsTree,item);
		dialog->DoModal();
	}
	delete dialog;
}

void CToolItemConfig::OnItemDelete(){
	HTREEITEM item = m_itemsTree.GetSelectedItem();
	if (item==NULL) return;
	HTREEITEM selItem= m_itemsTree.GetNextSiblingItem(item);
	if (selItem==NULL) selItem=m_itemsTree.GetParentItem(item);
	if (m_itemsTree.ItemHasChildren(item) && m_itemsTree.GetItemData(item)==0){//Branch node
		if(AfxMessageBox("Are you sure you want to delete this branch and all its items?",MB_OKCANCEL)==IDOK) 
			m_itemsTree.DeleteItem(item);
	} else {//Item node
		m_itemsTree.DeleteItem(item);
	}

	if (selItem!=NULL){
		m_itemsTree.SetItemState(selItem,TVIS_SELECTED,TVIS_SELECTED);
		m_itemsTree.SelectItem(selItem);
	}
	
}

char *CToolItemConfig::parseName(char* name) {
	char *buf=NULL;
	int len=strlen(name);
	buf=(char*)malloc(len+1);
	buf[0]='\0';

	for (int k = len; k>=0; k--){
		
		if (name[k] == '['){
			buf [k] = '\0';
			memcpy(buf, name, k);
			break;
		}
	}
	return buf;
}
int CToolItemConfig::parseId(char* name) {
	int len=strlen(name);
	for (int k = len; k>=0; k--){
		if (name[k] == '['){
			return atoi((char*)(name+k+1));
		}
	}
	return 0;
}

void CToolItemConfig::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	return;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM treeItem=pNMTreeView->itemNew.hItem;
	
	//Test playground
	// TODO: Add your control notification handler code here
	m_itemsTree.SetItemState(treeItem,TVIS_SELECTED,TVIS_SELECTED);

	char buf[1111];
	sprintf(buf,"Selection Changed:%s",m_itemsTree.GetItemText(treeItem));
	//AfxMessageBox(buf);
	return;
	//Single child case
	//HTREEITEM treeItem=m_itemsTree.GetSelectedItem();
	if (treeItem){
		BOOL isChecked=m_itemsTree.GetCheck(treeItem);

		CImageList imgList;
		imgList.Create(12,12,ILC_COLOR8,3,0);

		CBitmap bitmap1;
		bitmap1.LoadBitmap(IDB_UNCHECK);
		imgList.Add(&bitmap1,RGB(0,0,255));
		CBitmap bitmap2;
		bitmap2.LoadBitmap(IDB_CHECK);
		imgList.Add(&bitmap2,RGB(0,0,255));
		CBitmap bitmap3;
		bitmap3.LoadBitmap(IDB_HALFCHECK);
		imgList.Add(&bitmap3,RGB(0,0,255));

		//m_itemsTree.SetItemImage(treeItem,(int)imgList2.ExtractIcon(0),);
		//m_itemsTree.SetItemImage(treeItem,(int)imgList.ExtractIcon(isChecked),(int)imgList.ExtractIcon(isChecked));

		//m_itemsTree.SetCheck(treeItem,!isChecked);
		//m_itemsTree.Expand(treeItem,1);
		//m_itemsTree.Expand(treeItem,1);
		//m_itemsTree.SelectItem(0);
		//m_itemsTree.SetIndent(40);
		//m_itemsTree.SetIndent(40);
		//m_itemsTree.SetTextColor(RGB(255,0,0));
		//m_itemsTree.SetItemState(treeItem,(isChecked<<3),0x100-1);
		int nImage=-5;
		//m_itemsTree.SetBkColor(RGB(255,255,0));
		//m_itemsTree.SetItemHeight(40);
		int nSelectedImage=-5;
		//m_itemsTree.SetItemData(treeItem,2);
		m_itemsTree.GetItemImage(treeItem, nImage,nSelectedImage);
		//m_itemsTree.EditLabel(treeItem);
		//m_itemsTree.SetInsertMark(treeItem,TRUE);
	}
	*pResult = 0;
}

void SetChildrenCheckBold(CTreeCtrl* treeCtrl,HTREEITEM treeItem,BOOL check){
	//Recursively sets children as checked & bold if check or not checkednor bold if !check
	treeCtrl->SetCheck(treeItem,check);
	HTREEITEM item=treeCtrl->GetNextItem(treeItem,TVGN_CHILD);
	if (item!=NULL){
		treeCtrl->SetItemState(treeItem,check?TVIS_BOLD:0,TVIS_BOLD);
	}
	while (item!=NULL){
		SetChildrenCheckBold(treeCtrl,item,check);
		item=treeCtrl->GetNextItem(item,TVGN_NEXT);
	}
}

void SetParentsCheckBold(CTreeCtrl* treeCtrl,HTREEITEM treeItem,BOOL check){
	//Recursively looks at the parents of treeItem and check if all children are checked
	if (check){
		//At least one child is checked so only need to check for Boldness
		HTREEITEM parent=treeCtrl->GetNextItem(treeItem,TVGN_PARENT);
		if(parent==NULL) return;
		int keepBold=1;
		HTREEITEM item=treeCtrl->GetNextItem(parent,TVGN_CHILD);
		while(item!=NULL && keepBold){
			BOOL aa=treeCtrl->ItemHasChildren(item);
			int bb=treeCtrl->GetItemState(item,TVIS_BOLD)&TVIS_BOLD;
			keepBold&=!treeCtrl->ItemHasChildren(item) || treeCtrl->GetItemState(item,TVIS_BOLD)&TVIS_BOLD;
			keepBold&=treeCtrl->GetCheck(item);
			item=treeCtrl->GetNextItem(item,TVGN_NEXT);
		}
		if (keepBold) treeCtrl->SetItemState(parent,TVIS_BOLD,TVIS_BOLD);
		treeCtrl->SetCheck(parent,TRUE);
		SetParentsCheckBold(treeCtrl,parent,check);
	}else{
		//At least one child is unchecked so only need to check if the parent is checked
		HTREEITEM parent=treeCtrl->GetNextItem(treeItem,TVGN_PARENT);
		if(parent==NULL) return;
		int keepCheck=0;
		HTREEITEM item=treeCtrl->GetNextItem(parent,TVGN_CHILD);
		while(item!=NULL && !keepCheck){
			keepCheck|=treeCtrl->GetCheck(item);
			item=treeCtrl->GetNextItem(item,TVGN_NEXT);
		}
		if (!keepCheck) treeCtrl->SetCheck(parent,FALSE);
		treeCtrl->SetItemState(parent,0,TVIS_BOLD);
		SetParentsCheckBold(treeCtrl,parent,check);

	}
}

void CToolItemConfig::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//If tree is clicked, check the location of the click and see if it was on a checkbox
	POINT pt;
	GetCursorPos(&pt);
	m_itemsTree.ScreenToClient(&pt);
	TVHITTESTINFO hti;
	hti.pt.x = pt.x;
	hti.pt.y = pt.y;
	m_itemsTree.HitTest(&hti);

	if (!(hti.flags & TVHT_ONITEMSTATEICON && hti.hItem)) return;
	HTREEITEM treeItem=hti.hItem;
	char buf[1111];
	sprintf(buf,"Clicked on:%s",m_itemsTree.GetItemText(treeItem));
	//AfxMessageBox(buf);
	BOOL isChecked=m_itemsTree.GetCheck(treeItem);
	BOOL isBold=m_itemsTree.GetItemState(treeItem,TVIS_BOLD)&TVIS_BOLD;


	sprintf(buf,"Current Selection:%s",m_itemsTree.GetItemText(m_itemsTree.GetSelectedItem()));
	//AfxMessageBox(buf);
	m_itemsTree.SelectItem(treeItem);
	
	if(isChecked){//Uncheck
		SetChildrenCheckBold(&m_itemsTree,treeItem,FALSE);
		SetParentsCheckBold(&m_itemsTree,treeItem,FALSE);
		m_itemsTree.SetCheck(treeItem,TRUE);//Make checked so MFC will uncheck
	}else{//Check
		SetChildrenCheckBold(&m_itemsTree,treeItem,TRUE);
		SetParentsCheckBold(&m_itemsTree,treeItem,TRUE);
		m_itemsTree.SetCheck(treeItem,FALSE);//Make unchecked so MFC will check
	}

	*pResult = 0;
}

void CToolItemConfig::OnBegindragTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	char buf[111];

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	//Image list for drag image
    CImageList* imList=new CImageList();

	itemOrigin=pNMTreeView->itemNew.hItem;
	//imList=m_itemsTree.CreateDragImage(itemOrigin);

	
	imList->Create(13,13,ILC_COLOR8|ILC_MASK,1,1);
	CBitmap bitmap1;
	bitmap1.LoadBitmap(IDB_UNCHECK);//uses temporary indicator until CreateDragImage magically starts working
	imList->Add(&bitmap1,RGB(0,0,255));

	imList->BeginDrag(0,CPoint(0,0));
	ImageList_DragEnter(m_itemsTree,pNMTreeView->ptDrag.x,pNMTreeView->ptDrag.y);
	ShowCursor(FALSE);
	SetCapture();
	
	delete imList;
	Dragging=TRUE;
	htDest.hItem=NULL;
	*pResult = 0;
}

void CToolItemConfig::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	TVHITTESTINFO tvht;
	int xOffset=-6;//-24;
	int yOffset=-6;//-151;
	
	CDialog::OnMouseMove(nFlags, point);
	if (Dragging){
		//Moves Drag Image to cursor position
		this->ClientToScreen(&point);
		((CWnd*)&m_itemsTree)->ScreenToClient(&point);
		CImageList::DragMove(CPoint(point.x+xOffset,point.y+yOffset));
		//CImageList::DragShowNolock(FALSE);//stops drawing drag image to draw the TreeInsertMark
		htDest.pt.x = point.x;
		htDest.pt.y = point.y;

		//Test a second point above cursor
		//If still on the same item then we are in bottom half, otherwise we are in the top half of the item
		tvht.pt.x = point.x;
		tvht.pt.y = point.y-10;

		m_itemsTree.HitTest(&htDest);
		m_itemsTree.HitTest(&tvht);

		insertAfter=htDest.hItem==tvht.hItem;//If in bottom half set after. If in top, set before
		m_itemsTree.SetInsertMark(htDest.hItem,insertAfter);

		//CImageList::DragShowNolock(TRUE);
		//SetInsertMark
	}
}

void CToolItemConfig::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonUp(nFlags, point);
	//Handle the item that has been dragged to a location.
	if (Dragging){
		CImageList::DragLeave(&m_itemsTree);
		CImageList::EndDrag();
		m_itemsTree.SetInsertMark(NULL);
		ReleaseCapture();
		ShowCursor(TRUE);
		Dragging = FALSE;

		//currently no legal branch moving until we can copy entire tree structures
		int isLegalMove=!m_itemsTree.ItemHasChildren(itemOrigin);// || m_itemsTree.GetParentItem(htDest.hItem)==m_itemsTree.GetParentItem(itemOrigin);
		if (htDest.hItem && isLegalMove){
			//Gather information from item being deleted
			int isChecked=m_itemsTree.GetCheck(itemOrigin);
			int data=m_itemsTree.GetItemData(itemOrigin);
			CString cText=m_itemsTree.GetItemText(itemOrigin);
			char* text=(char *)(LPCTSTR)cText;
			//Gather info for destination location
			HTREEITEM parentItem=m_itemsTree.GetParentItem(htDest.hItem);
			if (parentItem==NULL) parentItem=TVI_ROOT;
			HTREEITEM insertAfterItem=insertAfter?htDest.hItem:m_itemsTree.GetPrevSiblingItem(htDest.hItem);
			if (insertAfterItem==NULL) insertAfterItem=TVI_FIRST;
			if (insertAfterItem==itemOrigin) return;
				

			//Keep deleting branches without children until we find one with childnren
			HTREEITEM sibling;
			while(TRUE){
				sibling = m_itemsTree.GetNextSiblingItem(itemOrigin);
				if (sibling==NULL) sibling = m_itemsTree.GetPrevSiblingItem(itemOrigin);

				if (sibling==NULL){
					sibling = m_itemsTree.GetParentItem(itemOrigin);
				} else{
					m_itemsTree.DeleteItem(itemOrigin);//Found siblings, return
					break;
				}
				m_itemsTree.DeleteItem(itemOrigin);

				break;//Exits after only 1 iteration

				if (sibling==NULL) break; //no items in list, reached top
				itemOrigin = sibling;
			}

			HTREEITEM item = m_itemsTree.InsertItem(DEF_MASK,text, 0,0,0,0,data,parentItem, insertAfterItem);
			m_itemsTree.SetCheck(item,isChecked);

			if (sibling!=NULL){
				int sibChecked=m_itemsTree.GetCheck(sibling);
				SetParentsCheckBold(&m_itemsTree,sibling,sibChecked);
			}
			SetParentsCheckBold(&m_itemsTree,item,isChecked);
		}
	}
}
