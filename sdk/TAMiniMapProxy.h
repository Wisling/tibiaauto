// MemReaderProxy.h: interface for the CMemReaderProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TAMINIMAPPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
#define AFX_TAMINIMAPPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_

#include "TibiaMiniMapPoint.h"
#include "MemConstData.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTAMiniMapProxy
{
public:
	int isPointInMiniMap(int x,int y,int z);
	CUIntArray * findPathOnMiniMap(int x,int y,int z,int x2,int y2,int z2);
	CTibiaMiniMapPoint * getMiniMapPoint(int x,int y,int z);
	void setMiniMapPoint(int x,int y,int z,int col,int spd);
	void findPathStop();
	bool isFindPathStopped();
	int getCurrentDistance();


	CTAMiniMapProxy();
	virtual ~CTAMiniMapProxy();

	
private:
	static HMODULE dllModule;
};

#endif // !defined(AFX_TAMINIMAPPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
