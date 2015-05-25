#pragma warning( disable : 4786 )
#include "stdafx.h"
using namespace std;
#include <vector>
#include <map>
#include "winbase.h"

struct cmp_str
{
	bool operator()(char const *a, char const *b) const
	{
		return strcmp(a, b) < 0;
	}
};

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CTibiaList Implementation //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
class CTibiaListData {
	int val;
	char* s;
	int val2;
	int type;
public:
	CTibiaListData(int a_val, char* a_s)
	{
		s = (char*)malloc(strlen(a_s) + 1);
		strcpy(s, a_s);

		val  = a_val;
		val2 = 0;
		SetType(0);
	}

	CTibiaListData(int a_val, char* a_s, int a_val2, int a_val3)
	{
		s = (char*)malloc(strlen(a_s) + 1);
		strcpy(s, a_s);

		val  = a_val;
		val2 = a_val2;
		SetType(a_val3);
	}

	char* GetText()
	{
		return s;
	}

	int GetValue()
	{
		return val;
	}

	int GetType()
	{
		return type;
	}

	int GetExtraInfo()
	{
		return val2;
	}

	void SetExtraInfo(int a_val2)
	{
		val2 = a_val2;
	}

	void SetValue(int a_val)
	{
		val = a_val;
	}

	void SetType(int a_val)
	{
		type = a_val;
	}

	void SetText(char* a_s)
	{
		free(s);
		s = (char*)malloc(strlen(a_s) + 1);
		strcpy(s, a_s);
	}

	~CTibiaListData()
	{
		free(s);
	}

	void toString(char* outStr)
	{
		int ind = strlen(outStr);
		sprintf(outStr + ind, "(%d,%s,%d)\n", val, s, val2);
	}
};

typedef map<char *, CTibiaListData*, cmp_str> StrMap;
class CTibiaList {
private:
public:
	StrMap strMap;// Important Note:  NEVER use strMap[] except to change the key.  Use strMap.find() for lookups
	CTypedPtrArray<CPtrArray, CTibiaListData*> dataList;
	CUIntArray valList;
	int memSize;
	CTibiaList()
	{
		SetMemSize(8);
	}

	~CTibiaList()
	{
		RemoveAll();
	}

	bool Add(int a_val, char* a_s, int a_val2 = 0, int a_val3 = 0)
	{
		CTibiaListData* data = new CTibiaListData(a_val, a_s, a_val2, a_val3);
		if (Add(data))
			return TRUE;
		delete data;
		return FALSE;
	}

