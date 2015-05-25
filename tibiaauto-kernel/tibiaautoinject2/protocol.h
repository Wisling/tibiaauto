#if !defined(AFX_PROTOCOL_H__D1220A19_D7FC_4B51_900D_224C75834004__INCLUDED_)
#define AFX_PROTOCOL_H__D1220A19_D7FC_4B51_900D_224C75834004__INCLUDED_

// protocol.h : Defines the entry point for the DLL application.
//

#include <string>
#include <iostream>
#include <fstream>


#define NETWORKMESSAGE_MAXSIZE 65536


class NetworkMessage
{
public:
	NetworkMessage();
	NetworkMessage(char* rawMsg);
	NetworkMessage(char* rawMsg, int len);
	virtual ~NetworkMessage()
	{
	};

	unsigned char GetByte();
	unsigned int GetInt();
	unsigned short GetShort();
	unsigned char PeekByte();
	unsigned int PeekInt();
	unsigned short PeekShort();
	bool canAdd(int size);
	bool canTake(int size);
	void sizeError(const char*, int);

	void RefreshSize();
	void ResetPos();
	std::string GetString(int stringLen = 0);
	std::string GetHex(int len = 0);
	void AddByte(unsigned char c);
	void AddInt(unsigned int i);
	void AddShort(unsigned short s);
	void AddString(const char* value);
	void AddBytes(const char* bytes, int size);

	unsigned char msgBuf[NETWORKMESSAGE_MAXSIZE];
	int msgSize, readPos;
};

class Protocol
{
public:
	Protocol()
	{
	}

	virtual ~Protocol()
	{
	}

	static void Protocol::outputPacket(NetworkMessage &msg);
	static void Protocol::parsePacketIn(NetworkMessage &msg);
	static void Protocol::parsePacketOut(NetworkMessage &msg);
};

#endif //AFX_PROTOCOL_H__D1220A19_D7FC_4B51_900D_224C75834004__INCLUDED_
