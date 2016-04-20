#include "stdafx.h"
#include "TibiaStructures.h"


CTibiaListData::CTibiaListData(int a_val, char* a_s, int a_val2, int a_val3)
{
	s = (char*)malloc(strlen(a_s) + 1);
	strcpy(s, a_s);

	val = a_val;
	val2 = a_val2;
	SetType(a_val3);
}

CTibiaListData::CTibiaListData(int a_val, char* a_s)
{
	s = (char*)malloc(strlen(a_s) + 1);
	strcpy(s, a_s);

	val = a_val;
	val2 = 0;
	SetType(0);
}

char* CTibiaListData::GetText()
{
	return s;
}

int CTibiaListData::GetValue()
{
	return val;
}

int CTibiaListData::GetType()
{
	return type;
}

int CTibiaListData::GetExtraInfo()
{
	return val2;
}

void CTibiaListData::SetExtraInfo(int a_val2)
{
	val2 = a_val2;
}

void CTibiaListData::SetValue(int a_val)
{
	val = a_val;
}

void CTibiaListData::SetType(int a_val)
{
	type = a_val;
}

void CTibiaListData::SetText(char* a_s)
{
	free(s);
	s = (char*)malloc(strlen(a_s) + 1);
	strcpy(s, a_s);
}

CTibiaListData::~CTibiaListData()
{
	free(s);
}

void CTibiaListData::toString(char* outStr)
{
	int ind = strlen(outStr);
	sprintf(outStr + ind, "(%d,%s,%d)\n", val, s, val2);
}

CTibiaList::CTibiaList()
{
	SetMemSize(8);
}

CTibiaList::~CTibiaList()
{
	RemoveAll();
}

bool CTibiaList::Add(CTibiaListData* data)
{
	if (data == NULL)
		return FALSE;
	int a_val = data->GetValue();
	int type = data->GetType();
	char* a_s = data->GetText();
	if (HasText(a_s))
		return FALSE;

	dataList.Add(data);
	valList.Add(a_val);
	strMap[a_s] = data;
	if (GetCount() > memSize * 1.5)
		SetMemSize(memSize * 2);
	return TRUE;
}

bool CTibiaList::Add(int a_val, char* a_s, int a_val2 /*= 0*/, int a_val3 /*= 0*/)
{
	CTibiaListData* data = new CTibiaListData(a_val, a_s, a_val2, a_val3);
	if (Add(data))
		return TRUE;
	delete data;
	return FALSE;
}

bool CTibiaList::HasValue(int val)
{
	int size = dataList.GetSize();
	for (int i = 0; i < size; i++)
	{
		if (valList[i] == val)
			return TRUE;
	}
	return FALSE;
}

bool CTibiaList::HasText(char* s)
{
	return strMap.find(s) != strMap.end();
}

int CTibiaList::GetCount()
{
	return dataList.GetSize();
}

int CTibiaList::GetIndex(int val)
{
	int size = dataList.GetSize();
	for (int i = 0; i < size; i++)
	{
		if (valList[i] == val)
			return i;
	}
	return -1;
}

char* CTibiaList::GetText(int val)
{
	int size = dataList.GetSize();
	for (int i = 0; i < size; i++)
	{
		if (valList[i] == val)
			return dataList[i]->GetText();
	}
	return NULL;
}

int CTibiaList::GetValue(char* s)
{
	StrMap::iterator iter = strMap.find(s);
	if (iter == strMap.end())
		return 0;
	return iter->second->GetValue();
}

int CTibiaList::GetExtraInfo(int val)
{
	int size = dataList.GetSize();
	for (int i = 0; i < size; i++)
	{
		if (valList[i] == val)
			return dataList[i]->GetExtraInfo();
	}
	return 0;
}

int CTibiaList::GetExtraInfo(char* s)
{
	StrMap::iterator iter = strMap.find(s);
	if (iter == strMap.end())
		return 0;
	return iter->second->GetExtraInfo();
}

int CTibiaList::GetValueAtIndex(int ind)
{
	if (ind >= 0 && ind < dataList.GetSize())
		return valList[ind];
	return 0;
}

char* CTibiaList::GetTextAtIndex(int ind)
{
	if (ind >= 0 && ind < dataList.GetSize())
		return dataList[ind]->GetText();
	return NULL;
}

int CTibiaList::GetTypeAtIndex(int ind)
{
	if (ind >= 0 && ind < dataList.GetSize())
		return dataList[ind]->GetType();
	return NULL;
}

int CTibiaList::GetExtraInfoAtIndex(int ind)
{
	if (ind >= 0 && ind < dataList.GetSize())
		return dataList[ind]->GetExtraInfo();
	return 0;
}

void CTibiaList::SetValueAtIndex(int ind, int val)
{
	if (!(ind >= 0 && ind < dataList.GetSize()))
		AfxMessageBox("CTibiaList index out of bounds!");
	dataList[ind]->SetValue(val);
	valList[ind] = val;
}

