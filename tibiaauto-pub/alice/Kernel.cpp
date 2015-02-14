#include <stdafx.h>

/**
 * Kernel - Brains of the operation
 *
 * @author	Jonathan Roewen
 */
#include "Kernel.h"

#include <fstream>
#include <iostream>
#include <strstream>

#include "Match.h"
#include "Utils.h"
#include "Parser.h"
#include "Handler.h"
#include "Responder.h"
#include "Nodemaster.h"
#include "Substituter.h"
#include "StringTokenizer.h"
#include "TemplateProcessor.h"
#include "PreParser.h"
#include "AimlWriter.h"
#include "NodeWalker.h"
#include "SaxParser.h"
#include "Stream.h"
#include "Config.h"
#include "TimeKeeper.h"

IrcConfig ic;

int totalCategories = 0;
long totalSize = 0;
long totalNodes = 0;

const int Kernel::CONTEXT = 1;
const int Kernel::INPUT = 2;
const int Kernel::THAT = 3;
const int Kernel::TOPIC = 4;

const string Kernel::constants = " <pattern> <that> <topic> ";

PredicateEngine *Kernel::predicates = new PredicateEngine();

static bool trimming = true;

extern long totalTime;


string templateString(PElement);
string toString(PElement);
void prettyPrintHeader(const string &, int, int, long);
string getSentence(string &input);

string toString(PElement element) {
	if (element == NULL) {
		return "*";
	}
	string result;
	PElement child = element->getChild();
	while (child != NULL) {
		if (child->getTagname() == "#text") {
			//	Then this child only contains text, no markup
			result += child->getText();
		} else if (child->getTagname() == "#cdata") {
			;
		} else {
			//	Since only using on <that> and <pattern>
			//	The tag can only be <bot_xxx/> or <bot name="xxx"/> or <name/>
			if (child->getTagname() == "name") {
				result += Kernel::respond("BOT NAME", "unknown");
			} else
			if (child->getTagname() == "bot") {
				//	Is a <bot name="xxx"/> predicate
				result += Kernel::respond("BOT " + toUpper(child->getAttribute("name")), "unknown");
			} else {
				//	It is old-skool <bot_xxx/>
				result += Kernel::respond("BOT " + toUpper(child->getTagname().substr(4)), "unknown");
			}
		}
		child = child->getNextSibling();
	}
	return trim(toUpper(result));//Substituter::normalize(result);
}

string templateString(PElement element) {
	if (element == NULL) {
		return "";
	}
	string result;
	PElement child = element->getChild();
	while (child != NULL) {
		if (child->getTagname() == "#text") {
			result += child->getText();
		} else if (child->getTagname() == "#cdata") {
			result += child->getText(false);
		} else {
			string tag = "<";
			if (child->hasNamespace()) {
				tag += child->getNamespace() + ":";
			}
			tag += child->getTagname();
			if (child->hasAttributes()) {
				map<string, string> attr = child->getAttributes();
				map<string, string>::const_iterator itr = attr.begin();
				while (itr != attr.end()) {
					tag += " " + (*itr).first + "=\"" + (*itr).second + "\"";
					itr++;
				}
			}
			if (child->hasChildren()) {
				tag += ">" + templateString(child);
				tag += "</";
				if (child->hasNamespace()) {
					tag += child->getNamespace() + ":";
				}
				tag += child->getTagname() + ">";
			} else {
				tag += "/>";
			}
			result += tag;
		}
		child = child->getNextSibling();
	}
	return result;
}

