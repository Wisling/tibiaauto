#include "stdafx.h"

#include "SystemHooks.h"
#include "TibiaHooks.h"
#include "tibiaautoinject2.h"
#include "detours.h"


int(WINAPI * Real_send)(SOCKET s, char* buf, int len, int flags) = NULL;
int(WINAPI * Real_recv)(SOCKET s, char* buf, int len, int flags) = NULL;
int(WINAPI * Real_connect)(SOCKET s, const struct sockaddr* name, int namelen) = NULL;
SOCKET(WINAPI * Real_socket)(int af, int type, int protocol) = NULL;

int WINAPI Mine_send(SOCKET s, char* buf, int len, int flags);

char lastConnectName[16];
int lastSendFlags;

// the encryption buffer (to avoid decryption on send)
char encryptBeforeBuf[5000];
char encryptAfterBuf[5000];
int encryptLen;
int encryptPos;
int encryptPrevPtr = 0;
int encryptKeyPtr = 0;

void InitialiseSystemHooks()
{
	Real_send = (int(WINAPI *)(SOCKET, char*, int, int))DetourFindFunction("wsock32.dll", "send");
	Real_recv = (int(WINAPI *)(SOCKET, char*, int, int))DetourFindFunction("wsock32.dll", "recv");
	Real_connect = (int(WINAPI *)(SOCKET, const struct sockaddr*, int))DetourFindFunction("wsock32.dll", "connect");
	Real_socket = (SOCKET(WINAPI *)(int, int, int))DetourFindFunction("wsock32.dll", "socket");
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)Real_send, Mine_send);
	DetourAttach(&(PVOID&)Real_recv, Mine_recv);
	DetourAttach(&(PVOID&)Real_connect, Mine_connect);
	DetourAttach(&(PVOID&)Real_socket, Mine_socket);
	DetourTransactionCommit();
};

int WINAPI Mine_recv(SOCKET s, char* buf, int len, int flags)
{
	if (debugFile && COMPLEX && !SENTONLY)
		WriteOutDebug("!!! recv !!!\r\n");
	int offset = 0;

	int realRecvLen = 0;

	// use "standard" tibia own socket
	if (strncmp(buf + 15, "Aprem", 5) == 0)
		(buf + 15)[0] = 'o';
	realRecvLen = Real_recv(s, buf + offset, len - offset, flags);
	if (realRecvLen != -1)
	{
		if (debugFile && !SENTONLY)
		{
			if (COMPLEX)
				WriteOutDebug("realRecvLen=%d\r\n", realRecvLen);

			bufToHexString(buf, realRecvLen);
			WriteOutDebug("<- [%x] %s\r\n", socket, bufToHexStringRet);

			parseMessage(buf, realRecvLen, debugFile, 0, 1);
		}
	}


	if (debugFile)
		fflush(debugFile);

	tibiaSocket = s;


	return realRecvLen + offset;
}

SOCKET WINAPI Mine_socket(int af, int type, int protocol)
{
	SOCKET s = Real_socket(af, type, protocol);

	if (debugFile && COMPLEX)
		WriteOutDebug("got socket: %d, %d, %d -> %d\r\n", af, type, protocol, s);

	tibiaSocket = s;

	return s;
}

int WINAPI Mine_connect(SOCKET s, const struct sockaddr* name, int namelen)
{
	if (debugFile && COMPLEX)
		WriteOutDebug("connect [pre]: %d, %x, %d\r\n", s, name, namelen);


	int ret = Real_connect(s, name, namelen);

	if (debugFile && COMPLEX)
		WriteOutDebug("connect: %d, %x, %d\r\n", s, name, namelen);

	memcpy(lastConnectName, name, 16);

	return ret;
};

