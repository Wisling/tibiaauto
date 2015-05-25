/**
 * TemplateProcessor - The big dude on campus
 *
 * @author	Jonathan Roewen
 */
#ifndef TEMPLATE_PROCESSOR_H
#define TEMPLATE_PROCESSOR_H

#include <map>
#include <string>

using namespace std;

#include "AimlProcessor.h"

class TemplateProcessor : public AimlProcessor
{
public:
	~TemplateProcessor();

	string process(Match *, PElement, Responder *, const string &);
	static AimlProcessor *getProcessor(const string &name);
	static string processTemplate(Match *, PElement, Responder *, const string &id);
private:
	static AimlProcessor *hook;
	static PElement checkCompatibility(PElement);
};

#endif // ifndef TEMPLATE_PROCESSOR_H
