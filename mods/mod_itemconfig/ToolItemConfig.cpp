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
static void SetParentsCheck(CTreeCtrl* treeCtrl,HTREEITEM treeItem,int check);
static void SetChildrenCheck(CTreeCtrl* treeCtrl,HTREEITEM treeItem,int check);
static void DeleteGUITreeItem(CTreeCtrl* treeCtrl,HTREEITEM treeItem,HTREEITEM rootItem=0);
static HTREEITEM moveTree(CTreeCtrl* treeDst, TV_INSERTSTRUCT dest, CTreeCtrl* treeSrc,HTREEITEM src,HTREEITEM recursiveStop, bool MkCopy=FALSE);

CToolItemConfig::CToolItemConfig(CWnd* pParent /*=NULL*/)
: MyDialog(CToolItemConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolItemConfig)
	//}}AFX_DATA_INIT
	Dragging=FALSE;
	actionIndicator=0;
}

CToolItemConfig::cleanup()
{
	DeleteGUITreeItem(&m_itemsTree,TVI_ROOT);
	delete checkImgList;
}

void CToolItemConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolItemConfig)
	DDX_Control(pDX, IDC_ITEM_SORT, m_itemSort);
	DDX_Control(pDX, IDC_ITEMCONFIG_ITEMSTREE, m_itemsTree);
	DDX_Control(pDX, IDC_FRAME_ITEMS, m_ItemsFrame);
	DDX_Control(pDX, IDC_FRAME_FOOD, m_FoodFrame);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG_REFRESH, m_RefreshItems);
	DDX_Control(pDX, IDC_DELETE_FOOD, m_DeleteFood);
	DDX_Control(pDX, IDC_EDIT_FOOD, m_EditFood);
	DDX_Control(pDX, IDC_ADD_FOOD, m_AddFood);
	DDX_Control(pDX, IDC_DELETE_ITEM, m_DeleteItem);
	DDX_Control(pDX, IDC_EDIT_ITEM, m_EditItem);
	DDX_Control(pDX, IDC_ADD_ITEM, m_AddItem);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG_FOODLIST, m_foodList);
	DDX_Control(pDX, IDC_HELP_INFO, m_helpInfo);
	DDX_Control(pDX, IDC_SELECTEDTOBRANCH, m_selectedToBranch);
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
	ON_NOTIFY(TVN_BEGINDRAG, IDC_ITEMCONFIG_ITEMSTREE, OnBegindragTree)
	ON_NOTIFY(NM_CLICK, IDC_ITEMCONFIG_ITEMSTREE, OnClickTree)
	ON_NOTIFY(TVN_SELCHANGING, IDC_ITEMCONFIG_ITEMSTREE, OnSelchangingTree)
	ON_BN_CLICKED(IDC_ITEM_SORT, OnItemSort)
	ON_NOTIFY(TVN_KEYDOWN, IDC_ITEMCONFIG_ITEMSTREE, OnKeydownTree)
	ON_BN_CLICKED(IDC_HELP_INFO, OnHelpInfo)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_SELECTEDTOBRANCH, OnSelectedToBranch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolItemConfig message handlers

void CToolItemConfig::OnOK()
{
	CTibiaItemProxy itemProxy;
	ControlsToConfig();

	itemProxy.saveItemLists();
	itemProxy.refreshItemLists();
	ShowWindow(SW_HIDE);
}

void CToolItemConfig::ControlsToConfig(){
	CTibiaItemProxy itemProxy;
	itemProxy.setItemsTree(NULL);//deletes old tree and makes blank one
	CTibiaTree* dataTree=itemProxy.getItemsTree();
	CreateDataTree(dataTree,&m_itemsTree,TVI_ROOT);

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
}

void CToolItemConfig::OnCancel(){
	ConfigToControls();
	CDialog::OnCancel();
}

void CToolItemConfig::OnClose()
{
	ShowWindow(SW_HIDE);
}