void CTibiaList::SetTextAtIndex(int ind, char* s)
{
	if (!(ind >= 0 && ind < dataList.GetSize()))
		AfxMessageBox("CTibiaList index out of bounds!");
	if (HasText(s))
		return;
	CTibiaListData* data = dataList[ind];
	StrMap::iterator iter = strMap.find(data->GetText());
	if (iter == strMap.end())
		AfxMessageBox("Internal Error, strMap missing entry!");
	//free(iter->first);  string key is same as string within data
	strMap.erase(iter);
	data->SetText(s);
	strMap[data->GetText()] = data;
}

void CTibiaList::SetExtraInfoAtIndex(int ind, int val2)
{
	if (!(ind >= 0 && ind < dataList.GetSize()))
		AfxMessageBox("CTibiaList index out of bounds!");
	dataList[ind]->SetExtraInfo(val2);
}

bool CTibiaList::SetExtraInfo(int val, int val2)
{
	int ind = GetIndex(val);
	if (ind == -1)
		return FALSE;
	dataList[ind]->SetExtraInfo(val2);
	return TRUE;
}

bool CTibiaList::SetExtraInfo(char* s, int val2)
{
	StrMap::iterator iter = strMap.find(s);
	if (iter == strMap.end())
		return FALSE;
	iter->second->SetExtraInfo(val2);
	return TRUE;
}

void CTibiaList::SetTypeAtIndex(int ind, int val2)
{
	if (!(ind >= 0 && ind < dataList.GetSize()))
		AfxMessageBox("CTibiaList index out of bounds!");
	dataList[ind]->SetType(val2);
}

bool CTibiaList::SetType(int val, int val2)
{
	int ind = GetIndex(val);
	if (ind == -1)
		return FALSE;
	dataList[ind]->SetType(val2);
	return TRUE;
}

bool CTibiaList::SetType(char* s, int val2)
{
	StrMap::iterator iter = strMap.find(s);
	if (iter == strMap.end())
		return FALSE;
	iter->second->SetType(val2);
	return TRUE;
}

bool CTibiaList::Remove(int val)
{
	int i;
	int size = dataList.GetSize();
	for (i = 0; i < size; i++)
	{
		if (valList[i] == val)
			break;
	}
	if (i >= size)
		return FALSE;
	StrMap::iterator iter = strMap.find(dataList[i]->GetText());
	if (iter == strMap.end())
		return FALSE;
	dataList.RemoveAt(i);
	valList.RemoveAt(i);
	Free(iter);

	if (GetCount() * 4 <= memSize)
		SetMemSize(memSize / 2);
	return TRUE;
}

bool CTibiaList::Remove(char* s)
{
	StrMap::iterator iter = strMap.find(s);
	if (iter == strMap.end())
		return FALSE;
	int val = iter->second->GetValue();
	int i;
	int size = dataList.GetSize();
	for (i = 0; i < size; i++)
	{
		if (dataList[i]->GetValue() == val && !strcmp(dataList[i]->GetText(), s))
			break;
	}
	if (i >= size)
		return FALSE;
	dataList.RemoveAt(i);
	valList.RemoveAt(i);
	Free(iter);

	if (GetCount() * 4 <= memSize)
		SetMemSize(memSize / 2);
	return TRUE;
}

bool CTibiaList::RemoveAtIndex(int ind)
{
	if (!(ind >= 0 && ind < dataList.GetSize()))
		AfxMessageBox("CTibiaList index out of bounds!");
	StrMap::iterator iter = strMap.find(dataList[ind]->GetText());
	if (iter == strMap.end())
		return FALSE;
	dataList.RemoveAt(ind);
	valList.RemoveAt(ind);
	Free(iter);

	if (GetCount() * 4 <= memSize)
		SetMemSize(memSize / 2);
	return TRUE;
}

CUIntArray* CTibiaList::GetArrayPtr()
{
	return &valList;
}

void CTibiaList::RemoveAll()
{
	StrMap::iterator iter = strMap.begin();
	while (iter != strMap.end())
	{
		StrMap::iterator iterTmp = iter;
		iter++;
		Free(iterTmp);
	}
	strMap.clear();
	dataList.RemoveAll();
	valList.RemoveAll();
	SetMemSize(8);
}

void CTibiaList::toString2(char* outStr)
{
	int ind = strlen(outStr);
	sprintf(outStr + ind, "List:");
	StrMap::iterator iter = strMap.begin();
	while (iter != strMap.end())
	{
		CTibiaListData* data = iter->second;
		data->toString(outStr);
		iter++;
	}
}

