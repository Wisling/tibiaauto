// stdafx.cpp : source file that includes just the standard includes
//	tibiaauto.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "time.h"

extern HANDLE hPipe;

void ipcMessage::send()
{
	this->tm = time(NULL);
	DWORD cbWritten;
	BOOL fSuccess = WriteFile( 
		hPipe,
		this,
		sizeof(struct ipcMessage), 
		&cbWritten,
		NULL);
}
