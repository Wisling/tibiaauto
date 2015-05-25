#include <stdafx.h>

#include "Kernel.h"
#include "Utils.h"
#include "Config.h"

#include "SocketHandler.h"

#include "IrcResponder.h"
#include "HTTPProcessor.h"
#include "Stream.h"
#include "Main.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <strstream>
#include <iterator>
#include <algorithm>
#include <stdio.h>

using namespace std;

long totalTime = 0;

bool serverRunning = true;

Kernel  *kernel = NULL;

bool bootstrap        = true;
bool convert          = false;
bool debug            = false;
bool trace            = false;
int maxRecursiveDepth = 32;
string botName        = "J-Alice";
string connectPattern = "CONNECT";
string noMatchPattern = "NOMATCH";
string loopPattern    = "LOSTINLOOP";

vector<IrcConfig>       ircConfigs;
HttpConfig httpConfig;
XmlRpcConfig xmlrpcConfig;
AdminConfig adminConfig;

void loadConfig();
void run();
void clean(string &);
void process(string &, int, string &, string &);

int init_jalice(int argc, char **argv)
{
	getStream("Console")->Write("Initializing J-Alice\n");
	bool bootstrap = true;
	bool convert   = false;

	kernel = new Kernel();

	loadConfig();

	if (argc > 0)
	{
		string s = argv[argc - 1];
		if (s == "--nobootstrap")
		{
			bootstrap = false;
		}
		else if (s == "--bootstrap")
		{
			bootstrap = true;
		}
		else if (s == "--convert")
		{
			bootstrap = false;
			convert   = true;
		}
	}

	if (bootstrap)
	{
		string bs = kernel->bootstrap() + "\n";
		getStream("Console")->Write(bs.c_str());
	}
	if (convert)
	{
		kernel->convertTempAiml();
		return 0;
	}
	char str[1024];
	sprintf(str, "Total time to startup: %dms\n", totalTime);
	getStream("Console")->Write(str);

	return 0;
}

#ifndef GUI
int main(int argc, char **argv)
{
	init_jalice(argc, argv);

	run_jalice();
	cleanup_jalice();

	cout << "\nJ-Alice has been shutdown" << endl;
}

#endif // ifndef GUI

void clean(string &input)
{
	string::size_type ix = input.find_first_of(";\n\t\r");
	if (ix == string::npos)
		return;
	input = input.erase(ix);
	ix    = input.find_last_not_of(" ");
	input = input.erase(ix + 1);
}

void loadConfig()
{
	ifstream fin("j-alice.ini", ios::binary | ios::in);
	if (!fin.is_open())
		return;
	string line, section, subsection, property, value;
	int ircsection = -1;
	while (!fin.eof())
	{
		getline(fin, line);
		clean(line);
		if (line.length() > 0)
		{
			if (line[0] == '[')
			{
				section    = line.substr(1, line.length() - 2);
				subsection = "";
			}
			else if (line[0] == '-')
			{
				subsection = line.substr(2, line.length() - 4);
				//	Since sub-section currently only applies to IRC
				//	take a short-cut ;-)
				ircsection++;
				ircConfigs.push_back(IrcConfig());
				ircConfigs[ircsection].description = subsection;
			}
			else
			{
				string::size_type st = line.find("=");
				property = line.substr(0, st);
				value    = line.substr(st + 1);
				process(section, ircsection, property, value);
			}
		}
	}
	fin.close();
}

