#include <stdafx.h>

/**
 * Handler - Solution for no dynamic loading
 *
 * @author	Jonathan Roewen
 */
#include "Handler.h"

#include "Match.h"
#include "Responder.h"
#include "TemplateProcessor.h"
#include "BrProcessor.h"
#include "IdProcessor.h"
#include "SrProcessor.h"
#include "BotProcessor.h"
#include "GetProcessor.h"
#include "IrcProcessor.h"
#include "SetProcessor.h"
#include "NameProcessor.h"
#include "SraiProcessor.h"
#include "StarProcessor.h"
#include "ThatProcessor.h"
#include "InputProcessor.h"
#include "LearnProcessor.h"
#include "ThinkProcessor.h"
#include "FormalProcessor.h"
#include "PersonProcessor.h"    //	+ Person2 and Gender
#include "RandomProcessor.h"
#include "SentenceProcessor.h"
#include "ThatstarProcessor.h"
#include "ConditionProcessor.h"
#include "LowercaseProcessor.h"
#include "NormalizeProcessor.h"
#include "TopicstarProcessor.h"
#include "UppercaseProcessor.h"
#include "HTTPProcessor.h"
#include "SecureProcessor.h"    //	Contains AuthenticateProcessor too
#if defined(ENABLE_JAVASCRIPT)
	#include "JavaScriptProcessor.h"
#endif // if defined(ENABLE_JAVASCRIPT)
#include "XmlSocketResponder.h"
//#include "TimerProcessor.h"
#include "SubstituteProcessor.h"
#include "SystemProcessor.h"
#include "VersionProcessor.h"
#include "ImpExpProcessor.h"

map<string, AimlProcessor *> Handler::processors;

void Handler::init() {
	processors["br"]        = new BrProcessor();
	processors["id"]        = new IdProcessor();
	processors["sr"]        = new SrProcessor();
	processors["bot"]       = new BotProcessor();
	processors["get"]       = new GetProcessor();
	processors["irc"]       = new IrcProcessor();
	processors["set"]       = new SetProcessor();
	processors["name"]      = new NameProcessor();
	processors["srai"]      = new SraiProcessor();
	processors["star"]      = new StarProcessor();
	processors["that"]      = new ThatProcessor();
	processors["input"]     = new InputProcessor();
	processors["learn"]     = new LearnProcessor();
	processors["think"]     = new ThinkProcessor();
	processors["formal"]    = new FormalProcessor();
	processors["person"]    = new PersonProcessor();
	processors["random"]    = new RandomProcessor();
	processors["sentence"]  = new SentenceProcessor();
	processors["thatstar"]  = new ThatstarProcessor();
	processors["condition"] = new ConditionProcessor();
	processors["lowercase"] = new LowercaseProcessor();
	processors["normalize"] = new NormalizeProcessor();
	processors["topicstar"] = new TopicstarProcessor();
	processors["uppercase"] = new UppercaseProcessor();
	processors["template"]  = new TemplateProcessor();
	processors["http"]      = new HTTPProcessor();
	processors["secure"]    = new SecureProcessor();
	processors["system"]    = new SystemProcessor();
#if defined(ENABLE_JAVASCRIPT)
	processors["javascript"] = new JavaScriptProcessor();
#endif // if defined(ENABLE_JAVASCRIPT)
	processors["authenticate"] = new AuthenticateProcessor();

	processors["person2"]   = new Person2Processor();
	processors["gender"]    = new GenderProcessor();
	processors["xmlsocket"] = new XmlSocketProcessor();
//	processors["timer"] = new TimerProcessor();
	processors["substitute"] = new SubstituteProcessor();
	processors["version"]    = new VersionProcessor();
	processors["implode"]    = new ImplodeProcessor();
	processors["explode"]    = new ExplodeProcessor();
}

bool Handler::hasProcessor(const string &name) {
	return processors.find(name) != processors.end();
}
