#ifndef STREAM_H
#define STREAM_H

#include <map>
#include <string>
using namespace std;

class Stream
{
public:
	virtual void Read(const char *str);
	virtual void Write(const char *str);
};

class StreamProvider
{

public:
	virtual Stream *getStream(const char *str);
};

Stream* getStream(const char *szName);
void setStreamProvider(StreamProvider *streamProvider);

#endif