	bool Add(CTibiaListData* data)
	{
		if (data == NULL)
			return FALSE;
		int a_val = data->GetValue();
		int type  = data->GetType();
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

	bool HasValue(int val)
	{
		int size = dataList.GetSize();
		for (int i = 0; i < size; i++)
		{
			if (valList[i] == val)
				return TRUE;
		}
		return FALSE;
	}

	bool HasText(char* s)
	{
		return strMap.find(s) != strMap.end();
	}

	int GetCount()
	{
		return dataList.GetSize();
	}

	int GetIndex(int val)
	{
		int size = dataList.GetSize();
		for (int i = 0; i < size; i++)
		{
			if (valList[i] == val)
				return i;
		}
		return -1;
	}

	char* GetText(int val)
	{
		int size = dataList.GetSize();
		for (int i = 0; i < size; i++)
		{
			if (valList[i] == val)
				return dataList[i]->GetText();
		}
		return NULL;
	}

	int GetValue(char* s)
	{
		StrMap::iterator iter = strMap.find(s);
		if (iter == strMap.end())
			return 0;
		return iter->second->GetValue();
	}

	int GetExtraInfo(char* s)
	{
		StrMap::iterator iter = strMap.find(s);
		if (iter == strMap.end())
			return 0;
		return iter->second->GetExtraInfo();
	}

	int GetExtraInfo(int val)
	{
		int size = dataList.GetSize();
		for (int i = 0; i < size; i++)
		{
			if (valList[i] == val)
				return dataList[i]->GetExtraInfo();
		}
		return 0;
	}

	int GetValueAtIndex(int ind)
	{
		if (ind >= 0 && ind < dataList.GetSize())
			return valList[ind];
		return 0;
	}

	char* GetTextAtIndex(int ind)
	{
		if (ind >= 0 && ind < dataList.GetSize())
			return dataList[ind]->GetText();
		return NULL;
	}

	int GetTypeAtIndex(int ind)
	{
		if (ind >= 0 && ind < dataList.GetSize())
			return dataList[ind]->GetType();
		return NULL;
	}

	int GetExtraInfoAtIndex(int ind)
	{
		if (ind >= 0 && ind < dataList.GetSize())
			return dataList[ind]->GetExtraInfo();
		return 0;
	}

	void SetValueAtIndex(int ind, int val)
	{
		if (!(ind >= 0 && ind < dataList.GetSize()))
			AfxMessageBox("CTibiaList index out of bounds!");
		dataList[ind]->SetValue(val);
		valList[ind] = val;
	}

	void SetTextAtIndex(int ind, char* s)
	{
		if (!(ind >= 0 && ind < dataList.GetSize()))
			AfxMessageBox("CTibiaList index out of bounds!");
		if (HasText(s))
			return;
		CTibiaListData* data  = dataList[ind];
		StrMap::iterator iter = strMap.find(data->GetText());
		if (iter == strMap.end())
			AfxMessageBox("Internal Error, strMap missing entry!");
		//free(iter->first);  string key is same as string within data
		strMap.erase(iter);
		data->SetText(s);
		strMap[data->GetText()] = data;
	}

	void SetExtraInfoAtIndex(int ind, int val2)
	{
		if (!(ind >= 0 && ind < dataList.GetSize()))
			AfxMessageBox("CTibiaList index out of bounds!");
		dataList[ind]->SetExtraInfo(val2);
	}

	bool SetExtraInfo(char* s, int val2)
	{
		StrMap::iterator iter = strMap.find(s);
		if (iter == strMap.end())
			return FALSE;
		iter->second->SetExtraInfo(val2);
		return TRUE;
	}

	bool SetExtraInfo(int val, int val2)
	{
		int ind = GetIndex(val);
		if (ind == -1)
			return FALSE;
		dataList[ind]->SetExtraInfo(val2);
		return TRUE;
	}

	void SetTypeAtIndex(int ind, int val2)
	{
		if (!(ind >= 0 && ind < dataList.GetSize()))
			AfxMessageBox("CTibiaList index out of bounds!");
		dataList[ind]->SetType(val2);
	}

	bool SetType(char* s, int val2)
	{
		StrMap::iterator iter = strMap.find(s);
		if (iter == strMap.end())
			return FALSE;
		iter->second->SetType(val2);
		return TRUE;
	}

	bool SetType(int val, int val2)
	{
		int ind = GetIndex(val);
		if (ind == -1)
			return FALSE;
		dataList[ind]->SetType(val2);
		return TRUE;
	}

	bool Remove(char* s)
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

	bool Remove(int val)
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

	bool RemoveAtIndex(int ind)
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

	CUIntArray* GetArrayPtr()
	{
		return &valList;
	}

	void RemoveAll()
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

	void toString2(char* outStr)
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

	void toString(char* outStr)
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

private:
	void Free(StrMap::iterator it)
	{
		delete it->second;//data contains same char* as map key
		strMap.erase(it);
	}

	void SetMemSize(int new_size)
	{
		memSize = max(new_size, 1);
		valList.SetSize(GetCount(), memSize);
		dataList.SetSize(GetCount(), memSize);
	}
};
///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CTibiaHashMap Implementation INCOMPLETE //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
typedef map<int, CTibiaListData*> IntMap;
typedef map<int, StrMap::iterator> StrIterMap;
typedef map<int, IntMap::iterator> IntIterMap;
class CTibiaHashMap {
private:
	StrMap strMap;
	IntMap intMap;
	int counter;
	StrIterMap iterSetS;
	IntIterMap iterSetI;
	map<int, int> isNewIter;
	int count;
	bool uniqueInt;
	bool uniqueStr;
public:
	CTibiaHashMap()
	{
		counter   = 0x10000;
		count     = 0;
		uniqueInt = TRUE;
		uniqueStr = TRUE;
	}

	// At least one should be true in order to use the map properly
	CTibiaHashMap(bool a_uniqueInt, bool a_uniqueStr)
	{
		counter   = 0x10000;
		count     = 0;
		uniqueInt = a_uniqueInt;
		uniqueStr = a_uniqueStr;
	}

	~CTibiaHashMap()
	{
		RemoveAll();
	}

	int InitIter()
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