void CTibiaList::toString(char* outStr)
{
	int ind = strlen(outStr);
	sprintf(outStr, "List:");
	int size = GetCount();
	for (int i = 0; i < size; i++)
	{
		CTibiaListData* data = dataList[i];
		data->toString(outStr);
	}
	StrMap::iterator iter = strMap.begin();
	while (iter != strMap.end())
	{
		sprintf(outStr + strlen(outStr), "%s-", iter->first);
		iter++;
	}
}

void CTibiaList::Free(StrMap::iterator it)
{
	delete it->second;//data contains same char* as map key
	strMap.erase(it);
}

void CTibiaList::SetMemSize(int new_size)
{
	memSize = max(new_size, 1);
	valList.SetSize(GetCount(), memSize);
	dataList.SetSize(GetCount(), memSize);
}

CTibiaHashMap::CTibiaHashMap(bool a_uniqueInt, bool a_uniqueStr)
{
	counter = 0x10000;
	count = 0;
	uniqueInt = a_uniqueInt;
	uniqueStr = a_uniqueStr;
}

CTibiaHashMap::CTibiaHashMap()
{
	counter = 0x10000;
	count = 0;
	uniqueInt = TRUE;
	uniqueStr = TRUE;
}

CTibiaHashMap::~CTibiaHashMap()
{
	RemoveAll();
}

int CTibiaHashMap::InitIter()
{
	int code = counter++;
	//Used to prevent race condition ;P
	if (isNewIter.find(code) == isNewIter.end())
	{
		AfxMessageBox("You Lucky Bastard!");
		return InitIter();
	}
	isNewIter[code] = 1;
	return code;
}

bool CTibiaHashMap::IncrementIter(int code)
{
	int* isNew = &isNewIter[code];
	if (uniqueStr)
	{
		StrIterMap::iterator _iter = iterSetS.find(code);
		StrMap::iterator iter;
		if (*isNew == 1)
		{
			iter = strMap.begin();
			iterSetS[code] = iter;
		}
		else if (_iter != iterSetS.end())
		{
			iter = _iter->second;
			iter++;
		}
		else
		{
			return false;
		}
		*isNew = 0;
		if (iter != strMap.end())
		{
			return true;
		}
		else
		{
			iterSetS.erase(iterSetS.find(code));
			isNewIter.erase(isNewIter.find(code));
			return false;
		}
	}
	else
	{
		IntIterMap::iterator _iter = iterSetI.find(code);
		IntMap::iterator iter;
		if (*isNew == 1)
		{
			iter = intMap.begin();
			iterSetI[code] = iter;
		}
		else if (_iter != iterSetI.end())
		{
			iter = _iter->second;
			iter++;
		}
		else
		{
			return 0;
		}
		*isNew = 0;
		if (iter != intMap.end())
		{
			return true;
		}
		else
		{
			iterSetI.erase(iterSetI.find(code));
			isNewIter.erase(isNewIter.find(code));
			return false;
		}
	}
}

int CTibiaHashMap::GetValueAtIter(int code)
{
	if (uniqueStr)
	{
		StrIterMap::iterator iter = iterSetS.find(code);
		if (iter == iterSetS.end())
			return 0;
		return iter->second->second->GetValue();
	}
	else
	{
		IntIterMap::iterator iter = iterSetI.find(code);
		if (iter == iterSetI.end())
			return 0;
		return iter->second->second->GetValue();
	}
}

char* CTibiaHashMap::GetTextAtIter(int code)
{
	if (uniqueStr)
	{
		StrIterMap::iterator iter = iterSetS.find(code);
		if (iter == iterSetS.end())
			return NULL;
		return iter->second->second->GetText();
	}
	else
	{
		IntIterMap::iterator iter = iterSetI.find(code);
		if (iter == iterSetI.end())
			return NULL;
		return iter->second->second->GetText();
	}
}

int CTibiaHashMap::GetExtraInfoAtIter(int code)
{
	if (uniqueStr)
	{
		StrIterMap::iterator iter = iterSetS.find(code);
		if (iter == iterSetS.end())
			return 0;
		return iter->second->second->GetExtraInfo();
	}
	else
	{
		IntIterMap::iterator iter = iterSetI.find(code);
		if (iter == iterSetI.end())
			return 0;
		return iter->second->second->GetExtraInfo();
	}
}

bool CTibiaHashMap::Add(CTibiaListData* data)
{
	if (data == NULL)
		return FALSE;
	int a_val = data->GetValue();
	char* a_s = data->GetText();
	//Important and ensures that no null or 0 entries are entered depending on whether
	//strings or ints are unique.  Also prevents duplicate values in the maps which are indicated as unique.
	if ((HasText(a_s) || a_s == NULL) && uniqueStr || (HasValue(a_val) || a_val == 0) && uniqueInt)
		return FALSE;

	char* s = (char*)malloc(strlen(a_s) + 1);
	strcpy(s, a_s);
	if (uniqueInt)
		intMap[a_val] = data;
	if (uniqueStr)
		strMap[s] = data;
	count++;
	return TRUE;
}