int WINAPI Mine_send(SOCKET s, char* buf, int len, int flags)
{
	int i;
	int identical = 1;
	if (len != encryptPos + 8 + 6)
	{
		identical = 0;
	}
	else
	{
		for (i = 0; i < encryptPos + 8; i++)
		{
			if (buf[6 + i] != encryptAfterBuf[i])
				identical = 0;
		}
	}
	if (debugFile && COMPLEX)
		WriteOutDebug("!!! send !!! [identical=%d]\r\n", identical);

	if (debugFile && !SENTONLY)
	{
		//bufToHexString(buf,len);
		//WriteOutDebug("E> [%x] %s\r\n",socket,bufToHexStringRet);
	}
	if (identical)
	{
		// pass for further processing only when we know content before encryption

		parseMessage(encryptBeforeBuf, encryptLen, debugFile, 1, 1);
		if (debugFile)
		{
			bufToHexString(encryptBeforeBuf, encryptLen);
			WriteOutDebug("-> [%x] %s\r\n", socket, bufToHexStringRet);
		}

		if (parseMessageForTibiaAction(encryptBeforeBuf, encryptLen))
			return len;
	}

	if (debugFile)
		fflush(debugFile);


	tibiaSocket = s;
	lastSendFlags = flags;


	int ret = 0;
	ret = Real_send(s, buf, len, flags);

	return ret;
}

// direction 0 - incoming (recv); 1 outgoing (send)
void parseMessage(char *buf, int realRecvLen, FILE *debugFile, int direction, int depth)
{
	// 45D565, 4F9520 hook for calculating everything
	// stack overflow protecion
	if (depth > 20)
	{
		if (debugFile && COMPLEX)
			WriteOutDebug("!!! stack overflow protection run\r\n");
		return;
	}

	int packetSizeH = buf[1];
	int packetSizeL = buf[0];
	if (packetSizeH < 0)
		packetSizeH += 256;
	if (packetSizeL < 0)
		packetSizeL += 256;
	int packetSize = packetSizeH * 256 + packetSizeL + 2;


	if (packetSize > realRecvLen)
	{
		if (debugFile && COMPLEX)
			WriteOutDebug("!!! underrun\r\n");
		return;
	}


	int code = buf[2];
	if (code < 0)
		code += 256;


	if (debugFile && COMPLEX)
		WriteOutDebug("### got bytes = %d; packet size = %d; code=0x%x\r\n", realRecvLen, packetSize, code);

	if (direction == 0)
	{
		// this is 'recv' direction: decrypt and parse message
		if (encryptKeyPtr)
		{
			int afterDecryptCode;
			static char decryptedBuf[100000];
			int i;

			memcpy(decryptedBuf, buf + 6, packetSize - 6); // Remember CRC bytes are NOT encrypted either sending or recieving.
														   //WriteOutDebug("# decrypted content follows #\r\n");

			for (i = 0; i < packetSize - 6; i += 8)
			{
				myInterceptDecrypt((int)(decryptedBuf + i), encryptKeyPtr);
			}
			afterDecryptCode = decryptedBuf[2];
			if (afterDecryptCode < 0)
				afterDecryptCode += 256;
			int afterDecryptLenH = decryptedBuf[1];
			int afterDecryptLenL = decryptedBuf[0];
			if (afterDecryptLenH < 0)
				afterDecryptLenH += 256;
			if (afterDecryptLenL < 0)
				afterDecryptLenL += 256;
			int afterDecryptLen = afterDecryptLenH * 256 + afterDecryptLenL + 2;

			if (debugFile && !SENTONLY)
			{
				//WriteOutDebug("$$$ len=%d code = 0x%x\r\n",afterDecryptLen,afterDecryptCode);
				bufToHexString(decryptedBuf, afterDecryptLen);
				WriteOutDebug("<- [%x] %s\r\n", socket, bufToHexStringRet);
			}
		}
	}

	if (packetSize < realRecvLen)
		parseMessage(buf + packetSize, realRecvLen - packetSize, debugFile, direction, depth + 1);
	// tail recursion
};

