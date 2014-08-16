// TibiaItem.cpp: implementation of the CTibiaItem class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TibiaStructures.h"
#include "TibiaItem.h"

#include "Util.h"

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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "TibiaItem_codes.cpp"


int CTibiaItem::itemListsFresh=0;
int CTibiaItem::xmlInitialised=0;

CTibiaTree* CTibiaItem::itemTree=new CTibiaTree(new CTibiaTreeBranchData("Root"));
CTibiaList CTibiaItem::foodList;
CTibiaList CTibiaItem::lootList;
CTibiaList CTibiaItem::itemList;
CTibiaList CTibiaItem::typedList;
CTibiaList CTibiaItem::constCodeList;

void parseItemsBranch(DOMNode* listNode,CTibiaTree* parent);
void saveItemsBranch(DOMNode* node, CTibiaTree* parent, DOMDocument* doc);
void traverseTreeForItemList(CTibiaTree* parent, CTibiaList* list);
void traverseTreeForLootList(CTibiaTree* parent, CTibiaList* list);

extern CRITICAL_SECTION ItemsInitCriticalSection;


CTibiaItem::CTibiaItem()
{
	objectId=0;
	quantity=0;
	pos=0;
}

//Run when closing down
void CTibiaItem::cleanup(){
	delete CTibiaItem::itemTree;
}

//Returns "unknown" if name not in list
char * CTibiaItem::getItemName(int objectId)
{
	refreshItemLists();

	char * ret=itemList.GetText(objectId);
	if (ret!=NULL) return ret;

	return "unknown";
}

//Returns "unknown" if name not in list
char * CTibiaItem::getTypedItemName(int objectId)
{
	refreshItemLists();

	char * ret=typedList.GetText(objectId);
	if (ret!=NULL) return ret;

	return "unknown";
}

//Returns the objectId associated with a name and 0 if not in list
int CTibiaItem::getItemId(char *name)
{
	refreshItemLists();
	return itemList.GetValue(name);
}
int CTibiaItem::getTypedItemId(char *name)
{
	refreshItemLists();
	return typedList.GetValue(name);
}

int CTibiaItem::getItemIndex(int objectId)
{
	refreshItemLists();
	return itemList.GetIndex(objectId);
}

int CTibiaItem::getTypedItemIndex(int objectId)
{
	refreshItemLists();
	return typedList.GetIndex(objectId);
}

int CTibiaItem::getFoodIndex(int objectId)
{
	refreshItemLists();
	return foodList.GetIndex(objectId);
}

int CTibiaItem::getLootItemIndex(int objectId)
{
	refreshItemLists();
	return lootList.GetIndex(objectId);
}

int CTibiaItem::getItemIdAtIndex(int ind)
{
	refreshItemLists();
	return itemList.GetValueAtIndex(ind);
}

int CTibiaItem::getItemTypeAtIndex(int ind)
{
	refreshItemLists();
	return itemList.GetTypeAtIndex(ind);
}

int CTibiaItem::getTypedItemIdAtIndex(int ind)
{
	refreshItemLists();
	return typedList.GetValueAtIndex(ind);
}

int CTibiaItem::getFoodIdAtIndex(int ind)
{
	refreshItemLists();
	return foodList.GetValueAtIndex(ind);
}
int CTibiaItem::getLootItemIdAtIndex(int ind)
{
	refreshItemLists();
	return lootList.GetValueAtIndex(ind);
}

char* CTibiaItem::getItemNameAtIndex(int ind)
{
	refreshItemLists();
	return itemList.GetTextAtIndex(ind);
}

char* CTibiaItem::getTypedItemNameAtIndex(int ind)
{
	refreshItemLists();
	return typedList.GetTextAtIndex(ind);
}

char* CTibiaItem::getFoodNameAtIndex(int ind)
{
	refreshItemLists();
	return foodList.GetTextAtIndex(ind);
}
char* CTibiaItem::getLootItemNameAtIndex(int ind)
{
	refreshItemLists();
	return lootList.GetTextAtIndex(ind);
}

int CTibiaItem::getFoodTimeAtIndex(int ind){
	refreshItemLists();
	return foodList.GetExtraInfoAtIndex(ind);
}