void prettyPrintHeader(const string &filename, int size, int totalSize, long time) {
	totalTime += time;
	string fn = filename;
	string::size_type f = fn.find_last_of("\\/:");
	if (f != string::npos) {
		fn = fn.substr(f + 1);
	}
	char sz[1024];
	Stream *out = getStream("Console");
	
	out->Read("Learnt: ");
	
	out->Read(fn.c_str());
	if (fn.length() >= 20) {
		cout << endl << "                            ";
	} else {
		for (size_t ix = 0; ix < 20 - fn.length(); ++ix) {
			cout << " ";
		}
	}
	int len = 0;
	if (size == 0) {
		++len;
	}
	for (int iy = size; iy > 0 && len < 6; iy = iy / 10) {
		++len;
	}
	for (int ix = 0; ix < 6 - len; ++ix) {
		cout << " ";
	}
	sprintf(sz, " %d categories ", size);
	out->Read(sz);
	len = 0;
	for (int iy2 = totalSize; iy2 > 0 && len < 6; iy2 = iy2 / 10) {
		++len;
	}
	for (int ix2 = 0; ix2 < 10 - len; ++ix2) {
		cout << " ";
	}
	sprintf(sz, "%d total ", totalSize);
	out->Read(sz);
	
	len = 0;
	for (int iy3 = (int)time; iy3 > 0 && len < 6; iy3 = iy3 / 10) {
		++len;
	}
	for (int ix3 = 0; ix3 < 6 - len; ++ix3) {
		cout << " ";
	}
	if (len != 0) {
		cout << " ";
	}
	sprintf(sz, "%dms\n", time);
	out->Read(sz);

}

Nodemaster *Kernel::root = new Nodemaster();

Kernel::Kernel() {
	char installPath[1024];
	unsigned long installPathLen=1023;
	installPath[0]='\0';
	HKEY hkey=NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey))
	{
		RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
		RegCloseKey(hkey);
	}
	if (!strlen(installPath))
	{
		AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
		exit(1);
	}

	new Substituter();
	Handler::init();
	Kernel::loadSubstitutions();
	char pathBuf[2048];
	sprintf(pathBuf,"%s\\mods\\std-startup.xml",installPath);
	if (!Kernel::load(pathBuf)) {
		getStream("Console")->Write("Shutting down (cannot run without std-startup.xml file)");
		serverRunning = false;
	}
	Kernel::loadTemporaryData();
}

Kernel::~Kernel() {
	;
}

string Kernel::bootstrap() {
	trimming = false;
	string result = Kernel::respond("bootstrap", "localhost");
	trimming = true;

	return result;
}

bool Kernel::load(const string &file) {
	SaxParser p(new PreParser(file));
	bool loadedOk = true;
	int total = totalCategories;
	long time = timerMillis();
	ifstream fin;
	fin.open(file.c_str(), ios::in | ios::binary);
	if (fin.is_open()) {
		p.parse(fin);
		fin.close();
	} else {
		loadedOk = false;
		cout << "File " << file << " could not be found" << endl;
	}
	if (loadedOk) {
		prettyPrintHeader(file, (totalCategories - total), totalCategories, (timerMillis() - time));
	}
	
	return loadedOk;
}

void Kernel::convertTempAiml() {
	PElement eAiml(new Element("aiml"));
	PElement eCategory,
		eContext,
		ePattern,
		eThat,
		eTopic,
		eTemplate,
		eText;
	
	ifstream fin("temporary.data", ios::in | ios::binary);
	
	if (!fin.is_open()) {
		cerr << "temporary.data does not exist .. cannot oonvert" << endl;
		return;
	}
	while (fin.good()) {
		eCategory = eContext = ePattern = eThat = eTopic = eTemplate = eText = NULL;
		string context, pattern, that, topic, _template;
		getline(fin, context, ',');
		if (context.empty()) {
			break;
		}
		getline(fin, pattern, ',');
		getline(fin, that, ',');
		getline(fin, topic, ',');
		getline(fin, _template);
		eCategory = new Element("category");
		ePattern = new Element("pattern");
		eText = new Element("#text");
		eText->setText(toUpper(pattern));
		ePattern->addChild(eText);
		eCategory->addChild(ePattern);
		if (that != "*") {
			eThat = new Element("that");
			eText = new Element("#text");
			eText->setText(toUpper(that));
			eThat->addChild(eText);
			eCategory->addChild(eThat);
		}
		eTemplate = new Element("template");
		eText = new Element("#text");
		eText->setText(_template);
		eTemplate->addChild(eText);
		eCategory->addChild(eTemplate);
		if (topic != "*") {
			eTopic = new Element("topic");
			eTopic->setAttribute("name", toUpper(topic));
			eTopic->addChild(eCategory);
		}
		if (context != "*") {
			eContext = new Element("context");
			eContext->setAttribute("name", toUpper(context));
			if (eTopic != NULL) {
				eContext->addChild(eTopic);
			} else {
				eContext->addChild(eCategory);
			}
		}
		if (eContext != NULL) {
			eAiml->addChild(eContext);
		}
		else if (eTopic != NULL) {
			eAiml->addChild(eTopic);
		}
		else {
			eAiml->addChild(eCategory);
		}
	}
	fin.close();
	////////////
	ofstream fout("temporary.aiml", ios::out | ios::binary | ios::trunc);
	if (!fout.is_open()) {
		cerr << "could not open temporary.aiml for writing" << endl;
		return;
	}
	string buffer = AimlWriter::prettyAiml(eAiml);
	if (buffer.empty()) {
		fout.close();
		return;
	}
	fout << buffer << endl;
	fout.close();
}

