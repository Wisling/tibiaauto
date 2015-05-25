#include "stdafx.h"
#include "helpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

BOOL GetErrorDescription( DWORD error_code, CString& error_string )
{
	BOOL rc = TRUE;

	error_string.Empty();

	LPVOID message_buffer = NULL;

	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	                NULL,
	                error_code,
	                MAKELANGID( LANG_NEUTRAL, SUBLANG_SYS_DEFAULT ),
	                (LPTSTR) &message_buffer,
	                0,
	                NULL );

	error_string = (LPCTSTR) message_buffer;

	::LocalFree( message_buffer );
	message_buffer = NULL;

	if ( error_string.GetLength() == 0 )
	{
		CString frm = _T("Unknown error.");
		error_string.Format(frm, error_code );
		rc = FALSE;
	}

	return rc;
}

void DumpErrorCode(DWORD dwErrCode, DWORD dwLine, char *szFileName)
{
#ifdef _UNICODE

	DWORD dwFileName   = MultiByteToWideChar(CP_ACP, 0, szFileName, -1, NULL, 0);
	WCHAR *wszFileName = new WCHAR[dwFileName];
	MultiByteToWideChar(CP_ACP, 0, szFileName, -1, wszFileName, dwFileName);

	CString str;
	GetErrorDescription(dwErrCode, str);
	TRACE(_T("%s(%d): 0x%.8x: %s"), wszFileName, dwLine, dwErrCode, str);

	delete [] wszFileName;
#else // ifdef _UNICODE
	CString str;
	GetErrorDescription(dwErrCode, str);
	TRACE(_T("%s(%d): 0x%.8x: %s"), szFileName, dwLine, dwErrCode, str);
#endif // ifdef _UNICODE
}

void UsrDump(TCHAR *msg, BYTE *pData, int len)
{
	try
	{
		va_list argList;
		va_start(argList, len);
		CString trmsg;

		if (pData == NULL)
			return;

		trmsg.Format(msg, argList);

		TRACE(_T("%s%s"), trmsg, _T("\n"));
		//TRACE(_T("\n"));
		TRACE(_T("Address:\t0x%.8x\nOffset\t\tData\n"), pData);
		//TRACE(_T("Offset\t\tData\n"));

		CString str;

		int c = len / 16;
		int i;

		for (i = 0; i < c; i++)
		{
			str.Format(_T("0x%.8x\t%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n"),
			           i * 16,
			           pData[0], pData[1], pData[2], pData[3], pData[4], pData[5], pData[6], pData[7],
			           pData[8], pData[9], pData[10], pData[11], pData[12], pData[13], pData[14], pData[15],
			           GetPrintCharOrDot(pData[0]), GetPrintCharOrDot(pData[1]), GetPrintCharOrDot(pData[2]), GetPrintCharOrDot(pData[3]), GetPrintCharOrDot(pData[4]), GetPrintCharOrDot(pData[5]), GetPrintCharOrDot(pData[6]), GetPrintCharOrDot(pData[7]),
			           GetPrintCharOrDot(pData[8]), GetPrintCharOrDot(pData[9]), GetPrintCharOrDot(pData[10]), GetPrintCharOrDot(pData[11]), GetPrintCharOrDot(pData[12]), GetPrintCharOrDot(pData[13]), GetPrintCharOrDot(pData[14]), GetPrintCharOrDot(pData[15]));
			TRACE(str);
			pData += 16;
		}

		c = len % 16;

		CString numbers;
		if (c != 0)
		{
			str.Format(_T("0x%.8x\t"), i * 16);
			numbers += str;
			//TRACE(str);
		}

		for (int j = 0; j < c; j++)
		{
			str.Format(_T("%.2x "), pData[j]);
			numbers += str;
			//TRACE(str);
		}
		//TRACE(numbers);

		CString spaces;
		if (c != 0)
		{
			int k = c * 3;
			for (int j = k; j < 47; j++)
			{
				//TRACE(_T(" "));
				spaces += _T(" ");
			}
			//TRACE(_T(" "));
			spaces += _T(" ");
		}
		//TRACE(numbers+spaces);

		CString chars;
		for (int j = 0; j < c; j++)
		{
			str.Format(_T("%c"), GetPrintCharOrDot(pData[j]));
			//TRACE(str);
			chars += str;
		}
		//TRACE(numbers + spaces + chars);

		if (c != 0)
			TRACE(numbers + spaces + chars + _T("\n"));

		va_end(argList);
	}
	catch (...)
	{
		TRACE(_T("LINE %d: Unknown exception.\n"), __LINE__);
		::SetLastError((unsigned long)NTE_FAIL);
	}
}

CString GetDumpString(BYTE *pData, int len)
{
	CString dumped;

	try
	{
		CString str;

		int c = len / 16;
		int i;
		for (i = 0; i < c; i++)
		{
			str.Format(_T("0x%.8x     %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"),
			           i * 16,
			           pData[0], pData[1], pData[2], pData[3], pData[4], pData[5], pData[6], pData[7],
			           pData[8], pData[9], pData[10], pData[11], pData[12], pData[13], pData[14], pData[15],
			           GetPrintCharOrDot(pData[0]), GetPrintCharOrDot(pData[1]), GetPrintCharOrDot(pData[2]), GetPrintCharOrDot(pData[3]), GetPrintCharOrDot(pData[4]), GetPrintCharOrDot(pData[5]), GetPrintCharOrDot(pData[6]), GetPrintCharOrDot(pData[7]),
			           GetPrintCharOrDot(pData[8]), GetPrintCharOrDot(pData[9]), GetPrintCharOrDot(pData[10]), GetPrintCharOrDot(pData[11]), GetPrintCharOrDot(pData[12]), GetPrintCharOrDot(pData[13]), GetPrintCharOrDot(pData[14]), GetPrintCharOrDot(pData[15]));
			if (len > 16)
			{
				str += (TCHAR)13;
				str += (TCHAR)10;
			}
			dumped += str;
			pData  += 16;
		}

		c = len % 16;

		CString numbers;
		if (c != 0)
		{
			str.Format(_T("0x%.8x     "), i * 16);
			numbers += str;
		}

		for (int j = 0; j < c; j++)
		{
			str.Format(_T("%.2x "), pData[j]);
			numbers += str;
		}

		CString spaces;
		if (c != 0)
		{
			int k = c * 3;
			for (int j = k; j < 47; j++)
			{
				spaces += _T(" ");
			}
			spaces += _T("  ");
		}

		CString chars;
		for (int j = 0; j < c; j++)
		{
			str.Format(_T("%c"), GetPrintCharOrDot(pData[j]));
			chars += str;
		}

		if (c != 0)
		{
			dumped += numbers + spaces + chars + (TCHAR)13;
			dumped += (TCHAR)10;
		}
	}
	catch (...)
	{
		TRACE(_T("LINE %d: Unknown exception.\n"), __LINE__);
		::SetLastError((unsigned long)NTE_FAIL);
	}

	return dumped;
}