bool CTibiaHashMap::Add(int a_val, char* a_s, int a_val2, int a_val3 /*= 0*/)
{
	CTibiaListData* data = new CTibiaListData(a_val, a_s, a_val2, a_val3);
	if (Add(data))
		return TRUE;
	delete data;
	return FALSE;
}

bool CTibiaHashMap::HasValue(int val)
{
	return intMap.find(val) != intMap.end();
}

bool CTibiaHashMap::HasText(char* s)
{
	return strMap.find(s) != strMap.end();
}

int CTibiaHashMap::GetCount()
{
	return count;
}

char* CTibiaHashMap::GetText(int val)
{
	if (!HasValue(val))
		return NULL;
	CTibiaListData* data = intMap[val];
	return data->GetText();
}

int CTibiaHashMap::GetValue(char* s)
{
	if (!HasText(s))
		return 0;
	CTibiaListData* data = strMap[s];
	return data->GetValue();
}

int CTibiaHashMap::GetExtraInfo(int val)
{
	if (!HasValue(val))
		return 0;
	CTibiaListData* data = intMap[val];
	return data->GetExtraInfo();
}

int CTibiaHashMap::GetExtraInfo(char* s)
{
	if (!HasText(s))
		return 0;
	CTibiaListData* data = strMap[s];
	return data->GetExtraInfo();
}

bool CTibiaHashMap::SetExtraInfo(int val, int val2)
{
	if (!HasValue(val))
		return FALSE;
	CTibiaListData* data = intMap[val];
	data->SetExtraInfo(val2);
	return TRUE;
}

bool CTibiaHashMap::SetExtraInfo(char* s, int val2)
{
	if (!HasText(s))
		return FALSE;
	CTibiaListData* data = strMap[s];
	data->SetExtraInfo(val2);
	return TRUE;
}

bool CTibiaHashMap::Remove(int val)
{
	IntMap::iterator iter1 = intMap.find(val);
	CTibiaListData* data = iter1->second;
	if (data == NULL)
		return FALSE;
	StrMap::iterator iter2 = strMap.find(data->GetText());
	if (iter1 == intMap.end())
		return FALSE;
	Free(iter1);
	if (iter2 == strMap.end())
		return FALSE;
	Free(iter2);
	count--;
	return TRUE;
}

bool CTibiaHashMap::Remove(char* s)
{
	StrMap::iterator iter1 = strMap.find(s);
	CTibiaListData* data = iter1->second;
	if (data == NULL)
		return FALSE;
	int val = data->GetValue();
	IntMap::iterator iter2 = intMap.find(val);
	if (iter1 == strMap.end())
		return FALSE;
	Free(iter1);
	if (iter2 == intMap.end())
		return FALSE;
	Free(iter2);
	count--;
	return TRUE;
}

CTibiaListData** CTibiaHashMap::GetAll()
{
	CTibiaListData** ret = (CTibiaListData**)malloc(sizeof(CTibiaListData*) * (count + 1));
	IntMap::const_iterator iter = intMap.begin();
	int i = 0;
	while (iter != intMap.end())
	{
		ret[i++] = iter->second;
		iter++;
	}
	ret[i] = NULL;
	return ret;
}

void CTibiaHashMap::RemoveAll()
{
	StrMap::iterator iter1 = strMap.begin();
	while (iter1 != strMap.end())
	{
		Free(iter1);
		iter1++;
	}
	IntMap::iterator iter2 = intMap.begin();
	while (iter2 != intMap.end())
	{
		Free(iter2);
		iter2++;
	}
	count = 0;
	strMap.clear();
	intMap.clear();
}

void CTibiaHashMap::toString(char* outStr)
{
	int ind = strlen(outStr);
	sprintf(outStr + ind, "Map:");
	IntMap::const_iterator iter = intMap.begin();
	while (iter != intMap.end())
	{
		CTibiaListData* data = iter->second;
		data->toString(outStr);
		iter++;
	}
}

void CTibiaHashMap::Free(IntMap::iterator it)
{
	intMap.erase(it);
	delete it->second;
}

void CTibiaHashMap::Free(StrMap::iterator it)
{
	strMap.erase(it);
	free(it->first);
	delete it->second;
}

CTibiaTreeData::CTibiaTreeData(CTibiaTreeDataType a_type)
{
	nodeType = a_type;
}

CTibiaTreeData::CTibiaTreeData()
{
	nodeType = TT_ITEM_NODE;
}

CTibiaTreeDataType CTibiaTreeData::GetType()
{
	return nodeType;
}

CTibiaTreeData::~CTibiaTreeData()
{
	//gets called after inherited class' destructor
}

void CTibiaTreeData::toString(char* outStr)
{
	int ind = strlen(outStr);
	sprintf(outStr + ind, "\n");
}

