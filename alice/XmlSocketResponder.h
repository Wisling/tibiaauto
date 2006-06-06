#ifndef XML_SOCKET_RESPONDER_H
#define XML_SOCKET_RESPONDER_H

#include "Responder.h"
#include "AimlProcessor.h"
#include "ServerSocket.h"
#include "Socket.h"
#include "Parser.h"

using namespace std;

class XmlSocketResponder : public Responder, public SocketListener
{
public:
	XmlSocketResponder(Socket *);
	virtual ~XmlSocketResponder() {
	}
	void send(const string &);
	//	Inherited from Responder
	string respond(Match *, PElement, const string &);
	//	Inherited from SocketListener
	void recv(string &);
	void connected();
	void disconnected(const string &);
private:
	string buffer;
	Socket *client;
	string botName;
};

class XmlSocketProcessor : public AimlProcessor,
		public ServerSocketListener
{
public:
	XmlSocketProcessor();
	virtual ~XmlSocketProcessor() { }
	//	Inherited from AimlProcessor
	string getName() const {
		return "xmlsocket";
	}
	string getVersion() const {
		return "1.0";
	}
	string process(Match *, PElement, Responder *, const string &);
	//	Inherited from ServerSocketListener
	void shutdown(const string &);
	void awaitingClient(Socket *);
private:
	int port;
	ServerSocket *server;
};

#endif	//	XML_SOCKET_RESPONDER_H
