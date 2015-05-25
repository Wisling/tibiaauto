
#include <stdafx.h>
#include <iostream>
#include <map>

#include "Stream.h"

using namespace std;

static StreamProvider* streamProvider = NULL;

void Stream::Read(const char *str)
{
}

void Stream::Write(const char *str)
{
	cout << str << endl;
}

Stream *StreamProvider::getStream(const char *strName)
{
	static map<string, Stream> streams;
	return &streams[strName];
}


Stream* getStream(const char *szName)
{
	if(streamProvider == NULL)
		setStreamProvider(new StreamProvider());
	return streamProvider->getStream(szName);
}

void setStreamProvider(
        StreamProvider *newStreamProvider)
{
	streamProvider = newStreamProvider;
}
