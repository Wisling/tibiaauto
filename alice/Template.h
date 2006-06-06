#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <fstream>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

class Template
{
public:
	Template(const string &fn, streamsize st, streamsize ed)
		: start(st), end(ed) {
		for (int ix = 0; ix < filenames.size(); ++ix) {
			if (filenames[ix] == fn) {
				filenameIx = ix;
				return;
			}
		}
		filenames.push_back(fn);
		ifstream fin;
		fin.open(fn.c_str(), ios::binary);
		fin.seekg(0, ios::end);
		int s = fin.tellg();
		filesizes.push_back(s);
		filenameIx = filenames.size() - 1;
	}
	string getFilename();
	static void reloadFile(int index);
	int filenameIx;
	
	string fetch();
private:
	static const char *getFilename(int index);
	
	static vector<string> filenames;
	static vector<long> filesizes;
	streamsize start, end;
//	string contents;
};
#endif

