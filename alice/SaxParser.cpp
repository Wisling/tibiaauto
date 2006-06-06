#include <stdafx.h>

#include "SaxParser.h"

#include "Utils.h"
#include "StringTokenizer.h"

using namespace std;

void SaxParser::parse(istream &in) {
	char buf;
	bool addedSpace = false;
	bool insideTag = false;
	
	vector<char> vBuffer;
	vBuffer.reserve(10000);

	istreambuf_iterator<char> itr(in);
	istreambuf_iterator<char> eos;
	
	for (int i = 0; itr != eos; ++itr, ++i) {
		buf = *itr;
		
		switch (buf) {
		case '\0':
			//	I don't know why, but...
			itr = eos;
			continue;
		case '\n':
		case '\r':
		case '\t':
		//	This space stuff is pissing me off!
		//	if (!insideTag && !addedSpace) {
		//		vBuffer.push_back(' ');
		//		addedSpace = true;
		//	}
			continue;
		case '<':
			insideTag = true;
			break;
		case '>':
			insideTag = false;
		}
		
		vBuffer.push_back(buf);
		
		switch (buf) {
		case '-':
			if (vBuffer.size() == 4 && string(vBuffer.begin(), vBuffer.end()) == "<!--") {
				//	Then skip comment
				++itr; ++i;
				while (itr != eos) {
					buf = *itr;
					vBuffer.push_back(buf);
					if (vBuffer.size() >= 3 && string(vBuffer.end() - 3, vBuffer.end()) == "-->") {
						break;
					}
					++itr;
					++i;
				}
				string str(vBuffer.begin(), vBuffer.end());
				vBuffer.clear();
				continue;
			}
			break;
		case 'l':
		case 'E':
			if (
				(vBuffer.size() == 5 && string(vBuffer.begin(), vBuffer.end()) == "<?xml")
			||	(vBuffer.size() == 9 && string(vBuffer.begin(), vBuffer.end()) == "<!DOCTYPE")
			) {
				//	Then skip doctype or xml declaration
				++itr; ++i;
				while (itr != eos) {
					buf = *itr;
					if (buf == '>') {
						break;
					}
					++itr;
					++i;
				}
				vBuffer.clear();
				vBuffer.push_back('>');
				continue;
			}
			break;
		case '[':
			if (vBuffer.size() == 9 && string(vBuffer.begin(), vBuffer.end()) == "<![CDATA[") {
				//	Then build up cdata section
				vBuffer.clear();
				++itr; ++i;
				while (itr != eos) {
					buf = *itr;
					vBuffer.push_back(buf);
					if (buf == '>' && string(vBuffer.end() - 3, vBuffer.end()) == "]]>") {
						string s(vBuffer.begin(), vBuffer.end() - 3);
						listener->elementCData(s, i);
						break;
					}
					++itr;
					++i;
				}
				vBuffer.clear();
				continue;
			}
			break;
		case '>':
			if (vBuffer[0] == '<') {
				addElement(vBuffer, i);
				vBuffer.clear();
				addedSpace = false;
				continue;
			}
			break;
		case '<':
			if (vBuffer.size() > 1) {
				//	Then have text and tag is starting
				string s(vBuffer.begin(), vBuffer.end() - 1);
				listener->elementText(s, i);
				vBuffer.clear();
				vBuffer.push_back('<');
				addedSpace = false;
				continue;
			}
			break;
		}
	}
}

void SaxParser::addElement(vector<char> &vb, int index) {
	++index;
	vector<char>::iterator front, back, temp;
	front = vb.begin();
	back = vb.end();
	
	front++; back--;	//	remove '<' and '>'
	
	if (*front == '/') {
		//--	closing tag
		listener->elementFinished(string(front + 1, back), index);
		return;
	}
	for (temp = front; temp != back; ++temp) {
		if (*temp == ' ') {
			//--	attributes
			string tagName(front, temp);
			attributeMap am;
			bool selfclosing = false;
			if (*(--back) == '/') {
				selfclosing = true;
			} else {
				++back;
			}
			if (++temp >= back || temp == vb.end()) {
				front = vb.begin() + 1;
				back = vb.end() - 1;
				break;
			}
			string attributes(temp, back);
			string aPair, name = "";
			StringTokenizer pairs(attributes, "=");
			while (pairs.hasMoreTokens()) {
				aPair = trim(pairs.nextToken());
				if (name.empty()) {
					name = aPair;
				} else {
					if (aPair[0] == '\'') {
						StringTokenizer pair(trim(aPair, "'"), "'");
						am[name] = pair.nextToken();
						name = trim(pair.nextToken());
					} else {
						StringTokenizer pair(trim(aPair, "\""), "\"");
						am[name] = pair.nextToken();
						name = trim(pair.nextToken());
					}
				}
			}
			listener->elementStarted(tagName, &am, index);
			
			if (selfclosing) {
				listener->elementFinished(tagName, index);
			}
			return;
		}
	}
	//--	no attributes
	if (*(--back) == '/') {
		//--	self-closing
		string name(front,back);
		listener->elementStarted(name, index);
		listener->elementFinished(name, index);
		return;
	} else {
		listener->elementStarted(string(front,back + 1), index);
		return;
	}
}