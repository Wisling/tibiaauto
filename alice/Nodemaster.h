/**
 * Nodemaster - The real nodemapper ;-)
 *
 * @author	Jonathan Roewen
 */
#ifndef NODEMASTER_H
#define NODEMASTER_H

#include <map>
#include <vector>
#include <string>

using namespace std;

#include "Template.h"

class NodeWalker;

class Nodemaster
{
public:
	Nodemaster() {
		templates = NULL;
	}
	~Nodemaster() { }
	
	Nodemaster *getChild(const string &);
	void addChild(const string &, Nodemaster *);
	string getTemplate(bool reload = true);
	Template *getActualTemplate();
	void addTemplate(Template *t);
	bool hasTemplate();
private:
	map<char *, Nodemaster *> children;
//	vector<Template> templates;
	Template *templates;
//	Template *templates;	
//	unsigned int lastIndex;
	friend class NodeWalker;
};

typedef map<char *, Nodemaster *> childrenMap;

#endif  
