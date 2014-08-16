// Trigger.h: interface for the CTrigger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIGGER_H__B0138190_08E8_4A6C_8081_68BAC0A9A4D9__INCLUDED_)
#define AFX_TRIGGER_H__B0138190_08E8_4A6C_8081_68BAC0A9A4D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <list>

using namespace std;

class CTrigger
{
friend static ostream &operator<<(ostream &out, const CTrigger &t);
public:
	int getMultiIntTrigger(int);
	int getIntTrigger();
	void setIntTrigger(int);
	void setTriggerText(CString);
	CString getTriggerText();
	void setType(int);
	int getType();
	CTrigger();
	CTrigger(int, CString);
	CTrigger(CTrigger*);
	~CTrigger();

protected:
	int type;
	CString triggerText;
	list<int> intTriggerList;
	list<CString> strTriggerList;
private:
	int triggerCount;
};

#endif // !defined(AFX_TRIGGER_H__B0138190_08E8_4A6C_8081_68BAC0A9A4D9__INCLUDED_)
