#include <stdafx.h>

#include "PreParser.h"

#include "Kernel.h"
#include "Template.h"
#include "Nodemaster.h"
#include "Utils.h"

int templateDepth = 0;
extern int totalCategories;
extern string botName;

void PreParser::elementStarted(const string &name, int index) {
	elementStarted(name, NULL, index);
}

void PreParser::elementStarted(const string &name, attributeMap *attr, int index) {
	if (name == "context" && templateDepth < 1)
	{
		if (attr != NULL)
		{
			attributeMap::const_iterator itr = attr->find("name");
			if (itr != attr->end())
				context = (*itr).second;
		}
	}
	else if (name == "topic" && templateDepth < 1)
	{
		if (attr != NULL)
		{
			attributeMap::const_iterator itr = attr->find("name");
			if (itr != attr->end())
				topic = (*itr).second;
		}
	}
	else if (name == "pattern" && templateDepth == 0)
	{
		pattern = "";
	}
	else if (name == "template")
	{
		if (++templateDepth == 1)
			templateBegin = index;
	}
	else if (name == "that" && templateDepth < 1)
	{
		if (that == "*")
			that = "";
	}
	else if (name == "bot")
	{
		if (currentTag == "pattern")
		{
			if (attr != NULL)
			{
				attributeMap::const_iterator itr = attr->find("name");
				if (itr != attr->end())
				{
					string value = (*itr).second;
					if (value == "name")
					{
						pattern += botName;
					}
					else
					{
						//	Avoid double space
						pattern = trim(pattern);
						if (!pattern.empty())
							pattern += " ";
					}
				}
			}
			return; //	Only if we're in the <pattern>
		}
	}
	else if (name == "name")
	{
		if (currentTag == "pattern")
		{
			pattern += botName;
			return; //	Only if we're in the <pattern>
		}
	}
	else
	{
		return;
	}
	currentTag = name;
}

void PreParser::elementFinished(const string &name, int index) {
	if (name == "context" && templateDepth < 1)
	{
		context = "*";
	}
	else if (name == "topic" && templateDepth < 1)
	{
		topic = "*";
	}
	else if (name == "pattern")
	{
		;
	}
	else if (name == "template" && --templateDepth == 0)
	{
		//	FIXME: It assumes <template></template>
		//	       Causes crash if it is <template/> when
		//         triggered
		int len = (index - templateBegin) - 11;
// slowness
		Nodemaster *node = Kernel::add(context, pattern, that, topic);
		that = "*";
		if (node != NULL)
		{
			node->addTemplate(new Template(filename, templateBegin, (index - 11)));
			++totalCategories;
		}
		templateBegin = -1;
	}
	else if (name == "that" && templateDepth < 1)
	{
		if (that.empty())
			that = "*";
	}
	else
	{
		return;
	}
	currentTag.erase();
}

void PreParser::elementText(const string &text, int) {
	if (currentTag == "pattern" && templateDepth == 0)
		pattern += text;
	else if (currentTag == "that" && templateDepth < 1)
		if (that.empty())
			that = text;
}

void PreParser::elementCData(const string &, int) {
	;
}
