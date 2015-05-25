#include <stdafx.h>

#include "TokenProxyListener.h"

#include <iostream>

using namespace std;

TokenProxyListener::TokenProxyListener(SocketListener *sl, const string &token) {
	separator = token;
	forward   = sl;
}

void TokenProxyListener::recv(string &str) {
	//	This overrides SocketListener::recv(string &);
	string::size_type index;
	buffer += str;
	while ((index = buffer.find(separator)) != string::npos)
	{
		string substring = buffer.substr(0, index);
		//	Omit the separator...
		buffer = buffer.erase(0, index + separator.length());
		forward->recv(substring);
	}
}
