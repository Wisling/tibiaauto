// IPCBackPipe.cpp: implementation of the CIPCBackPipe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IPCBackPipe.h"
#include "ipcm.h"

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
		
		// pipe handle is invalid - proceed
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
		
		// All pipe instances are busy, so wait for 10 seconds. 
		
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
	}

}




			
 
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
	

	
	for (i=0;i<pipeBackCacheCount;i++)
	{
		if (pipeBackCache[i].messageType==expectedType)
		{
			// message found in cache - return it then
			memcpy(mess,&pipeBackCache[i],sizeof(struct ipcMessage));
			// and move the remaining part
			for (i;i<pipeBackCacheCount-1;i++)
			{
				memcpy(&pipeBackCache[i],&pipeBackCache[i+1],sizeof(struct ipcMessage));
			}
			pipeBackCacheCount--;
			LeaveCriticalSection(&BackPipeQueueCriticalSection); 
			return 1;
		}
	}	
	
	for (;;)
	{
		DWORD totalBytesLeft,totalBytesAvail,bRead;
		// check whether there is anything waiting for us
		PeekNamedPipe(hPipeBack,mess,sizeof(struct ipcMessage),&bRead,&totalBytesAvail,&totalBytesLeft);
		
		
		if (totalBytesAvail)
		{
			
			if (ReadFile(hPipeBack,mess,sizeof(struct ipcMessage),&bRead,NULL))
			{
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
