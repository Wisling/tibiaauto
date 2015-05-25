/**
 * Kernel - Brains of the operation
 *
 * @author	Jonathan Roewen
 */
#ifndef KERNEL_H
#define KERNEL_H

#include <string>

using namespace std;

#include "Element.h"
#include "PredicateEngine.h"

class Match;
class Responder;
class Nodemaster;
class StringTokenizer;

class Kernel
{
public:
	Kernel();
	~Kernel();

	static string bootstrap();
	static bool load(const string &file);
	static void load10(const string &file, PElement &root);
	static void loadSubstitutions();
	static Nodemaster * add(const string &context, const string &pattern, const string &that, const string &topic);
	static Match *match(const string &context, const string &input, const string &that, const string &topic);
	static Match *match(Nodemaster *node, Nodemaster *parent, int state, const string &input, const string &star, const string &path);
	static string process(Match *m, PElement e, Responder *, const string &id);
	static string respond(const string &input, const string &id, const string &prefix = "");
	static string respond(const string &input, const string &id, bool srai, const string &prefix = "");
	static string respond(const string &input, const string &id, Responder *, bool srai = false, const string &prefix = "");
	static string respond(const string &input, const string &id, int depth, bool srai, const string &prefix = "");
	static string respond(const string &input, const string &id, Responder *, int depth, bool srai, const string &prefix = "");

	static void convertTempAiml();

	static PredicateEngine *predicates;
private:
	static void addStar(Match *, const string &, int);
	static void addPath(Match *, const string &, int);
	static bool lookup(const string &, const string &);
	static void loadTemporaryData();

	static Nodemaster *root;
	static const int CONTEXT, INPUT, THAT, TOPIC;
	static const string constants;
};

#endif // ifndef KERNEL_H