CUIntArray* CTibiaItem::getItemIdArrayPtr(){
	refreshItemLists();
	return itemList.GetArrayPtr();
}

CUIntArray* CTibiaItem::getTypedItemIdArrayPtr(int ind) {
	refreshItemLists();
	return typedList.GetArrayPtr();
}

void CTibiaItem::fillTypedItemIdArray(int ind) {
	refreshItemLists();
	typedList.RemoveAll();
	for (int loop = 0; loop < getItemCount(); loop++) {
		if (itemList.GetTypeAtIndex(loop) & ind)
			typedList.Add(itemList.GetValueAtIndex(loop), itemList.GetTextAtIndex(loop), itemList.GetExtraInfoAtIndex(loop), itemList.GetTypeAtIndex(loop));
	}
}

CUIntArray* CTibiaItem::getFoodIdArrayPtr(){
	refreshItemLists();
	return foodList.GetArrayPtr();
}

CUIntArray* CTibiaItem::getLootItemIdArrayPtr(){
	refreshItemLists();
	return lootList.GetArrayPtr();
}

void CTibiaItem::addItem(char *name, int objectId) {
	refreshItemLists();
	itemList.Add(objectId,name,0);
}
void CTibiaItem::addTypedItem(char *name, int objectId, int type) {
	refreshItemLists();
	typedList.Add(objectId,name,0,type);
}
void CTibiaItem::addFood(char *name, int objectId, int extraInfo) {
	refreshItemLists();
	foodList.Add(objectId,name,extraInfo);
}
void CTibiaItem::addLootItem(char *name, int objectId) {
	refreshItemLists();
	lootList.Add(objectId,name,0);
}

void CTibiaItem::removeItem(int ind) {
	refreshItemLists();
	itemList.RemoveAtIndex(ind);
}

void CTibiaItem::removeTypedItem(int ind) {
	refreshItemLists();
	typedList.RemoveAtIndex(ind);
}
void CTibiaItem::removeFood(int ind) {
	refreshItemLists();
	foodList.RemoveAtIndex(ind);
}
void CTibiaItem::removeLootItem(int ind) {
	refreshItemLists();
	lootList.RemoveAtIndex(ind);
}

void CTibiaItem::clearFoodList(){
	refreshItemLists();
	foodList.RemoveAll();
}

int CTibiaItem::getItemCount()
{
	refreshItemLists();
	return itemList.GetCount();
}

int CTibiaItem::getTypedItemCount()
{
	refreshItemLists();
	return typedList.GetCount();
}

int CTibiaItem::getFoodCount()
{
	refreshItemLists();
	return foodList.GetCount();
}
int CTibiaItem::getLootItemCount()
{
	refreshItemLists();
	return lootList.GetCount();
}

int CTibiaItem::getValueForConst(char *code)
{
	refreshItemLists();
	int len=strlen(code)+1;
	char* lcode=(char*)malloc(len);
	for (int i=0;i<len;i++){
		lcode[i]=tolower(code[i]);
	}
	int ret=constCodeList.GetValue(lcode);
	if (ret==0 && !constCodeList.HasText(lcode)){
		free(lcode);
		char msgBuf[128];
		sprintf(msgBuf,"ERROR: Unable to find value for const code='%s'!",code);
		AfxMessageBox(msgBuf);
		return 0;
	}
	free(lcode);
	return ret;
}

CTibiaTree * CTibiaItem::getItemsTree()
{
	refreshItemLists();
	if (!itemTree)
	{
		itemTree=new CTibiaTree(new CTibiaTreeBranchData("Root"));
	}
	return itemTree;
}

void CTibiaItem::setItemsTree(CTibiaTree * tibiaTree){
	if (itemTree) delete CTibiaItem::itemTree;
	if (!tibiaTree){
		itemTree=new CTibiaTree(new CTibiaTreeBranchData("Root"));
	}else{
		itemTree=tibiaTree;
	}
}