void process(string &s, int ss, string &p, string &v)
{
	if (p == "MaxRecursiveDepth")
	{
		maxRecursiveDepth = atoi(v.c_str());
	}
	else if (p == "BootstrapEnabled")
	{
		bootstrap = v == "true";
	}
	else if (p == "BotName")
	{
		botName = v;
	}
	else if (p == "ConnectPattern")
	{
		connectPattern = v;
	}
	else if (p == "NoMatchPattern")
	{
		noMatchPattern = v;
	}
	else if (p == "LoopPattern")
	{
		loopPattern = v;
	}
	else if (p == "ShowDebug")
	{
		debug = v == "true";
	}
	else if (p == "ShowExtendedInfo")
	{
		trace = v == "true";
	}
	else if (p == "Enabled")
	{
		bool enabled = v == "true";
		if (s == "IRC")
			ircConfigs[ss].enabled = enabled;
		else if (s == "Web Server")
			httpConfig.enabled = enabled;
		else if (s == "XML-RPC Server")
			xmlrpcConfig.enabled = enabled;
	}
	else if (p == "Host")
	{
		ircConfigs[ss].host = v;
	}
	else if (p == "Port")
	{
		int port = atoi(v.c_str());
		if (s == "IRC")
			ircConfigs[ss].port = port;
		else if (s == "Web Server")
			httpConfig.port = port;
		else if (s == "XML-RPC Server")
			xmlrpcConfig.port = port;
		else if (s == "Admin Web Server")
			adminConfig.port = port;
	}
	else if (p == "Nicks")
	{
		strstream nicks;
		nicks << v;
		string str;
		while (!nicks.eof())
		{
			getline(nicks, str, ',');
			ircConfigs[ss].nicks.push_back(trim(str));
		}
	}
	else if (p == "Channels")
	{
		strstream channels;
		channels << v;
		string str;
		while (!channels.eof())
		{
			getline(channels, str, ',');
			ircConfigs[ss].channels.push_back(trim(str));
		}
	}
	else if (p == "PatternPrefix")
	{
		if (s == "IRC")
			ircConfigs[ss].patternPrefix = v;
		else if (s == "Web Server")
			httpConfig.patternPrefix = v;
		else if (s == "XML-RPC Server")
			xmlrpcConfig.patternPrefix = v;
	}
	else if (p == "AutoReconnect")
	{
		bool ar = v == "true";
		ircConfigs[ss].autoReconnect = ar;
	}
	else if (p == "MaxRetries")
	{
		int retries = atoi(v.c_str());
		ircConfigs[ss].maxRetries = retries;
	}
	else if (p == "TemplateFile")
	{
		httpConfig.templateFile = v;
	}
	else if (p == "StartupPattern")
	{
		if (s == "IRC")
			ircConfigs[ss].startupPattern = v;
		else if (s == "Web Server")
			httpConfig.startupPattern = v;
		else if (s == "XML-RPC Server")
			xmlrpcConfig.startupPattern = v;
	}
	else if (p == "Username")
	{
		adminConfig.username = v;
	}
	else if (p == "Password")
	{
		adminConfig.password = v;
	}
	else if (p == "PassThruPort")
	{
		if (s == "IRC")
			ircConfigs[ss].ptPort = atoi(v.c_str());
	}
}

void run_jalice()
{
	//	All enabled IRC Clients:
	for (unsigned int ix = 0; ix < ircConfigs.size(); ++ix)
	{
		if (ircConfigs[ix].enabled)
			new IrcResponder(ircConfigs[ix]);
	}

	//	The Web Server (if enabled)
	if (httpConfig.enabled)
		new HTTPServer();

	//	The XML-RPC Server (if enabled)
	if (xmlrpcConfig.enabled)
	{
//		ServerSocket *xmlrpc = new ServerSocket(xmlrpcConfig.port);
//		SocketHandler::addSocket("XML-RPC Server", xmlrpc);
		//XmlRpcServer(xmlrpc);
	}

	//	Start the SocketHandler loop
	SocketHandler::runLoop();
	//	Once this loop has finished, bot will shutdown
}

void cleanup_jalice()
{
	serverRunning = false;
	getStream("Console")->Write("Please wait while J-Alice terminates...");
	if (kernel != NULL)
	{
		delete Kernel::predicates;
		getStream("Console")->Write("Saved predicates");
		delete kernel;
		getStream("Console")->Write("Engine stopped");
		kernel = NULL;
	}
}
