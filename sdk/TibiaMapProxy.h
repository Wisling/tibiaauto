// MemReaderProxy.h: interface for the CMemReaderProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIAMAPPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
#define AFX_TIBIAMAPPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_

#include "TibiaContainer.h"
#include "TibiaCharacter.h"
#include "TibiaItem.h"
#include "MemConstData.h"

#include "TibiaMiniMapPoint.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTibiaMapProxy
{
public:
	void removePointAvailable(int x,int y,int z);
	void prohPointClear();
	void prohPointAdd(int x, int y, int z);
	int getPointUpDown(int x,int y,int z);
	int getPointUpDownNoProh(int x,int y,int z);
	void setPointUpDown(int x,int y,int z,int updown);
	int getPrevPointZ(int x,int y, int z);
	int getPrevPointY(int x,int y, int z);
	int getPrevPointX(int x,int y, int z);
	void clearPrevPoint();
	void clearDistance();
	void clearLocalPrevPoint(int x,int y, int z,int radius);
	void setPrevPoint(int x,int y,int z,int prevX, int prevY, int prevZ);
	void setBestPrevPoint(int x,int y,int z,int prevX, int prevY, int prevZ);
	struct point getRandomPoint();
	void clear();
	void setPointAsAvailable(int x,int y,int z);
	int isPointAvailable(int x,int y,int z);
	int isPointAvailableNoProh(int x,int y,int z);
	int size();
	struct point getPointByNr(int nr);
	point getDestPoint(int x,int y, int z);
	void setDestPoint(int x,int y,int z,int destX, int destY, int destZ);
	int getPointSpeed(int x,int y, int z);
	void setPointSpeed(int x, int y, int z,int speed);
	int getPointDistance(int x,int y, int z);
	void setPointDistance(int x, int y, int z,int dist);
	int calcDistance(int x, int y, int z, int prevX, int prevY, int prevZ);

	CTibiaMapProxy();
	virtual ~CTibiaMapProxy();

	
private:
	static HMODULE dllModule;
};

#endif // !defined(AFX_TIBIAMAPPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
