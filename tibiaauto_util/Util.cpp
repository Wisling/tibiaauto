// Util.cpp: implementation of the CUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Util.h"
#include "Queue.h"

#include "TibiaContainer.h"
#include "TibiaMap.h"
#include "PackSender.h"
#include "MemReader.h"

extern CTibiaMap tibiaMap;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUtil::CUtil()
{

}

CUtil::~CUtil()
{

}

CTibiaItem * CUtil::lookupItem(int containerNr, CUIntArray *itemsAccepted)
{	
	CMemReader reader;
	int foodContNr=-1;	
	 
	int itemNr;
	
	CTibiaContainer *container = reader.readContainer(containerNr);
	for (itemNr=0;itemNr<container->itemsInside;itemNr++)
	{
		CTibiaItem *item = (CTibiaItem *)container->items.GetAt(itemNr);
		int pos;
		for (pos=0;pos<itemsAccepted->GetSize();pos++)
		{
			if (item->objectId==itemsAccepted->GetAt(pos))
			{
				// item found!
				CTibiaItem *retItem = new CTibiaItem();				
				retItem->pos=itemNr;
				retItem->objectId=item->objectId;
				retItem->quantity=item->quantity;				
	
				delete container;
				return retItem;
			}
		};
	};				
	
	delete container;
	return NULL;
}

CTibiaItem * CUtil::lookupItem(int containerNr, CUIntArray *itemsAccepted,int qty)
{	
	CMemReader reader;
	int foodContNr=-1;	
	 
	int itemNr;
	
	CTibiaContainer *container = reader.readContainer(containerNr);
	for (itemNr=0;itemNr<container->itemsInside;itemNr++)
	{
		CTibiaItem *item = (CTibiaItem *)container->items.GetAt(itemNr);
		int pos;
		for (pos=0;pos<itemsAccepted->GetSize();pos++)
		{
			if (item->objectId==itemsAccepted->GetAt(pos)&&item->quantity==qty)
			{
				// item found!
				CTibiaItem *retItem = new CTibiaItem();				
				retItem->pos=itemNr;
				retItem->objectId=item->objectId;
				retItem->quantity=item->quantity;				
	
				delete container;
				return retItem;
			}
		};
	};				
	
	delete container;
	return NULL;
}

void CUtil::findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int path[])
{
	findPathOnMap(startX,startY,startZ,endX,endY,endZ,path,0);
}