void CTibiaTreeBranchData::toString(char* outStr)
{
	int ind = strlen(outStr);
	sprintf(outStr + ind, "(Branch:%s)\n", this->name);
}

CTibiaTreeBranchData::CTibiaTreeBranchData(char *a_name)
{
	nodeType = TT_BRANCH_NODE;

	int len = strlen(a_name) + 1;
	name = (char*)malloc(len);
	memcpy(name, a_name, len);
}

CTibiaTreeBranchData* CTibiaTreeBranchData::newCTibiaTreeBranchData(char *a_name)
{
	return new CTibiaTreeBranchData(a_name);
}

CTibiaTreeBranchData::~CTibiaTreeBranchData()
{
	free(name);
	name = NULL;
}

char* CTibiaTreeBranchData::GetName()
{
	return name;
}

void CTibiaTreeItemData::toString(char* outStr)
{
	int ind = strlen(outStr);
	sprintf(outStr + ind, "(Item:%s,%d,%d)\n", this->name, this->id, this->looted);
}

CTibiaTreeItemData::CTibiaTreeItemData(char *a_name, int a_id, bool a_looted, int i_type /*= 0*/)
{
	if (!strcmp(a_name, "Wooden Doll"))
		int a = 0;
	nodeType = TT_ITEM_NODE;
	id = a_id;
	looted = a_looted;
	type = i_type;

	int len = strlen(a_name) + 1;
	name = (char*)malloc(len);
	memcpy(name, a_name, len);
}

CTibiaTreeItemData* CTibiaTreeItemData::newCTibiaTreeItemData(char *a_name, int a_id, bool a_looted, int i_type /*= 0*/)
{
	return new CTibiaTreeItemData(a_name, a_id, a_looted, i_type);
}

CTibiaTreeItemData::~CTibiaTreeItemData()
{
	free(name);
	name = NULL;
}

void CTibiaTreeItemData::SetId(int idIn)
{
	id = idIn;
}

int CTibiaTreeItemData::GetId()
{
	return id;
}

int CTibiaTreeItemData::GetItemType()
{
	return type;
}

void CTibiaTreeItemData::SetType(int typeIn)
{
	type = typeIn;
}

bool CTibiaTreeItemData::IsLooted()
{
	return looted;
}

void CTibiaTreeItemData::SetIsLooted(bool b_looted)
{
	looted = b_looted;
}

char* CTibiaTreeItemData::GetName()
{
	return name;
}

void CTibiaTreeItemData::SetName(char* nameIn)
{
	memcpy(name, nameIn, strlen(nameIn) + 1);
}

CTibiaTree::CTibiaTree(CTibiaTreeData* a_data)
{
	data = a_data;
}

CTibiaTree::~CTibiaTree()
{
	delete data;
	data = NULL;
	int size = children.size();
	for (int i = size - 1; i >= 0; i--)
	{
		CTibiaTree* ptTree = children[i];
		children.pop_back();
		delete ptTree;
	}
}

CTibiaTree* CTibiaTree::AddChild(CTibiaTreeData* a_data)
{
	CTibiaTree* ret = new CTibiaTree(a_data);
	children.push_back(ret);
	return ret;
}

bool CTibiaTree::HasChildren()
{
	return children.size() > 0;
}

void CTibiaTree::toString(char* outStr, int level)
{
	int ind = strlen(outStr);
	memset(outStr + ind, '-', level);
	memset(outStr + ind + level, 0, 1);
	data->toString(outStr);
	int size = children.size();
	for (int i = 0; i < size; i++)
	{
		children[i]->toString(outStr, level + 1);
	}
}

void CTibiaTree::toString(char* outStr)
{
	sprintf(outStr, "Tree:");
	toString(outStr, 0);
}


template <class T>
CTibiaPipeItem<T>::CTibiaPipeItem(UINT a, T b)
{
	handle = a;
	data = b;
}

template <class T>
char* CTibiaPipe<T>::toString()
{
	char *ret = (char*)malloc(pipeCacheSize * 25 + 100);
	ret[0] = 0;
	sprintf(ret + strlen(ret), "[s:%d,c:%d,sz:%d]", pipeCacheStart, pipeCacheCount, pipeCacheSize);
	for (int i = 0; i < pipeCacheSize + 1; i++)
	{
		sprintf(ret + strlen(ret), "(%d,%X)", pipeCache[i].handle, pipeCache[i].data);
	}
	sprintf(ret + strlen(ret), "\n");
	return ret;
}

