// IPCBackPipe.cpp: implementation of the CIPCBackPipe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IPCBackPipe.h"
#include "ipcm.h"
#include "time.h"
#include "PackSender.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int CIPCBackPipe::initialised=0;
HANDLE CIPCBackPipe::hPipeBack=INVALID_HANDLE_VALUE;
struct ipcMessage *CIPCBackPipe::pipeBackCache=NULL;
int CIPCBackPipe::pipeBackCacheSize=0;
int CIPCBackPipe::pipeBackCacheCount=0;
FILE *debugFile=NULL;

int PIPE_CLEAN_AT_COUNT = 100;
int PIPE_REMOVE_AT_SECS = 10;
int sentErrMsg = 0;

CRITICAL_SECTION BackPipeQueueCriticalSection;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCBackPipe::CIPCBackPipe()
{	

}

CIPCBackPipe::~CIPCBackPipe()
{

}

void CIPCBackPipe::InitialiseIPC()
{
	char buf[128];	
	char lpszPipename[128];
	int pid = ::GetCurrentProcessId();
	sprintf(lpszPipename,"\\\\.\\pipe\\tibiaAutoPipe-back-%d",pid);	

	while (1)
	{		
		
		hPipeBack = CreateFile( 
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE, 
			0,              // no sharing 
			NULL,           // no security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 		
		
		// pipe handle is valid - proceed
		if (hPipeBack != INVALID_HANDLE_VALUE) 	
		{
			break;
		}		
		
		
		if (GetLastError() != ERROR_PIPE_BUSY) 
		{
			
			sprintf(buf,"Could not open pipe (not busy): %d",GetLastError());
			//AfxMessageBox(buf); 
			Sleep(50);
			continue;
		}
		
		// All pipe instances are busy, so wait for 1 second. 
		
		if (! WaitNamedPipe(lpszPipename, 1000) ) 
		{
			sprintf(buf,"Could not open pipe (busy too long): %d",GetLastError());
			AfxMessageBox(buf); 
			Sleep(50);
			continue;
		}
	}
	
	DWORD dwMode = PIPE_READMODE_MESSAGE; 
	BOOL fSuccess = SetNamedPipeHandleState( 
		hPipeBack,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 	
	if (!fSuccess) 
	{	
		InitialiseIPC();
	} else {
		initialised=1;
		if (debugFile) {
			fprintf(debugFile,"[debug] straight IPC initialised ok\n");
		}

	}

}


// puts hPipeBack data into pipe managed solely by TA
int CIPCBackPipe::readFromPipe(struct ipcMessage *mess, int expectedType)
{
	static int initialisedCriticalSection=0;
	int i;
	
	if (!initialised)
	{
		InitialiseIPC();
		
	}
	if (!initialisedCriticalSection)
	{
		InitializeCriticalSection(&BackPipeQueueCriticalSection);
		initialisedCriticalSection=1;
	}
	

	EnterCriticalSection(&BackPipeQueueCriticalSection);
	

	int firstType = 0;
	if (pipeBackCacheCount > 0){
		firstType = pipeBackCache[0].messageType;
	}

	if (pipeBackCacheCount >= PIPE_CLEAN_AT_COUNT){ // SOMEONE ISN'T READING FROM THEIR PIPE FAST ENOUGH!! DELETE THEIR ENTRIES!!!!
		int j = 0;
		for (i=0;i<pipeBackCacheCount;i++)
		{
			if (pipeBackCache[i].messageType != firstType){
				if (i != j){
					memcpy(&pipeBackCache[j],&pipeBackCache[i],sizeof(struct ipcMessage));
				}
				j++;
			}
		}
		pipeBackCacheCount = j;
		if (!sentErrMsg){
			sentErrMsg = 1;
			char errBuf[256];
			sprintf(errBuf, "Registered pipe handle %d is not being read from fast enough. Recieved %d entries in %d seconds. Please fix this!", firstType,i-j,PIPE_REMOVE_AT_SECS);
			MessageBox(NULL, errBuf, "DEBUG MESSAGE", 0);
		}
	}

	//Check our own TA pipe first
	int j = 0;//keep track of kept items
	for (i=0;i<pipeBackCacheCount;i++)
	{
		if (abs(time(NULL) - pipeBackCache[i].tm) > 10000 && !sentErrMsg){
			sentErrMsg = 1;
			char errBuf[256];
			sprintf(errBuf, "Received unrealistic time for sent ipcMessage. Time %d, sent time %d, type %d", time(NULL), pipeBackCache[i].tm,pipeBackCache[0].messageType);
			MessageBox(NULL, errBuf, "DEBUG MESSAGE", MB_OK);
		}
		if (pipeBackCache[i].messageType==expectedType)
		{
			// message found in cache - return it then
			memcpy(mess,&pipeBackCache[i],sizeof(struct ipcMessage));
			// and move the remaining part
			for (i=i+1;i<pipeBackCacheCount;i++ && j++)
			{
				memcpy(&pipeBackCache[j],&pipeBackCache[i],sizeof(struct ipcMessage));
			}
			pipeBackCacheCount=j;
			LeaveCriticalSection(&BackPipeQueueCriticalSection);
			return 1;
		} else if(time(NULL) <= pipeBackCache[i].tm + PIPE_REMOVE_AT_SECS) { //keep this item
			if (i != j){
				memcpy(&pipeBackCache[j],&pipeBackCache[i],sizeof(struct ipcMessage));
			}
			j++;
		}
	}	
	pipeBackCacheCount=j;

	// calls to CPythonEngine::backpipeTamsgTick() frequently flushes this hPipeBack pipe
	for (;;) //check pipe from Tibia
	{
		DWORD totalBytesLeft,totalBytesAvail,bRead;
		// check whether there is anything waiting for us
		PeekNamedPipe(hPipeBack,mess,sizeof(struct ipcMessage),&bRead,&totalBytesAvail,&totalBytesLeft);
		
		
		if (totalBytesAvail)
		{
			
			if (ReadFile(hPipeBack,mess,sizeof(struct ipcMessage),&bRead,NULL))
			{
				if(time(NULL) <= mess->tm + PIPE_REMOVE_AT_SECS){ //if pipe item is not too old return/store it
					if (mess->messageType==expectedType)
					{
						LeaveCriticalSection(&BackPipeQueueCriticalSection);
						return 1;
					} else {
						// put to cache
						if (pipeBackCacheCount>=pipeBackCacheSize)
							enlargePipeBackCache();
						memcpy(&pipeBackCache[pipeBackCacheCount++],mess,sizeof(struct ipcMessage));
						continue;
					}
				}
			} else {
				LeaveCriticalSection(&BackPipeQueueCriticalSection);
				return 0;
			}
		} else {
			LeaveCriticalSection(&BackPipeQueueCriticalSection);
			return 0;
		}
	}
	LeaveCriticalSection(&BackPipeQueueCriticalSection); 
	return 0;
}

void CIPCBackPipe::enlargePipeBackCache()
{
	pipeBackCacheSize=pipeBackCacheSize*2+3;
	pipeBackCache=(struct ipcMessage *)realloc(pipeBackCache,sizeof(struct ipcMessage)*pipeBackCacheSize);	
}