void Kernel::loadTemporaryData() {
	//	Format: <pattern>,<that>,<topic>,<template><eol>
	ifstream fin("temporary.data", ios::in | ios::binary);
	if (!fin.is_open()) {
		return;
	}
	while (fin.good()) {
		string context, pattern, that, topic, _template;
		getline(fin, context, ',');
		getline(fin, pattern, ',');
		getline(fin, that, ',');
		getline(fin, topic, ',');
		streamoff startIndex = fin.tellg();
		getline(fin, _template);
		streamoff endIndex = fin.tellg();
		
		Nodemaster *node = add("*", pattern, that, topic);
		if (node != NULL) {
			node->addTemplate(
				new Template("temporary.data", startIndex, endIndex)
			);
			++totalCategories;
		}
	}
	fin.close();
}

void Kernel::loadSubstitutions() {
	char installPath[1024];
	unsigned long installPathLen=1023;
	installPath[0]='\0';
	HKEY hkey=NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey))
	{
		RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
		RegCloseKey(hkey);
	}
	if (!strlen(installPath))
	{
		AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
		exit(1);
	}
	
	char pathBuf[2048];
	sprintf(pathBuf,"%s\\mods\\substitutions.xml",installPath);
	ifstream fin;
	PElement root;
	fin.open(pathBuf, ios::in | ios::binary);
	
	if (fin.is_open()) {
		SaxParser *p = new SaxParser(new Parser());
		p->parse(fin);
		root = ((Parser *)p->getListener())->getRoot();
		delete p;
		fin.close();
		
		if (root == NULL) {
			return;
		}
	} else {
		return;
	}
	
	velement array;
	velement_it ix;
	root->getChildren("substitute", &array);
	for (ix = array.begin(); ix != array.end(); ++ix) {
		string name = (*ix)->getAttribute("name");
		string find = (*ix)->getAttribute("find");
		string replace = (*ix)->getAttribute("replace");
		Substituter::addSubstitute(name, find, replace);
	}
}

/*
	Instead of forcing Nodemaster to uppercase the keys, uppercase
	if necessary prior, reducing the need to call the slow toUpper
	function.
	
	I think this & match() are the only places where Nodemaster's
	addChild() and getChild() functions are used... hopefully.
*/
Nodemaster *Kernel::add(const string &context, const string &pattern, const string &that, const string &topic) {
	if (context.empty() || pattern.empty() || that.empty() || topic.empty()) {
		return NULL;
	}
	string path = context + " <pattern> " + pattern + " <that> " + that + " <topic> " + topic;
	path = toUpper(path);
	Nodemaster *node = root;
	Nodemaster *child = NULL;
	StringTokenizer st(path, " ");
	string word;
	while (st.hasMoreTokens()) {
		word = st.nextToken();
//	slowness
		child = node->getChild(word);
		if (child == NULL) {
			child = new Nodemaster();
//	slowness
			node->addChild(word, child);
		}
		node = child;
	}
	return node;
}

Match *Kernel::match(const string &context, const string &input, const string &that, const string &topic) {
	Match *m = NULL;
	string path = context + " <pattern> " + input + " <that> " + that + " <topic> " + topic;
	getStream("Match")->Read(string("Input: " + path + "\n").c_str());
	m = match(root, root, CONTEXT, path, "", "");
	if (m != NULL) {
		m->setPath(m->getContextPattern() + " : " + m->getInputPattern() + " : " + m->getThatPattern() + " : " + m->getTopicPattern());
		getStream("Match")->Write(string("Match: " + m->getPath() + "\n").c_str());
	} else {
		getStream("Match")->Write("No match found\n");
	}
	return m;
}

