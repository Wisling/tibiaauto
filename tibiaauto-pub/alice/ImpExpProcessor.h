#ifndef IMP_EXP_PROCESSOR_H
#define IMP_EXP_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>

using namespace std;

class ImplodeProcessor : public AimlProcessor
{
public:
	string process(Match *m, PElement e, Responder *r, const string &id)
	{
		string s = Kernel::process(m, e, r, id);

		string rs = "";

		for (size_t i = 0; i < s.length(); i += 2)
		{
			rs += s[i];
		}

		return rs;
	}
};

class ExplodeProcessor : public AimlProcessor
{
public:
	string process(Match *m, PElement e, Responder *r, const string &id)
	{
		string s = Kernel::process(m, e, r, id);

		string rs = "";

		for (size_t i = 0; i < s.length(); ++i)
		{
			rs += s[i] + " ";
		}

		return rs;
	}
};

#endif // ifndef IMP_EXP_PROCESSOR_H
