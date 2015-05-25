/**
 * JavaScriptProcessor - Experimental
 *
 * Uses Mozilla's SpiderMonkey JavaScript Engine. Hopefully, it will
 * work okay.
 *
 * Currently supported AIML tags usable using the global bot object:
 *
 * <star index="n"/>
 * <get name="property"/>
 * <set name="property">value</set>
 * <thatstar index="n"/>
 * <topicstar index="n"/>
 *
 * @author	Jonathan Roewen
 */
#ifndef JAVA_SCRIPT_PROCESSOR_H
#define JAVA_SCRIPT_PROCESSOR_H

#include "AimlProcessor.h"
#include "Kernel.h"

#include <string>
#include <iostream>

//	Prolly need something for MacOS X later too...
#if defined(__BEOS__)
 #define XP_BEOS
#elif defined(WIN32)
 #define XP_PC
 #define XP_WIN
#elif defined(MACOS)
 #define XP_MAC
#else // if defined(__BEOS__)
 #define XP_UNIX
#endif // if defined(__BEOS__)

#include <jsapi.h>

using namespace std;

static void
my_ErrorReporter(JSContext *, const char *message, JSErrorReport *report)
{
//    int i, j, k, n;
	string prefix, tmp;

	if (!report)
	{
		cerr << message << endl;
		return;
	}

	/* Conditionally ignore reported warnings. */
	if (JSREPORT_IS_WARNING(report->flags))
		return;

	prefix = "";
	if (report->filename)
		prefix = string(report->filename) + ":";
	if (report->lineno)
	{
		tmp     = prefix;
		prefix  = tmp;
		prefix += report->lineno;
	}

	/* If there were no filename or lineno, the prefix might be empty */
	if (prefix.length() > 0)
		cerr << prefix;
	cerr << message << endl;

	if (!report->linebuf)
		return;

	/* report->linebuf usually ends with a newline. */
	cerr << prefix << report->linebuf << endl;
}

JSClass globalClass = {
	"global", 0,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
};

JSClass botClass = {
	"Bot", 0,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
};

static struct {
	Match *match;
	PElement element;
	Responder *responder;
	string id;
} JSP_Vars;

//	Functions for the botClass
static JSBool botGet(JSContext *cx, JSObject *, uintN, jsval *argv, jsval *rval)
{
	JSString *predicate, *result;

	if (!(predicate = JS_ValueToString(cx, argv[0])))
	{
		cerr << "couldn't get a string" << endl;
		return JS_FALSE;
	}
	string p = JS_GetStringBytes(predicate);
	string r = Kernel::predicates->getValue(p, JSP_Vars.id);
	char *cs = get_string(r);
	result = JS_NewString(cx, cs, r.length());
	*rval  = STRING_TO_JSVAL(result);
	return JS_TRUE;
}

static JSBool botSet(JSContext *cx, JSObject *, uintN argc, jsval *argv, jsval *rval)
{
	JSString *property, *value;

	if (argc != 2)
		return JS_FALSE;
	if (!(property = JS_ValueToString(cx, argv[0])))
		return JS_FALSE;
	if (!(value = JS_ValueToString(cx, argv[1])))
		return JS_FALSE;

	string p = JS_GetStringBytes(property);
	string v = JS_GetStringBytes(value);
	Kernel::predicates->setValue(p, JSP_Vars.id, v);
	*rval = STRING_TO_JSVAL(value);
	return JS_TRUE;
}

static JSBool botStar(JSContext *cx, JSObject *, uintN, jsval *argv, jsval *rval)
{
	JSString *result;
	jsint index;

	if (!JS_ValueToInt32(cx, argv[0], &index))
	{
		cerr << "Couldn't get an int value" << endl;
		return JS_FALSE;
	}

	string r = JSP_Vars.match->getInputStar(index);
	char *cs = get_string(r);
	result = JS_NewString(cx, cs, r.length());
	*rval  = STRING_TO_JSVAL(result);
	return JS_TRUE;
}

