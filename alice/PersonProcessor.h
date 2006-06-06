/**
 * PersonProcessor - Retrieves the bot's name (shortcut)
 *
 * @author	Jonathan Roewen
 */
#ifndef PERSON_PROCESSOR_H
#define PERSON_PROCESSOR_H

#include "AimlProcessor.h"
#include "Substituter.h"
#include "Kernel.h"

#include <string>

using namespace std;

class PersonProcessor : public AimlProcessor
{
public:
	~PersonProcessor() { }
	
	string process(Match *m, PElement e, Responder *r, const string &id) {
		string result = "";
		string index = e->getAttribute("index");
		if (index.empty()) {
			index = "1";
		}
		if (e->hasChildren()) {
			result = Kernel::process(m, e, r, id);
		} else {
			result = m->getInputStar(index[0] - '0');
		}
		return Substituter::substitute(result, "person");
	}
};

class Person2Processor : public AimlProcessor
{
public:
	~Person2Processor() { }
	
	string getName() const {
		return "person2";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *m, PElement e, Responder *r, const string &id) {
		string result = "";
		string index = e->getAttribute("index");
		if (index.empty()) {
			index = "1";
		}
		if (e->hasChildren()) {
			result = Kernel::process(m, e, r, id);
		} else {
			result = m->getInputStar(index[0] - '0');
		}
		return Substituter::substitute(result, "person2");
	}
};

class GenderProcessor : public AimlProcessor
{
public:
	~GenderProcessor() { }
	
	string getName() const {
		return "gender";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *m, PElement e, Responder *r, const string &id) {
		string result = "";
		string index = e->getAttribute("index");
		if (index.empty()) {
			index = "1";
		}
		if (e->hasChildren()) {
			result = Kernel::process(m, e, r, id);
		} else {
			result = m->getInputStar(index[0] - '0');
		}
		return Substituter::substitute(result, "gender");
	}
};

#endif
