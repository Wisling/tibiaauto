/**
 * LowercaseProcessor - Transforms output to lowercase
 *
 * @author	Jonathan Roewen
 */
#ifndef LOWERCASE_PROCESSOR_H
#define LOWERCASE_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"
#include "Utils.h"

#include <string>

using namespace std;

class LowercaseProcessor : public AimlProcessor
{
public:
	~LowercaseProcessor() {
	}

	string process(Match *m, PElement e, Responder *r, const string &id) {
		string result = Kernel::process(m, e, r, id);
		return toLower(result);
	}
};

#endif // ifndef LOWERCASE_PROCESSOR_H
