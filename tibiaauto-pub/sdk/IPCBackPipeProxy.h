#if !defined(AFX_IPCBACKPIPEPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
#define AFX_IPCBACKPIPEPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ipcm.h"

class CIPCBackPipeProxy
{
public:
	int readFromPipe(struct ipcMessage *mess, int expectedType);
	void InitialiseIPC();

	CIPCBackPipeProxy();
	virtual ~CIPCBackPipeProxy();

private:
	static HMODULE dllModule;
};

#endif // !defined(AFX_IPCBACKPIPEPROXY_H__DD3AACBE_D136_47DC_B222_5D2BE0DFADE2__INCLUDED_)