template <class T>
bool CTibiaPipe<T>::growCheck()
{
	if (MAX_PIPE_SIZE && MAX_PIPE_SIZE == pipeCacheSize)
		return false;                                              // no change
	if (pipeCacheCount == pipeCacheSize)
	{
		int tmpSize = pipeCacheSize * 2;
		if (MAX_PIPE_SIZE)
			tmpSize = min(MAX_PIPE_SIZE, tmpSize);

		PItem *tmpPipe = (PItem *)calloc(tmpSize, sizeof(PItem));

		memcpy(tmpPipe, pipeCache + pipeCacheStart, (pipeCacheSize - pipeCacheStart) * sizeof(PItem));
		memcpy(tmpPipe + pipeCacheSize - pipeCacheStart, pipeCache, (getEndSlot()) * sizeof(PItem));

		free(pipeCache);
		pipeCache = tmpPipe;
		pipeCacheSize = tmpSize;
		pipeCacheStart = 0;
	}
	return true;
}

template <class T>
int CTibiaPipe<T>::getEndSlot()
{
	return (pipeCacheStart + pipeCacheCount) % pipeCacheSize;
}

template <class T>
void CTibiaPipe<T>::CleanFirstHandle()
{
	if (pipeCacheCount)
		CleanOutHandle(pipeCache[pipeCacheStart].handle);
}

template <class T>
void CTibiaPipe<T>::CleanOutHandle(UINT handle)
{
	EnterCriticalSection(&PipeCriticalSection);
	int locMove, locFill;
	if (pipeCacheCount)
	{
		int i = pipeCacheStart, j = pipeCacheStart;
		for (i; i < pipeCacheStart + pipeCacheCount; i++)
		{
			locMove = (i) % pipeCacheSize;
			if (handle != pipeCache[locMove].handle)
			{
				if (i != j)
				{
					locFill = j % pipeCacheSize;
					pipeCache[locFill] = pipeCache[locMove];
				}
				j++;
			}
		}
		i = j;
		for (i; i < pipeCacheStart + pipeCacheCount; i++)
		{
			locFill = i % pipeCacheSize;
			memset(&(pipeCache[locFill]), 0, sizeof(PItem));
		}
		pipeCacheCount = j - pipeCacheStart;
	}
	LeaveCriticalSection(&PipeCriticalSection);
	return;
}

template <class T>
bool CTibiaPipe<T>::RemoveNext(UINT* handle, T* data)
{
	EnterCriticalSection(&PipeCriticalSection);
	bool ret = false;
	if (pipeCacheCount)
	{
		ret = true;
		*handle = pipeCache[pipeCacheStart].handle;
		*data = pipeCache[pipeCacheStart].data;
		memset(&(pipeCache[pipeCacheStart]), 0, sizeof(PItem));
		pipeCacheStart = (pipeCacheStart + 1) % pipeCacheSize;
		pipeCacheCount--;
	}
	LeaveCriticalSection(&PipeCriticalSection);
	return ret;
}

template <class T>
bool CTibiaPipe<T>::RemoveNext(UINT handle, T* data)
{
	EnterCriticalSection(&PipeCriticalSection);
	bool ret = false;
	if (pipeCacheCount)
	{
		int i;
		for (i = pipeCacheStart; i < pipeCacheStart + pipeCacheCount; i++)
		{
			int loc = i % pipeCacheSize;
			if (pipeCache[loc].handle == handle)
			{
				*data = pipeCache[loc].data;
				ret = true;
				break;
			}
		}
		if (ret)
		{
			//if we are close to start, move previous entries
			int locFill, locMove;
			if (i - pipeCacheStart <= (pipeCacheStart + pipeCacheCount) - i)
			{
				for (i; i > pipeCacheStart; i--)
				{
					locFill = i % pipeCacheSize;
					locMove = (i - 1) % pipeCacheSize;
					pipeCache[locFill] = pipeCache[locMove];
				}
				locFill = i % pipeCacheSize;
				memset(&(pipeCache[locFill]), 0, sizeof(PItem));
				pipeCacheStart = (pipeCacheStart + 1) % pipeCacheSize;
				pipeCacheCount--;
			}
			else     // if we are closer to end, then move later entries
			{
				for (i; i < pipeCacheStart + pipeCacheCount - 1; i++)
				{
					locFill = i % pipeCacheSize;
					locMove = (i + 1) % pipeCacheSize;
					pipeCache[locFill] = pipeCache[locMove];
				}
				locFill = i % pipeCacheSize;
				memset(&(pipeCache[locFill]), 0, sizeof(PItem));
				pipeCacheCount--;
			}
		}
	}
	LeaveCriticalSection(&PipeCriticalSection);
	return ret;
}

template <class T>
bool CTibiaPipe<T>::Add(UINT handle, T data)
{
	EnterCriticalSection(&PipeCriticalSection);
	bool ret = false;
	if (growCheck())
	{
		pipeCache[getEndSlot()] = PItem(handle, data);
		pipeCacheCount++;
		ret = true;
	}
	LeaveCriticalSection(&PipeCriticalSection);
	return ret;
}

template <class T>
CTibiaPipe<T>::~CTibiaPipe()
{
	DeleteCriticalSection(&PipeCriticalSection);
	free(pipeCache);
}

