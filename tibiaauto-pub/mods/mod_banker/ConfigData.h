#pragma once
#include <cstdint>

struct Banker {
	char bankerName[64];
	int bankerX;
	int bankerY;
	int bankerZ;
};

class CConfigData {
public:
	CConfigData();
	virtual ~CConfigData();

	uint8_t path[15];
	int targetX;
	int targetY;
	int targetZ;
	Banker banker;
	int minimumGoldToBank;
	int cashOnHand;
	char modPriorityStr[2];
	int changeGold;
	int capsLimit;
	int stopByBanker;
	int drawUpTo;
};