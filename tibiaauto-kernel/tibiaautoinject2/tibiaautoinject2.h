#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include "regex.h"

class CIPCPipeBack;
using namespace std;

#define TA_MESSAGE_QLEN 10
#define MAX_CREATUREINFO 1300
#define MAX_PAYLOAD_LEN 1024
#define MAX_PRINTEXT_LEN 1024
#define STRBUFLEN 655360
#define RECV_REGEX_MAX 500

struct actionRegexData {
	int handle;
	int inUse;
	regex_t preg;
	actionRegexData()
	{
		inUse = 0;
		handle = NULL;
	}
};

struct parcelRecvActionData {
	int handle;
	int totalCount;
	int countLeft;
	int len;
	char actionData[MAX_PAYLOAD_LEN - sizeof(int) * 4];
	parcelRecvActionData()
	{
		memset(actionData, 0, sizeof(actionData));
	}
};

struct packStream {
	char* s;
	int length;
	int pos;
};

struct HUD {
	CPoint pos;
	int redColor;
	int blueColor;
	int greenColor;
	string message;
public:
	HUD()
	{
		CPoint pos;
		redColor = 0;
		blueColor = 0;
		greenColor = 0;
		message[0] = '\0';
	}
};

extern CIPCPipeBack ipcPipeBack;
extern vector<HUD> vecHUD;
extern SOCKET tibiaSocket;
extern int ignoreLookEnd;
extern bool showManaBar;

extern char taMessage[TA_MESSAGE_QLEN][32000];
extern int taMessageStart;
extern int taMessageEnd;

extern char creatureInfoPlayerName[MAX_CREATUREINFO][32];
extern char creatureInfoPlayerInfo1[MAX_CREATUREINFO][500];
extern char creatureInfoPlayerInfo2[MAX_CREATUREINFO][500];

extern actionRegexData recvRegex[RECV_REGEX_MAX];
extern int recvRegexCount;

extern FILE *debugFile;
extern int COMPLEX;
extern int SENTONLY;
extern char bufToHexStringRet[STRBUFLEN];

void WriteOutDebug(const char * format, ...);
void bufToHexString(unsigned char *buf, int len);
void bufToHexString(char *buf, int len);
void sendTAMessage(char* message);
void parseRecvActionData(int handle, char* data, int len);
int parseMessageForTibiaAction(char *buf, int len);