static void DeleteGUITreeItem(CTreeCtrl* treeCtrl,HTREEITEM treeItem,HTREEITEM rootItem/*=0*/){
	if (!rootItem) rootItem=treeItem;//sets the root item for rest of recursive calls
	HTREEITEM child=treeCtrl->GetChildItem(treeItem);
	while (child!=NULL){
		CString tmp=treeCtrl->GetItemText(child);
		DeleteGUITreeItem(treeCtrl,child,rootItem);
		child=treeCtrl->GetNextSiblingItem(child);//iterate
	}

	if (treeItem!=TVI_ROOT){
		CGUITreeItemData *itemData=(CGUITreeItemData*)treeCtrl->GetItemData(treeItem);
		if (itemData) delete itemData;
		if (treeItem==rootItem) treeCtrl->DeleteItem(rootItem);//delete everything at the end
	} else if (rootItem==TVI_ROOT){
		treeCtrl->DeleteAllItems();//delete everything at the end
	}
}

static int CreateGUITree(CTreeCtrl* treeCtrl,HTREEITEM guiTree,CTibiaTree* dataTree){
	//recirsively ensures all children of dataTree are added as children of guiTree
	int size=dataTree->children.size();

	int ret=-1;
	for (int i=0;i<size;i++){
		CTibiaTree* child = dataTree->children[i];
		int val;
		if (!child->HasChildren() && child->data->GetType()==TT_ITEM_NODE){//Item node
			char* txt=NULL;
			CTibiaTreeItemData* data=(CTibiaTreeItemData*)child->data;
			txt=(char*)malloc(strlen(data->GetName())+10);
			sprintf(txt,"%s[%d]",data->GetName(),data->GetId());
			
			CGUITreeItemData *itemData=new CGUITreeItemData(data->GetId(),data->GetItemType());
			HTREEITEM newItem = treeCtrl->InsertItem(DEF_MASK,txt, 0,0,0,0,(long)itemData,guiTree, TVI_LAST);
			free(txt);

			//2 is fully checked, 0 is not checked
			val=data->IsLooted()?2:0;
			treeCtrl->SetItemImage(newItem,val,val);
		}else if(child->HasChildren() && child->data->GetType()==TT_BRANCH_NODE){//Branch node
			CTibiaTreeBranchData* data=(CTibiaTreeBranchData*)child->data;

			HTREEITEM newItem=treeCtrl->InsertItem(data->GetName(),guiTree, TVI_LAST);
			val=CreateGUITree(treeCtrl,newItem,child);//return value indicates if children are checked

			//2 is fully checked, 0 is not checked, 1 is half-checked
			treeCtrl->SetItemImage(newItem,val,val);
			treeCtrl->SetItemState(newItem,TVIS_BOLD,TVIS_BOLD);

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
		if (treeCtrl->ItemHasChildren(child) && treeCtrl->GetItemData(child)==0){//Branch node
			CString cText=treeCtrl->GetItemText(child);
			char* text=(char *)(LPCTSTR)cText;
			CTibiaTree* newTree=dataTree->AddChild(CTibiaTreeBranchData::newCTibiaTreeBranchData(text));

			CreateDataTree(newTree,treeCtrl,child);
		} else if (!treeCtrl->ItemHasChildren(child) && treeCtrl->GetItemData(child)!=NULL) {//Item node
			CGUITreeItemData* itemData = (CGUITreeItemData*)treeCtrl->GetItemData(child);
			CString cText=treeCtrl->GetItemText(child);
			char* text=(char *)(LPCTSTR)cText;
			//Parse text
			for (int i=strlen(text)-1;i>=0;i--){
				if (text[i]=='['){
					text[i]='\0';
					break;
				}
			}
			int curCheck;
			treeCtrl->GetItemImage(child,curCheck,curCheck);
			dataTree->AddChild(CTibiaTreeItemData::newCTibiaTreeItemData(text,itemData->id,curCheck!=0, itemData->type));
		} else{//empty branch case
		}

		child=treeCtrl->GetNextSiblingItem(child);//iterate
	}
}

void CToolItemConfig::ConfigToControls(){
	CancelTwoStepOperations();
	char buf[16384];
	CTibiaItemProxy itemProxy;

	//Create Food List
	while (m_foodList.GetCount()) m_foodList.DeleteString(0);
	int size = itemProxy.getFoodCount();
	for (int i=0;i<size;i++)
	{
		sprintf(buf,"%s[%d]",itemProxy.getFoodNameAtIndex(i),itemProxy.getFoodIdAtIndex(i));
		int ind=m_foodList.AddString(buf);
		m_foodList.SetItemData(ind,itemProxy.getFoodTimeAtIndex(i));
	}

	//Create Item Tree
	m_itemsTree.ModifyStyle(TVS_DISABLEDRAGDROP,TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_TRACKSELECT);
	CTibiaTree* itemsTree=(CTibiaTree*)itemProxy.getItemsTree();
	DeleteGUITreeItem(&m_itemsTree,TVI_ROOT);
	CreateGUITree(&m_itemsTree,TVI_ROOT,itemsTree);
	m_itemsTree.SelectItem(m_itemsTree.GetRootItem());
	m_itemsTree.SelectDropTarget(m_itemsTree.GetRootItem());//needed once to add blue hilighting for the rest of the tree's use
}

void CToolItemConfig::OnToolItemconfigRefresh()
{
	CTibiaItemProxy itemProxy;
	itemProxy.refreshItemLists();
	ConfigToControls();
}



void CToolItemConfig::DoSetButtonSkin(){
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_Cancel);
	skin.SetButtonSkin(	m_RefreshItems);
	skin.SetButtonSkin(	m_EditFood);
	skin.SetButtonSkin(	m_DeleteFood);
	skin.SetButtonSkin(	m_AddFood);
	skin.SetButtonSkin(	m_AddItem);
	skin.SetButtonSkin(	m_EditItem);
	skin.SetButtonSkin(	m_DeleteItem);
	skin.SetButtonSkin(	m_itemSort);
	skin.SetButtonSkin(	m_helpInfo);
	skin.SetButtonSkin(	m_selectedToBranch);
}

