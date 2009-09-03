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

	CTibiaMapProxy();
	virtual ~CTibiaMapProxy();

	
private:
	static HMODULE dllModule;
};

#endif // !defined(AFX_TIBIAMAPPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
