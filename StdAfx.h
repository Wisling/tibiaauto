// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if !defined(AFX_STDAFX_H__1653369B_A72F_492F_86F6_61C14CEA9F84__INCLUDED_)
#define AFX_STDAFX_H__1653369B_A72F_492F_86F6_61C14CEA9F84__INCLUDED_

#if _MSC_VER > 1000
#pragma warning( disable : 4786 503 )
#pragma once
#endif // _MSC_VER > 1000

#define MAX_STRING_LEN 65536
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxsock.h>
#include <Tlhelp32.h>


#include <afxinet.h>
#include <afxtempl.h>
#include <queue>
#include <vector>

#include "ipcm.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__1653369B_A72F_492F_86F6_61C14CEA9F84__INCLUDED_)
