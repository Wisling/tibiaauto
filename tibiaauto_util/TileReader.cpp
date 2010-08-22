// TileReader.cpp: implementation of the CTileReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "TileReader.h"
#include "Util.h"
#include <time.h>

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
int CTileReader::xmlInitialised=0;
CTibiaTile *CTileReader::tiles[MAX_TILES];


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTileReader::CTileReader()
{
	if (!xmlInitialised)
	{
		XMLPlatformUtils::Initialize();
		xmlInitialised=1;
		loadTiles();
		xmlInitialised=2;
	}
	if (xmlInitialised!=2){ //wait 10 seconds to load
		for (int wait=200;xmlInitialised!=2;wait--)
			Sleep(50);
		if (xmlInitialised!=2) AfxMessageBox("Waited 10 seconds to load items.  Items failed to load.");
	}

}

CTileReader::~CTileReader()
{
}

void CTileReader::cleanup()
{
	for (int i=0;i<MAX_TILES;i++) {
		delete(tiles[i]);
		tiles[i]=NULL;
	}
}


CTibiaTile *CTileReader::getTile(int tileNr)
{
	CTibiaTile* ret=tiles[tileNr];
	return ret;
}

void CTileReader::setTile(int tileNr, CTibiaTile *newTile) {
	tiles[tileNr] = newTile;
	saveTiles();
}

void CTileReader::loadTiles() {
	int i;
	for (i=0;i<MAX_TILES;i++) {
		tiles[i]=new CTibiaTile();
	}
	XercesDOMParser *parser = new XercesDOMParser();
	try {	
		char installPath[1024];
		unsigned long installPathLen=1023;
		installPath[0]='\0';
		HKEY hkey=NULL;
		if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey)) {
			RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
			RegCloseKey(hkey);
		}
		if (!strlen(installPath)) {
			AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
			exit(1);
		}
		int rootNr;
		char pathBuf[2048];
		sprintf(pathBuf,"%s\\mods\\tibiaauto-tiles.xml",installPath);
		parser->parse(pathBuf);
		DOMNode  *doc = parser->getDocument();	
		int rootCount=doc->getChildNodes()->getLength();		
		for (rootNr=0;rootNr<rootCount;rootNr++) {			
			DOMNode *root = doc->getChildNodes()->item(rootNr);
			if (wcscmp(root->getNodeName(),_L("tiles")))
				continue;					
			DOMNode *item = root->getFirstChild();
			if (item) {
				do 	{			
					if (!wcscmp(item->getNodeName(),_L("tile"))) {	
						int tileId = CUtil::getNodeIntAttribute(item,_L("id"));
						
						tiles[tileId]->blocking=CUtil::getNodeIntAttribute(item,_L("blocking"));						
						tiles[tileId]->canWalkThrough=CUtil::getNodeIntAttribute(item,_L("canWalkThrough"));
						tiles[tileId]->goDown=CUtil::getNodeIntAttribute(item,_L("goDown"));
						tiles[tileId]->goUp=CUtil::getNodeIntAttribute(item,_L("goUp"));
						tiles[tileId]->ground=CUtil::getNodeIntAttribute(item,_L("ground"));
						tiles[tileId]->isContainer=CUtil::getNodeIntAttribute(item,_L("isContainer"));
						tiles[tileId]->isDepot=CUtil::getNodeIntAttribute(item,_L("isDepot"));
						tiles[tileId]->requireRope=CUtil::getNodeIntAttribute(item,_L("requireRope"));
						tiles[tileId]->requireShovel=CUtil::getNodeIntAttribute(item,_L("requireShovel"));
						tiles[tileId]->requireUse=CUtil::getNodeIntAttribute(item,_L("requireUse"));
						tiles[tileId]->speed=CUtil::getNodeIntAttribute(item,_L("speed"));												
						tiles[tileId]->minimapColor=CUtil::getNodeIntAttribute(item,_L("minimapColor"));
						tiles[tileId]->notMoveable=CUtil::getNodeIntAttribute(item,_L("notMoveable"));
						tiles[tileId]->stackable=CUtil::getNodeIntAttribute(item,_L("stackable"));
						tiles[tileId]->alwaysOnTop=CUtil::getNodeIntAttribute(item,_L("alwaysOnTop"));
						tiles[tileId]->moreAlwaysOnTop=CUtil::getNodeIntAttribute(item,_L("moreAlwaysOnTop"));
						tiles[tileId]->isTeleporter=CUtil::getNodeIntAttribute(item,_L("isTeleporter"));
						tiles[tileId]->isReadable=CUtil::getNodeIntAttribute(item,_L("isReadable"));
						tiles[tileId]->isFluid=CUtil::getNodeIntAttribute(item,_L("isFluid"));
						tiles[tileId]->isRune=CUtil::getNodeIntAttribute(item,_L("isRune"));
						tiles[tileId]->isUseable=CUtil::getNodeIntAttribute(item,_L("isUseableWith"));
						tiles[tileId]->isUseableImmobile=CUtil::getNodeIntAttribute(item,_L("isUseableImmobile"));
						tiles[tileId]->blockPathFind=CUtil::getNodeIntAttribute(item,_L("blockPathFind"));
					}		
				} while ((item=item->getNextSibling())!=NULL);
			}
		}				
	} 
	catch (...) {
		AfxMessageBox("Unable to load tile definitions!");
	}
	
	delete parser;		
}

