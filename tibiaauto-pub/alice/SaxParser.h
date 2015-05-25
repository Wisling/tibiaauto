#ifndef SAX_PARSER_H
#define SAX_PARSER_H

#include <string>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

typedef map<string, string> attributeMap;

class SaxListener
{
public:
	SaxListener() {
	}
	virtual ~SaxListener() {
	}

	virtual void elementStarted(const string &, int)                 = 0;
	virtual void elementFinished(const string &, int)                = 0;
	virtual void elementStarted(const string &, attributeMap *, int) = 0;
	virtual void elementText(const string &, int)                    = 0;
	virtual void elementCData(const string &, int)                   = 0;
};

class SaxParser
{
public:
	SaxParser(SaxListener *sl) {
		listener = sl;
	}
	~SaxParser() {
		delete listener;
	}
	void parse(istream &);
	SaxListener *getListener() {
		return listener;
	}
private:
	void addElement(string &, int);
	void addElement(vector<char> &, int);

	SaxListener *listener;
};

#endif // ifndef SAX_PARSER_H