int lastAction = 0;
void sendBufferViaSocket(unsigned char *buffer)
{
	// if we don't yet have key pointer then don't do anything
	if (!encryptKeyPtr)
		return;
	int i;
	unsigned char outBuf[1006];
	unsigned char* outBufHeader = outBuf;
	unsigned char* outBufPayload = outBuf + 6;
	int payloadLen = *((unsigned short*)buffer) + 2; // length stored on first 2 bytes of payload doesn't include these 2 bytes
	if (payloadLen > 8 * 125) // this is to check to see if outBuf is long enough
							  //this should never be reached, but just in case 1000 is not arbitrarily high enough we can catch this error
		MessageBox(NULL, "Trying to send packet > 1000 characters!", "", 0);
	memcpy(outBufPayload, buffer, payloadLen);
	int outBufLen = payloadLen;
	if (payloadLen % 8 != 0)
		outBufLen += 8 - (payloadLen % 8); // packet length is now 8-byte encryptions + 4 CRC bytes + 2 bytes packet header, align payload to encryption size

										   //before encryption
	if (debugFile && COMPLEX)
	{
		bufToHexString(buffer, payloadLen);
		WriteOutDebug("Before Encryption: ~~~~~~~~~~~~~~~~~~~~~~\r\n");
		WriteOutDebug("-> [%x] %s\r\n", socket, bufToHexStringRet);
		WriteOutDebug("outBufLen = %d\r\n", outBufLen);
	}

	for (i = 0; i < outBufLen; i += 8)
	{
		myInterceptEncrypt((int)(outBufPayload + i), encryptKeyPtr);
	}
	//after encryption
	if (debugFile && COMPLEX)
	{
		WriteOutDebug("After Encryption: ~~~~~~~~~~~~~~~~~~~~~~\r\n");
		bufToHexString(outBufPayload, outBufLen);
		WriteOutDebug("-> [%x] %s\r\n", socket, bufToHexStringRet);
		WriteOutDebug("outBufLen = %d\r\n", outBufLen);
	}
	adler(outBufPayload, outBufLen, outBufHeader + 2);
	int lengthWithChecksum = outBufLen + 4;
	*((unsigned short*)outBufHeader) = lengthWithChecksum;

	// make sure that packets go at most once every minDistance ms
	int minDistance = 175;
	minDistance = 1;
	int nowAction = GetTickCount();
	if (nowAction - lastAction < minDistance && nowAction - lastAction > 0)
		Sleep(minDistance - (nowAction - lastAction));
	if (debugFile && COMPLEX)
	{
		WriteOutDebug("sending; waited %dms delta=%dms [%d]\r\n", minDistance - (nowAction - lastAction), nowAction - lastAction, time(NULL));
		WriteOutDebug("outBufLen = %d\r\n", outBufLen);
	}
	lastAction = GetTickCount();

	int ret = Mine_send(tibiaSocket, (char*)outBuf, lengthWithChecksum + 2, 0);

	if (debugFile && COMPLEX)
	{
		WriteOutDebug("sent %d bytes, ret=%d, lastSendFlags=%d\r\n", lengthWithChecksum + 2, ret, lastSendFlags);
		WriteOutDebug("outBufLen = %d\r\n", outBufLen);
	}
}

#define MOD_ADLER 65521
void adler(unsigned char *data, size_t len, unsigned char *outBuf)
{
	/*
	The is the CRC algorithim. I could not nor would I try to find the actual one Tibia
	uses. So I have copied this of and modified it to fit the data we use.
	*/
	int count = 0;
	int a = 1;
	int b = 0;

	while (len > 0)
	{
		size_t tlen = len > 5552 ? 5552 : len;
		len -= tlen;
		do
		{
			a += (int)data[count] >= 0 ? (int)data[count] : 256 + (int)data[count];
			b += a;
			count++;
		} while (--tlen);

		a %= MOD_ADLER;
		b %= MOD_ADLER;
	}
	outBuf[0] = a & 0xff;
	outBuf[1] = (a >> 8) & 0xff;
	outBuf[2] = b & 0xff;
	outBuf[3] = (b >> 8) & 0xff;
}