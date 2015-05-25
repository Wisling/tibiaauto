#ifndef PARSER_H
#define PARSER_H

#include "SaxParser.h"
#include "Element.h"

#include <stack>

using namespace std;

class Parser : public SaxListener
{
public:
	Parser()
	{
		root = current = PElement(new Element("root"));
	}

	PElement getRoot() const;
private:
	PElement current, root;
	stack<PElement> recurser;

	void elementStarted(const string &, int);
	void elementFinished(const string &, int);
	void elementStarted(const string &, attributeMap *, int);
	void elementText(const string &, int);
	void elementCData(const string &, int);
};

#endif // ifndef PARSER_H
