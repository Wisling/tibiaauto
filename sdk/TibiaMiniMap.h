// TibiaMiniMap.h: interface for the CTibiaMiniMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIAMINIMAP_H__0536B572_E292_4473_886A_38C9D6C71128__INCLUDED_)
#define AFX_TIBIAMINIMAP_H__0536B572_E292_4473_886A_38C9D6C71128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTibiaMiniMap  
{
public:
	CTibiaMiniMap();
	virtual ~CTibiaMiniMap();
	int pointsAttached;
	int segmentPrev;
	int segmentNext;
	int x;
	int y;
	int z;
	// map data is available but is skipped by purpose (too big)
};

#endif // !defined(AFX_TIBIAMINIMAP_H__0536B572_E292_4473_886A_38C9D6C71128__INCLUDED_)
