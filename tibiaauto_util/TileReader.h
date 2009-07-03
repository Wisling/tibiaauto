// TileReader.h: interface for the CTileReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILEREADER_H__D5B45C88_FF5F_4F3C_8978_7A12D58F2DDC__INCLUDED_)
#define AFX_TILEREADER_H__D5B45C88_FF5F_4F3C_8978_7A12D58F2DDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TibiaTile.h"

#define MAX_TILES 20000

class CTileReader  
{
public:
	CTileReader();
	virtual ~CTileReader();
	CTibiaTile *getTile(int tileNr);
	void setTile(int, CTibiaTile *);
private:
	void loadTiles();
	void saveTiles();
	static int xmlInitialised;
	static CTibiaTile *tiles[MAX_TILES];

};

#endif // !defined(AFX_TILEREADER_H__D5B45C88_FF5F_4F3C_8978_7A12D58F2DDC__INCLUDED_)