	//Must iterate through all elements in order for iterator to be cleaned.
	bool IncrementIter(int code)
	{
		int* isNew = &isNewIter[code];
		if (uniqueStr)
		{
			StrIterMap::iterator _iter = iterSetS.find(code);
			StrMap::iterator iter;
			if (*isNew == 1)
			{
				iter           = strMap.begin();
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
				iter           = intMap.begin();
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

	int GetValueAtIter(int code)
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

	char* GetTextAtIter(int code)
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

	int GetExtraInfoAtIter(int code)
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

	bool Add(int a_val, char* a_s, int a_val2, int a_val3 = 0)
	{
		CTibiaListData* data = new CTibiaListData(a_val, a_s, a_val2, a_val3);
		if (Add(data))
			return TRUE;
		delete data;
		return FALSE;
	}

	bool Add(CTibiaListData* data)
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

	bool HasValue(int val)
	{
		return intMap.find(val) != intMap.end();
	}

	bool HasText(char* s)
	{
		return strMap.find(s) != strMap.end();
	}

	int GetCount()
	{
		return count;
	}

	char* GetText(int val)
	{
		if (!HasValue(val))
			return NULL;
		CTibiaListData* data = intMap[val];
		return data->GetText();
	}

	int GetValue(char* s)
	{
		if (!HasText(s))
			return 0;
		CTibiaListData* data = strMap[s];
		return data->GetValue();
	}

	int GetExtraInfo(char* s)
	{
		if (!HasText(s))
			return 0;
		CTibiaListData* data = strMap[s];
		return data->GetExtraInfo();
	}

	int GetExtraInfo(int val)
	{
		if (!HasValue(val))
			return 0;
		CTibiaListData* data = intMap[val];
		return data->GetExtraInfo();
	}

	bool SetExtraInfo(char* s, int val2)
	{
		if (!HasText(s))
			return FALSE;
		CTibiaListData* data = strMap[s];
		data->SetExtraInfo(val2);
		return TRUE;
	}

	bool SetExtraInfo(int val, int val2)
	{
		if (!HasValue(val))
			return FALSE;
		CTibiaListData* data = intMap[val];
		data->SetExtraInfo(val2);
		return TRUE;
	}

	bool Remove(char* s)
	{
		StrMap::iterator iter1 = strMap.find(s);
		CTibiaListData* data   = iter1->second;
		if (data == NULL)
			return FALSE;
		int val                = data->GetValue();
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

	bool Remove(int val)
	{
		IntMap::iterator iter1 = intMap.find(val);
		CTibiaListData* data   = iter1->second;
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

	CTibiaListData** GetAll()
	{
		CTibiaListData** ret        = (CTibiaListData**)malloc(sizeof(CTibiaListData*) * (count + 1));
		IntMap::const_iterator iter = intMap.begin();
		int i                       = 0;
		while (iter != intMap.end())
		{
			ret[i++] = iter->second;
			iter++;
		}
		ret[i] = NULL;
		return ret;
	}

	void RemoveAll()
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

	void toString(char* outStr)
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

private:
	void Free(StrMap::iterator it)
	{
		strMap.erase(it);
		free(it->first);
		delete it->second;
	}

	void Free(IntMap::iterator it)
	{
		intMap.erase(it);
		delete it->second;
	}
};

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CTibiaTree Implementation //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
typedef enum {
	TT_BRANCH_NODE = 1,
	TT_ITEM_NODE   = 2
}CTibiaTreeDataType;

class CTibiaTreeData {
protected:
	CTibiaTreeDataType nodeType;
public:
	CTibiaTreeData()
	{
		nodeType = TT_ITEM_NODE;
	}

	CTibiaTreeData(CTibiaTreeDataType a_type)
	{
		nodeType = a_type;
	}

	CTibiaTreeDataType GetType()
	{
		return nodeType;
	}

	virtual ~CTibiaTreeData()
	{
		//gets called after inherited class' destructor
	}

	virtual void toString(char* outStr)
	{
		int ind = strlen(outStr);
		sprintf(outStr + ind, "\n");
	}
};

class CTibiaTreeBranchData : public CTibiaTreeData
{
	char *name;
public:
	CTibiaTreeBranchData(char *a_name)
	{
		nodeType = TT_BRANCH_NODE;

		int len = strlen(a_name) + 1;
		name = (char*)malloc(len);
		memcpy(name, a_name, len);
	}

	static CTibiaTreeBranchData* newCTibiaTreeBranchData(char *a_name)
	{
		return new CTibiaTreeBranchData(a_name);
	}

	virtual ~CTibiaTreeBranchData()
	{
		free(name);
		name = NULL;
	}

	char* GetName()
	{
		return name;
	}

	virtual void CTibiaTreeBranchData::toString(char* outStr)
	{
		int ind = strlen(outStr);
		sprintf(outStr + ind, "(Branch:%s)\n", this->name);
	}
};

class CTibiaTreeItemData : public CTibiaTreeData
{
public:
	CTibiaTreeItemData(char *a_name, int a_id, bool a_looted, int i_type = 0)
	{
		if (!strcmp(a_name, "Wooden Doll"))
			int a = 0;
		nodeType = TT_ITEM_NODE;
		id       = a_id;
		looted   = a_looted;
		type     = i_type;

		int len = strlen(a_name) + 1;
		name = (char*)malloc(len);
		memcpy(name, a_name, len);
	}

	static CTibiaTreeItemData* newCTibiaTreeItemData(char *a_name, int a_id, bool a_looted, int i_type = 0)
	{
		return new CTibiaTreeItemData(a_name, a_id, a_looted, i_type);
	}

	virtual ~CTibiaTreeItemData()
	{
		free(name);
		name = NULL;
	}

	void SetId(int idIn)
	{
		id = idIn;
	}

	int GetId()
	{
		return id;
	}

	int GetItemType()
	{
		return type;
	}

	void SetType(int typeIn)
	{
		type = typeIn;
	}

	bool IsLooted()
	{
		return looted;
	}

	void SetIsLooted(bool b_looted)
	{
		looted = b_looted;
	}

	char* GetName()
	{
		return name;
	}

	void SetName(char* nameIn)
	{
		memcpy(name, nameIn, strlen(nameIn) + 1);
	}

	virtual void CTibiaTreeItemData::toString(char* outStr)
	{
		int ind = strlen(outStr);
		sprintf(outStr + ind, "(Item:%s,%d,%d)\n", this->name, this->id, this->looted);
	}

private:
	int type;
	char *name;
	int id;
	bool looted;
};

class CTibiaTree {
public:
	CTibiaTreeData* data;
	vector<CTibiaTree*> children;
	CTibiaTree(CTibiaTreeData* a_data)
	{
		data = a_data;
	}

	~CTibiaTree()
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

	CTibiaTree* AddChild(CTibiaTreeData* a_data)
	{
		CTibiaTree* ret = new CTibiaTree(a_data);
		children.push_back(ret);
		return ret;
	}

	bool HasChildren()
	{
		return children.size() > 0;
	}

	void toString(char* outStr)
	{
		sprintf(outStr, "Tree:");
		toString(outStr, 0);
	}

	void toString(char* outStr, int level)
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
};

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CTibiaQueue Implementation //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
template <class T>
class CTibiaQueue {
private:
	int count;
	int origTabSize;
	int start;
	int end;
public:
	int tabSize;
	T*tab;
	CTibiaQueue()
	{
		origTabSize = 15;
		tabSize     = origTabSize;
		count       = 0;
		tab         = (T*)malloc(sizeof(T) * tabSize);
		memset(tab, 0, sizeof(T) * tabSize);
		start = 0;
		end   = 0;
	}

	~CTibiaQueue()
	{
		free(tab);
	}

	void Add(T ptr)
	{
		growCheck();
		tab[end] = ptr;
		end      = (end + 1) % tabSize;
		count++;
	}

	T Remove()
	{
		if (!count)
			return T();
		T ret = tab[start];
		tab[start] = T();
		start      = (start + 1) % tabSize;
		count--;
		shrinkCheck();
		return ret;
	}

	T Peek()
	{
		if (!count)
			return T();
		return tab[start];
	}

	int GetCount()
	{
		return count;
	}

	char* toString()
	{
		return toString(tab, tabSize);
	}

private:
	char* toString(T* tab, int tabSize)
	{
		char * ret = (char*)malloc(tabSize * 10 + 100);
		ret[0] = 0;
		sprintf(ret + strlen(ret), "[s:%d,e:%d,c:%d,sz:%d][", start, end, count, tabSize);
		for (int i = 0; i < tabSize; i++)
		{
			sprintf(ret + strlen(ret), "%d ", tab[i]);
		}
		ret[strlen(ret) - 1] = 0;
		sprintf(ret + strlen(ret), "]");
		return ret;
	}

	void growCheck()
	{
		if (count == tabSize)//start==end
		{
			int tmpTabSize = tabSize * 2;
			T* tmpTab      = (T*)malloc(sizeof(T) * tmpTabSize);
			memset(tmpTab, 0, sizeof(T) * tmpTabSize);

			memcpy(tmpTab, tab + start, (tabSize - start) * sizeof(T));
			memcpy(tmpTab + tabSize - start, tab, (end) * sizeof(T));

			free(tab);
			tab     = tmpTab;
			tabSize = tmpTabSize;
			start   = 0;
			end     = count;
		}
	}

	void shrinkCheck()
	{
		if (tabSize > origTabSize && count == tabSize / 4)//start!=end
		{
			int tmpTabSize = tabSize / 2;
			T* tmpTab      = (T*)malloc(sizeof(T) * tmpTabSize);
			memset(tmpTab, 0, sizeof(T) * tmpTabSize);

			memcpy(tmpTab, tab + start, min(count, tabSize - start) * sizeof(T));
			memcpy(tmpTab + min(count, tabSize - start), tab, (start >= end ? end : 0) * sizeof(T));

			free(tab);
			tabSize = tmpTabSize;
			tab     = tmpTab;
			start   = 0;
			end     = count;
		}
	}

	/*
	   void testAddUntilFull(){
	        for (int i=0;i<origTabSize;i++){
	                Add (i+1);
	        }
	   }
	   void testPop(){
	        Remove();
	        for (int i=0;i<origTabSize;i++){
	                Add (i+1);
	        }
	        while (count>4){
	                Remove();
	        }
	   }
	   void testGrowAligned(){
	        for (int i=0;i<origTabSize*1.5;i++){
	                Add (i+1);
	        }
	   }
	   void testGrowUnaligned(){
	        for (int i=0;i<origTabSize*1.5;i++){
	                Add (i+1);
	                if(i==2){
	                        Remove();
	                        Remove();
	                }
	        }
	   }
	   void testGrowLooped(){
	        for (int i=0;i<origTabSize*2;i++){
	                Add (i+1);
	                if(i==3){
	                        Remove();
	                        Remove();
	                        Remove();
	                        Remove();
	                }
	        }
	   }
	   void testShrinkAligned(){
	        for (int i=0;i<origTabSize*4;i++){
	                Add (i+1);
	        }
	        for (i=0;i<origTabSize*3;i++){
	                Remove ();
	        }
	   }
	   void testShrinkUnaligned(){
	        origTabSize=1;
	        for (int i=0;i<18;i++){
	                if (i==7){
	                        while (count>1+tabSize/4){Remove();}
	                }
	                Add (i+1);
	        }
	        while(Remove()){}
	   }
	   void testMass(){
	        int runtime=0;
	        int i=1;
	        while(count<1000){
	                if (rand()%100>51) Remove();
	                else Add(i++);
	                runtime++;
	        }
	        runtime=0;
	        while(count>0){
	                if (rand()%100>49) Remove();
	                else Add(i++);
	                runtime++;
	        }
	   }
	 */
};


///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CTibiaPriorityQueue Implementation //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
class PQitem {
public:
	int val;
	DWORD data;
	PQitem()
	{
		val  = 0;
		data = NULL;
	}

	PQitem(int a_val, DWORD a_data)
	{
		val  = a_val;
		data = a_data;
	}
};
class CTibiaPriorityQueue {
	vector<PQitem> pq;
public:
	CTibiaPriorityQueue()
	{
	}

	~CTibiaPriorityQueue()
	{
	}

	void CTibiaPriorityQueue::Add(int val, DWORD ptr)
	{
		int ind = pq.size();
		pq.push_back(PQitem(val, ptr));
		int ind2 = (ind - 1) / 2;
		if (pq[ind2].val > val)
		{
			while (pq[ind2].val > val && ind != 0)
			{
				pq[ind] = pq[ind2];
				ind     = ind2;
				ind2    = (ind - 1) / 2;
			}
			pq[ind] = PQitem(val, ptr);
		}
	}

	DWORD CTibiaPriorityQueue::Remove()
	{
		if (pq.size() == 0)
			return 0;
		DWORD ret = pq[0].data;
		pq[0].data = 0;
		size_t ind = 0;
		while (true)
		{
			if (ind * 2 + 2 < pq.size() && pq.back().val > pq[ind * 2 + 2].val)
			{
				if (pq[ind * 2 + 2].val < pq[ind * 2 + 1].val)
				{
					pq[ind] = pq[ind * 2 + 2];
					ind     = ind * 2 + 2;
				}
				else
				{
					pq[ind] = pq[ind * 2 + 1];
					ind     = ind * 2 + 1;
				}
			}
			else if (ind * 2 + 1 < pq.size() && pq.back().val > pq[ind * 2 + 1].val)
			{
				pq[ind] = pq[ind * 2 + 1];
				ind     = ind * 2 + 1;
			}
			else
			{
				break;
			}
		}
		pq[ind] = pq.back();
		pq.pop_back();
		return ret;
	}

	DWORD CTibiaPriorityQueue::Peek()
	{
		if (pq.size() == 0)
			return 0;
		return pq[0].data;
	}

	int CTibiaPriorityQueue::GetCount()
	{
		return pq.size();
	}

	char* CTibiaPriorityQueue::toString2()
	{
		char* ret = (char*)malloc(pq.size() * 10 + 100);
		char* tmp = ret;
		ret[0] = '-';
		ret[1] = 0;
		for (size_t i = 0; i < pq.size(); i++)
		{
			tmp = tmp + strlen(tmp);
			sprintf(tmp, "%d,", pq[i].val);
		}
		return ret;
	}

	char* CTibiaPriorityQueue::toString()
	{
		char* ret = (char*)malloc(pq.size() * 10 + 100);
		char* tmp = ret;
		ret[0] = '-';
		ret[1] = 0;
		size_t ind   = 0;
		int wentBack = 0;
		while (true && pq.size() != 0)
		{
			if (!wentBack)
			{
				tmp = tmp + strlen(tmp);
				sprintf(tmp, "%d", pq[ind].val);
			}
			if (ind * 2 + 1 < pq.size() && !wentBack)
			{
				tmp = tmp + strlen(tmp);
				sprintf(tmp, " [");
				ind = ind * 2 + 1;
			}
			else if (ind % 2 == 1 && ind + 1 < pq.size())
			{
				tmp = tmp + strlen(tmp);
				sprintf(tmp, ",");
				ind      = ind + 1;
				wentBack = 0;
			}
			else if (ind % 2 == 0 && ind != 0)
			{
				tmp = tmp + strlen(tmp);
				sprintf(tmp, "] ");
				ind      = (ind - 1) / 2;
				wentBack = 1;
			}
			else
			{
				break;
			}
		}
		return ret;
	}

private:
	void CTibiaPriorityQueue::testAddBack()
	{
		for (int i = 15; i > 0; i--)
		{
			Add(i, i);
		}
		//cout<<toString();
	}

	void CTibiaPriorityQueue::testAddFor()
	{
		for (int i = 0; i < 15; i++)
		{
			Add(i, i);
		}
		//cout<<toString2();
	}

	void CTibiaPriorityQueue::testAddAlt()
	{
		int n = 1;
		for (int i = 0; i < 15; i++)
		{
			Add(i * n + 20, i * n + 20);
			n = -n;
		}
		//cout<<toString();
	}

	void CTibiaPriorityQueue::testAddRandom()
	{
		for (int i = 0; i < 15; i++)
		{
			Add(rand() % 100, i);
		}
		//cout<<toString();
	}

	void CTibiaPriorityQueue::testRemove()
	{
		for (int i = 7; i > 0; i--)
		{
			Add(i, i);
		}
		Remove();
		//cout<<toString();
	}

	void CTibiaPriorityQueue::testManyRemove()
	{
		int i;
		for (i = 0; i < 15; i++)
		{
			Add(rand() % 100, i);
		}
		for (i = 0; i < 15; i++)
		{
			//cout<<toString()<<endl;
			Remove();
		}
		//cout<<toString2()<<endl;
	}

	void CTibiaPriorityQueue::testRemoveRandom()
	{
		for (int i = 0; i < 10000; i++)
		{
			if (rand() % 100 < 49)
				Add(rand() % 100, i);
			else
				Remove();
			//cout<<toString2()<<endl;
		}
	}
};

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CTibiaPipe Implementation //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
template <class T>
struct CTibiaPipeItem {
	UINT handle;
	T data;
	CTibiaPipeItem(UINT a, T b)
	{
		handle = a;
		data   = b;
	}
};
template <class T>
class CTibiaPipe {
	typedef CTibiaPipeItem<T> PItem;
	CRITICAL_SECTION PipeCriticalSection;
	PItem *pipeCache;
	int pipeCacheSize;
	int pipeCacheCount;
	int pipeCacheStart;
	int MAX_PIPE_SIZE;
public:
	CTibiaPipe(int max_size = 0)
	{
		pipeCache     = NULL;
		MAX_PIPE_SIZE = max_size;
		InitializeCriticalSection(&PipeCriticalSection);
		pipeCacheSize  = 5;
		pipeCacheStart = 0;
		pipeCacheCount = 0;
		pipeCache      = (PItem *)calloc(sizeof(PItem), pipeCacheSize);
	}

	~CTibiaPipe()
	{
		DeleteCriticalSection(&PipeCriticalSection);
		free(pipeCache);
	}

	bool Add(UINT handle, T data)
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

	bool RemoveNext(UINT handle, T* data)
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
					ret   = true;
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
						locFill            = i % pipeCacheSize;
						locMove            = (i - 1) % pipeCacheSize;
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
						locFill            = i % pipeCacheSize;
						locMove            = (i + 1) % pipeCacheSize;
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

	bool RemoveNext(UINT* handle, T* data)
	{
		EnterCriticalSection(&PipeCriticalSection);
		bool ret = false;
		if (pipeCacheCount)
		{
			ret     = true;
			*handle = pipeCache[pipeCacheStart].handle;
			*data   = pipeCache[pipeCacheStart].data;
			memset(&(pipeCache[pipeCacheStart]), 0, sizeof(PItem));
			pipeCacheStart = (pipeCacheStart + 1) % pipeCacheSize;
			pipeCacheCount--;
		}
		LeaveCriticalSection(&PipeCriticalSection);
		return ret;
	}

	void CleanOutHandle(UINT handle)
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
						locFill            = j % pipeCacheSize;
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

	void CleanFirstHandle()
	{
		if (pipeCacheCount)
			CleanOutHandle(pipeCache[pipeCacheStart].handle);
	}

private:
	int getEndSlot()
	{
		return (pipeCacheStart + pipeCacheCount) % pipeCacheSize;
	}

	bool growCheck()
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
			pipeCache      = tmpPipe;
			pipeCacheSize  = tmpSize;
			pipeCacheStart = 0;
		}
		return true;
	}

	char* toString()
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

	void ShowTest(const char *s, int value, int requiredValue)
	{
		cout << s << " " << value << " " << requiredValue;
		if (value == requiredValue)
			cout << endl;
		else
			cout << " ***FAILED***" << endl;
	}

/*
        void testAddOne(){
                Add(rand(),T(1));
                cout << toString();
        }
        void testAddOverflow(){
                for (int i=0;i<11;i++){
                        cout << Add(rand(),T(i))<< endl;
                }
                cout << toString();
        }
        void testAddRemoveHandle(){
                Add(0xffffffff,T(1));
                Add(0xfffffffe,T(2));
                Add(0xffffffff,T(3));
                Add(0xfffffffe,T(4));
                Add(0xfffffffd,T(5));
                Add(0xfffffffe,T(6));
                Add(0xfffffffe,T(7));
                T data;
                cout << toString()<< endl;
                cout<<RemoveNext(0xfffffffe, &data)<<endl;
                cout << toString()<< endl;
                cout<<RemoveNext(0x1, &data)<<endl;
                cout << toString()<< endl;
                cout<<RemoveNext(0xfffffffe, &data)<<endl;
                cout << toString()<< endl;
        }
        void testAddRemoveRandom(){
                int j=0;
                for (int i=0;i<100;i++){
                        if (rand()%100 <= 70){
                                Add(0xffffffff,T(j++));
                        } else {
                                T data;
                                UINT handle;
                                RemoveNext(&handle, &data);
                                cout << i << " " << handle<<","<<data<<endl;
                        }
                }
                cout << toString()<< endl;
        }
        void testCleanHandle(){
                Add(0xffffffff,T(1));
                Add(0xfffffffe,T(2));
                Add(0xffffffff,T(3));
                Add(0xfffffffe,T(4));
                Add(0xfffffffd,T(5));
                Add(0xfffffffe,T(6));
                Add(0xfffffffe,T(7));
                T data;
                cout << toString()<< endl;
                CleanOutHandle(0xfffffffe);
                cout << toString()<< endl;
                CleanOutHandle(0xffffffff);
                cout << toString()<< endl;
        }
        void testCleanFirstHandle(){
                Add(0xffffffff,T(1));
                Add(0xfffffffd,T(2));
                Add(0xfffffffe,T(3));
                Add(0xfffffffe,T(4));
                Add(0xfffffffd,T(5));
                T data;
                UINT handle;
                RemoveNext(&handle, &data);
                Add(0xffffffff,T(6));
                cout << toString()<< endl;
                CleanFirstHandle();
                cout << toString()<< endl;
                CleanFirstHandle();
                cout << toString()<< endl;
                CleanFirstHandle();
                cout << toString()<< endl;
        }
        void testRemoveFromBothEnds(){
                Add(0xffffffff,T(1));
                Add(0xfffffffd,T(2));
                Add(0xffffffff,T(3));
                Add(0xffffffff,T(4));
                Add(0xfffffffe,T(5));
                T data;
                UINT handle;
                RemoveNext(&handle, &data);
                Add(0xffffffff,T(6));
                cout << toString()<< endl;
                RemoveNext(0xfffffffe, &data);
                cout << toString()<< endl;
                RemoveNext(0xffffffff, &data);
                cout << toString()<< endl;
        }
        void testVarious(){
                srand(time(NULL));
                int j=0;
                for (int i=0;i<100;i++){
                        int r=rand()%100;
                        T data;
                        UINT handle;
                        if (r < 60){
                                Add(rand()%10,T(j++));
                                cout << "add1 ";
                        } else if(r < 75) {
                                CleanFirstHandle();
                                cout << "clnf ";
                        } else if(r < 80) {
                                RemoveNext(rand()%10,&data);
                                cout << "rndr ";
                        } else {
                                RemoveNext(&handle, &data);
                                cout << "rem1 ";
                        }
                        cout << toString()<< endl;
                }
                cout << toString()<< endl;
        }*/
};


///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CTibiaPercentile Implementation //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
#define SWAP(a, b) {a = a ^ b; b = a ^ b; a = a ^ b; }

//holds an array of up to MAX_SIZE representing a simple random sampling from all values added to the sample
//each value added up until MAX_SIZE is reached has a 100% chance of being in the sample
//after MAX_SIZE is reached, the chance of any particular value that has ever been added being in the sample is (MAX_SIZE/totalAdds)%
//a MAX_ADDS value can be specified which limits (MAX_SIZE/totalAdds), stopping the probability of adding a value from decreasing any further
//	this has the effect of decreasing the inclusion probability for older values and increasing the inclusion probability for newer ones
class CTibiaPercentile {
public:
	friend class TestCTibiaPercentile;
	int* sample;
	unsigned int size, MAX_SIZE, totalAdds, MAX_ADDS;
	CTibiaPercentile(unsigned int MAX_SIZE, unsigned int MAX_ADDS = int(0x7FFFFFF / 100))
	{
		this->MAX_SIZE = MAX_SIZE;
		this->MAX_ADDS = MAX_ADDS;
		sample         = (int*)calloc(this->MAX_SIZE, sizeof(int));
		size           = 0;
		totalAdds      = 0;
	}

	~CTibiaPercentile()
	{
		free(sample);
	}

	void add(int val)
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

	int getPercentile(int p)
	{
		if (size == 0 || !(p >= 0 && p <= 99))
			return 0;
		return sample[size * p / 100];
	}

	void verify(char* a, char* b)
	{
	}

private:
	void shiftIntoPosition(unsigned int index)
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

	void strSample(char* buf)
	{
		buf[0] = 0;
		for (unsigned int i = 0; i < size; i++)
		{
			sprintf(&buf[strlen(buf)], "%d,", sample[i]);
		}
	}
};

class TestCTibiaPercentile {
	char buf[10000];
	char buf2[10000];
public:
	TestCTibiaPercentile()
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

	void testAdd1()
	{
		CTibiaPercentile p = CTibiaPercentile(3);
		p.add(10);
		p.strSample(buf);
		verify("testAdd1", buf, "10,");
		verify("testAdd1Perc0", 10, p.getPercentile(0));
		verify("testAdd1Perc99", 10, p.getPercentile(99));
	}

	void testAdd3()
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

	void testAdd100()
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

	void testAddMore()
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

	void testUniformSample10()
	{
		CTibiaPercentile p = CTibiaPercentile(10);
		for (int i = 0; i < 1000; i++)
		{
			p.add(rand() % 10);
		}
		p.strSample(buf);
		verify("testUniformSample10Seq", buf);
	}

	void testUniformSample100()
	{
		int samp           = 100;
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

	void testUniformSample1000()
	{
		int samp           = 1000;
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

	void testActualData()
	{
		int diceNum        = 4;
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
		int max          = 0;
		int step         = 3;
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

private:
	bool verify(const char* name, const char* a, const char* b, int printLong = 1)
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

	bool verify(const char* name, int a, int b, int printLong = 1)
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

	bool verify(const char* name, const char* a)
	{
		printf("VERIFY %s %s\n", name, a);
		return 1;
	}

	bool verify(const char* name, int a)
	{
		printf("VERIFY %s %d\n", name, a);
		return 1;
	}
};
