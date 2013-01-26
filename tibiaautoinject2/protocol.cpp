
// protocol.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "protocol.h"
#include "ModuleUtil.h"
#include "ModuleProxy.h"
#include "MemReaderProxy.h"
#include <fstream>
#include "time.h"
#include "ipcm.h"
#include "deelx.h"

//NetworkMessage is a message buffer that can read and write out a NetworkMessage
//the first two bytes indicate the length of the remaining bytes
//the remaining bytes compose the NetworkMessage
//readPos is the absolute position within msgBuf
//msgSize is the length of msgBuf (the first two bytes are msgBuf-2)
NetworkMessage::NetworkMessage(){
	readPos = 2;
	msgSize = 2;
	NetworkMessage::RefreshSize();
}

//Copies rawMsg into our msgBuf using the first two bytes as the length
NetworkMessage::NetworkMessage(char* rawMsg){
	msgBuf[0] = 0;
	readPos = 2;
	msgSize = 2;
	if (rawMsg!=NULL){
		msgSize = ((unsigned int)rawMsg[0]) | ((unsigned int)(rawMsg[1]) << 8)+2;
		if (msgSize<=NETWORKMESSAGE_MAXSIZE){
			memcpy(msgBuf,rawMsg,msgSize);
		} else {
			msgSize=0;
			AfxMessageBox("TibiaautoInject2: Requested NetworkMessage buffer more than 65536.");
		}
	} else {
		AfxMessageBox("TibiaautoInject2: NetworkMessage constructor sent NULL string");
	}
}

//Copies rawMsg into our msgBuf using len as the length
NetworkMessage::NetworkMessage(char* rawMsg, int len){
	msgBuf[0] = 0;
	readPos = 2;
	msgSize = len+2;
	if (rawMsg!=NULL){
		if (msgSize<=NETWORKMESSAGE_MAXSIZE){
			msgBuf[0] = len%256;
			msgBuf[1] = (len<<256)%256;
			memcpy(msgBuf+2,rawMsg,len);
		} else {
			msgSize=0;
			AfxMessageBox("TibiaautoInject2: Requested NetworkMessage buffer more than 65536.");
		}
	} else {
		AfxMessageBox("TibiaautoInject2: NetworkMessage constructor sent NULL string");
	}
}

int isSpellMessage(const char *msg)
{
	int pos;
	const char *spellPre[] = 
	{
		"ex",
		"ad",
		"ut",
		"al",
		NULL
	};
	const char *spellSuf[] = 
	{
		"ana",
		"eta",
		"evo",
		"ito",
		"ori",
		"ura",
		"ani",
		"iva",
		"amo",
		NULL
	};
	char newmsg[128];
	newmsg[0]=0;
	//get 5 characters from msg discarding all spaces
	int count=0;
	for (int i=0;count<5&&msg[i];i++){
		if (msg[i]!=' '){
			newmsg[count]=msg[i];
			count++;
		}
	}
	newmsg[count]='\0';
	if (strlen(newmsg)!=5) return 0;
	//if string starts with prefix and is = prefix+suffix, return 1
	for (pos=0;spellPre[pos];pos++){
		if (strnicmp(newmsg,spellPre[pos],2)==0){
			for (int pos2=0;spellSuf[pos2];pos2++){
				char tmp[10];
				tmp[0]=0;
				strcat(tmp,spellPre[pos]);
				strcat(tmp,spellSuf[pos2]);
				tmp[5]='\0';
				if (strnicmp(newmsg,tmp,5)==0){
					return 1;
				}
			}
		}
	}
	return 0;
}

bool NetworkMessage::canAdd(int size) {return (size + readPos <= NETWORKMESSAGE_MAXSIZE);}

void NetworkMessage::RefreshSize(){
	msgBuf[0] = (unsigned char)(msgSize-2);
	msgBuf[1] = (unsigned char)((msgSize-2)<<8);
}

void NetworkMessage::ResetPos(){
	readPos = 2;
}

unsigned char NetworkMessage::GetByte(){ return msgBuf[readPos++]; }

unsigned int NetworkMessage::GetInt(){
	unsigned int ans= *(int*)(msgBuf+readPos);
	readPos+=4;
	return ans;
}

unsigned short NetworkMessage::GetShort(){
	unsigned short ans= *(short*)(msgBuf+readPos);
	readPos+=2;
	return ans;
}

unsigned char NetworkMessage::PeekByte(){ return msgBuf[readPos]; }

unsigned int NetworkMessage::PeekInt(){
	return *(int*)(msgBuf+readPos);
}

unsigned short NetworkMessage::PeekShort(){
	return *(short*)(msgBuf+readPos);
}

