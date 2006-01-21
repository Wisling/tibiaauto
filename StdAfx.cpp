// stdafx.cpp : source file that includes just the standard includes
//	tibiaauto.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

extern HANDLE hPipe;


void ipcMessage::send()
{
	DWORD cbWritten;
	BOOL fSuccess = WriteFile( 
		hPipe,
		this,
		sizeof(struct ipcMessage), 
		&cbWritten,
		NULL);
}