void parseItemsBranch(DOMNode* listNode,CTibiaTree* parent)
{
	for (int itemNr=0;itemNr<listNode->getChildNodes()->getLength();itemNr++)
	{

		int attrNr;
		DOMNode *item = listNode->getChildNodes()->item(itemNr);
		
		if (item->getNodeType()!=1) continue;
		if (!wcscmp(item->getNodeName(),_L("item"))){

			
			int objectId=0;
			int objectLoot=0;
			int type = 0;
			char *objectName=NULL;
			
			for (attrNr=0;attrNr<item->getAttributes()->getLength();attrNr++)
			{
				DOMNode *attrNode = item->getAttributes()->item(attrNr);

				if (!wcscmp(attrNode->getNodeName(),_L("name")))
					objectName=CUtil::wc2c(attrNode->getNodeValue());
				if (!wcscmp(attrNode->getNodeName(),_L("id")))
				{
					char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
					sscanf(idTmp,"0x%x",&objectId);
					if (objectId==0)
						sscanf(idTmp,"%d",&objectId);
					free(idTmp);
				}
				if (!wcscmp(attrNode->getNodeName(),_L("looted")))
				{
					char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
					sscanf(idTmp,"%d",&objectLoot);
					free(idTmp);
				}
				if (!wcscmp(attrNode->getNodeName(),_L("type")))
				{
					char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
					sscanf(idTmp,"%d",&type);
					free(idTmp);
				}
			}
			if (!objectId||!objectName||!strlen(objectName))
			{
				if (objectName) free(objectName);
				continue;
			}
			parent->AddChild(new CTibiaTreeItemData(objectName,objectId,objectLoot!=0, type));
			if (objectName) free(objectName);
		}
		if (!wcscmp(item->getNodeName(),_L("branch"))){
			char *branchName=NULL;
			
			for (attrNr=0;attrNr<item->getAttributes()->getLength();attrNr++)
			{
				DOMNode *attrNode = item->getAttributes()->item(attrNr);
				if (!wcscmp(attrNode->getNodeName(),_L("name")))
					branchName=CUtil::wc2c(attrNode->getNodeValue());
			}
			if (!branchName||!strlen(branchName))
			{
				if (branchName) free(branchName);
				continue;
			}

			CTibiaTree* child= parent->AddChild(new CTibiaTreeBranchData(branchName));
			parseItemsBranch(item,child);
			if (branchName) free(branchName);
		}
	}
}

void traverseTreeForItemList(CTibiaTree* parent, CTibiaList* list){
	int size=parent->children.size();
	for (int i=0;i<size;i++){
		CTibiaTree* child=parent->children[i];
		if (!child->HasChildren() && child->data->GetType()==TT_ITEM_NODE){
			CTibiaTreeItemData* data = (CTibiaTreeItemData*)(child->data);

			char* name=data->GetName();
			//Add to list, but if failed, display item info
			if(! list->Add(data->GetId(), data->GetName(), 0, data->GetItemType())){
				char buf[1024];
				char trunc_name[500];
				int len=min(strlen(name),500);
				memcpy(trunc_name,name,len);
				trunc_name[len]='\0';
				sprintf(buf,"Item already added under different name or ID. Failed to add item 0x%x:%s",data->GetId(),trunc_name);
			}
		} else if (child->HasChildren() && child->data->GetType()==TT_BRANCH_NODE){
			//recurse for all children
			traverseTreeForItemList(child, list);
		}
	}
}
void traverseTreeForLootList(CTibiaTree* parent, CTibiaList* list){
	int size=parent->children.size();
	for (int i=0;i<size;i++){
		CTibiaTree* child=parent->children[i];
		if (!child->HasChildren() && child->data->GetType()==TT_ITEM_NODE){
			CTibiaTreeItemData* data = (CTibiaTreeItemData*)(child->data);
			if (data->IsLooted()){
				list->Add(data->GetId(),data->GetName(),data->GetItemType());
			}
		} else if (child->HasChildren() && child->data->GetType()==TT_BRANCH_NODE){
			//recurse for all children
			traverseTreeForLootList(child, list);
		}
	}
}