std::string NetworkMessage::GetString(int stringLen/*=0*/){
	if(!stringLen)
		stringLen=GetShort();
	if(stringLen > (NETWORKMESSAGE_MAXSIZE - readPos-1)){
		AfxMessageBox("TibiaautoInject2: NetworkMessage GetString trying to read past buffer");
		return std::string();
	}
	char* v = (char*)(msgBuf + readPos);
	readPos += stringLen;
	return std::string(v,stringLen);
}

void NetworkMessage::AddByte(unsigned char c){
	if(!canAdd(1)) return;
	msgBuf[readPos++]=c;
	msgSize++;
}

void NetworkMessage::AddInt(unsigned int i){
	if(!canAdd(4)) return;
	*(unsigned int*)(msgBuf+readPos)=i;
	readPos += 4;
	msgSize += 4;
}

void NetworkMessage::AddShort(unsigned short s){
	if(!canAdd(2)) return;
	*(unsigned short*)(msgBuf+readPos)=s;
	readPos += 2;
	msgSize += 2;
}

void NetworkMessage::AddString(const char* value){
	int stringLen = strlen(value);
	if(!canAdd(stringLen + 2) || stringLen > 8192)
		return;

	AddShort(stringLen);
	strcpy((char*)(msgBuf + readPos), value);
	readPos += stringLen;
	msgSize += stringLen;
}

void NetworkMessage::AddBytes(const char* bytes, int size){
	if(!canAdd(size) || size > 8192)
		return;

	memcpy(msgBuf + readPos, bytes, size);
	readPos += size;
	msgSize += size;
}

void Protocol::outputPacket(NetworkMessage &msg){
	CMemReaderProxy reader;
	unsigned char recvbyte = msg.PeekByte();
	NetworkMessage msgNew=NetworkMessage();
	switch(recvbyte)
	{
		case 0x89: //parseTextWindow(msg);
			msgNew.AddByte(msg.GetByte());
			msgNew.AddInt(msg.GetInt());
			msgNew.AddString("<removed window text>");
			msg=msgNew;
			break;
		case 0x8A: //parseHouseWindow(msg);
			msgNew.AddByte(msg.GetByte());
			msgNew.AddByte(msg.GetByte());
			msgNew.AddInt(msg.GetInt());
			msgNew.AddString("<removed house window info>");
			msg=msgNew;
			break;
		case 0x96: //say something
			{
			msgNew.AddByte(msg.GetByte());
			unsigned short channelId = 0;
			unsigned char type=msg.GetByte();
			msgNew.AddByte(type);
			switch(type){
				case 0x05://SPEAK_PRIVATE
				case 0x0E://SPEAK_PRIVATE_RED
				case 0x0A://SPEAK_RVR_ANSWER
					msg.GetString();
					msgNew.AddString("<priv name removed>");
					break;
				case 0x07://SPEAK_CHANNEL_Y
				case 0x0D://SPEAK_CHANNEL_RN
				case 0x11://SPEAK_CHANNEL_RA
					msgNew.AddShort(msg.GetShort());
					break;
				default:
					break;
			}
			if (!isSpellMessage(msg.GetString().c_str())){
				msgNew.AddString("<removed message text>");
				msg=msgNew;
			}
			break;
			}
		case 0xAB: //parseChannelInvite(msg);
			msgNew.AddByte(msg.GetByte());
			msgNew.AddString("<invite name removed>");
			msg=msgNew;
			break;
		case 0xAC: //parseChannelExclude(msg);
			msgNew.AddByte(msg.GetByte());
			msgNew.AddString("<exclude name removed>");
			msg=msgNew;
			break;
		case 0xDC: //parseAddVip(msg);
			msgNew.AddByte(msg.GetByte());
			msgNew.AddString("<VIP name removed>");
			msg=msgNew;
			break;
		case 0xE8: //parseDebugAssert(msg);
			msgNew.AddByte(msg.GetByte());
			msgNew.AddString("(removed assert)");
			msgNew.AddString("(removed date	)");
			msgNew.AddString("(removed description)");
			msgNew.AddString("(removed comment)");
			msg=msgNew;
			break;

		case 0x14: // logout
		case 0x1E: // keep alive / ping response
		case 0x64: // move with steps
		case 0x65: // move north e
		case 0x66: // move east f
		case 0x67: // move south g
		case 0x68: // move west h
		case 0x69: // stop-autowalk i
		case 0x6A: // parseMove(msg, NORTHEAST); j
		case 0x6B: // parseMove(msg, SOUTHEAST); k
		case 0x6C: // parseMove(msg, SOUTHWEST); l
		case 0x6D: // parseMove(msg, NORTHWEST); m
		case 0x6F: // turn north n
		case 0x70: // turn east o
		case 0x71: // turn south p
		case 0x72: // turn west q	
		case 0x78: // throw item r
		case 0x79: // description in shop window
		case 0x7A: // player bought from shop
		case 0x7B: // player sold to shop
		case 0x7C: // player closed shop window
		case 0x7D: // Request trade
		case 0x7E: // Look at an item in trade
		case 0x7F: // Accept trade
		case 0x80: // close/cancel trade
		case 0x82: // use item
		case 0x83: // use item extended
		case 0x84: // battle window
		case 0x85: // rotate item
		case 0x87: // close container
		case 0x88: //"up-arrow" - container
		case 0x8C: // loot at square
		case 0x97: // request channels
		case 0x98: // open channel
		case 0x99: // close channel
		case 0x9A: // open priv
		case 0x9B: //process report
		case 0x9C: //gm closes report
		case 0x9D: //player cancels report
		case 0x9E: // close NPC
		case 0xA0: // set attack and follow mode
		case 0xA1: // attack
		case 0xA2: //follow
		case 0xA3: // invite party
		case 0xA4: // join party
		case 0xA5: // revoke party
		case 0xA6: // pass leadership
		case 0xA7: // leave party
		case 0xA8: // share exp
		case 0xAA: //parseCreatePrivateChannel(msg);
		case 0xBE: // cancel move
		case 0xC9: //client request to resend the tile
		case 0xCA: //client request to resend the container (happens when you store more than container maxsize)
		case 0xD2: // request outfit
		case 0xD3: // set outfit
		case 0xDD: //parseRemoveVip(msg);
		case 0xE6: //parseBugReport(msg);
		case 0xE7: //parseViolationWindow(msg);
		case 0xF0: //parseQuests(msg);
		case 0xF1: //parseQuestInfo(msg);
		case 0xF5: //request market info(itemId);
			//F5 B7 0D
		case 0xF6: //Post a buy/sell item
			// 0=buy,1=sell  itemid  quant  cost  ??  Anon
			// F6 00 16 0E 01 00 6F 00 00 00 00
			// F6 01 06 0E 03 00 03 00 00 00 00
		case 0xF8: //Accept buy/sell offer
			// saleId  ??  amount
			// F8 05 AE 1B 4F 00 00 05 01
		default:
			break;
	}
	msg.RefreshSize();
	char path[1024];
	CModuleUtil::getInstallPath(path);
	char pathBuf[2048];
	sprintf(pathBuf,"%s\\tascripts\\botting %d statistics.txt",path,reader.getProcessId());
	std::ofstream fout(pathBuf,std::ios::out|std::ios::app|std::ios::binary);
	int tm=time(NULL);
	fout.write((char*)&tm,4);
	fout.write((char*)msg.msgBuf,msg.msgSize);
	fout.write("\xff\xff",2);
	fout.close();
}


