#ifndef AIML_WRITER_H
#define AIML_WRITER_H

#include <string>
#include "Element.h"

using namespace std;

class AimlWriter
{
public:
	static string parse(PElement, bool pretty, const string &rootName = "", bool addRoot = false);
	static string recurse(PElement);
	static string prettyAiml(PElement);
};

#endif