template <class T>
CTibiaPipe<T>::CTibiaPipe(int max_size /*= 0*/)
{
	pipeCache = NULL;
	MAX_PIPE_SIZE = max_size;
	InitializeCriticalSection(&PipeCriticalSection);
	pipeCacheSize = 5;
	pipeCacheStart = 0;
	pipeCacheCount = 0;
	pipeCache = (PItem *)calloc(sizeof(PItem), pipeCacheSize);
}

void CTibiaPercentile::shiftIntoPosition(unsigned int index)
{
	while (index != 0 && sample[index - 1] > sample[index])
	{
		SWAP(sample[index - 1], sample[index]);
		index--;
	}
	while (index + 1 < size && sample[index + 1] < sample[index])
	{
		SWAP(sample[index + 1], sample[index]);
		index++;
	}
}

void CTibiaPercentile::strSample(char* buf)
{
	buf[0] = 0;
	for (unsigned int i = 0; i < size; i++)
	{
		sprintf(&buf[strlen(buf)], "%d,", sample[i]);
	}
}

CTibiaPercentile::CTibiaPercentile(unsigned int MAX_SIZE, unsigned int MAX_ADDS /*= int(0x7FFFFFF / 100)*/)
{
	this->MAX_SIZE = MAX_SIZE;
	this->MAX_ADDS = MAX_ADDS;
	sample = (int*)calloc(this->MAX_SIZE, sizeof(int));
	size = 0;
	totalAdds = 0;
}

CTibiaPercentile::~CTibiaPercentile()
{
	free(sample);
}

void CTibiaPercentile::add(int val)
{
	if (totalAdds < MAX_ADDS)
		totalAdds++;
	if (size < MAX_SIZE)
	{
		sample[size] = val;
		size++;
		shiftIntoPosition(size - 1);
	}
	else if (rand() % totalAdds < MAX_SIZE)   //has 100/totalAdds chance of being added to sample
	{
		int replacedPos = rand() % MAX_SIZE; //each value has equal chance of being replaced
		sample[replacedPos] = val;
		shiftIntoPosition(replacedPos);
	}
	else
	{
	}        // Do not add to sample
}

int CTibiaPercentile::getPercentile(int p)
{
	if (size == 0 || !(p >= 0 && p <= 99))
		return 0;
	return sample[size * p / 100];
}

void CTibiaPercentile::verify(char* a, char* b)
{

}

TestCTibiaPercentile::TestCTibiaPercentile()
{
	testAdd1();
	testAdd3();
	testAdd100();
	testAddMore();
	testUniformSample10();
	testUniformSample100();
	testUniformSample1000();
	testActualData();
}

void TestCTibiaPercentile::testAdd1()
{
	CTibiaPercentile p = CTibiaPercentile(3);
	p.add(10);
	p.strSample(buf);
	verify("testAdd1", buf, "10,");
	verify("testAdd1Perc0", 10, p.getPercentile(0));
	verify("testAdd1Perc99", 10, p.getPercentile(99));
}

void TestCTibiaPercentile::testAdd3()
{
	{CTibiaPercentile p = CTibiaPercentile(3);
	p.add(20);
	p.add(15);
	p.add(10);
	p.strSample(buf);
	verify("testAdd3Reverse", buf, "10,15,20,");
	}
	{CTibiaPercentile p = CTibiaPercentile(3);
	p.add(10);
	p.add(15);
	p.add(20);
	p.strSample(buf);
	verify("testAdd3Forward", buf, "10,15,20,");
	}
	{CTibiaPercentile p = CTibiaPercentile(3);
	p.add(15);
	p.add(10);
	p.add(20);
	p.strSample(buf);
	verify("testAdd3Mix1", buf, "10,15,20,");
	}
	CTibiaPercentile p = CTibiaPercentile(3);
	p.add(20);
	p.add(10);
	p.add(15);
	p.strSample(buf);
	verify("testAdd3Mix2", buf, "10,15,20,");
	verify("testAdd3Perc0", 10, p.getPercentile(0));
	verify("testAdd3Perc33", 10, p.getPercentile(33));
	verify("testAdd3Perc34", 15, p.getPercentile(34));
	verify("testAdd3Perc66", 15, p.getPercentile(66));
	verify("testAdd3Perc67", 20, p.getPercentile(67));
	verify("testAdd3Perc99", 20, p.getPercentile(99));
}

void TestCTibiaPercentile::testAdd100()
{
	CTibiaPercentile p = CTibiaPercentile(100);
	{for (int i = 0; i < 100; i++)
	{
		p.add(i);
	}
	}
	p.strSample(buf);
	verify("testAdd100Sequence", buf);
	CTibiaPercentile q = CTibiaPercentile(100);
	{for (int i = 0; i < 100; i++)
	{
		q.add(p.getPercentile(i));
	}
	}
	p.strSample(buf);
	q.strSample(buf2);
	verify("testAdd100Percs", buf, buf2, 0);
}

