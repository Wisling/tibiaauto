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
		for (size_t ix = 0; ix < filenames.size(); ++ix) {
			if (filenames[ix] == fn) {
				filenameIx = ix;
				return;
			}
		}
		filenames.push_back(fn);
		ifstream fin;
		fin.open(fn.c_str(), ios::binary);
		fin.seekg(0, ios::end);
		streamoff s = fin.tellg();
		filesizes.push_back((unsigned long)s);
		filenameIx = filenames.size() - 1;
	}
	string getFilename();
	static void reloadFile(int index);
	int filenameIx;
	
	string fetch();
private:
	static const char *getFilename(int index);
	
	static vector<string> filenames;
	static vector<unsigned long> filesizes;
	streamsize start, end;
//	string contents;
};
#endif

