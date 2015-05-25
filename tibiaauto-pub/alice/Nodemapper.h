/**
 * Nodemapper - ABC for storing brain contents (AIML)
 *
 * @author	Jonathan Roewen
 */
#ifndef NODEMAPPER_H
#define NODEMAPPER_H

#include <string>

#include "Template.h"

class Nodemapper
{
public:
	virtual ~Nodemapper() {
	}

	virtual Nodemapper *getChild(const string &)        = 0;
	virtual void addChild(const string &, Nodemapper *) = 0;
	virtual string getTemplate()                        = 0;
	virtual void addTemplate(const Template &)          = 0;
	virtual string getAuthor()                          = 0;
	virtual void addAuthor(const string &)              = 0;
	virtual string getFilename()                        = 0;
	virtual void addFilename(const string &)            = 0;
};

#endif // ifndef NODEMAPPER_H
