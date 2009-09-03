// TibiaItem.cpp: implementation of the CTibiaItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
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

#define ITEM	1
#define FOOD	2
#define LOOT	3	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "TibiaItem_codes.cpp"


CUIntArray *CTibiaItem::foodList=NULL;
int CTibiaItem::itemListsFresh=0;
int CTibiaItem::xmlInitialised=0;

char CTibiaItem::itemsItems[MAX_ITEMS][MAX_ITEM_LEN];
int CTibiaItem::itemsItemsId[MAX_ITEMS];
int CTibiaItem::itemsItemsExtra[MAX_ITEMS];
int CTibiaItem::itemsItemsCount=0;
char CTibiaItem::itemsFood[MAX_ITEMS][MAX_ITEM_LEN];
int CTibiaItem::itemsFoodId[MAX_ITEMS];
int CTibiaItem::itemsFoodTime[MAX_ITEMS];
int CTibiaItem::itemsFoodCount=0;
char CTibiaItem::itemsCorpses[MAX_ITEMS][MAX_ITEM_LEN];
int CTibiaItem::itemsCorpsesId[MAX_ITEMS];
int CTibiaItem::itemsCorpsesCount=0;
char CTibiaItem::itemsLooted[MAX_ITEMS][MAX_ITEM_LEN];
int CTibiaItem::itemsLootedId[MAX_ITEMS];
int CTibiaItem::itemsLootedExtra[MAX_ITEMS];
int CTibiaItem::itemsLootedCount=0;
char CTibiaItem::constsCode[MAX_ITEMS][MAX_ITEM_LEN];
int CTibiaItem::constsValue[MAX_ITEMS];
int CTibiaItem::constsCount;

extern CRITICAL_SECTION ItemsInitCriticalSection;


CTibiaItem::CTibiaItem()
{
	objectId=0;
	quantity=0;
	pos=0;
}


int CTibiaItem::getIndex(int objectId, int type) {	
	int i;	
	refreshItemLists();
	if (type == ITEM) {
		for (i=0;i<itemsItemsCount;i++) 	{
			if (itemsItemsId[i] == objectId)
				return i;
		}
	}
	if (type == FOOD) {
		for (i=0;i<itemsItemsCount;i++) 	{
			if (itemsFoodId[i] == objectId)
				return i;
		}
	}
	if (type == LOOT) {
		for (i=0;i<itemsItemsCount;i++) 	{
			if (itemsLootedId[i] == objectId)
				return i;
		}
	}
	return -1;
}

void CTibiaItem::setExtraInfo(int index, int info, int type) {
	if (type == ITEM) {
		itemsItemsExtra[index] = info;
	}
	if (type == FOOD) {
		itemsFoodTime[index] = info;
	}
	if (type == LOOT) {
		itemsLootedExtra[index] = info;
	}
}

char * CTibiaItem::getName(int objectId)
{	
	int i;
	
	refreshItemLists();

	for (i=0;i<itemsItemsCount;i++)
	{
		if (itemsItemsId[i]==objectId)
			return itemsItems[i];
	}		
	
	
	return "unknown";
}

void CTibiaItem::setName(int index, char *name, int type) {	
	if (type == ITEM) {
		memcpy(itemsItems[index], name, 59);
	}
	if (type == FOOD) {
		memcpy(itemsFood[index], name, 59);
	}
	if (type == LOOT) {
		memcpy(itemsLooted[index], name, 59);
	}
}

void CTibiaItem::addName(char *name, int type) {	
	if (type == ITEM) {
		if (itemsItemsCount < MAX_ITEMS)
			setName(itemsItemsCount, name, type);
	}
	if (type == FOOD) {
		if (itemsItemsCount < MAX_ITEMS)
			setName(itemsFoodCount, name, type);
	}
	if (type == LOOT) {
		if (itemsItemsCount < MAX_ITEMS)
			setName(itemsLootedCount, name, type);
	}
}

void CTibiaItem::setObjectId(int index, int objectId, int type) {
	if (type == ITEM) {
		itemsItemsId[index] = objectId;
	}
	if (type == FOOD) {
		itemsFoodId[index] = objectId;
	}
	if (type == LOOT) {
		itemsLootedId[index] = objectId;
	}
}

void CTibiaItem::addObjectId(int objectId, int type) {	
	if (type == ITEM) {
		if (itemsItemsCount < MAX_ITEMS)
			setObjectId(itemsItemsCount, objectId, type);
	}
	if (type == FOOD) {
		if (itemsItemsCount < MAX_ITEMS)
			setObjectId(itemsFoodCount, objectId, type);
	}
	if (type == LOOT) {
		if (itemsItemsCount < MAX_ITEMS)
			setObjectId(itemsLootedCount, objectId, type);
	}
}