void TestCTibiaPercentile::testAddMore()
{
	{CTibiaPercentile p = CTibiaPercentile(3);
	p.add(20);
	p.add(15);
	p.add(10);
	for (int i = 0; i < 100; i++)
	{
		p.add(17);
	}
	p.strSample(buf);
	verify("testAddMoreInnerPos", buf, "17,17,17,");
	}
	{CTibiaPercentile p = CTibiaPercentile(3);
	p.add(20);
	p.add(15);
	p.add(10);
	for (int i = 0; i < 100; i++)
	{
		p.add(22);
	}
	p.strSample(buf);
	verify("testAddMoreBegPos", buf, "22,22,22,");
	}
	{CTibiaPercentile p = CTibiaPercentile(3);
	p.add(20);
	p.add(15);
	p.add(10);
	for (int i = 0; i < 100; i++)
	{
		p.add(7);
	}
	p.strSample(buf);
	verify("testAddMoreEndPos", buf, "7,7,7,");
	}
}

void TestCTibiaPercentile::testUniformSample10()
{
	CTibiaPercentile p = CTibiaPercentile(10);
	for (int i = 0; i < 1000; i++)
	{
		p.add(rand() % 10);
	}
	p.strSample(buf);
	verify("testUniformSample10Seq", buf);
}

void TestCTibiaPercentile::testUniformSample100()
{
	int samp = 100;
	CTibiaPercentile p = CTibiaPercentile(samp);
	{for (int i = 0; i < samp * 100; i++)
	{
		p.add(rand() % samp);
	}
	}
	p.strSample(buf);
	verify("testUniformSample100Seq", buf);
	int sumSq = 0;
	{for (int i = 0; i < 100; i++)
	{
		sumSq += (p.getPercentile(i) - i * samp / 100) * (p.getPercentile(i) - i * samp / 100);
	}
	}
	verify("testUniformSample100Var", sumSq / 100);
}

void TestCTibiaPercentile::testUniformSample1000()
{
	int samp = 1000;
	CTibiaPercentile p = CTibiaPercentile(samp);
	{for (int i = 0; i < samp * 100; i++)
	{
		p.add(rand() % samp);
	}
	}
	int sumSq = 0;
	{for (int i = 0; i < 100; i++)
	{
		sumSq += (p.getPercentile(i) - i * samp / 100) * (p.getPercentile(i) - i * samp / 100);
	}
	}
	verify("testUniformSample1000Var", sumSq / 100);
}

void TestCTibiaPercentile::testActualData()
{
	int diceNum = 4;
	CTibiaPercentile p = CTibiaPercentile(1000);
	{for (int i = 0; i < 10000; i++)
	{
		int total = 0;
		for (int dice = 0; dice < diceNum; dice++)
		{
			total += rand() % 250;
		}
		p.add(total);
	}
	}
	int ASTERISK_LEN = 75;
	int max = 0;
	int step = 3;
	{for (int i = step; i < 100 - step; i = i + step)
	{
		int avgslope = 0;
		for (int s = 0; s < step; s++)
			avgslope += 1000 / (p.getPercentile(i + s) - p.getPercentile(i + s - step)) / step;
		max = max < avgslope ? avgslope : max;
	}
	}
	{for (int i = step; i < 100; i = i + step)
	{
		int avgslope = 0;
		for (int s = 0; s < step; s++)
			avgslope += ASTERISK_LEN * 1000 / max / (p.getPercentile(i + s) - p.getPercentile(i + s - step)) / step;
		for (int j = 0; j < avgslope; j++)
			printf("*");
		printf("\n");
	}
	}
}

bool TestCTibiaPercentile::verify(const char* name, int a)
{
	printf("VERIFY %s %d\n", name, a);
	return 1;
}

bool TestCTibiaPercentile::verify(const char* name, const char* a)
{
	printf("VERIFY %s %s\n", name, a);
	return 1;
}

bool TestCTibiaPercentile::verify(const char* name, int a, int b, int printLong /*= 1*/)
{
	if (a == b)
	{
		if (printLong)
			printf("PASSED %s %d == %d\n", name, a, b);
		else
			printf("PASSED %s\n", name);
		return 1;
	}
	printf("*****FAILED***** %s %d != %d\n", name, a, b);
	return 0;
}

bool TestCTibiaPercentile::verify(const char* name, const char* a, const char* b, int printLong /*= 1*/)
{
	if (strcmp(a, b) == 0)
	{
		if (printLong)
			printf("PASSED %s %s == %s\n", name, a, b);
		else
			printf("PASSED %s\n", name);
		return 1;
	}
	printf("*****FAILED***** %s %s != %s\n", name, a, b);
	return 0;
}