void traverseTreeToClearLootItems(CTibiaTree* parent, CTibiaList* list){
	int size=parent->children.size();
	for (int i=0;i<size;i++){
		CTibiaTree* child=parent->children[i];
		if (!child->HasChildren() && child->data->GetType()==TT_ITEM_NODE){
			CTibiaTreeItemData* data = (CTibiaTreeItemData*)(child->data);
			if (data->IsLooted()){
				data->SetIsLooted(false);
			}
		} else if (child->HasChildren() && child->data->GetType()==TT_BRANCH_NODE){
			//recurse for all children
			traverseTreeToClearLootItems(child, list);
		}
	}
}
void CTibiaItem::clearLootItems(){
	refreshItemLists();
	lootList.RemoveAll();
	traverseTreeToClearLootItems(itemTree, &lootList);
}

bool traverseTreeToSetAsLooted(CTibiaTree* parent, CTibiaList* list,int objectId){
	int size=parent->children.size();
	for (int i=0;i<size;i++){
		CTibiaTree* child=parent->children[i];
		if (!child->HasChildren() && child->data->GetType()==TT_ITEM_NODE){
			CTibiaTreeItemData* data = (CTibiaTreeItemData*)(child->data);
			if (data->GetId()==objectId){
				data->SetIsLooted(true);
				list->Add(data->GetId(),data->GetName(),data->GetItemType());
				return true;
			}
		} else if (child->HasChildren() && child->data->GetType()==TT_BRANCH_NODE){
			//recurse for all children
			if (traverseTreeToSetAsLooted(child, list, objectId)) return true;
		}
	}
	return false;
}

void CTibiaItem::setItemAsLooted(int objectId){
	refreshItemLists();
	char tempName[32];
	sprintf(tempName,"%d",objectId);

	if (!traverseTreeToSetAsLooted(itemTree, &lootList, objectId)) lootList.Add(objectId,tempName,0);
}