void CTileReader::saveTiles() {
	XercesDOMParser *parser = new XercesDOMParser();
	try {	
		char installPath[1024] = {'\0'};
		unsigned long installPathLen=1023;
		HKEY hkey=NULL;
		if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey)) {
			RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
			RegCloseKey(hkey);
		}
		if (!strlen(installPath)) {
			AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
			exit(1);
		}
		int rootNr;
		char pathBuf[2048];
		sprintf(pathBuf,"%s\\mods\\tibiaauto-tiles.xml",installPath);
		parser->parse(pathBuf);
		DOMNode  *doc = parser->getDocument();	
		int rootCount=doc->getChildNodes()->getLength();		
		for (rootNr=0;rootNr<rootCount;rootNr++) {			
			DOMNode *root = doc->getChildNodes()->item(rootNr);
			if (wcscmp(root->getNodeName(),_L("tiles")))
				continue;					
			DOMNode *item = root->getFirstChild();
			if (item) {
				do {			
					if (!wcscmp(item->getNodeName(),_L("tile"))) {
						int tileId = CUtil::getNodeIntAttribute(item,_L("id"));
						CUtil::setNodeIntAttribute(item,_L("blocking"), tiles[tileId]->blocking);						
						CUtil::setNodeIntAttribute(item,_L("canWalkThrough"),tiles[tileId]->canWalkThrough);
						CUtil::setNodeIntAttribute(item,_L("goDown"), tiles[tileId]->goDown);
						CUtil::setNodeIntAttribute(item,_L("goUp"), tiles[tileId]->goUp);
						CUtil::setNodeIntAttribute(item,_L("ground"),tiles[tileId]->ground);
						CUtil::setNodeIntAttribute(item,_L("isContainer"), tiles[tileId]->isContainer);
						CUtil::setNodeIntAttribute(item,_L("isDepot"), tiles[tileId]->isDepot);
						CUtil::setNodeIntAttribute(item,_L("requireRope"), tiles[tileId]->requireRope);
						CUtil::setNodeIntAttribute(item,_L("requireShovel"), tiles[tileId]->requireShovel);
						CUtil::setNodeIntAttribute(item,_L("requireUse"), tiles[tileId]->requireUse);
						CUtil::setNodeIntAttribute(item,_L("speed"), tiles[tileId]->speed);												
						CUtil::setNodeIntAttribute(item,_L("minimapColor"), tiles[tileId]->minimapColor);
						CUtil::setNodeIntAttribute(item,_L("notMoveable"), tiles[tileId]->notMoveable);
						CUtil::setNodeIntAttribute(item,_L("stackable"),  tiles[tileId]->stackable);
						CUtil::setNodeIntAttribute(item,_L("alwaysOnTop"), tiles[tileId]->alwaysOnTop);
						CUtil::setNodeIntAttribute(item,_L("moreAlwaysOnTop"), tiles[tileId]->moreAlwaysOnTop);
						CUtil::setNodeIntAttribute(item,_L("isTeleporter"), tiles[tileId]->isTeleporter);
						CUtil::setNodeIntAttribute(item,_L("isReadable"), tiles[tileId]->isReadable);
						CUtil::setNodeIntAttribute(item,_L("isFluid"), tiles[tileId]->isFluid);
						CUtil::setNodeIntAttribute(item,_L("isRune"), tiles[tileId]->isRune);
						CUtil::setNodeIntAttribute(item,_L("isUseableWith"), tiles[tileId]->isUseable);
						CUtil::setNodeIntAttribute(item,_L("isUseableImmobile"), tiles[tileId]->isUseableImmobile);
						CUtil::setNodeIntAttribute(item,_L("blockPathFind"), tiles[tileId]->blockPathFind);
					}		
				} while ((item=item->getNextSibling())!=NULL);
			}
		}
		XMLCh tempStr[100];
		XMLString::transcode("LS", tempStr, 99);
		DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
		DOMWriter* theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();
		if( theSerializer->canSetFeature( xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true ) ){
			theSerializer->setFeature( xercesc::XMLUni::fgDOMWRTFormatPrettyPrint , true );
		}
		xercesc::XMLFormatTarget *outfile = new xercesc::LocalFileFormatTarget(pathBuf) ;
		theSerializer->writeNode(outfile, *doc);			
		theSerializer->release();
		delete outfile;
		delete theSerializer;
	}
	catch (...) {
		AfxMessageBox("Unable to save tile definitions!");
	}
	delete parser;
}
