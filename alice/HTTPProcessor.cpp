#include <stdafx.h>

#include "HTTPProcessor.h"

#include "ServerSocket.h"
#include "Socket.h"
#include "SocketHandler.h"

#include <stdlib.h>
#include <string>

#include "Kernel.h"
#include "TokenProxyListener.h"
#include "Logger.h"
#include "Config.h"

#include "Responder.h"

#include "Utils.h"
#include "StringTokenizer.h"
#include "Stream.h"

#include <iostream>

using namespace std;

string templateContents;

class HTTPListener : public SocketListener, Responder {
public:
	HTTPListener(Socket *s) {
		client = s;
		int n = ++numClients * 25;
		while (n > 0) {
			char ch = (char)(n % 10) + '0';
			uid = ch + uid;
			n /= 10;
		}
		uid = "http client " + uid;
		SocketHandler::addSocket(uid, client);
	}
	virtual ~HTTPListener() {
	}
	void recv(string &s);
	void disconnected(const string &) {
		//	Needed here as well, for cases when recv() doesn't get called...
		SocketHandler::removeSocket(uid);
		--HTTPListener::numClients;
	}
	Socket *getSocket(){ return client;}
	void connected() {
		;//cout << "Client connected" << endl;
	}
	string respond(Match *, PElement e, const string &) {
		if (e->getNamespace() == "http") {
			string tag = e->getTagname();
			if (tag == "quit") {
				serverRunning = false;	//	Natural quit
			}
		}
		return "";
	}
private:
	Socket *client;
	string uid;
	static int numClients;
};

string HTTPProcessor::process(Match *, PElement, Responder *, const string &){
/*	string name = Kernel::process(m, e->getChild("name"), r, id);
	int port = atoi(Kernel::process(m, e->getChild("port"), r, id).c_str());
	
	ServerSocket *server = new ServerSocket(port);
	SocketHandler::addSocket(name, server);
	(new HTTPServer(server))->setUID(name);*/
	return "starting web server from AIML is currently unsupported";
}

HTTPServer::HTTPServer() {
	server = new ServerSocket(httpConfig.port);
	SocketHandler::addSocket("Web Server", server);
	ifstream fin(httpConfig.templateFile.c_str());
	if (fin.is_open()) {
		string line;
		while (!fin.eof()) {
			getline(fin, line);
			templateContents += line + "\n";
		}
		fin.close();
	}
	server->setServerListener(this);
	if (server->init()) {
		char str[1024];
		sprintf(str, "Starting up web server (listening on port %d)\n", httpConfig.port);
		getStream("Console")->Write(str);
	}	//	No else - will display a shutdown message via server->init()
}

void HTTPServer::shutdown(const string &msg) {
	cout << "Shutting down server: " << msg << endl;
	string err = "Shutting down server: " + msg + "\n";
	getStream("Console")->Write(err.c_str());
}

void HTTPServer::awaitingClient(Socket* socket) {
	//cout << "HTTPServer::awaitingClient(" << socket->getSD() <<  ")" << endl << flush;
	socket->setListener(new TokenProxyListener(new HTTPListener(socket)));
//	socket->process();
}

int HTTPListener::numClients = 0;

void HTTPListener::recv(string &s) {
	//--	A proper GET request for J-Alice:
	//--	MUST have an "input" field.
	//--	Optionally, an "id" field to use instead of the
	//--	client's IP/Hostname.
	string input;
	string id;
	if (s.find("GET") == 0) {
		//--	This is the GET header
		string::size_type ix = s.find("?");
		string::size_type iy = s.find(" ", ++ix);
		string request = s.substr(ix,iy-ix);
		StringTokenizer st(request, "&");
		while (st.hasMoreTokens()) {
			string tok = st.nextToken();
			if (tok.find("input=") == 0) {
				tok.erase(0, 6);
				input = tok;
			} else if (tok.find("id=") == 0) {
				tok.erase(0,3);
				id = tok;
			}
		}
		if (input.empty()) {
			input = "CONNECT";
		} else {
			input = urldecode(input);
		}
		if (id.empty()) {
			id = client->getPeerName();
		} else {
			id = urldecode(id);
		}
	} else {
		return;
	}
	client->write("HTTP/1.0 200 OK\r\n");
	client->write("Server: J-Alice Web Server\r\n");
	client->write("Content-Type: text/html; charset=utf-8\r\n");
	client->write("\r\n");
	string response = Kernel::respond(input, id, this);
	string str;
	if (templateContents.length() == 0) {
		str = "<html>";
		str += "<head><title>I am J-Alice!</title></head>";
		str += "<body>";
		str += "<h1>J-Alice Web Server</h1><br>";
		str += botName;
		str += ": ";
		str += response;
		str += "<p>";
		str += "<form>";
		str += "Response: <input type=text length=255 name=input> <input type=submit title=Send text=Send>";
		str += "</form>";
		str += "</p>";
		str += "</body>";
		str += "</html>";
		str += "\r\n";
	} else {
		str = templateContents;
		const string tag = "<bot output>";
		string::size_type ix = str.find(tag);
		while (ix != string::npos) {
			string tail = str.substr(ix + tag.length());
			str = str.substr(0, ix) + response + tail;
			ix = str.find(tag);
		}
	}
	client->write(str.c_str());
	
	Logger::log(id + " > " + input + "\nbot > " + response, "http-server");
	SocketHandler::removeSocket(uid);
	--HTTPListener::numClients;
}
