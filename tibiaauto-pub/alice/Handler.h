/**
 * Handler - Solution for no dynamic loading at present
 *
 * @author	Jonathan Roewen
 */
#ifndef HANDLER_H
#define HANDLER_H

#include <map>
#include <string>

using namespace std;

#include "AimlProcessor.h"

class Handler
{
public:
	static void init();
	static bool hasProcessor(const string &name);
	
	static map<string, AimlProcessor *> processors;
};

#endif
