#ifndef CONFIG_H
#define CONFIG_H

#include "Kernel.h"

#include <vector>
#include <string>

using namespace std;

extern Kernel  *kernel;

extern bool bootstrap;
extern bool convert;
extern bool debug;
extern bool trace;
extern int maxRecursiveDepth;
extern string botName;
extern string connectPattern;
extern string noMatchPattern;
extern string loopPattern;

typedef struct {
	bool enabled;
	string host;
	short port;
	vector<string>  nicks;
	vector<string>  channels;
	string patternPrefix;
	bool autoReconnect;
	int maxRetries;
	string startupPattern;
	string description;
	short ptPort;
} IrcConfig;

typedef struct {
	bool enabled;
	short port;
	string templateFile;
	string patternPrefix;
	string startupPattern;
} HttpConfig;

typedef struct {
	bool enabled;
	short port;
	string patternPrefix;
	string startupPattern;
} XmlRpcConfig;

typedef struct {
	short port;
	string username;
	string password;
} AdminConfig;

extern vector<IrcConfig>       ircConfigs;
extern HttpConfig httpConfig;
extern XmlRpcConfig xmlrpcConfig;
extern AdminConfig adminConfig;

#endif  //	CONFIG_H
