// Helpers.h : header file
//

#ifndef HELPERS_H__INCLUDED
#define HELPERS_H__INCLUDED

/////////////////////////////////////////////////////////////////////////////
// debug functions
void UsrDump(TCHAR *msg, BYTE *pData, int len);
CString GetDumpString(BYTE *pData, int len);
void DumpErrorCode(DWORD dwErrCode, DWORD dwLine, char *szFileName);
BOOL GetErrorDescription(DWORD error_code, CString& error_string);

inline TCHAR GetPrintCharOrDot(TCHAR c)
{
	if (_istprint(c))
		return c;
	else
		return (TCHAR)'.';
}

#ifdef _DEBUG
#define DUMP                            ::UsrDump
#else // ifdef _DEBUG
#define DUMP              1 ? (void)0 : ::UsrDump
#endif // ifdef _DEBUG

#endif // HELPERS_H__INCLUDED
