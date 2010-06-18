// Trigger.cpp: implementation of the CTrigger class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_autogo.h"
#include "Trigger.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrigger::CTrigger() {
	setType(UNDEFINED);
	setTriggerText("");
}

CTrigger::CTrigger(int typeIn, CString textIn) {
	setType(typeIn);
	setTriggerText(textIn);
	switch (type) {
	case INTEGER:
		setIntTrigger(atoi(textIn));	
		break;
	case POINT:
		int index, index2;
		index = textIn.Find(',');
		setIntTrigger(atoi(textIn.Mid(1, index)));

		index2 = textIn.Find(',', ++index);
		setIntTrigger(atoi(textIn.Mid(index, index2 - index)));

		index = textIn.Find(')', ++index2);
		setIntTrigger(atoi(textIn.Mid(index2, index - index2)));

		setIntTrigger(atoi(textIn.Right(textIn.GetLength() - ++index)));
		break;
	}
}

CTrigger::CTrigger(CTrigger *triggerPtrIn) {
	setType(triggerPtrIn->getType());
	setTriggerText(triggerPtrIn->getTriggerText());
}

CTrigger::~CTrigger() {

}

int CTrigger::getType() {
	return type;
}

void CTrigger::setType(int typeIn) {
	type = typeIn;
}

ostream &operator<<(ostream &out, CTrigger &t) {
	out << t.getType() << " [" << (LPCTSTR) t.getTriggerText() << "]";
	return out;
}

void CTrigger::setTriggerText(CString textIn) { 
	triggerText = textIn;
	switch (type) {
	case INTEGER:
		setIntTrigger(atoi(textIn));	
		break;
	case POINT:
		int index, index2;
		index = textIn.Find(',');
		setIntTrigger(atoi(textIn.Mid(1, index)));

		index2 = textIn.Find(',', ++index);
		setIntTrigger(atoi(textIn.Mid(index, index2 - index)));

		index = textIn.Find(')', ++index2);
		setIntTrigger(atoi(textIn.Mid(index2, index - index2)));

		setIntTrigger(atoi(textIn.Right(textIn.GetLength() - ++index)));
		break;
	}
}

CString CTrigger::getTriggerText() {
	return triggerText;
}

void CTrigger::setIntTrigger(int intIn) {
	switch (type) {
	case INTEGER:
		if (intTriggerList.size())
			intTriggerList.clear();
		intTriggerList.push_back(intIn);
		break;
	case POINT:
		if (intTriggerList.size() > 4)
			intTriggerList.clear();
		intTriggerList.push_back(intIn);
		break;
	}
}		

int CTrigger::getIntTrigger() {
	list<int>::iterator intItr = intTriggerList.begin();
	return *intItr;
}

int CTrigger::getMultiIntTrigger(int index) {
	list<int>::iterator intListItr = intTriggerList.begin();
	for (int loop = 0; loop < index; loop++)
		intListItr++;
	return *intListItr;
}
