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
		if (tileId==99||tileData->ground||tileData->alwaysOnTop)
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


		if (tileId==99||tileData->ground||tileData->alwaysOnTop)
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
	//if (stackCount >= 10)
	//	return reader.mapGetPointItemId(point(x,y,0),0);
	return 0;
}

int isItemOnTop(int x,int y,int *itemArr,int itemArrSize)
{
	CMemReaderProxy reader;
	int pos;	
	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));	
	for (pos=1;pos<stackCount;pos++)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		CTibiaTile *tileData = reader.getTibiaTile(tileId);
		if (tileId==99||tileData->ground||tileData->alwaysOnTop)
		{
			continue;
		}		
		int i;
		for (i=0;i<itemArrSize;i++)
		{
			if (tileId==itemArr[i]) return tileId;
		}
		return 0;
	}
	return 0;
}

int isItemCovered(int x,int y,int *itemArr,int itemArrSize)
{
	CMemReaderProxy reader;
	int pos;
	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
	for (pos=1;pos<stackCount;pos++)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		CTibiaTile *tileData = reader.getTibiaTile(tileId);


		if (tileId==99||tileData->ground||tileData->alwaysOnTop)
		{
			continue;
		}	
		int i;
		for (i=0;i<itemArrSize;i++)
		{
			if (tileId==itemArr[i]) return 0;
		}		
		break;
	}
	for (;pos<stackCount;pos++)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		int i;
		for (i=0;i<itemArrSize;i++)
		{
			if (tileId==itemArr[i]) return tileId;
		}
	}
	//if (stackCount >= 10)
	//	return reader.mapGetPointItemId(point(x,y,0),0);
	return 0;
}

int itemOnTopIndex(int x,int y,int z)
{
	CMemReaderProxy reader;
	int pos;
	int stackCount=reader.mapGetPointItemsCount(point(x,y,z));
	for (pos=1;pos<stackCount;pos++)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,z),pos);
		CTibiaTile *tileData = reader.getTibiaTile(tileId);
		if (tileId==99||tileData->ground||tileData->alwaysOnTop)
		{
			continue;
		}
		return pos;
	}
	return stackCount-1;
}

int itemOnTopIndex(int x,int y)
{
	return itemOnTopIndex(x,y,0);
}

int itemOnTopCode(int x,int y)
{

	CMemReaderProxy reader;
	int pos=itemOnTopIndex(x,y);
	if (pos!=-1)
	{
		return reader.mapGetPointItemId(point(x,y,0),pos);
	}
	return 0;
}

int itemSeenOnTopIndex(int x,int y)
{
	CMemReaderProxy reader;
	int pos;	
	int stackCount=reader.mapGetPointItemsCount(point(x,y,0));
	for (pos=0;pos<stackCount;pos++)
	{
		int tileId = reader.mapGetPointItemId(point(x,y,0),pos);
		CTibiaTile *tileData = reader.getTibiaTile(tileId);
		if (tileData->ground||tileData->alwaysOnTop)
		{
			continue;
		}		
		return pos;
	}
	return stackCount-1;
}

int itemSeenOnTopCode(int x,int y)
{

	CMemReaderProxy reader;
	int pos=itemSeenOnTopIndex(x,y);
	if (pos!=-1)
	{
		return reader.mapGetPointItemId(point(x,y,0),pos);
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
		if (tileId==99||tileData->ground||tileData->alwaysOnTop)
		{
			continue;
		}		
		int qty=reader.mapGetPointItemExtraInfo(point(x,y,0),pos,1);
		return qty?qty:1;
	}
	return 1;

}