void Protocol::parsePacketIn(NetworkMessage &msg,CIPCPipeBack &ipcPipeBack){
	CMemReaderProxy reader;
	unsigned char recvbyte = msg.GetByte();
	switch(recvbyte)
	{
		case 0xB4:
			{
			unsigned char infoType = msg.GetByte();
			switch(infoType)
			{
				case 0x16://22
					{
					msg.GetString(15);
					std::string text = msg.GetString();
					//AfxMessageBox(text);
					static CRegexpT <char> reHitpointsNoAttacker("You lose (\\d+) hitpoints?\\.", IGNORECASE);
					MatchResult res1 = reHitpointsNoAttacker.Match(text.c_str());
					if(res1.IsMatched()){
						int hpLost=atoi(text.substr(res1.GetGroupStart(1),res1.GetGroupEnd(1)-res1.GetGroupStart(1)).c_str());
						struct ipcMessage mess;
						memcpy(mess.payload,&hpLost,sizeof(int));
						mess.messageType=1101;
						ipcPipeBack.send(mess);
					}
					/* not yet useful
					static CRegexpT <char> reHitpointsWithAttacker("You lose (\\d+) hitpoints? due to an attack by an? (.*)\\.", IGNORECASE);
					MatchResult res2 = reHitpointsNoAttacker.Match(text.c_str());
					if(res2.IsMatched()){
						int hpLost=atoi(text.substr(res2.GetGroupStart(1),res2.GetGroupEnd(1)-res2.GetGroupStart(1)).c_str());
						std::string attackerName = text.substr(res2.GetGroupStart(2),res2.GetGroupEnd(2)-res2.GetGroupStart(2));
						struct ipcMessage mess;
						memcpy(mess.payload,&hpLost,sizeof(int));
						strncpy(mess.payload+4,attackerName.c_str(),min(attackerName.length()+1,sizeof(mess.payload)-4));
						mess.payload[sizeof(mess.payload)-1] = 0;
						mess.messageType=1102;
						ipcPipeBack.send(mess);
					}
					*/
					}
					break;
				default: break;
			}
			}
			break;
		default: break;
	}
}
void Protocol::parsePacketOut(NetworkMessage &msg){
	CMemReaderProxy reader;
	unsigned char recvbyte = msg.PeekByte();
	switch(recvbyte)
	{
		default: break;
	}
}