BOOL CToolItemConfig::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();
	
	checkImgList=new CImageList();
	checkImgList->Create(13,13,ILC_COLOR8|ILC_MASK,0,1);

	CBitmap checks;
	checks.LoadBitmap(IDB_CHECKS);
	checkImgList->Add(&checks, RGB(0, 0, 255));
	m_itemsTree.SetImageList(checkImgList,TVSIL_NORMAL);

	//OnToolItemconfigRefresh();
	ConfigToControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CToolItemConfig::OnFoodAdd() {
	CancelTwoStepOperations();
	CFoodAdd *dialog = new CFoodAdd(&m_foodList);
	dialog->DoModal();
	delete dialog;
}
void CToolItemConfig::OnFoodEdit() {
	CancelTwoStepOperations();
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
	CancelTwoStepOperations();
	int index = m_foodList.GetCurSel();
	m_foodList.DeleteString(index);
	m_foodList.SetCurSel(index);
}

void CToolItemConfig::OnAddItem()
{
	CancelTwoStepOperations();
	HTREEITEM item=m_itemsTree.GetSelectedItem();
	if (item==NULL) item=TVI_ROOT;
	CItemAdd *dialog;
	dialog = new CItemAdd(&m_itemsTree,item);
	dialog->DoModal();
	delete dialog;

	item=m_itemsTree.GetSelectedItem();//CItemAdd changes selected item
	m_itemsTree.SelectItem(item);
	SetParentsCheck(&m_itemsTree,item,0);
}

void CToolItemConfig::OnItemEdit() {
	CancelTwoStepOperations();
	HTREEITEM item=m_itemsTree.GetSelectedItem();
	if (item==NULL) return;
	CItemEdit *dialog;
	dialog = new CItemEdit(&m_itemsTree,item);
	dialog->DoModal();
	delete dialog;
}

