/**
 * SecureProcessor - To prevent unauthorised access to AIML
 * content
 *
 * @author	Jonathan Roewen
 */
#ifndef SECURE_PROCESSOR_H
#define SECURE_PROCESSOR_H

#include <string>
#include <vector>
#include <algorithm>	//	For find() right?
#include "AimlProcessor.h"
#include "Kernel.h"

using namespace std;

/** This comment contains code to put into a .cpp file .. I'm lazy!

vector<string> SecureProcessor::users;	//	stops link errors

	Or can this be put just after the class decl? **/
	static vector<string> users;

class SecureProcessor : public AimlProcessor
{
public:
	SecureProcessor() {
		//	The console user .. this does have one flaw though:
		//	If an IRC user has nick 'localhost', then they will
		//	be one of the authenticated users :-(
		users.push_back("localhost");
		users.push_back("system");
	}
	~SecureProcessor() { }
	
	string process(Match *m, PElement e, Responder *r, const string &id) {
		vector<string>::iterator itr = find(users.begin(), users.end(), id);
		if (itr == users.end()) {
			//	User not in list, so not authenticated
			return e->getAttribute("error");// +e->getTagname() + ":"+e->getNamespace();
			//	That should be changed somehow
			//	Perhaps like Kernel::respond("DENYUSER", id);
		}
		return Kernel::process(m, e, r, id);
	}
	static void addAuthenticatedUser(const string &id) {
		users.push_back(id);
	}
};

#endif

#ifndef AUTHENTICATE_PROCESSOR_H
#define AUTHENTICATE_PROCESSOR_H

#include <string>
#include "AimlProcessor.h"

using namespace std;

class AuthenticateProcessor : public AimlProcessor
{
public:
	~AuthenticateProcessor() { }
	
	string process(Match *, PElement, Responder *, const string &id) {
		SecureProcessor::addAuthenticatedUser(id);
		return "authorised";
	}
};

#endif
