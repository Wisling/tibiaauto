/**
 * Match - The foundation of matches
 *
 * @author	Jonathan Roewen
 */
#ifndef MATCH_H
#define MATCH_H

#include "Element.h"

#include <string>
#include <vector>

using namespace std;

class Nodemaster;

class Match
{
public:
	void addContextStar(const string &s);
	void addInputStar(const string &s);
	void addThatStar(const string &s);
	void addTopicStar(const string &s);
	void setContextPattern(const string &s);
	void setInputPattern(const string &s);
	void setThatPattern(const string &s);
	void setTopicPattern(const string &s);
	string getContextPattern() const;
	string getInputPattern() const;
	string getThatPattern() const;
	string getTopicPattern() const;
	void setPath(const string &s);
	string getPath() const;
	void setNode(Nodemaster *n);
	Nodemaster *getNode() const;
	string getTemplate() const;
	string getContextStar(unsigned int i) const;
	string getInputStar(unsigned int i) const;
	string getThatStar(unsigned int i) const;
	string getTopicStar(unsigned int i) const;
	unsigned int getContextStarCount() const;
	unsigned int getInputStarCount() const;
	unsigned int getThatStarCount() const;
	unsigned int getTopicStarCount() const;
private:
	vector<string> contextStar, inputStar, thatStar, topicStar;
	string contextPattern, inputPattern, thatPattern, topicPattern;
	string path;
	Nodemaster *node;
};

#endif // ifndef MATCH_H
