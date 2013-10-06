// TibiaMiniMapPoint.h: interface for the CTibiaMiniMapPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIAMINIMAPPOINT_H__8571B5CD_B0D8_4F43_B4A0_D523C9730686__INCLUDED_)
#define AFX_TIBIAMINIMAPPOINT_H__8571B5CD_B0D8_4F43_B4A0_D523C9730686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTibiaMiniMapPoint  
{
public:
	CTibiaMiniMapPoint();
	CTibiaMiniMapPoint(int x,int y,int z,int colour,int speed);
	virtual ~CTibiaMiniMapPoint();
	int x;
	int y;
	int z;
	int colour;
	int speed;
};

#endif // !defined(AFX_TIBIAMINIMAPPOINT_H__8571B5CD_B0D8_4F43_B4A0_D523C9730686__INCLUDED_)