void CUtil::findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int path[], int allowSteponGoUp)
{
	char buf[256];
	CQueue *queue = new CQueue();

	point p;
	p.x=endX;
	p.y=endY;
	p.z=endZ;

	queue->add(p);

	tibiaMap.clearPrevPoint();
	path[0]=0;
	memset(path,0x00,sizeof(int)*5000);
		
	while (queue->size())
	{	
		point currentPoint = queue->getFirst();
		sprintf(buf,"queue size = %d; cur=(%d,%d,%d)",queue->size(),currentPoint.x,currentPoint.y,currentPoint.z);
		//AfxMessageBox(buf);
		if (currentPoint.x==startX&&currentPoint.y==startY&&currentPoint.z==startZ)
		{						
			sprintf(buf,"find path (%d,%d,%d)->(%d,%d,%d)",startX,startY,startZ,endX,endY,endZ);
			//AfxMessageBox(buf);
			//AfxMessageBox("got it");
			int pos=0;
			while (!(currentPoint.x==endX&&currentPoint.y==endY&&currentPoint.z==endZ)&&pos<10)
			{
				int nextX = tibiaMap.getPrevPointX(currentPoint.x,currentPoint.y,currentPoint.z);
				int nextY = tibiaMap.getPrevPointY(currentPoint.x,currentPoint.y,currentPoint.z);
				int nextZ = tibiaMap.getPrevPointZ(currentPoint.x,currentPoint.y,currentPoint.z);
				if (currentPoint.x+1==nextX&&currentPoint.y==nextY&&currentPoint.z==nextZ)
					path[pos++]=1;
				if (currentPoint.x-1==nextX&&currentPoint.y==nextY&&currentPoint.z==nextZ)
					path[pos++]=5;
				if (currentPoint.x==nextX&&currentPoint.y+1==nextY&&currentPoint.z==nextZ)
					path[pos++]=7;
				if (currentPoint.x==nextX&&currentPoint.y-1==nextY&&currentPoint.z==nextZ)
					path[pos++]=3;
				if (currentPoint.x==nextX&&currentPoint.y==nextY&&currentPoint.z==nextZ+1)
				{
					// go to up/down point - if we are there return the special 
					// up/down code; otherwise just walk there
					if (pos==0)
					{
						path[pos++]=0xD0;
					}					
					return;
				}
				if (currentPoint.x==nextX&&currentPoint.y==nextY&&currentPoint.z==nextZ-1)
				{
					// go to up/down point - if we are there return the special 
					// up/down code; otherwise just walk there
					if (pos==0)
					{
						path[pos++]=0xD1;
					} 					
					return;
				}
				currentPoint.x=nextX;
				currentPoint.y=nextY;
				currentPoint.z=nextZ;
			};	
			sprintf(buf,"result found - len=%d",pos);			
			
			return;
		}
		
		/**
		 * For each direction - check whether the next point is available on the map
		 * and whether it has not been visited before.
		 */
		int x=currentPoint.x;
		int y=currentPoint.y;
		int z=currentPoint.z;
		if (tibiaMap.isPointAvailable(x+1,y,z)&&tibiaMap.getPrevPointX(x+1,y,z)==0
			&&(tibiaMap.getPointUpDown(x+1,y,z)<=allowSteponGoUp||startZ!=endZ))
		{
			queue->add(x+1,y,z);
			tibiaMap.setPrevPoint(x+1,y,z,x,y,z);
		}
		if (tibiaMap.isPointAvailable(x-1,y,z)&&tibiaMap.getPrevPointX(x-1,y,z)==0
			&&(tibiaMap.getPointUpDown(x-1,y,z)<=allowSteponGoUp||startZ!=endZ))
		{
			queue->add(x-1,y,z);
			tibiaMap.setPrevPoint(x-1,y,z,x,y,z);
		}
		if (tibiaMap.isPointAvailable(x,y+1,z)&&tibiaMap.getPrevPointX(x,y+1,z)==0
			&&(tibiaMap.getPointUpDown(x,y+1,z)<=allowSteponGoUp||startZ!=endZ))
		{
			queue->add(x,y+1,z);
			tibiaMap.setPrevPoint(x,y+1,z,x,y,z);
		}
		if (tibiaMap.isPointAvailable(x,y-1,z)&&tibiaMap.getPrevPointX(x,y-1,z)==0
			&&(tibiaMap.getPointUpDown(x,y-1,z)<=allowSteponGoUp||startZ!=endZ))
		{
			queue->add(x,y-1,z);
			tibiaMap.setPrevPoint(x,y-1,z,x,y,z);
		}
		if (tibiaMap.isPointAvailable(x,y,z+1)&&tibiaMap.getPrevPointX(x,y,z+1)==0&&
			tibiaMap.getPointUpDown(x,y,z+1)==1)
		{
			queue->add(x,y,z+1);
			tibiaMap.setPrevPoint(x,y,z+1,x,y,z);
		}
		if (tibiaMap.isPointAvailable(x,y,z-1)&&tibiaMap.getPrevPointX(x,y,z-1)==0&&
			tibiaMap.getPointUpDown(x,y,z-1)==2)
		{
			queue->add(x,y,z-1);
			tibiaMap.setPrevPoint(x,y,z-1,x,y,z);
		}
	}

	delete queue;	

	if (!allowSteponGoUp)
		findPathOnMap(startX,startY,startZ,endX,endY,endZ,path,1);
}


