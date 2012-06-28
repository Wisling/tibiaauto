// MessageHandler.cpp: implementation of the CMessageHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MessageHandler.h"
#include "IPCBackPipe.h"
#include "TibiaStructures.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define MAX_PAYLOAD_LEN 1024

//recvGameActions

CRITICAL_SECTION MessageHandlerEditCriticalSection;

struct message{
	int size;
	char* data;
};
struct ipcMessage
{
	int messageType;
	char payload[1024];
	UINT tm;
public:
	ipcMessage(){
		messageType = 0;
		memset(payload,0,1024);
		tm = 0;
	}
};

struct parcelRecvActionData{
	int handle;
	int countLeft;
	int len;
	char actionData[MAX_PAYLOAD_LEN-4*3];
	parcelRecvActionData(){
		memset(actionData,0,sizeof(actionData));
	}
};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageHandler::CMessageHandler(int a_messageInPipe, int a_messageOutPipe)
{	
	messageInPipe = a_messageInPipe;
	messageOutPipe = a_messageOutPipe;
	CTibiaPipe<message> messagesContainer;
}

CMessageHandler::~CMessageHandler()
{
}

int initHandler(){
	//assign variables
	//initialize critical section
	//
	return 0;
}
/*
struct parcelRecvActionData{
	int handle;
	int countLeft;
	int len;
	char actionData[MAX_PAYLOAD_LEN-4*3];
	parcelRecvActionData(){
		memset(actionData,0,sizeof(actionData));
	}
};*/

int CMessageHandler::getNextMessage(char* ret, int* size){
	if (ret!=NULL){
		// variable not nulled
	}

	//Update array with messages from pipe
	CIPCBackPipe backPipe;
	struct ipcMessage mess;
	
	for (int maxtimes = 5; backPipe.readFromPipe(&mess, this->messageInPipe) && maxtimes>0; maxtimes--){
		parcelRecvActionData* pd = (parcelRecvActionData*)&(mess.payload);
		int countLeft = pd->countLeft;
		char* buf = (char*)malloc(sizeof(pd->actionData)*(countLeft+1));
		memcpy(buf,pd->actionData,pd->len);
		int offset = pd->len;
		while (countLeft){
			if (backPipe.readFromPipe(&mess, this->messageInPipe)){
				memcpy(buf+offset,pd->actionData,pd->len);
				offset += pd->len;
			}
		}
	}
	return 0;
}
