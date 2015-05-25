#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

#include <map>
#include <string>

using namespace std;

class Socket;

class SocketHandler {
public:
	static void runLoop();
	static void addSocket(string uid, Socket *s);
	static void removeSocket(string uid);
private:
	static map<string, Socket *> sockets;
	static bool alreadyRunning;
	friend class Socket;
};

#endif // ifndef SOCKET_HANDLER_H
