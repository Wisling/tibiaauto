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

int parseDurationMinutes(CString s){
	int days=0,hours=0,mins=0;
	int indDay = s.Find("d");
	int indHour = s.Find("h");
	int indMin= s.Find("m");
	if(indDay==-1){
		days = 0;
		indDay=0;
	}else{
		days = atoi(s.Right(indDay));
		indDay++;
	}
	if(indHour==-1){
		hours = 0;
		indHour=indDay;
	}else{
		hours = atoi(s.Mid(indDay,indHour-indDay));
		indHour++;
	}
	if(indMin==-1){
		mins = atoi(s.Mid(indHour,999));
	}else{
		mins = atoi(s.Mid(indHour,indMin-indHour));
	}
	return days*24*60+hours*60+mins;
}
int parseDurationSeconds(CString s){
	int hours=0,mins=0,secs=0;
	int indHour = s.Find("h");
	int indMin= s.Find("m");
	int indSec = s.Find("s");
	if(indHour==-1){
		hours = 0;
		indHour = 0;
	}else{
		hours = atoi(s.Right(indHour));
		indHour++;
	}
	if(indMin==-1){
		mins = 0;
		indMin = indHour;
	}else{
		mins = atoi(s.Mid(indHour,indMin-indHour));
		indMin++;
	}
	if(indSec==-1){
		secs = atoi(s.Mid(indMin,999));
	}else{
		secs = atoi(s.Mid(indMin,indSec-indMin));
	}
	return hours*60*60+mins*60+secs;
}
CTrigger::CTrigger(int typeIn, CString textIn) {
	setType(typeIn);
	setTriggerText(textIn);
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

static ostream &operator<<(ostream &out, CTrigger &t) {
	out << t.getType() << " [" << (LPCTSTR) t.getTriggerText() << "]";
	return out;
}

void CTrigger::setTriggerText(CString textIn) {
	switch (type) {
	case STRING:
		triggerText = textIn;
		break;
	case INTEGER:
		{
		int val = atoi(textIn);
		setIntTrigger(val);
		sprintf(triggerText.GetBuffer(128),"%d",val);
		triggerText.ReleaseBuffer();
		break;
		}
	case PointTRIGGER:
		{
		int x,y,z,r;
		int index, index2;
		index = textIn.Find(',');

		if(index!=-1) x=atoi(textIn.Mid(1, index));
		else x=0;

		index2 = textIn.Find(',', ++index);
		if(index2!=-1) y=atoi(textIn.Mid(index, index2 - index));
		else y=0;

		index = textIn.Find(')', ++index2);
		if (index!=-1) z=atoi(textIn.Mid(index2, index - index2));
		else z=0;

		r=atoi(textIn.Right(textIn.GetLength() - ++index));
		setIntTrigger(x);
		setIntTrigger(y);
		setIntTrigger(z);
		setIntTrigger(r);
		sprintf(triggerText.GetBuffer(128),"(%d,%d,%d)%d",x,y,z,r);
		triggerText.ReleaseBuffer();
		break;
		}
	case DURATIONMIN:
		{
		int mins = parseDurationMinutes(textIn);
		setIntTrigger(mins);
		char* buf = triggerText.GetBuffer(128);
		if (mins>=60) sprintf(buf,"%dh%dm",mins/60,mins%60);
		else sprintf(buf,"%dm",mins);
		triggerText.ReleaseBuffer();
		break;
		}
	case DURATIONSEC:
		{
		int secs = parseDurationSeconds(textIn);
		setIntTrigger(secs);
		char* buf = triggerText.GetBuffer(128);
		if (secs>=60) sprintf(buf,"%dm%ds",secs/60,secs%60);
		else sprintf(buf,"%ds",secs);
		triggerText.ReleaseBuffer();
		break;
		}
	default:
		triggerText = "Error:Unknown Trigger Type";
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
	case PointTRIGGER:
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