Match *Kernel::match(Nodemaster *node, Nodemaster *parent, int state, const string &input, const string &star, const string &path) {
	StringTokenizer st(input);
	
	if (st.countTokens() == 0) {
		if (node->hasTemplate()) {
			Match *m = new Match();
			m->setNode(node);
			m->addTopicStar(trim(star));
			m->setTopicPattern(trim(path));
			return m;
		}
		return NULL;
	}
	
	string word = st.nextToken();
	string uWord = toUpper(word);
	string tail = "";
	Match *m = NULL;
	
	if (st.hasMoreTokens()) {
		tail = trim(input.substr(word.length()));
	}
	
	int index = constants.find(" " + word + " ");
	
	if (index != string::npos) {
		if (node->getChild(uWord) != NULL) {
			m = match(node->getChild(uWord), node, state + 1, tail, "", "");
			if (m != NULL) {
				addStar(m, trim(star), state);
				addPath(m, trim(path), state);
			}
		}
		return m;
	}
	if (node->getChild("_") != NULL) {
		m = match(node->getChild("_"), node, state, tail, word, path + " _");
		if (m != NULL) {
			addStar(m, trim(star), state);
			return m;
		}
	}
	if (node->getChild(uWord) != NULL) {
		m = match(node->getChild(uWord), node, state, tail, star, path + " " + word);
		if (m != NULL) {
			return m;
		}
	}
	if (node->getChild("@") != NULL) {
	//	This one is a bit of a mystery to get figured out
		//Vector queries = node.getQueries();
		//for (int ix = 0; ix < queries.size(); ++ix) {
			//String type = (String)queries.elementAt(ix);
			//if (lookup(type, star.trim()) == true) {
				//m = match(node.getChild("@" + type), node, state, tail, word, path + " @" + type);
				//if (m != null) {
					//addStar(m, star.trim(), state);
					//return m;
				//}
			//}
		//}
	}
	if (node->getChild("*") != NULL) {
		m = match(node->getChild("*"), node, state, tail, word, path + " *");
		if (m != NULL) {
			addStar(m, trim(star), state);
			return m;
		}
	}
	if (node == parent->getChild("*") ||
			node == parent->getChild("_") ||
			node == parent->getChild("@")) {
		return match(node, parent, state, tail, star + " " + word, path);
	}
	return NULL;
}

void Kernel::addStar(Match *m, const string &value, int state) {
	if (value.empty()) {
		return;
	}
	switch (state) {
		case CONTEXT:
			m->addContextStar(value);
			return;
		case INPUT:
			m->addInputStar(value);
			return;
		case THAT:
			m->addThatStar(value);
			return;
		case TOPIC:
			m->addTopicStar(value);
			return;
	}
}

void Kernel::addPath(Match *m, const string &value, int state) {
	if (value.empty()) {
		return;
	}
	switch (state) {
		case CONTEXT:
			m->setContextPattern(value);
			return;
		case INPUT:
			m->setInputPattern(value);
			return;
		case THAT:
			m->setThatPattern(value);
			return;
		case TOPIC:
			m->setTopicPattern(value);
			return;
	}
}

bool Kernel::lookup(const string &, const string &) {
	//Object o = querySets.get(type);
	//if (o == null) {
		//return false;
	//}
	
	//Enumeration items = ((Vector)o).elements();
	//while (items.hasMoreElements()) {
		//String item = (String)items.nextElement();
		//if (item.startsWith("@")) {
			//String subset = item.substring(1);
			//if (lookup(subset, input) == true) {
				//return true;
			//} else {
				//item = subset;
			//}
		//}
		//--	REGEX HERE WOULD ROCK
		//bool result = Pattern.matches(item, input.toUpperCase());
		//if (result == true) {
			//return true;
		//}
	//}
	
	return false;
}

string Kernel::process(Match *m, PElement e, Responder *r, const string &id) {
	return TemplateProcessor::processTemplate(m, e, r, id);
}

string Kernel::respond(const string &input, const string &id, const string &prefix) {
	return respond(input, id, 1, false, prefix);
}

string Kernel::respond(const string &input, const string &id, bool srai, const string &prefix) {
	return respond(input, id, 1, srai, prefix);
}

