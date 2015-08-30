#pragma once
#include "tibiaauto_util.h"

#include "TibiaTile.h"

#define MAX_TILES 50000

class TIBIAAUTOUTIL_API CTileReader
{
private:
	CTileReader();
	virtual ~CTileReader();
	CTileReader(CTileReader const&);
	void operator=(CTileReader const&);
public:
	static CTileReader& getTileReader()
	{
		static CTileReader singleton;
		return singleton;
	}
	CTibiaTile *getTile(int tileNr);
	void setTile(int, CTibiaTile *);
	void cleanup();
private:
	void loadTiles();
	void saveTiles();
	static int xmlInitialised;
	static CTibiaTile *tiles[MAX_TILES];
};
