#ifndef PRE_PARSER_H
#define PRE_PARSER_H

#include "SaxParser.h"

using namespace std;

class PreParser : public SaxListener
{
public:
	PreParser(const string &fn = "")
	{
		filename      = fn;
		context       = "*";
		topic         = "*";
		that          = "*";
		pattern       = "";
		templateBegin = -1;
	}

	virtual ~PreParser()
	{
	}

	void elementStarted(const string &, int);
	void elementFinished(const string &, int);
	void elementStarted(const string &, attributeMap *, int);
	void elementText(const string &, int);
	void elementCData(const string &, int);
private:
	string pattern, topic, that, currentTag, filename;
	string context;
	int templateBegin;
};

#endif // ifndef PRE_PARSER_H