string Kernel::respond(const string &input, const string &id, Responder *r, bool srai, const string &prefix) {
	return respond(input, id, r, 1, srai, prefix);
}

string Kernel::respond(const string &input, const string &id, int depth, bool srai, const string &prefix) {
	/**
	 * Instead of NULL, should be: new TextResponder() or something.
	 * NULL isn't going to work later on
	 */
	return respond(input, id, NULL, depth, srai, prefix);
}

static int recursionDepth = 0;

string getSentence(string &input) {
	string output = "";
	string::size_type index = 0, length = input.length();
	char ch;
	
	while (index < length) {
		ch = input[index];
		if (ch == '.' || ch == '?' || ch == '!') {
			if (++index == length) {
				output = input.substr(0, index - 1);
				input.erase();
				return output;
			}
			if (input[index] == ' ') {
				output = input.substr(0, index - 1);
				input.erase(0, index);
				return output;
			}
		} else {
			++index;
		}
	}
	output = input;
	input.erase();
	return output;
}

static long timingResponse = 0;

string Kernel::respond(const string &input, const string &id, Responder *r, int, bool srai, const string &prefix) {
	if (!srai) {
		recursionDepth = 0;
		timingResponse = timerMillis();
	}
	//	I want this to be configurable...
	if (++recursionDepth > maxRecursiveDepth) {
		predicates->addHistory("that", id, "");
		cerr << "AIML contains an infinite loop" << endl;
		cerr << "Input involved in loop: " << input << endl;
		return "";
	}
	string currentResponse = "", buffer = "";
	Match *m = NULL;
	if (!srai) {
		predicates->addHistory("input", id, input);
	}
	string sentence, inString = input;
	while (!(sentence = getSentence(inString)).empty()) {
		sentence = trim(sentence);
		string originalInput = sentence;
		if (sentence.length() < 1) {
			continue;
		}
		sentence = prefix + " " + Substituter::substitute(sentence);
		string context = predicates->getValue("context", id);
		context = Substituter::substitute(context);
		string that = predicates->getValue("that", id);
		if (!srai) {
			StringTokenizer stThat(that, ".?!");
			while (stThat.hasMoreTokens()) {
				string t = stThat.nextToken();
				t = trim(t);
				if (!t.empty()) {
					that = t;
					predicates->addHistory("that", id, that);
				}
			}
		}
		that = Substituter::substitute(that);
		string topic = predicates->getValue("topic", id);
		topic = Substituter::substitute(topic);
		if (that.empty()) {
			that = "*";
		}
		if (topic.empty()) {
			topic = "*";
		}
		if (context.empty()) {
			context = "*";
		}
	
		//--	DEBUGGING LINE
		string ktr = sentence + "\n";
		getStream("Kernel")->Read(ktr.c_str());
	
		m = match(context, sentence, that, topic);
		if (m == NULL) {
				cerr << "There is no match for input: " << sentence << endl;
		} else {
			cerr << endl;
			cerr << "INPUT: " << originalInput << endl;
			cerr << "MATCH PATH: " << m->getPath() << endl;
			cerr << "FILENAME: " << m->getNode()->getActualTemplate()->getFilename() << endl;
			
			string tmpl = "<template>" + m->getTemplate() + "</template>";
			strstream ss;
			ss << tmpl << endl;
			
			SaxParser *p = new SaxParser(new Parser());
			p->parse(ss);
			
			currentResponse = Kernel::process(m, ((Parser *)p->getListener())->getRoot(), r, id);
			
			predicates->setValue("beforethat", id, that);
			predicates->setValue("that", id, currentResponse);
			
			delete p;
		}
		if (m != NULL) {
			delete m;
			if (srai) {
				--recursionDepth;
				return currentResponse;
			} else {
				buffer += currentResponse + " ";
			}
		}
	}
	string result = Substituter::substitute(buffer, "output");
	--recursionDepth;
	if (!srai) {
		timingResponse = timerMillis() - timingResponse;
		cerr << "TIME: " << timingResponse << "ms" << endl;
	}
	//--	DEBUGGING LINE
	string ktw = result + "\n";
	getStream("Kernel")->Write(ktw.c_str());
	if (trimming) {
		return trim(result, " \t\r\n");
	}
	return result;
}