void CTibiaItem::addExtraInfo(int extraInfo, int type) {	
	if (type == ITEM) {
		if (itemsItemsCount < MAX_ITEMS)
			setExtraInfo(itemsItemsCount, extraInfo, type);
	}
	if (type == FOOD) {
		if (itemsItemsCount < MAX_ITEMS)
			setExtraInfo(itemsFoodCount, extraInfo, type);
	}
	if (type == LOOT) {
		if (itemsItemsCount < MAX_ITEMS)
			setExtraInfo(itemsLootedCount, extraInfo, type);
	}
}

int CTibiaItem::getObjectId(char *name)
{
	int i;
	
	refreshItemLists();

	for (i=0;i<itemsItemsCount;i++)
	{
		if (!strcmp(itemsItems[i],name))
			return itemsItemsId[i];
	}		
	
	
	return 0;
}

int CTibiaItem::getFoodId(char *name)
{
	int i;
	
	refreshItemLists();

	for (i=0;i<itemsFoodCount;i++)
	{
		if (!strcmp(itemsFood[i],name))
			return itemsFoodId[i];
	}		
	
	
	return 0;
}

int CTibiaItem::getLootItemId(char *name)
{
	int i;
	
	refreshItemLists();

	for (i=0;i<itemsLootedCount;i++)
	{
		if (!strcmp(itemsLooted[i],name))
			return itemsLootedId[i];
	}		
	
	
	return 0;
}

void CTibiaItem::addItem(char *name, int objectId, int extraInfo, int type) {
	if (type == ITEM) {
		addName(name, ITEM);
		addObjectId(objectId, ITEM);
		addExtraInfo(extraInfo, ITEM);
		itemsItemsCount++;
	}
	if (type == FOOD) {
		addName(name, FOOD);
		addObjectId(objectId, FOOD);
		addExtraInfo(extraInfo, FOOD);
		itemsFoodCount++;
	}
	if (type == LOOT) {
		addName(name, LOOT);
		addObjectId(objectId, LOOT);
		addExtraInfo(extraInfo, LOOT);
		itemsLootedCount++;
	}
	
	//AfxMessageBox("Item Added");
}

int CTibiaItem::getCorpseIdByCreatureName(char *name)
{		
	int i;
	refreshItemLists();

	for (i=0;i<itemsCorpsesCount;i++)
	{
		if (!strcmp(itemsCorpses[i],name))
			return itemsCorpsesId[i];
	}
	
	return 0;
}

int CTibiaItem::getValueForConst(char *code)
{		
	refreshItemLists();	

	int i;
	int len;	
	char lookupCode[256];
	strcpy(lookupCode,code);

	len=strlen(lookupCode);
	for (i=0;i<len;i++)
		lookupCode[i]=tolower(lookupCode[i]);

	for (i=0;i<constsCount;i++)
	{
		if (!strcmp(constsCode[i],lookupCode))
			return constsValue[i];
	}

	char msgBuf[128];
	sprintf(msgBuf,"ERROR: Unable to find value for const code='%s'!",lookupCode);
	AfxMessageBox(msgBuf);
	
	return 0;
}

CUIntArray * CTibiaItem::getItemsFood()
{	
	refreshItemLists();
	if (!foodList)
	{
		foodList = new CUIntArray();
		int i;
		for (i=0;i<itemsFoodCount;i++)
		{
			foodList->Add(itemsFoodId[i]);			
		}
	}
	return foodList;
}
void CTibiaItem::setCount(int type, int newCount) {
	if (type == ITEM)
		itemsItemsCount = newCount;
	if (type == LOOT)
		itemsLootedCount = newCount;
	if (type == FOOD)
		itemsFoodCount = newCount;
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

		itemsItemsCount=0;
		itemsLootedCount=0;
		itemsCorpsesCount=0;
		itemsFoodCount=0;
		constsCount=0;
				
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
				if (!wcscmp(listNode->getNodeName(),_L("items"))) {
					
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
						memcpy(itemsItems[itemsItemsCount],objectName,strlen(objectName)+1);
						itemsItemsId[itemsItemsCount]=objectId;
						itemsItemsCount++;
						if (objectName) free(objectName);
					}
				}
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
						
						
						memcpy(itemsFood[itemsFoodCount],objectName,strlen(objectName)+1);
						itemsFoodId[itemsFoodCount]=objectId;
						itemsFoodTime[itemsFoodCount] = eatTime;
						itemsFoodCount++;
						if (objectName) free(objectName);
					}
				}

				if (!wcscmp(listNode->getNodeName(),_L("corpses"))) {
					
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
						
						memcpy(itemsCorpses[itemsCorpsesCount],objectName,strlen(objectName)+1);
						itemsCorpsesId[itemsCorpsesCount]=objectId;
						itemsCorpsesCount++;
						if (objectName) free(objectName);
					}
				}

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
						
						memcpy(itemsLooted[itemsLootedCount],objectName,strlen(objectName)+1);
						itemsLootedId[itemsLootedCount]=objectId;
						itemsLootedCount++;
						if (objectName) free(objectName);
					}
				}

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
								if (idTmp[0]=='0'&&idTmp[1]=='x')
								{
									// 0xFFFF pattern
									sscanf(idTmp,"0x%x",&constValue);
								} else {
									// 1234 pattern
									sscanf(idTmp,"%d",&constValue);
								}
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
						
						memcpy(constsCode[constsCount],constCode,strlen(constCode)+1);
						constsValue[constsCount]=constValue;
						constsCount++;

						if (constCode) free(constCode);						
					}
				}
				
			}
		}
		
	} catch (...)
	{
		AfxMessageBox("Unable to load item definitions!");
	}

	delete parser;		


	// remove cached food list
	delete foodList;
	foodList=NULL;
	LeaveCriticalSection(&ItemsInitCriticalSection);
}
 