void CToolItemConfig::OnItemDelete(){
	CancelTwoStepOperations();
	HTREEITEM item = m_itemsTree.GetSelectedItem();
	if (item==NULL) return;
	HTREEITEM selItem= m_itemsTree.GetNextSiblingItem(item);
	if (selItem==NULL) selItem=m_itemsTree.GetParentItem(item);
	if (m_itemsTree.ItemHasChildren(item) && m_itemsTree.GetItemData(item)==0){//Branch node
		if(AfxMessageBox("Are you sure you want to delete this branch and all its items?",MB_OKCANCEL)!=IDOK) {
			return;
		}
	}
	DeleteGUITreeItem(&m_itemsTree,item);

	if (selItem!=NULL){
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

void SetChildrenCheck(CTreeCtrl* treeCtrl,HTREEITEM treeItem,BOOL check){
	//Recursively sets children as checked if check or not checked !check
	int val=check?2:0;
	treeCtrl->SetItemImage(treeItem,val,val);
	HTREEITEM item=treeCtrl->GetNextItem(treeItem,TVGN_CHILD);
	while (item!=NULL){
		SetChildrenCheck(treeCtrl,item,check);
		item=treeCtrl->GetNextItem(item,TVGN_NEXT);
	}
}

void SetParentsCheck(CTreeCtrl* treeCtrl,HTREEITEM treeItem,int check){
	//Recursively looks at the parents of treeItem and check if all children are checked
	if (check){
		//At least one child is checked so no need to consider unchecked
		HTREEITEM parent=treeCtrl->GetNextItem(treeItem,TVGN_PARENT);
		if(parent==NULL) return;
		int keepFullCheck=check==2;
		HTREEITEM item=treeCtrl->GetNextItem(parent,TVGN_CHILD);
		while(item!=NULL && keepFullCheck){
			int curCheck;
			treeCtrl->GetItemImage(item,curCheck,curCheck);
			keepFullCheck&=curCheck==2;
			item=treeCtrl->GetNextItem(item,TVGN_NEXT);
		}
		int val;
		if (keepFullCheck) val=2;
		else val=1;
		treeCtrl->SetItemImage(parent,val,val);
		SetParentsCheck(treeCtrl,parent,val);
	}else{
		//At least one child is unchecked so no need to consider full check
		HTREEITEM parent=treeCtrl->GetNextItem(treeItem,TVGN_PARENT);
		if(parent==NULL) return;
		int keepCheck=0;
		HTREEITEM item=treeCtrl->GetNextItem(parent,TVGN_CHILD);
		while(item!=NULL && !keepCheck){
			int curCheck;
			treeCtrl->GetItemImage(item,curCheck,curCheck);
			keepCheck|=curCheck==2;
			item=treeCtrl->GetNextItem(item,TVGN_NEXT);
		}
		int val;
		if (!keepCheck) val=0;
		else val=1;
		treeCtrl->SetItemImage(parent,val,val);
		SetParentsCheck(treeCtrl,parent,val);

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

	if (hti.flags & TVHT_ONITEMICON && hti.hItem){
		CancelTwoStepOperations();
		HTREEITEM treeItem=hti.hItem;

		int isChecked;
		m_itemsTree.GetItemImage(treeItem,isChecked,isChecked);

		//Coding error in GetItemState requires &TVIS_BOLD
		int isBold=m_itemsTree.GetItemState(treeItem,TVIS_BOLD)&TVIS_BOLD;//Coding error in GetItemState requires &TVIS_BOLD
		//Coding error in GetItemState requires &TVIS_BOLD

		m_itemsTree.SelectItem(treeItem);
		
		if(isChecked){//Uncheck
			SetChildrenCheck(&m_itemsTree,treeItem,FALSE);
			SetParentsCheck(&m_itemsTree,treeItem,0);
		}else{//Check
			SetChildrenCheck(&m_itemsTree,treeItem,TRUE);
			SetParentsCheck(&m_itemsTree,treeItem,2);
		}
	}
	else if (hti.flags & TVHT_NOWHERE && !hti.hItem){
		m_itemsTree.SelectItem(NULL);
	}
	*pResult = 0;
}

void CToolItemConfig::OnBegindragTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	CancelTwoStepOperations();
	//Disabled Drag and Drop image as it was problematic

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	//Image list for drag image
    //CImageList* imList=new CImageList();

	itemOrigin=pNMTreeView->itemNew.hItem;
	m_itemsTree.SelectItem(NULL);
	m_itemsTree.SelectItem(itemOrigin);

	//imList=m_itemsTree.CreateDragImage(itemOrigin);
	//m_itemsTree.SetItemState(itemOrigin,TVIS_CUT,TVIS_CUT);

	RECT rcItem;
	m_itemsTree.GetItemRect(itemOrigin,&rcItem,0);
	HTREEITEM item=itemOrigin;
	int level = 0;
	while (item!=NULL){
		level++;
		item = m_itemsTree.GetParentItem(item);
	}
	int indent = m_itemsTree.GetIndent();
	m_itemsTree.SetInsertMark(itemOrigin);


	offsetPt.x=(rcItem.left+(level+1)*indent-13-3)-pNMTreeView->ptDrag.x;//13 is width of picture -3 for good measure
	offsetPt.y=rcItem.top-pNMTreeView->ptDrag.y;

	//imList->BeginDrag(0,CPoint(0,0));
	//ImageList_DragEnter(m_itemsTree,pNMTreeView->ptDrag.x,pNMTreeView->ptDrag.y);
	ShowCursor(FALSE);
	SetCapture();
	
	//delete imList;
	Dragging=TRUE;
	htDest.hItem=NULL;
	OnMouseMove(0,CPoint());
	*pResult = 0;
}

void CToolItemConfig::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	TVHITTESTINFO tvht;
	TVHITTESTINFO tvhtLast;
	TVHITTESTINFO tvhtLastOff;
	CDialog::OnMouseMove(nFlags, point);

	if (Dragging){
		//Moves Drag Image to cursor position
		this->ClientToScreen(&point);
		((CWnd*)&m_itemsTree)->ScreenToClient(&point);
		//CImageList::DragMove(CPoint(point.x+offsetPt.x,point.y+offsetPt.y));
		//CImageList::DragShowNolock(FALSE);//stops drawing drag image to draw the TreeInsertMark
		htDest.pt.x = point.x;
		htDest.pt.y = point.y;

		//Test a second point above cursor
		//If still on the same item then we are in bottom half, otherwise we are in the top half of the item
		tvht.pt.x = point.x;
		tvht.pt.y = point.y-10;

		//If last values have changed, redraw the insert mark
		tvhtLast.pt.x = lastInsertMark.x;
		tvhtLast.pt.y = lastInsertMark.y;
		tvhtLastOff.pt.x = lastInsertMark.x;
		tvhtLastOff.pt.y = lastInsertMark.y-10;

		m_itemsTree.HitTest(&htDest);
		m_itemsTree.HitTest(&tvht);
		m_itemsTree.HitTest(&tvhtLast);
		m_itemsTree.HitTest(&tvhtLastOff);

		insertAfter=htDest.hItem==tvht.hItem;//If in bottom half set after. If in top, set before

		if (tvhtLast.hItem!=htDest.hItem || tvhtLastOff.hItem!=tvht.hItem){
			m_itemsTree.SetInsertMark(htDest.hItem,insertAfter);
			lastInsertMark=CPoint(point.x,point.y);
		}
		//CImageList::DragShowNolock(TRUE);
	}
}

HTREEITEM moveTree(CTreeCtrl* treeDst, TV_INSERTSTRUCT dest, CTreeCtrl* treeSrc,HTREEITEM src,HTREEITEM recursiveStop, bool MkCopy)
{
	//useful to know if this is the initial call
	int isRootNode=recursiveStop==NULL;

	dest.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_STATE|TVIF_SELECTEDIMAGE;
	dest.item.stateMask=0xFFFFFFFF;
	dest.item.state=treeSrc->GetItemState(src,dest.item.stateMask);
	CString cText=treeSrc->GetItemText(src);
	char* text=(char *)(LPCTSTR)cText;
	dest.item.pszText=text;
	treeSrc->GetItemImage(src,dest.item.iImage,dest.item.iSelectedImage);
	HTREEITEM parent=treeDst->InsertItem(&dest);
	CGUITreeItemData* itemData=(CGUITreeItemData*)treeSrc->GetItemData(src);
	if (itemData) treeDst->SetItemData(parent,(long)new CGUITreeItemData(itemData->id,itemData->type));

	if(recursiveStop==NULL) recursiveStop=parent;
	HTREEITEM  child = treeSrc->GetChildItem(src);
	while (child!=NULL){
		if (child!=recursiveStop){
			TV_INSERTSTRUCT newDest;
			newDest.hParent=parent;
			newDest.hInsertAfter=TVI_LAST;
			moveTree(treeDst,newDest,treeSrc,child,recursiveStop,MkCopy);
		}
		child = treeSrc->GetNextSiblingItem(child);
	}
	if (isRootNode){
		if (!MkCopy){
			HTREEITEM sibling=treeSrc->GetNextSiblingItem(src);
			DeleteGUITreeItem(treeSrc,src);//delete root item when finished
			if (sibling!=NULL){
				int sibCheck;
				treeSrc->GetItemImage(sibling,sibCheck,sibCheck);
				SetParentsCheck(treeDst,sibling,sibCheck);
			}
		}
		if (parent!=NULL){
			int curCheck;
			treeSrc->GetItemImage(parent,curCheck,curCheck);
			SetParentsCheck(treeDst,parent,curCheck);
		}
	}
	return parent;
}

void CToolItemConfig::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonUp(nFlags, point);
	//Handle the item that has been dragged to a location.
	if (Dragging){
		//CImageList::DragLeave(&m_itemsTree);
		//CImageList::EndDrag();
		m_itemsTree.SetInsertMark(NULL);
		ReleaseCapture();
		ShowCursor(TRUE);
		Dragging = FALSE;

		//currently no legal branch moving until we can copy entire tree structures
		int isLegalMove=1;//!m_itemsTree.ItemHasChildren(itemOrigin);// || m_itemsTree.GetParentItem(htDest.hItem)==m_itemsTree.GetParentItem(itemOrigin);
		if (htDest.hItem && isLegalMove){
			m_itemsTree.SelectItem(NULL);
			TV_INSERTSTRUCT dest;
			dest.hParent=m_itemsTree.GetParentItem(htDest.hItem);
			if (dest.hParent==NULL) dest.hParent=TVI_ROOT;
			dest.hInsertAfter=insertAfter?htDest.hItem:m_itemsTree.GetPrevSiblingItem(htDest.hItem);
			if (dest.hInsertAfter==NULL) dest.hInsertAfter=TVI_FIRST;

			if (dest.hInsertAfter==itemOrigin) return;

			//Check for branch being added to itself
			HTREEITEM parent=dest.hParent;
			while (parent!=TVI_ROOT && parent!=NULL){
				if (parent==itemOrigin) return;
				parent=m_itemsTree.GetParentItem(parent);
			}

			HTREEITEM destItem=moveTree(&m_itemsTree,dest,&m_itemsTree,itemOrigin,NULL);

			m_itemsTree.SelectItem(destItem);
		}
	}
}