static JSBool botThatstar(JSContext *cx, JSObject *, uintN, jsval *argv, jsval *rval)
{
	JSString *result;
	jsint index;

	if (!JS_ValueToInt32(cx, argv[0], &index))
		return JS_FALSE;

	string r = JSP_Vars.match->getThatStar(index);
	char *cs = get_string(r);
	result = JS_NewString(cx, cs, r.length());
	*rval  = STRING_TO_JSVAL(result);
	return JS_TRUE;
}

static JSBool botTopicstar(JSContext *cx, JSObject *, uintN, jsval *argv, jsval *rval)
{
	JSString *result;
	jsint index;

	if (!JS_ValueToInt32(cx, argv[0], &index))
		return JS_FALSE;

	string r = JSP_Vars.match->getTopicStar(index);
	char *cs = get_string(r);
	result = JS_NewString(cx, cs, r.length());
	*rval  = STRING_TO_JSVAL(result);
	return JS_TRUE;
}

static JSFunctionSpec botFunctions[] = {
	{"get", botGet, 1},
	{"set", botSet, 2},
	{"star", botStar, 1},
	{"thatstar", botThatstar, 1},
	{"topicstar", botTopicstar, 1},
	{0}
};

class JavaScriptProcessor : public AimlProcessor
{
public:
	~JavaScriptProcessor()
	{
		if (initialised)
		{
			JS_DestroyContext(context);
			JS_DestroyRuntime(runtime);
		}
	}

	JavaScriptProcessor()
	{
		initialised = false;
		//	Initialise the JavaScript Runtime (give it 8MB)
		runtime = JS_NewRuntime(8L * 1024L * 1024L);
		if (!runtime)
		{
			//	Creation failed...
			cerr << "Failed to initialise JavaScript Runtime" << endl;
			return;
		}
		context = JS_NewContext(runtime, 8192);
		if (context == NULL)
		{
			cerr << "Failed to create context for JavaScript engine" << endl;
			return;
		}
		JS_SetErrorReporter(context, my_ErrorReporter);
		globalObject = JS_NewObject(context, &globalClass, NULL, NULL);
		if (!(builtinObjects = JS_InitStandardClasses(context, globalObject)))
		{
			cerr << "Failed to initialise global objects" << endl;
			return;
		}
		//	What is a JSPROP_ENUMERATE .. what else can it be?
		botObject = JS_DefineObject(context, globalObject, "bot", &botClass, 0, 0);

		//if (!JS_DefineFunction(context, botObject, "get", get, 1, 0)) {
		//	cerr << "Unable to create the get method for the bot global object" << endl;
		//}
		if (!JS_DefineFunctions(context, botObject, botFunctions))
			cerr << "Unable to create the bot functions" << endl;
		initialised = true;
	}

	string process(Match *m, PElement e, Responder *r, const string &id)
	{
		if (!initialised)
			return "javascript not initialised";
		//	Set the variables
		JSP_Vars.match     = m;
		JSP_Vars.element   = e;
		JSP_Vars.responder = r;
		JSP_Vars.id        = id;

		string input = Kernel::process(m, e, r, id);
		//...
		jsval rval;
		JSString *str;
		char *filename = NULL;
		int lineNo     = 0;
		string result  = "failure";
		if (JS_EvaluateScript(context, globalObject, input.c_str(), input.length(), filename, lineNo, &rval))
		{
			str    = JS_ValueToString(context, rval);
			result = JS_GetStringBytes(str);
		}
		//	Clear the variables
		JSP_Vars.match     = NULL;
		JSP_Vars.element   = NULL;
		JSP_Vars.responder = NULL;
		JSP_Vars.id        = "";

		return result;
	}

private:
	JSRuntime *runtime;
	JSContext *context;
	JSObject *globalObject;
	//	This will represent the bot engine itself,
	//	and will give access to AIML functions
	//	through the global bot object.
	JSObject *botObject;
	JSBool builtinObjects;

	bool initialised;
};

#endif // ifndef JAVA_SCRIPT_PROCESSOR_H
