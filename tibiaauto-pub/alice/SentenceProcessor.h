/**
 * SentenceProcessor - Capitalises first character
 *
 * @author	Jonathan Roewen
 */
#ifndef SENTENCE_PROCESSOR_H
#define SENTENCE_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

#include <cctype>

using namespace std;

class SentenceProcessor : public AimlProcessor
{
public:
	~SentenceProcessor() {
	}

	string process(Match *m, PElement e, Responder *r, const string &id) {
		string result = Kernel::process(m, e, r, id);
		result[0] = toupper(result[0]);
		return result;
	}
};

#endif // ifndef SENTENCE_PROCESSOR_H