void CToolItemConfig::OnSelchangingTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	HTREEITEM item = m_itemsTree.GetDropHilightItem();
	m_itemsTree.SelectDropTarget(NULL);
	
	*pResult = 0;
}

void CToolItemConfig::OnItemSort()
{
	CancelTwoStepOperations();
	HTREEITEM item= m_itemsTree.GetSelectedItem();
	if (item==NULL) item=TVI_ROOT;
	m_itemsTree.SortChildren(item);
}

void CToolItemConfig::CancelTwoStepOperations(){
	if(actionIndicator){
		m_itemsTree.SetItemState(actionItem,0,TVIS_CUT);
		actionItem=NULL;
		actionIndicator=0;
	}
}

void CToolItemConfig::OnKeydownTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	// pTVKeyDown->hdr
	if (pTVKeyDown->wVKey==VK_SPACE)
	{
		CancelTwoStepOperations();
		HTREEITEM treeItem=m_itemsTree.GetSelectedItem();
		if (treeItem==NULL) return;

		int isChecked;
		m_itemsTree.GetItemImage(treeItem,isChecked,isChecked);

		//Coding error in GetItemState requires &TVIS_BOLD
		int isBold=m_itemsTree.GetItemState(treeItem,TVIS_BOLD)&TVIS_BOLD;//Coding error in GetItemState requires &TVIS_BOLD
		//Coding error in GetItemState requires &TVIS_BOLD

		m_itemsTree.SelectItem(treeItem);
		
		if(isChecked){//Uncheck
			SetChildrenCheck(&m_itemsTree,treeItem,FALSE);
			SetParentsCheck(&m_itemsTree,treeItem,0);
		}else{//Check
			SetChildrenCheck(&m_itemsTree,treeItem,TRUE);
			SetParentsCheck(&m_itemsTree,treeItem,2);
		}
		
	}
	if (pTVKeyDown->wVKey==VK_DELETE){
		HTREEITEM treeItem=m_itemsTree.GetSelectedItem();
		if (treeItem==NULL) return;
		if (m_itemsTree.ItemHasChildren(treeItem) && m_itemsTree.GetItemData(treeItem)==0){//Branch node
			if(AfxMessageBox("Are you sure you want to delete this branch and all its items?",MB_OKCANCEL)!=IDOK) {
				return;
			}
		} else {
			if(AfxMessageBox("Are you sure you want to delete this item?",MB_OKCANCEL)!=IDOK) {
				return;
			}
		}
		DeleteGUITreeItem(&m_itemsTree,treeItem);

	}
	if (GetKeyState(VK_LCONTROL)>>1 || GetKeyState(VK_RCONTROL)>>1){ // Checks high order bit
		if (pTVKeyDown->wVKey=='C')
		{
			CancelTwoStepOperations();
			HTREEITEM item = m_itemsTree.GetSelectedItem();
			if (item){
				actionIndicator=1;
				actionItem=item;
				m_itemsTree.SetItemState(actionItem,TVIS_CUT,TVIS_CUT);
			}
		}
		if (pTVKeyDown->wVKey=='X')
		{
			CancelTwoStepOperations();
			HTREEITEM item = m_itemsTree.GetSelectedItem();
			if (item){
				actionIndicator=2;
				actionItem=item;
				m_itemsTree.SetItemState(actionItem,TVIS_CUT,TVIS_CUT);
			}
		}
		if (pTVKeyDown->wVKey=='V')
		{
			if (actionIndicator){
				m_itemsTree.SetItemState(actionItem,0,TVIS_CUT);
				TV_INSERTSTRUCT dest;

				HTREEITEM item = m_itemsTree.GetSelectedItem();
				if (item==NULL) dest.hParent=TVI_ROOT;
				else dest.hParent=m_itemsTree.GetParentItem(item);
				if (dest.hParent==NULL) dest.hParent=TVI_ROOT;

				dest.hInsertAfter=item;
				if (dest.hInsertAfter==NULL) dest.hInsertAfter=TVI_FIRST;

				if (dest.hInsertAfter==actionItem) return;

				//Check for branch being added to itself if CUTTING
				HTREEITEM parent=dest.hParent;
				while (parent!=TVI_ROOT && parent!=NULL && actionIndicator==2){
					if (parent==actionItem) return;
					parent=m_itemsTree.GetParentItem(parent);
				}

				HTREEITEM sibling=m_itemsTree.GetNextSiblingItem(actionItem);

				HTREEITEM destItem=moveTree(&m_itemsTree,dest,&m_itemsTree,actionItem,NULL,actionIndicator==1);//copy if actionIndicator==1

				if (actionIndicator==2 && sibling!=NULL){
					int sibCheck;
					m_itemsTree.GetItemImage(sibling,sibCheck,sibCheck);
					SetParentsCheck(&m_itemsTree,sibling,sibCheck);
				}
				if (destItem!=NULL){
					int curCheck;
					m_itemsTree.GetItemImage(destItem,curCheck,curCheck);
					SetParentsCheck(&m_itemsTree,destItem,curCheck);
				}
				m_itemsTree.SelectItem(destItem);

				actionIndicator=0;
				actionItem=NULL;
			}
		}
	}
	
	*pResult = 0;
}

