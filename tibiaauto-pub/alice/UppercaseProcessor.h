/**
 * UppercaseProcessor - Transforms output to uppercase
 *
 * @author	Jonathan Roewen
 */
#ifndef UPPERCASE_PROCESSOR_H
#define UPPERCASE_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"
#include "Utils.h"

#include <string>

using namespace std;

class UppercaseProcessor : public AimlProcessor
{
public:
	~UppercaseProcessor() { }
	
	string process(Match *m, PElement e, Responder *r, const string &id) {
		string result = Kernel::process(m, e, r, id);
		return toUpper(result);
	}
};

#endif
