#ifndef TOKEN_PROXY_LISTENER_H
#define TOKEN_PROXY_LISTENER_H

#include "Socket.h"

class TokenProxyListener : public SocketListener
{
public:
	TokenProxyListener(SocketListener *, const string &token = "\n");
	virtual ~TokenProxyListener()
	{
	}

	void recv(string &);
	void disconnected(const string &msg)
	{
		if (forward != NULL)
			forward->disconnected(msg);
	}

	void connected()
	{
		if (forward != NULL)
			forward->connected();
	}

private:
	string separator, buffer;
	SocketListener *forward;
};

#endif // ifndef TOKEN_PROXY_LISTENER_H
