#ifndef HTTPPRCCESSOR_H
#define HTTPPRCCESSOR_H

#include <string>

#include "AimlProcessor.h"

#include "ServerSocket.h"
#include "Socket.h"

class HTTPProcessor : public AimlProcessor{
public:
	string process(Match *, PElement, Responder *, const string &);

};

class HTTPServer : ServerSocketListener {
public:
	HTTPServer();
	virtual ~HTTPServer() { }
	void shutdown(const string &);
	void awaitingClient(Socket *);
private:
	ServerSocket *server;
};

#endif //HTTPPRCCESSOR_H

