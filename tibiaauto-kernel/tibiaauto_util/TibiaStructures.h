#pragma once
#include "tibiaauto_util.h"

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


class AStarNode
{
public:
	int x;
	int y;
	int z;
	int px;
	int py;
	int pz;
	int g;
	int h;
	
	AStarNode() {}

	AStarNode(int x1, int y1, int z1, int px1 = 0, int py1 = 0, int pz1 = 0, int g1 = 0, int h1 = 0)
	{
		x = x1;
		y = y1;
		z = z1;
		px = px1;
		py = py1;
		pz = pz1;
		g = g1;
		h = h1;
	}

	AStarNode copy()
	{
		return AStarNode(x, y, z, px, py, pz, g, h);
	}

	void copy(AStarNode& c)
	{
		x = c.x;
		y = c.y;
		z = c.z;
		px = c.px;
		py = c.py;
		pz = c.pz;
		g = c.g;
		h = c.h;
	}

	// Returns true if this node should be lower priority than the other
	bool operator<(const AStarNode& other) const
	{
		// If the predicted total distance to target is higher, make it lower priority
		return this->h > other.h;
	}
};

class AStarPriorityQueue : public priority_queue<AStarNode, vector<AStarNode>> {
public:
	vector<AStarNode>* Container()
	{
		return &c;
	}
};

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CTibiaList Implementation //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
class TIBIAAUTOUTIL_API CTibiaListData {
	int val;
	char* s;
	int val2;
	int type;
public:
	CTibiaListData(int a_val, char* a_s);

	CTibiaListData(int a_val, char* a_s, int a_val2, int a_val3);

	char* GetText();

	int GetValue();

	int GetType();

	int GetExtraInfo();

	void SetExtraInfo(int a_val2);

	void SetValue(int a_val);

	void SetType(int a_val);

	void SetText(char* a_s);

	~CTibiaListData();

	void toString(char* outStr);
};

typedef map<char *, CTibiaListData*, cmp_str> StrMap;
class TIBIAAUTOUTIL_API CTibiaList {
private:
public:
	StrMap strMap;// Important Note:  NEVER use strMap[] except to change the key.  Use strMap.find() for lookups
	CTypedPtrArray<CPtrArray, CTibiaListData*> dataList;
	CUIntArray valList;
	int memSize;
	CTibiaList();

	~CTibiaList();

	bool Add(int a_val, char* a_s, int a_val2 = 0, int a_val3 = 0);

	bool Add(CTibiaListData* data);

	bool HasValue(int val);

	bool HasText(char* s);

	int GetCount();

	int GetIndex(int val);

	char* GetText(int val);

	int GetValue(char* s);

	int GetExtraInfo(char* s);

	int GetExtraInfo(int val);

	int GetValueAtIndex(int ind);

	char* GetTextAtIndex(int ind);

	int GetTypeAtIndex(int ind);

	int GetExtraInfoAtIndex(int ind);

	void SetValueAtIndex(int ind, int val);

	void SetTextAtIndex(int ind, char* s);

	void SetExtraInfoAtIndex(int ind, int val2);

	bool SetExtraInfo(char* s, int val2);

	bool SetExtraInfo(int val, int val2);

	void SetTypeAtIndex(int ind, int val2);

	bool SetType(char* s, int val2);

	bool SetType(int val, int val2);

	bool Remove(char* s);

	bool Remove(int val);

	bool RemoveAtIndex(int ind);

	CUIntArray* GetArrayPtr();

	void RemoveAll();

	void toString2(char* outStr);

	void toString(char* outStr);

private:
	void Free(StrMap::iterator it);

	void SetMemSize(int new_size);
};
///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CTibiaHashMap Implementation INCOMPLETE //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
typedef map<int, CTibiaListData*> IntMap;
typedef map<int, StrMap::iterator> StrIterMap;
typedef map<int, IntMap::iterator> IntIterMap;
class TIBIAAUTOUTIL_API CTibiaHashMap {
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
	CTibiaHashMap();

	// At least one should be true in order to use the map properly
	CTibiaHashMap(bool a_uniqueInt, bool a_uniqueStr);

	~CTibiaHashMap();

	int InitIter();

	//Must iterate through all elements in order for iterator to be cleaned.
	bool IncrementIter(int code);

	int GetValueAtIter(int code);

	char* GetTextAtIter(int code);

	int GetExtraInfoAtIter(int code);

	bool Add(int a_val, char* a_s, int a_val2, int a_val3 = 0);

	bool Add(CTibiaListData* data);

	bool HasValue(int val);

	bool HasText(char* s);

	int GetCount();

	char* GetText(int val);

	int GetValue(char* s);

	int GetExtraInfo(char* s);

	int GetExtraInfo(int val);

