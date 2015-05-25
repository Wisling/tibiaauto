#include <stdafx.h>

/**
 * Before using Template, even though slower, I think used less
 * memory without this class .. can't remember.
 */
#include "Template.h"

#include "Kernel.h"

//	Can't do the auto-reloading from inside the Template function .. it's just
//	plain wrong!
//	Need a static function to reload the AIML (if required), then forward the
//	request on to the actual Template .. somehow :P

vector<string> Template::filenames;
vector<unsigned long> Template::filesizes;

string Template::fetch() {
	//	Get the filename ..
	const char *filename = Template::getFilename(filenameIx);
	//	When have <template/>
	if (end <= start)
		return "";
	streamsize length = end - start;
	ifstream fin(filename, ios::in | ios::binary);
	char *input = new char[(unsigned int)(length) + 1];
	if(!fin.is_open())
		return "Error reading file";
	fin.seekg(start);
	fin.read(input, length);
	fin.close();
	input[length] = '\0';
	string ret(input);
	delete [] input;
	return ret;
}

string Template::getFilename() {
	return filenames[filenameIx];
}

const char *Template::getFilename(int index) {
	return filenames[index].c_str();
}

void Template::reloadFile(int index) {
	string fn = filenames[index];
	long fs   = filesizes[index];
	ifstream fin;
	fin.open(fn.c_str(), ios::in | ios::binary);
	if (!fin.is_open())
	{
		cerr << "Severe internal error" << endl;
		return;
	}
	fin.seekg(0, ios::end);
	streamoff s = fin.tellg();
	fin.close();
	if (s != fs)
	{
		//	Reload .. return true
		filesizes[index] = (unsigned long)s;
		Kernel::load(fn);
	}
}
