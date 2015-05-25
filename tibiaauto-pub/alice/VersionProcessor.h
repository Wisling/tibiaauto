#ifndef VERSION_PROCESSOR_H
#define VERSION_PROCESSOR_H

#include "AimlProcessor.h"

#include <string>

#ifndef DATE_BUILT
#define DATE_BUILT __DATE__
#endif // ifndef DATE_BUILT
#define VERSION_STRING "J-Alice AIML Interpreter v0.5 (built: "DATE_BUILT ")"

using namespace std;

class VersionProcessor : public AimlProcessor
{
public:
	~VersionProcessor() {
	}

	string process(Match *, PElement, Responder *, const string &) {
		return VERSION_STRING;
	}
};

#endif // ifndef VERSION_PROCESSOR_H