	bool SetExtraInfo(char* s, int val2);

	bool SetExtraInfo(int val, int val2);

	bool Remove(char* s);

	bool Remove(int val);

	CTibiaListData** GetAll();

	void RemoveAll();

	void toString(char* outStr);

private:
	void Free(StrMap::iterator it);

	void Free(IntMap::iterator it);
};

///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CTibiaTree Implementation //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
typedef enum {
	TT_BRANCH_NODE = 1,
	TT_ITEM_NODE   = 2
}CTibiaTreeDataType;

class TIBIAAUTOUTIL_API CTibiaTreeData {
protected:
	CTibiaTreeDataType nodeType;
public:
	CTibiaTreeData();

	CTibiaTreeData(CTibiaTreeDataType a_type);

	CTibiaTreeDataType GetType();

	virtual ~CTibiaTreeData();

	virtual void toString(char* outStr);
};

class TIBIAAUTOUTIL_API CTibiaTreeBranchData : public CTibiaTreeData
{
	char *name;
public:
	CTibiaTreeBranchData(char *a_name);

	static CTibiaTreeBranchData* newCTibiaTreeBranchData(char *a_name);

	virtual ~CTibiaTreeBranchData();
	virtual void toString(char* outStr);

	char* GetName();
};

class TIBIAAUTOUTIL_API CTibiaTreeItemData : public CTibiaTreeData
{
public:
	CTibiaTreeItemData(char *a_name, int a_id, bool a_looted, int i_type = 0);

	static CTibiaTreeItemData* newCTibiaTreeItemData(char *a_name, int a_id, bool a_looted, int i_type = 0);

	virtual ~CTibiaTreeItemData();

	void SetId(int idIn);

	int GetId();

	int GetItemType();

	void SetType(int typeIn);

	bool IsLooted();

	void SetIsLooted(bool b_looted);

	char* GetName();

	void SetName(char* nameIn);
	virtual void toString(char* outStr);

private:
	int type;
	char *name;
	int id;
	bool looted;
};

class TIBIAAUTOUTIL_API CTibiaTree {
public:
	CTibiaTreeData* data;
	vector<CTibiaTree*> children;
	CTibiaTree(CTibiaTreeData* a_data);

	~CTibiaTree();

	CTibiaTree* AddChild(CTibiaTreeData* a_data);

	bool HasChildren();

	void toString(char* outStr);

	void toString(char* outStr, int level);
};


///////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CTibiaPipe Implementation //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
template <class T>
struct CTibiaPipeItem {
	UINT handle;
	T data;
	CTibiaPipeItem(UINT a, T b);
};
template <class T>
class TIBIAAUTOUTIL_API CTibiaPipe {
	typedef CTibiaPipeItem<T> PItem;
	CRITICAL_SECTION PipeCriticalSection;
	PItem *pipeCache;
	int pipeCacheSize;
	int pipeCacheCount;
	int pipeCacheStart;
	int MAX_PIPE_SIZE;
public:
	CTibiaPipe(int max_size = 0);

	~CTibiaPipe();

	bool Add(UINT handle, T data);

	bool RemoveNext(UINT handle, T* data);

	bool RemoveNext(UINT* handle, T* data);

	void CleanOutHandle(UINT handle);

	void CleanFirstHandle();

private:
	int getEndSlot();

	bool growCheck();

	char* toString();

	/*
	void ShowTest(const char *s, int value, int requiredValue)
	{
		cout << s << " " << value << " " << requiredValue;
		if (value == requiredValue)
			cout << endl;
		else
			cout << " ***FAILED***" << endl;
	}

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
class TIBIAAUTOUTIL_API CTibiaPercentile {
public:
	friend class TestCTibiaPercentile;
	int* sample;
	unsigned int size, MAX_SIZE, totalAdds, MAX_ADDS;
	CTibiaPercentile(unsigned int MAX_SIZE, unsigned int MAX_ADDS = int(0x7FFFFFF / 100));

	~CTibiaPercentile();

	void add(int val);

	int getPercentile(int p);

	void verify(char* a, char* b);

private:
	void shiftIntoPosition(unsigned int index);

	void strSample(char* buf);
};

class TestCTibiaPercentile {
	char buf[10000];
	char buf2[10000];
public:
	TestCTibiaPercentile();

	void testAdd1();

	void testAdd3();

	void testAdd100();

	void testAddMore();

	void testUniformSample10();

	void testUniformSample100();

	void testUniformSample1000();

	void testActualData();

private:
	bool verify(const char* name, const char* a, const char* b, int printLong = 1);

	bool verify(const char* name, int a, int b, int printLong = 1);

	bool verify(const char* name, const char* a);

	bool verify(const char* name, int a);
};
