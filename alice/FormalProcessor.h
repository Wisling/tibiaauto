/**
 * FormalProcessor - Capitalises first character of each word
 *
 * @author	Jonathan Roewen
 */
#ifndef FORMAL_PROCESSOR_H
#define FORMAL_PROCESSOR_H

#include "AimlProcessor.h"
#include "StringTokenizer.h"
#include "Kernel.h"

#include <string>

#include <cctype>

using namespace std;

class FormalProcessor : public AimlProcessor
{
public:
	~FormalProcessor() { }
	
	string process(Match *m, PElement e, Responder *r, const string &id) {
		string formal = Kernel::process(m, e, r, id);
		StringTokenizer st(formal, " \n\r\t", true);
		string result = "";
		while (st.hasMoreTokens()) {
			string word = st.nextToken();
			word[0] = toupper(word[0]);
			result += word;
		}
		return result;
	}
};

#endif
