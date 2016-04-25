#pragma once

#include "Windows.h"

extern char encryptBeforeBuf[5000];
extern char encryptAfterBuf[5000];
extern int encryptLen;
extern int encryptPos;
extern int encryptPrevPtr;
extern int encryptKeyPtr;

void InitialiseSystemHooks();
int WINAPI Mine_recv(SOCKET s, char* buf, int len, int flags);
SOCKET WINAPI Mine_socket(int af, int type, int protocol);
int WINAPI Mine_connect(SOCKET s, const struct sockaddr* name, int namelen);
int WINAPI Mine_send(SOCKET s, char* buf, int len, int flags);
void parseMessage(char *buf, int realRecvLen, FILE *debugFile, int direction, int depth);
void sendBufferViaSocket(unsigned char *buffer);
void adler(unsigned char *data, size_t len, unsigned char *outBuf);