#include <stdafx.h>

#include "Socket.h"

#include "Logger.h"

#ifdef WIN32
// #include <winsock2.h>
#else
 #include <sys/types.h>
 #include <netdb.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
#endif
//#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "SocketHandler.h"

#include <iostream>

#include "Compat.h"
#include "Utils.h"
#include "Stream.h"

using namespace std;

#ifdef WIN32
	extern "C" {
		void win32_init_sockets(void);
		void win32_cleanup_sockets(void);
		
		void win32_init_sockets(void) {
			WSADATA d;
			int err;
			
			err = WSAStartup(MAKEWORD(2, 0), &d);
			if (err != 0) {
				//	Notify user of failure
				cerr << "This application requires Windows Sockets 2.0" << endl;
				cerr << "Please contact your system administrator, or install" << endl;
				cerr << "an updated version." << endl;
				return;
			}
			
			if (LOBYTE(d.wVersion) != 2 || HIBYTE(d.wVersion) != 0) {
				//	Tell user that Winsock2 version is incompatible
				cerr << "The version of Winsock2 doesn't not support this" << endl;
				cerr << "application. Required version is 2.0." << endl;
				return;
			}
			
			//	Otherwise Winsock2 DLL is fine
		}
		
		void win32_cleanup_sockets(void) {
			WSACleanup();
		}
	}
#endif

string RECV = "recv: ";
string SEND = "sent: ";

Socket::Socket(int socket, const char* name, int p):listener(NULL), sd(socket),
		port(p) {
	//	Needed for inheritance
	this->host = mystrdup(name);
	#ifdef WIN32
		win32_init_sockets();
	#endif
}

Socket::Socket(const string &host, const int &port) {
	this->host = mystrdup(host.c_str());
	this->port = port;
	#ifdef WIN32
		win32_init_sockets();
	#endif
	listener = NULL;
	sd = -1;
	
	struct sockaddr_in sa;
	
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		disconnect("socket() failed");
		return;
	}
	
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(0);
	memset(sa.sin_zero, 0, sizeof(sa.sin_zero));
	
	if (bind(sd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		//	This isn't lethal .. can still continue
		perror("bind");
	}
}

Socket::~Socket() {
	disconnect();
	//delete [] host;
	host = NULL;
	listener = NULL;
	#ifdef WIN32
		win32_cleanup_sockets();
	#endif
}

const int buffer_size = 5000;

void Socket::connect() {
	if (sd < 0) {
		//	Constructing the socket failed previously
		if (listener != NULL) {
			listener->disconnected("couldn't create socket");
			return;
		}
	}
	struct sockaddr_in sa;
	struct hostent *he;
	
	he = gethostbyname(host);
	if (he == NULL) {
		Logger::log(string(host) + " could not be found [unknown host]", "networking");
		disconnect(string(string(host) + " unknown").c_str());
		return;
	}
	
	sa.sin_family = he->h_addrtype;
	memcpy((char *)&sa.sin_addr.s_addr, he->h_addr_list[0], he->h_length);
	sa.sin_port = (u_short)htons(port);
	memset(sa.sin_zero, 0, sizeof(sa.sin_zero));
	
	if (::connect(sd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		Logger::log("connection to " + string(host) + " failed", "networking");
		disconnect(string("failed connect to "+ string(host)).c_str());
		return;
	}
	if (listener != NULL) {
		listener->connected();
	}
}

void Socket::disconnect(const char *reason) {
	string out = "";
	if (reason != NULL) {
		out = reason;
	}
	if (sd != -1) {
		closesocket(sd);
		sd = -1;
		//	This should allow the service to disconnect
		//	without causing any dependency (ie: reason
		//	is NULL).
		if (listener != NULL && out.length() > 0) {
			//--	some reason, listener is invalid...
			listener->disconnected(out);
		}
	}
	listener = NULL;
}

void Socket::setListener(SocketListener *listener) {
	this->listener = listener;
}

SocketListener *Socket::getListener() {
	return listener;
}

int Socket::getSD() {
	return sd;
}

void Socket::process() {
	//	This gets called from SocketHandler when we
	//	have something waiting for us (data)
	string str;
	read(str);
}

int Socket::read(string &str) {
	int n = 0;
	int buf_size = buffer_size;
	if (buf_size > 1) {
		--buf_size;
	}
	char *buffer = new char[buffer_size];
	buffer = (char *)memset(buffer, 0, buffer_size);
	n = read_raw(buffer, buf_size);
	if (n > 0) {
		buffer[n] = '\0';
		str = string(buffer);
		//Logger::log(RECV + str, "sio.log", false);
		#ifdef GUI
		getStream(host)->Read(str.c_str());
		#endif
		if (listener != NULL) {
			listener->recv(str);
		}
	}/* else {
		Logger::log(RECV + ": no data", "sio.log", false);
	}*/
	delete [] buffer;
	return n;
}

int Socket::write(const string &str, bool appendNullByte) {
	int n = write_raw(str.c_str(), str.length());
	//Logger::log(SEND + str, "sio.log", false);
	#ifdef GUI
	getStream(host)->Write(str.c_str());
	#endif
	if (appendNullByte && n >= 0) {
		char ch = '\0';
		return n + write_raw(&ch, 1);
	}
	return n;
}

int Socket::read_raw(void *buffer, int buf_size) {
	int n = 0;
	if (sd < 0) {
		return n;
	}
	
	#ifdef WIN32
		n = recv(sd, (char FAR *)buffer, buf_size, 0);
	#else
		n = recv(sd, (char *)buffer, buf_size, 0);
	#endif
	if (n < 0) {
		if (errno != EWOULDBLOCK) {
			perror("recv");
			disconnect("error receiving data");
		}
	} else if (n == 0) {
		disconnect("connection closed by peer");
	}
	return n;
}

int Socket::write_raw(const void *data, int len) {
	int ret;
	if (sd == -1) {
		return 0;
	}
	#ifdef WIN32
		ret = send(sd, (char FAR *)data, len, 0);
	#else
		ret = send(sd, (const char *)data, len, 0);
	#endif
	if (ret < 0){
		perror("send");
		disconnect("connection terminated...");
	}
	return ret;
}