void CTibiaItem::refreshItemLists()
{
	EnterCriticalSection(&ItemsInitCriticalSection);
	if (itemListsFresh)
	{
		LeaveCriticalSection(&ItemsInitCriticalSection);
		return;
	}
	itemListsFresh=1;
	if (!xmlInitialised)
	{
		XMLPlatformUtils::Initialize();
		xmlInitialised=1;
	}

	char installPath[1024];
	unsigned long installPathLen=1023;
	installPath[0]='\0';
	HKEY hkey=NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey))
	{
		RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
		RegCloseKey(hkey);
	}
	if (!strlen(installPath))
	{
		AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
		exit(1);
	}
	
	
	

	XercesDOMParser *parser = new XercesDOMParser();
	try
	{

		int listNr,itemNr,rootNr;

		//reset all lists
		constCodeList.RemoveAll();
		foodList.RemoveAll();
		lootList.RemoveAll();
		itemList.RemoveAll();
		if (itemTree) delete itemTree;

		itemTree=new CTibiaTree(new CTibiaTreeBranchData("Root"));
				
		char pathBuf[2048];
		sprintf(pathBuf,"%s\\mods\\tibiaauto-items.xml",installPath);
		parser->parse(pathBuf);
		DOMNode  *doc = parser->getDocument();
		for (rootNr=0;rootNr<doc->getChildNodes()->getLength();rootNr++)
		{
			DOMNode *root = doc->getChildNodes()->item(rootNr);

			if (wcscmp(root->getNodeName(),_L("item-definitions")))
				continue;
			for (listNr=0;listNr<root->getChildNodes()->getLength();listNr++)
			{
				DOMNode *listNode = root->getChildNodes()->item(listNr);

				//ITEMS
				if (!wcscmp(listNode->getNodeName(),_L("items"))) {

					//recursively add to itemTree from XML tree, works with older versions
					parseItemsBranch(listNode,itemTree);

				}

				//FOOD
				if (!wcscmp(listNode->getNodeName(),_L("foods"))) {
					
					for (itemNr=0;itemNr<listNode->getChildNodes()->getLength();itemNr++)
					{
						int attrNr;
						DOMNode *item = listNode->getChildNodes()->item(itemNr);
						if (wcscmp(item->getNodeName(),_L("item")))
							continue;
						
						int objectId=0;
						int eatTime = 0;
						char *objectName=NULL;
						
						for (attrNr=0;attrNr<item->getAttributes()->getLength();attrNr++)
						{
							DOMNode *attrNode = item->getAttributes()->item(attrNr);
							if (!wcscmp(attrNode->getNodeName(),_L("name")))
								objectName=CUtil::wc2c(attrNode->getNodeValue());
							if (!wcscmp(attrNode->getNodeName(),_L("id")))
							{
								char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
								sscanf(idTmp,"0x%x",&objectId);
								if (objectId==0)
									sscanf(idTmp,"%d",&objectId);
								free(idTmp);
							}
							if (!wcscmp(attrNode->getNodeName(),_L("time"))) {
								char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
								sscanf(idTmp,"%d",&eatTime);
								free(idTmp);
							}
						}
						if (!objectId||!objectName||!strlen(objectName))
						{
							if (objectName) free(objectName);
							continue;
						}
						
						foodList.Add(objectId,objectName,eatTime);
						if (objectName) free(objectName);
						//delete item;//wis
					}
				}

				//CONSTS
				//SECTION IS UNUSED, FOR CURRENT TIBIA see next section for loading from const file
				if (!wcscmp(listNode->getNodeName(),_L("consts"))) {
					
					for (itemNr=0;itemNr<listNode->getChildNodes()->getLength();itemNr++)
					{
						int attrNr;
						DOMNode *item = listNode->getChildNodes()->item(itemNr);
						if (wcscmp(item->getNodeName(),_L("const")))
							continue;
						
						int constValue=0;
						char *constCode=NULL;
						
						for (attrNr=0;attrNr<item->getAttributes()->getLength();attrNr++)
						{
							DOMNode *attrNode = item->getAttributes()->item(attrNr);
							if (!wcscmp(attrNode->getNodeName(),_L("code")))
								constCode=CUtil::wc2c(attrNode->getNodeValue());
							if (!wcscmp(attrNode->getNodeName(),_L("value")))
							{
								char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
								sscanf(idTmp,"0x%x",&constValue);
								if (constValue==0)
									sscanf(idTmp,"%d",&constValue);
								free(idTmp);
							}
						}
						if (!constCode||!strlen(constCode))
						{
							if (constCode) free(constCode);
							continue;
						}

						int i,len;
						len=strlen(constCode);
						for (i=0;i<len;i++)
							constCode[i]=tolower(constCode[i]);
						
						constCodeList.Add(constValue,constCode,0);
						if (constCode) free(constCode);
						//delete item;//wis
					}
				}

				//LOOT
				//SECTION IS UNUSED, FOR CURRENT TIBIA
				if (!wcscmp(listNode->getNodeName(),_L("looted"))) {
					
					for (itemNr=0;itemNr<listNode->getChildNodes()->getLength();itemNr++)
					{
						int attrNr;
						DOMNode *item = listNode->getChildNodes()->item(itemNr);
						if (wcscmp(item->getNodeName(),_L("item")))
							continue;
						
						int objectId=0;
						char *objectName=NULL;
						
						for (attrNr=0;attrNr<item->getAttributes()->getLength();attrNr++)
						{
							DOMNode *attrNode = item->getAttributes()->item(attrNr);
							if (!wcscmp(attrNode->getNodeName(),_L("name")))
								objectName=CUtil::wc2c(attrNode->getNodeValue());
							if (!wcscmp(attrNode->getNodeName(),_L("id")))
							{
								char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
								sscanf(idTmp,"0x%x",&objectId);
								free(idTmp);
							}
						}
						if (!objectId||!objectName||!strlen(objectName))
						{
							if (objectName) free(objectName);
							continue;
						}
						lootList.Add(objectId,objectName,0);
						if (objectName) free(objectName);
					}
				}
			}
		}

		//Create data lists from tree structure loaded by XML
		traverseTreeForItemList(itemTree, &itemList);
		if (lootList.GetCount()==0) traverseTreeForLootList(itemTree, &lootList);// get loot from tree if not loaded from file

		sprintf(pathBuf,"%s\\mods\\tibiaauto-consts.xml",installPath);
		OFSTRUCT lpOpen;
		if (OpenFile(pathBuf,&lpOpen,OF_EXIST) != HFILE_ERROR){

			constCodeList.RemoveAll();

			delete parser;
			parser = new XercesDOMParser();
			parser->parse(pathBuf);
			doc = parser->getDocument();
			for (rootNr=0;rootNr<doc->getChildNodes()->getLength();rootNr++)
			{
				DOMNode *root = doc->getChildNodes()->item(rootNr);
					
				if (wcscmp(root->getNodeName(),_L("const-definitions")))
					continue;
				for (listNr=0;listNr<root->getChildNodes()->getLength();listNr++)
				{
					DOMNode *listNode = root->getChildNodes()->item(listNr);


					//CONSTS
					if (!wcscmp(listNode->getNodeName(),_L("consts"))) {
						
						for (itemNr=0;itemNr<listNode->getChildNodes()->getLength();itemNr++)
						{
							int attrNr;
							DOMNode *item = listNode->getChildNodes()->item(itemNr);
							if (wcscmp(item->getNodeName(),_L("const")))
								continue;
							
							int constValue=0;
							char *constCode=NULL;
							
							for (attrNr=0;attrNr<item->getAttributes()->getLength();attrNr++)
							{
								DOMNode *attrNode = item->getAttributes()->item(attrNr);
								if (!wcscmp(attrNode->getNodeName(),_L("code")))
									constCode=CUtil::wc2c(attrNode->getNodeValue());
								if (!wcscmp(attrNode->getNodeName(),_L("value")))
								{
									char *idTmp=CUtil::wc2c(attrNode->getNodeValue());
									sscanf(idTmp,"0x%x",&constValue);
									if (constValue==0)
										sscanf(idTmp,"%d",&constValue);
									free(idTmp);
								}
							}
							if (!constCode||!strlen(constCode))
							{
								if (constCode) free(constCode);
								continue;
							}

							int i,len;
							len=strlen(constCode);
							for (i=0;i<len;i++)
								constCode[i]=tolower(constCode[i]);
							
							constCodeList.Add(constValue,constCode,0);
							if (constCode) free(constCode);
						}
					}
				}
			}
		}//if file tibiaauto-consts.xml exists
	} catch (...)
	{
		AfxMessageBox("Unable to load const/item definitions!");
		/*
		AfxMessageBox("Debug Item Tree");
		char buf[11111];
		itemTree->toString(buf);
		AfxMessageBox(buf);
		AfxMessageBox("Debug Food");
		foodList.toString(buf);
		AfxMessageBox(buf);
		AfxMessageBox("Debug Consts");
		constCodeList.toString(buf);
		AfxMessageBox(buf);
		*/
	}

	delete parser;
	LeaveCriticalSection(&ItemsInitCriticalSection);
}

