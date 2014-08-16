/**
 * IrcResponder - Interacts with users via IRC
 *
 * @author	Jonathan Roewen
 */
#ifndef IRC_RESPONDER_H
#define IRC_RESPONDER_H

#include "Responder.h"
#include "Socket.h"
#include "ServerSocket.h"
#include "Config.h"

#include <queue>

using namespace std;

class IrcPassThru;

class IrcResponder : public Responder, SocketListener
{
public:
	IrcResponder(IrcConfig &);
	~IrcResponder();
	
	string respond(Match *, PElement, const string &);
	
	void execute();
	void processAlias(const string &);
	bool changeNick();
	
	void connected();
	void disconnected(const string &);
	void processLine(string &);
	void sendCommand(const string &);
	void recv(string &);	//	From SocketListener
	void send(const string &, bool fromIPT = false);
	void connect();
	void disconnect();
	void disconnected();
private:
	string nick, buffer, mNick, mChannel, pass;
	Socket *sock;	//	connects to remote server
	queue<string> q;
	IrcConfig config;
	int nickIndex;
	unsigned int retryCount;
	string serverHost;
	bool botControl;
	bool gotMotd;
	
	void start(int delay);
	
	friend class IrcPassThru;
	
	IrcPassThru *ipt;
};

#endif
