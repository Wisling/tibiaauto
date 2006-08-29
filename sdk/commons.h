/////////////////////////////////////////////////////////////////////////////
int isItemOnTop(int x,int y,int itemId)
{
	CMemReaderProxy reader;
	int pos;	
	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
	for (pos=1;pos<stackCount;pos++)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		CTibiaTile *tileData = reader.getTibiaTile(tileId);
		if (tileId==99||tileData->ground||tileData->alwaysOnTop||tileData->moreAlwaysOnTop)
		{
			continue;
		}		
		return (tileId==itemId);
	}
	return 0;
}

int isItemCovered(int x,int y,int itemId)
{
	CMemReaderProxy reader;
	int pos;	
	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
	for (pos=1;pos<stackCount;pos++)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		CTibiaTile *tileData = reader.getTibiaTile(tileId);
		if (tileId==99||tileData->ground||tileData->alwaysOnTop||tileData->moreAlwaysOnTop)
		{
			continue;
		}		
		if (tileId==itemId) return 0;
		break;
	}
	for (;pos<stackCount;pos++)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		if (tileId==itemId)
			return 1;
	}
	return 0;
}

int itemOnTopCode(int x,int y)
{
	CMemReaderProxy reader;
	int pos;	
	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
	for (pos=1;pos<stackCount;pos++)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		CTibiaTile *tileData = reader.getTibiaTile(tileId);
		if (tileId==99||tileData->ground||tileData->alwaysOnTop||tileData->moreAlwaysOnTop)
		{
			continue;
		}		
		return tileId;
	}
	return 0;
}

int itemOnTopQty(int x,int y)
{
	CMemReaderProxy reader;
	int pos;	
	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
	for (pos=1;pos<stackCount;pos++)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		CTibiaTile *tileData = reader.getTibiaTile(tileId);
		if (tileId==99||tileData->ground||tileData->alwaysOnTop||tileData->moreAlwaysOnTop)
		{
			continue;
		}		
		int qty=reader.mapGetPointItemExtraInfo(point(x,y,0),pos,1);
		return qty?qty:1;
	}
	return 1;

}