#include <stdafx.h>

#include "ServerSocket.h"
#include <iostream>

#ifdef WIN32
// #include <winsock2.h>
#else
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netdb.h>
 #include <netinet/in.h>
 #if !defined (__BEOS__)
  #include <arpa/inet.h>
 #endif
#endif
//#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>


using namespace std;

#include "SocketHandler.h"
#include "Compat.h"

//should this become a macro?
static bool set_non_block(int);
#if defined (__BEOS__)
	bool set_non_block(int socket){
		//gets set in SOCK_FLAGS
		return true;
	}

#elif defined (WIN32)
	bool set_non_block(int socket){
		u_long FAR one = 1;
		if (ioctlsocket(socket, FIONBIO, &one) != 0) {
			perror("ioctlsocket");
			return false;
		}
		return true;
	}
#else
	bool set_non_block(int socket){
		//heh nice & messy, but now i dont have to declare a variable
		if (fcntl(socket,F_SETFL,fcntl(socket,F_GETFL) | O_NONBLOCK) < 0) {
			perror("fcntl(F_SETFL)");
			return false;
		}
		return true;
	}

#endif

ServerSocket::ServerSocket(const int &port) {
	this->port = port;
	sd = -1;
	serverListener = NULL;
	
	#if defined (WIN32)
		char FAR one = 1;
	#else
		socklen_t one = 1;
	#endif
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		sd = -1;
		return;
	}
	//	Perhaps have to set as asynchronous?
	if(setsockopt(sd, SOL_SOCKET, SOCK_FLAGS, &one, sizeof(one)) < 0){
		perror("setsockopt");
		disconnect("Can't create a nonblocking server socket");
		return;
	}
	if(!set_non_block(sd)){
		disconnect("Can't create a nonblocking server socket");
		return;
	}
}

ServerSocket::~ServerSocket() {
	//	Should clean up the socket in Socket dtor
}

bool ServerSocket::init() {
	if (sd < 0) {
		if (serverListener != NULL) {
			serverListener->shutdown("couldn't create socket");
		}
		return false;
	}
	struct sockaddr_in sa;
	
	sa.sin_family = AF_INET;
	sa.sin_port = (u_short)htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(sa.sin_zero, 0, sizeof(sa.sin_zero));
	
	int result = -1;
	#ifdef WIN32
		result = bind(sd, (const struct sockaddr FAR *)&sa, sizeof(sa));
	#else
		result = bind(sd, (struct sockaddr *)&sa, sizeof(sa));
	#endif
	if (result < 0) {
		perror("bind");
		closesocket(sd);
		sd = -1;
		if (serverListener != NULL) {
			serverListener->shutdown("port in use");
		}
		return false;
	}
	
	if (listen(sd, 5) != 0) {
		perror("listen");
		closesocket(sd);
		sd = -1;
		if (serverListener != NULL) {
			serverListener->shutdown("unable to listen");
		}
		return false;
	}
	return true;
}

void ServerSocket::setServerListener(ServerSocketListener *serverListener) {
	this->serverListener = serverListener;
}

ServerSocketListener *ServerSocket::getServerListener() {
	return serverListener;
}

void ServerSocket::process() {
	//	This gets called from SocketHandler when we
	//	have something waiting for us (client connect)
	struct sockaddr_in peer_addr;
	socklen_t sock_len = sizeof(peer_addr);
	int in_sd = -1;
	in_sd = accept(sd, (struct sockaddr *)&peer_addr, &sock_len);
	if (in_sd < 0) {
		perror("accept");
		//	Perhaps we can drain the data?
		//	Otherwise select gets stuck in infinite loop
		Socket::process();
		//	But, serversocket gets disconnected! This was working before...
		return;
	}
	set_non_block(in_sd);
	if (serverListener != NULL) {	//	Umm, I guess inet_ntoa is from arpa/inet.h?
		serverListener->awaitingClient(new Socket(in_sd, inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port)));
	} else {
		cout << "no server listener available" << endl;
		closesocket(in_sd);
	}
}