void saveItemsBranch(DOMNode* node, CTibiaTree* parent, DOMDocument  *doc){
	int size=parent->children.size();
	for (int i=0;i<size;i++){
		CTibiaTree* child=parent->children[i];
		if (child->HasChildren() && child->data->GetType()==TT_BRANCH_NODE){
			CTibiaTreeBranchData* data = (CTibiaTreeBranchData*)(child->data);

			DOMElement* itemElem = doc->createElement(XMLString::transcode("branch"));
			itemElem->setAttribute(XMLString::transcode("name"), XMLString::transcode(data->GetName()));
			node->appendChild(itemElem);
			saveItemsBranch(itemElem, child, doc);
		} else if (!child->HasChildren() && child->data->GetType()==TT_ITEM_NODE){
			CTibiaTreeItemData* data = (CTibiaTreeItemData*)(child->data);
			char buf[512];

			int id = data->GetId();
			int type = data->GetItemType();
			bool looted = data->IsLooted();
			DOMElement*  itemElem = doc->createElement(XMLString::transcode("item"));
			itemElem->setAttribute(XMLString::transcode("name"), XMLString::transcode(data->GetName()));
			sprintf(buf,"0x%x",id);
			itemElem->setAttribute(XMLString::transcode("id"), XMLString::transcode(buf));
			sprintf(buf,"%d",looted?1:0);
			itemElem->setAttribute(XMLString::transcode("looted"), XMLString::transcode(buf));
			sprintf(buf,"%d",type);
			itemElem->setAttribute(XMLString::transcode("type"), XMLString::transcode(buf));
			node->appendChild(itemElem);
		}
	}
}

