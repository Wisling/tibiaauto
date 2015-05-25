// AliceProxy.h: interface for the CAliceProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALICEPROXY_H__1028AD77_7D6F_4ECB_86DB_1D581A98B9E2__INCLUDED_)
#define AFX_ALICEPROXY_H__1028AD77_7D6F_4ECB_86DB_1D581A98B9E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAliceProxy
{
public:
	char * respond(char *text, char *id);
	CAliceProxy();
	virtual ~CAliceProxy();

private:
	static HMODULE dllAlice;
};

#endif // !defined(AFX_ALICEPROXY_H__1028AD77_7D6F_4ECB_86DB_1D581A98B9E2__INCLUDED_)
