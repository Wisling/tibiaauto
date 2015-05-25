#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <vector>

using namespace std;

typedef vector<unsigned char> bvector;

class SocketListener {
public:
	virtual void connected()                  = 0;
	virtual void disconnected(const string &) = 0;
	virtual void recv(string &)               = 0;
};

class Socket {
public:
	/** constructor for use by creating a class for existing sockets
	   this expects u to allocate mem for the char* name since this socket is getting
	   created from a predefined thing*/
	Socket(int socket = -1, const char* name = NULL, int port = 0);
	Socket(const string &server, const int &port);
	virtual ~Socket();

	void connect();
	void disconnect(const char *reason = NULL);

	void setListener(SocketListener *);
	SocketListener *getListener();
	int getSD();
	const char* getPeerName()
	{
		return host;
	}

	virtual void process();

	int read(string &);
	int read(bvector &vBuf, int nCount = 1048);

	int write(const string &, bool appendNullByte = false);
	int write(bvector &vBuf, int nStart = 0, int nCount = 0);
protected:
	int read_raw(void *, int);
	int write_raw(const void *, int);

	SocketListener *listener;

	int sd, port;
	const char *host;
};

#endif // ifndef SOCKET_H
