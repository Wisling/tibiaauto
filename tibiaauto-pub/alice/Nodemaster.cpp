#include <stdafx.h>

/**
 * Nodemaster - The real nodemapper ;-)
 *
 * @author	Jonathan Roewen
 */
#include "Nodemaster.h"

#include "Utils.h"

Nodemaster *Nodemaster::getChild(const string &key)
{
	if (children.find(get_string(key)) == children.end())
		return NULL;
	return (*children.find(get_string(key))).second;
}

void Nodemaster::addChild(const string &key, Nodemaster *value)
{
	children[get_string(key)] = value;
}

//	Perhaps we do the automagic updating here?
string Nodemaster::getTemplate(bool reload)
{
/*	if (templates.empty()) {
                return "";
        }
        if (templates.size() == 1) {
                Template *t = templates[0];
                if (Template::reloadFile(t.filenameIx)) {
                        t = templates[0];
                }
                return t.fetch();
        }
        lastIndex = rand() % templates.size();
        Template t = templates[lastIndex];
        return t.fetch();
 */
	if (templates == NULL)
		return "";
	if (reload)
		Template::reloadFile(templates->filenameIx);
	return templates->fetch();

//	return templates->fetch();
}

void Nodemaster::addTemplate(Template *t)
{
//	totalSize += (value.length() * sizeof(char)) + sizeof(string);
	if (templates != NULL)
		delete templates;
	templates = t;
//	templates = t;
}

bool Nodemaster::hasTemplate()
{
	return templates != NULL;
}

Template *Nodemaster::getActualTemplate()
{
	return templates;
}
