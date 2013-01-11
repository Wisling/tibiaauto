// IPCPipeBack.h: interface for the ipcMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCBACKPIPE_H__054B62A3_8A94_4824_B73B_DD6C35175066__INCLUDED_)
#define AFX_CIPCBACKPIPE_H__054B62A3_8A94_4824_B73B_DD6C35175066__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "time.h"
#include "ipcm.h"
//#include <fstream>

class CIPCPipeBack
{
public:
	HANDLE hPipeBack;

public:
	CIPCPipeBack();
	virtual ~CIPCPipeBack();
	void InitialiseIPCback(int partnerProcessId,FILE* debugFile,int COMPLEX);
	int hasInvalidHandle();
	void closePipe();
	void send(ipcMessage mess);

};


#endif // !defined(AFX_CIPCBACKPIPE_H__054B62A3_8A94_4824_B73B_DD6C35175066__INCLUDED_)