void CTibiaItem::saveItemLists() {
	if (!xmlInitialised) {
		XMLPlatformUtils::Initialize();
		xmlInitialised=1;
	}

	char installPath[1024] = {'\0'};
	unsigned long installPathLen=1023;
	HKEY hkey = NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Tibia Auto\\", 0, KEY_ALL_ACCESS, &hkey)) {
		RegQueryValueEx(hkey, TEXT("Install_Dir"), NULL, NULL, (unsigned char *)installPath, &installPathLen);
		RegCloseKey(hkey);
	}
	if (!strlen(installPath)) {
		AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
		exit(1);
	}
	XercesDOMParser *parser = new XercesDOMParser();
	try{
		//int itemNr;
		char pathBuf[2048];
		sprintf(pathBuf,"%s\\mods\\tibiaauto-items.xml", installPath);

		DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("Core"));

		DOMDocument  *doc = impl->createDocument (0, XMLString::transcode("item-definitions"), 0);
		doc->createComment((const unsigned short *)"<!-- Tibia Items for Tibia -->");
		
		DOMElement *root = doc->getDocumentElement();
		
		//ITEMS
		DOMNode *itemsNode = doc->createElement(XMLString::transcode("items"));
		root->appendChild(itemsNode);
		//recursively save data structure to XML
		saveItemsBranch(itemsNode, itemTree, doc);

		//FOOD
		DOMNode *foodNode = doc->createElement(XMLString::transcode("foods"));
		root->appendChild(foodNode);
		{
			int size = foodList.GetCount();
			for (int i=0;i<size;i++) {
				char buf[512];
				DOMElement*  itemElem = doc->createElement(XMLString::transcode("item"));
				foodNode->appendChild(itemElem);
				char* name=foodList.GetTextAtIndex(i);
				int id=foodList.GetValueAtIndex(i);
				int time=foodList.GetExtraInfoAtIndex(i);

				sprintf(buf, "0x%x", id);
				itemElem->setAttribute(XMLString::transcode("id"), XMLString::transcode(buf));

				itemElem->setAttribute(XMLString::transcode("name"), XMLString::transcode(name));

				sprintf(buf, "%d", time);
				itemElem->setAttribute(XMLString::transcode("time"), XMLString::transcode(buf));

			}
		}
		//CONSTS
		/* never any need to save constants anymore
		DOMNode *constsNode = doc->createElement(XMLString::transcode("consts"));
		root->appendChild(constsNode);
		int size = constCodeList.GetCount();
		for (int i=0;i<size;i++){
			char buf[512];
			DOMElement*  itemElem = doc->createElement(XMLString::transcode("const"));
			constsNode->appendChild(itemElem);
			char* code=constCodeList.GetTextAtIndex(i);
			int value=constCodeList.GetValueAtIndex(i);

			sprintf(buf, "0x%x", value);
			itemElem->setAttribute(XMLString::transcode("value"), XMLString::transcode(buf));

			itemElem->setAttribute(XMLString::transcode("code"), XMLString::transcode(code));
		}
		*/

		XMLCh tempStr[100];
		XMLString::transcode("LS", tempStr, 99);
		impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
		DOMWriter* theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();
		if( theSerializer->canSetFeature( xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true ) ){
			theSerializer->setFeature( xercesc::XMLUni::fgDOMWRTFormatPrettyPrint , true );
		}
		xercesc::XMLFormatTarget *outfile = new xercesc::LocalFileFormatTarget(pathBuf) ;
		theSerializer->writeNode(outfile, *doc);
		theSerializer->release();
		doc->release();
		delete outfile;
		//delete theSerializer; //  Equivalent to the theSerializer->release() statement

	} catch (...) {
		AfxMessageBox("Unable to save item definitions!");
	}

	delete parser;
}