int CUtil::waitForOpenContainer(int contNr, int open)
{
	CMemReader reader;
	int t;
	for (t=0;t<20;t++)
	{
		CTibiaContainer *cont = reader.readContainer(9);
		if (cont->flagOnOff==open)
		{
			delete cont;
			return 1;
		}
		delete cont;
		Sleep(100);
	}
	return 0;
}

void CUtil::lootItemFromContainer(int conrNr, CUIntArray *acceptedItems)
{	
	CMemReader reader;
	CPackSender sender;
	CTibiaItem *item = CUtil::lookupItem(9,acceptedItems);
	CTibiaCharacter *self = reader.readSelfCharacter();
	
	if (item)
	{
		// find first free container
		int openCont;
		for (openCont=0;openCont<9;openCont++)
								{
			CTibiaContainer *targetCont = reader.readContainer(openCont);
			if (targetCont->flagOnOff&&targetCont->itemsInside<targetCont->size)
			{					
				// good, I can move to openCont
				sender.moveObjectBetweenContainers(item->objectId,0x49,item->pos,0x40+openCont,0,item->quantity?item->quantity:1);
				
				// wait for sync
				int t3=0;
				for (t3=0;t3<20;t3++)
				{
					CTibiaCharacter *selfNew = reader.readSelfCharacter();
					
					if (selfNew->cap!=self->cap)
					{
						// sync on, stop scanning containers
						delete selfNew;
						openCont=10;
						break;
					}
					
					delete selfNew;
					Sleep(100);
				}									
			}
			
			
			delete targetCont;						
		};
		delete item;
	}
	delete self;
}

char *CUtil::wc2c(const unsigned short *src)
{
	char *dest=(char *)malloc(MAX_STRING_LEN);
	memset(dest,0,MAX_STRING_LEN);
	if (src==NULL)
		return dest;
	WideCharToMultiByte( CP_ACP, 0, src, -1,dest, MAX_STRING_LEN/2-10, NULL, NULL );
	return dest;
}

char * CUtil::getNodeAttribute(DOMNode *node, char *attrName)
{	
	int attrNr;
	int attrCount=node->getAttributes()->getLength();
	

	for (attrNr=0;attrNr<attrCount;attrNr++)
	{
		DOMNode *attrNode = node->getAttributes()->item(attrNr);
		char *attrNameWide=CUtil::wc2c(attrNode->getNodeName());
		if (!strcmp(attrNameWide,attrName))
		{
			free(attrNameWide);
			return CUtil::wc2c(attrNode->getNodeValue());			
		}
		free(attrNameWide);
	}
	char *emptyRet=(char *)malloc(MAX_STRING_LEN);
	memset(emptyRet,0,MAX_STRING_LEN);
	return emptyRet;
	
}

char *CUtil::getNodeAttribute(DOMNode *node, unsigned short *attrName)
{
	DOMNode *attrNode = node->getAttributes()->getNamedItem((const unsigned short *)attrName);
	if (attrNode)
	{
		return CUtil::wc2c(attrNode->getNodeValue());
	} else {
		char *emptyRet=(char *)malloc(1);
		memset(emptyRet,0,1);
		return emptyRet;
	}
}

int CUtil::getNodeIntAttribute(DOMNode *node, char *attrName)
{
	char *attrValue=getNodeAttribute(node,attrName);
	int ret=atoi(attrValue);
	free(attrValue);
	return ret;
	
}

int CUtil::getNodeIntAttribute(DOMNode *node, unsigned short *attrName)
{
	DOMNode *attrNode = node->getAttributes()->getNamedItem((const unsigned short *)attrName);
	if (attrNode)
	{
		char buf[128];
		buf[0]=0;
		wcstombs(buf,attrNode->getNodeValue(),60);
		return atoi(buf);
	} else {
		return 0;
	};
}