void CToolItemConfig::OnHelpInfo()
{
	AfxMessageBox("Currently Implemented Features:\nPress Spacebar to check/uncheck\nUse arrow keys to browse tree\nClick on +- sign for expanding\nDrag and drop to move items and branches.\nButton actions are performed on selected items\nCopying, cutting and pasting items\n(Clicking in bottom whitespace clears selection/selects root)");

}

void addChildrenToTreeItem(CTreeCtrl* treeCtrl,HTREEITEM parentItem,HTREEITEM destItem){
	HTREEITEM child=treeCtrl->GetChildItem(parentItem);
	while (child!=NULL){

		if (treeCtrl->ItemHasChildren(child) && treeCtrl->GetItemData(child)==0){
			addChildrenToTreeItem(treeCtrl,child,destItem);
		} else if(!treeCtrl->ItemHasChildren(child) && treeCtrl->GetItemData(child)!=0) {
			int check;
			treeCtrl->GetItemImage(child,check,check);
			if (check==2){
				TV_INSERTSTRUCT dest;
				dest.hParent=destItem;
				dest.hInsertAfter=TVI_LAST;
				dest.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_STATE|TVIF_SELECTEDIMAGE;
				dest.item.stateMask=0xFFFFFFFF;
				dest.item.state=treeCtrl->GetItemState(child,dest.item.stateMask);
				CString cText=treeCtrl->GetItemText(child);
				char* text=(char *)(LPCTSTR)cText;
				dest.item.pszText=text;
				dest.item.iImage=0;
				dest.item.iSelectedImage=0;
				HTREEITEM item = treeCtrl->InsertItem(&dest);
				CGUITreeItemData* itemData=(CGUITreeItemData*)treeCtrl->GetItemData(child);
				if (itemData) treeCtrl->SetItemData(item,(long)new CGUITreeItemData(itemData->id,itemData->type));
			}
		}
		child=treeCtrl->GetNextSiblingItem(child);
	}
}

void CToolItemConfig::OnSelectedToBranch()
{
	HTREEITEM parentItem=m_itemsTree.GetSelectedItem();
	if (parentItem==NULL) parentItem=TVI_ROOT;
	if (parentItem!=TVI_ROOT && m_itemsTree.GetItemData(parentItem)!=0) parentItem=m_itemsTree.GetParentItem(parentItem);
	if (parentItem==NULL) parentItem=TVI_ROOT;
	HTREEITEM destItem = m_itemsTree.InsertItem("New Branch",parentItem, TVI_FIRST);

	addChildrenToTreeItem(&m_itemsTree,TVI_ROOT,destItem);
	SetParentsCheck(&m_itemsTree,destItem,0);
	m_itemsTree.SetItemState(destItem,TVIS_BOLD,TVIS_BOLD);
	m_itemsTree.SelectItem(destItem);
}