void CTibiaItem::saveItemLists() {
	if (!xmlInitialised) {
		XMLPlatformUtils::Initialize();
		xmlInitialised=1;
	}

	char buf[64];
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
	try {	
		int itemNr;		
		char pathBuf[2048];
		sprintf(pathBuf,"%s\\mods\\tibiaauto-items.xml", installPath);

		DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("Core"));

		DOMDocument  *doc = impl->createDocument (0, XMLString::transcode("item-definitions"), 0);
		
		DOMElement *root = doc->getDocumentElement();	
		
		DOMNode *itemsNode = doc->createElement(XMLString::transcode("items"));
		root->appendChild(itemsNode);
		for (itemNr = 0; itemNr < itemsItemsCount; itemNr++) {
			DOMElement*  itemElem = doc->createElement(XMLString::transcode("item"));
			itemsNode->appendChild(itemElem);
			sprintf(buf, "0x%x", itemsItemsId[itemNr]);
			itemElem->setAttribute(XMLString::transcode("id"), XMLString::transcode(buf));
			itemElem->setAttribute(XMLString::transcode("name"), XMLString::transcode(itemsItems[itemNr]));
		}
		
		DOMNode *foodNode = doc->createElement(XMLString::transcode("foods"));
		root->appendChild(foodNode);
		for (itemNr = 0;itemNr < itemsFoodCount; itemNr++) {
			DOMElement*  itemElem = doc->createElement(XMLString::transcode("item"));
			foodNode->appendChild(itemElem);
			sprintf(buf, "0x%x", itemsFoodId[itemNr]);
			itemElem->setAttribute(XMLString::transcode("id"), XMLString::transcode(buf));
			itemElem->setAttribute(XMLString::transcode("name"), XMLString::transcode(itemsFood[itemNr]));
			sprintf(buf, "%d", itemsFoodTime[itemNr]);
			itemElem->setAttribute(XMLString::transcode("time"), XMLString::transcode(buf));
		}
		
		DOMNode *corpsesNode = doc->createElement(XMLString::transcode("corpses"));
		root->appendChild(corpsesNode);
		for (itemNr = 0; itemNr < itemsCorpsesCount; itemNr++) {
			DOMElement*  itemElem = doc->createElement(XMLString::transcode("item"));
			corpsesNode->appendChild(itemElem);
			sprintf(buf, "0x%x", itemsCorpsesId[itemNr]);
			itemElem->setAttribute(XMLString::transcode("id"), XMLString::transcode(buf));
			itemElem->setAttribute(XMLString::transcode("name"), XMLString::transcode(itemsCorpses[itemNr]));
		}
		
		DOMNode *lootNode = doc->createElement(XMLString::transcode("looted"));
		root->appendChild(lootNode);
		for (itemNr = 0; itemNr < itemsLootedCount; itemNr++) {
			DOMElement*  itemElem = doc->createElement(XMLString::transcode("item"));
			lootNode->appendChild(itemElem);
			sprintf(buf, "0x%x", itemsLootedId[itemNr]);
			itemElem->setAttribute(XMLString::transcode("id"), XMLString::transcode(buf));
			itemElem->setAttribute(XMLString::transcode("name"), XMLString::transcode(itemsLooted[itemNr]));
		}
		
		DOMNode *constsNode = doc->createElement(XMLString::transcode("consts"));
		root->appendChild(constsNode);
		for (itemNr = 0; itemNr < constsCount; itemNr++) {
			DOMElement*  itemElem = doc->createElement(XMLString::transcode("const"));
			constsNode->appendChild(itemElem);
			sprintf(buf, "0x%x", constsValue[itemNr]);
			itemElem->setAttribute(XMLString::transcode("value"), XMLString::transcode(buf));
			itemElem->setAttribute(XMLString::transcode("code"), XMLString::transcode(constsCode[itemNr]));
		}


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
		delete theSerializer;
		
	}
	catch (...) {
		AfxMessageBox("Unable to save item definitions!");
	}

	delete parser;		
}
 
