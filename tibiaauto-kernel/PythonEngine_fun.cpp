#include "StdAfx.h"
#include "PythonEngine_fun.h"
#include "ModuleProxy.h"
#include "PythonScript.h"
#include <tibiaauto_util.h>
#include <MemUtil.h>
#include <MemReader.h>
#include <TibiaCharacter.h>
#include <TileReader.h>
#include <VariableStore.h>
#include <PackSender.h>
#include <ModuleUtil.h>
#include <TibiaMap.h>
#include <TAMiniMap.h>
#include <CreaturesReader.h>

__declspec(dllimport) char *kernelRespond(char *text, char *id);

PyObject *tibiaauto_reader_setProcessId(PyObject *self, PyObject *args)
{
	int processId;
	if (!PyArg_ParseTuple(args, "i", &processId))
		return NULL;
	CMemUtil::setGlobalProcessId(processId);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_readSelfCharacter(PyObject *self, PyObject *args)
{
	CTibiaCharacter *selfCh = CMemReader::getMemReader().readSelfCharacter();

	PyObject *ret =
	        Py_BuildValue("{s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:f,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:s,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:s}",
	                      "hp", selfCh->hp,
	                      "mana", selfCh->mana,
	                      "x", selfCh->x,
	                      "y", selfCh->y,
	                      "z", selfCh->z,
	                      "tibiaId", selfCh->tibiaId,
	                      "visible", selfCh->visible,
	                      "outfitId", selfCh->outfitId,
	                      "mountId", selfCh->mountId,
	                      "moving", selfCh->moving,
	                      "monsterType", selfCh->monsterType,
	                      "hpPercLeft", selfCh->hpPercLeft,
	                      "cap", selfCh->cap,
	                      "stamina", selfCh->stamina,
	                      "maxHp", selfCh->maxHp,
	                      "maxMana", selfCh->maxMana,
	                      "exp", selfCh->exp,
	                      "lvl", selfCh->lvl,
	                      "mlvl", selfCh->mlvl,
	                      "mlvlPercLeft", selfCh->mlvlPercLeft,
	                      "soulPoints", selfCh->soulPoints,
	                      "skillSword", selfCh->skillSword,
	                      "skillAxe", selfCh->skillAxe,
	                      "skillClub", selfCh->skillClub,
	                      "skillShield", selfCh->skillShield,
	                      "skillDist", selfCh->skillDist,
	                      "skillFish", selfCh->skillFish,
	                      "skillFist", selfCh->skillFist,
	                      "skillSwordPercLeft", selfCh->skillSwordPercLeft,
	                      "skillAxePercLeft", selfCh->skillAxePercLeft,
	                      "skillClubPercLeft", selfCh->skillClubPercLeft,
	                      "skillShieldPercLeft", selfCh->skillShieldPercLeft,
	                      "skillDistPercLeft", selfCh->skillDistPercLeft,
	                      "skillFishPercLeft", selfCh->skillFishPercLeft,
	                      "skillFistPercLeft", selfCh->skillFistPercLeft,
	                      "name", selfCh->name,
	                      "nr", selfCh->nr,
	                      "lastAttackTm", selfCh->lastAttackTm,
	                      "lookDirection", selfCh->lookDirection,
	                      "colorHead", selfCh->colorHead,
	                      "colorBody", selfCh->colorBody,
	                      "colorLegs", selfCh->colorLegs,
	                      "colorFoot", selfCh->colorFoot,
	                      "walkSpeed", selfCh->walkSpeed,
	                      "skulls", selfCh->skulls,
	                      "shields", selfCh->shields,
	                      "warIcon", selfCh->warIcon,
	                      "blocking", selfCh->blocking,
	                      "voc", selfCh->voc);

	delete selfCh;
	return ret;
}

PyObject *tibiaauto_reader_readOpenContainerCount(PyObject *self, PyObject *args)
{
	int arg1 = CMemReader::getMemReader().readOpenContainerCount();
	PyObject *ret = Py_BuildValue("i", arg1);
	return ret;
}

PyObject *tibiaauto_reader_readContainer(PyObject *self, PyObject *args)
{
	int contNr;
	if (!PyArg_ParseTuple(args, "i", &contNr))
		return NULL;
	CTibiaContainer *cont = CMemReader::getMemReader().readContainer(contNr);
	PyObject *ret         =
	        Py_BuildValue("{s:i,s:i,s:i,s:i,s:i}",
	                      "flagOnOff", cont->flagOnOff,
	                      "objectId", cont->objectId,
	                      "size", cont->size,
	                      "number", cont->number,
	                      "itemsInside", cont->itemsInside);


	delete cont;
	return ret;
}

PyObject *tibiaauto_reader_readContainerItem(PyObject *self, PyObject *args)
{
	
	int contNr;
	int itemNr;
	if (!PyArg_ParseTuple(args, "ii", &contNr, &itemNr))
		return NULL;
	if (contNr < 0 || contNr >= 16)
	{
		PyObject *retNULL =
		        Py_BuildValue("{s:i,s:i,s:i}",
		                      "objectId", 0,
		                      "quantity", 0,
		                      "pos", 0);
		return retNULL;//avoids returning NULL
	}
	CTibiaContainer *cont = CMemReader::getMemReader().readContainer(contNr);
	if (itemNr < 0 || itemNr >= cont->itemsInside)
	{
		PyObject *retNULL =
		        Py_BuildValue("{s:i,s:i,s:i}",
		                      "objectId", 0,
		                      "quantity", 0,
		                      "pos", 0);
		delete cont;
		return retNULL;//avoids returning NULL
	}
	CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(itemNr);

	PyObject *ret =
	        Py_BuildValue("{s:i,s:i,s:i}",
	                      "objectId", item->objectId,
	                      "quantity", item->quantity,
	                      "pos", item->pos);

	delete cont;
	return ret;
}

PyObject *tibiaauto_reader_writeSelfLightPower(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CMemReader::getMemReader().writeSelfLightPower(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_writeSelfLightColor(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CMemReader::getMemReader().writeSelfLightColor(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_readSelfLightPower(PyObject *self, PyObject *args)
{
	
	int arg1      = CMemReader::getMemReader().readSelfLightPower();
	PyObject *ret = Py_BuildValue("i", arg1);

	return ret;
}

PyObject *tibiaauto_reader_readSelfLightColor(PyObject *self, PyObject *args)
{
	
	int arg1      = CMemReader::getMemReader().readSelfLightColor();
	PyObject *ret = Py_BuildValue("i", arg1);

	return ret;
}

PyObject *tibiaauto_reader_cancelAttackCoords(PyObject *self, PyObject *args)
{
	
	CMemReader::getMemReader().cancelAttackCoords();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_writeGotoCoords(PyObject *self, PyObject *args)
{
	
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	CMemReader::getMemReader().writeGotoCoords(arg1, arg2, arg3);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_getLoggedCharNr(PyObject *self, PyObject *args)
{
	
	int arg1      = CMemReader::getMemReader().getLoggedCharNr();
	PyObject *ret = Py_BuildValue("i", arg1);

	return ret;
}

PyObject *tibiaauto_reader_getSelfEventFlags(PyObject *self, PyObject *args)
{
	
	int arg1      = CMemReader::getMemReader().getSelfEventFlags();
	PyObject *ret = Py_BuildValue("i", arg1);

	return ret;
}

PyObject *tibiaauto_reader_writeVisibleCreatureName(PyObject *self, PyObject *args)
{
	
	int arg1;
	char *arg2;
	if (!PyArg_ParseTuple(args, "is", &arg1, &arg2))
		return NULL;
	CMemReader::getMemReader().writeVisibleCreatureName(arg1, arg2);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_getTradeItemPartner(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CTibiaItem *item = CMemReader::getMemReader().getTradeItemPartner(arg1);
	PyObject *ret    =
	        Py_BuildValue("{s:i,s:i,s:i}",
	                      "objectId", item->objectId,
	                      "quantity", item->quantity,
	                      "pos", item->pos);


	delete item;
	return ret;
}

PyObject *tibiaauto_reader_getTradeItemSelf(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CTibiaItem *item = CMemReader::getMemReader().getTradeItemSelf(arg1);
	PyObject *ret    =
	        Py_BuildValue("{s:i,s:i,s:i}",
	                      "objectId", item->objectId,
	                      "quantity", item->quantity,
	                      "pos", item->pos);


	delete item;
	return ret;
}

PyObject *tibiaauto_reader_getTradeCountPartner(PyObject *self, PyObject *args)
{
	
	int arg1      = CMemReader::getMemReader().getTradeCountPartner();
	PyObject *ret = Py_BuildValue("i", arg1);

	return ret;
}

PyObject *tibiaauto_reader_getTradeCountSelf(PyObject *self, PyObject *args)
{
	
	int arg1      = CMemReader::getMemReader().getTradeCountSelf();
	PyObject *ret = Py_BuildValue("i", arg1);

	return ret;
}

PyObject *tibiaauto_reader_getAttackedCreature(PyObject *self, PyObject *args)
{
	
	int arg1      = CMemReader::getMemReader().getAttackedCreature();
	PyObject *ret = Py_BuildValue("i", arg1);

	return ret;
}

PyObject *tibiaauto_reader_setAttackedCreature(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;

	CMemReader::getMemReader().setAttackedCreature(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_getFollowedCreature(PyObject *self, PyObject *args)
{
	
	int arg1      = CMemReader::getMemReader().getFollowedCreature();
	PyObject *ret = Py_BuildValue("i", arg1);

	return ret;
}

PyObject *tibiaauto_reader_setFollowedCreature(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;

	CMemReader::getMemReader().setFollowedCreature(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_GetLoggedChar(PyObject *self, PyObject *args)
{
		int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	char name[65];
	CMemReader::getMemReader().GetLoggedChar(arg1, name, sizeof(name));
	PyObject *ret = Py_BuildValue("s", name);

	return ret;
}

PyObject *tibiaauto_reader_readBattleListMax(PyObject *self, PyObject *args)
{
	
	int arg1      = CMemReader::getMemReader().readBattleListMax();
	PyObject *ret = Py_BuildValue("i", arg1);

	return ret;
}

PyObject *tibiaauto_reader_readBattleListMin(PyObject *self, PyObject *args)
{
	
	int arg1      = CMemReader::getMemReader().readBattleListMin();
	PyObject *ret = Py_BuildValue("i", arg1);

	return ret;
}

PyObject *tibiaauto_reader_readVisibleCreature(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CTibiaCharacter *ch = CMemReader::getMemReader().readVisibleCreature(arg1);

	PyObject *ret =
	        Py_BuildValue("{s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:f,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:s,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:s}",
	                      "hp", ch->hp,
	                      "mana", ch->mana,
	                      "x", ch->x,
	                      "y", ch->y,
	                      "z", ch->z,
	                      "tibiaId", ch->tibiaId,
	                      "visible", ch->visible,
	                      "outfitId", ch->outfitId,
	                      "mountId", ch->mountId,
	                      "moving", ch->moving,
	                      "monsterType", ch->monsterType,
	                      "hpPercLeft", ch->hpPercLeft,
	                      "cap", ch->cap,
	                      "stamina", ch->stamina,
	                      "maxHp", ch->maxHp,
	                      "maxMana", ch->maxMana,
	                      "exp", ch->exp,
	                      "lvl", ch->lvl,
	                      "mlvl", ch->mlvl,
	                      "mlvlPercLeft", ch->mlvlPercLeft,
	                      "soulPoints", ch->soulPoints,
	                      "skillSword", ch->skillSword,
	                      "skillAxe", ch->skillAxe,
	                      "skillClub", ch->skillClub,
	                      "skillShield", ch->skillShield,
	                      "skillDist", ch->skillDist,
	                      "skillFish", ch->skillFish,
	                      "skillFist", ch->skillFist,
	                      "skillSwordPercLeft", ch->skillSwordPercLeft,
	                      "skillAxePercLeft", ch->skillAxePercLeft,
	                      "skillClubPercLeft", ch->skillClubPercLeft,
	                      "skillShieldPercLeft", ch->skillShieldPercLeft,
	                      "skillDistPercLeft", ch->skillDistPercLeft,
	                      "skillFishPercLeft", ch->skillFishPercLeft,
	                      "skillFistPercLeft", ch->skillFistPercLeft,
	                      "name", ch->name,
	                      "nr", ch->nr,
	                      "lastAttackTm", ch->lastAttackTm,
	                      "lookDirection", ch->lookDirection,
	                      "colorHead", ch->colorHead,
	                      "colorBody", ch->colorBody,
	                      "colorLegs", ch->colorLegs,
	                      "colorFoot", ch->colorFoot,
	                      "walkSpeed", ch->walkSpeed,
	                      "skulls", ch->skulls,
	                      "shields", ch->shields,
	                      "warIcon", ch->warIcon,
	                      "blocking", ch->blocking,
	                      "voc", ch->voc);


	delete ch;
	return ret;
}

PyObject *tibiaauto_reader_readItem(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CTibiaItem *item = CMemReader::getMemReader().readItem(arg1);
	PyObject *ret    =
	        Py_BuildValue("{s:i,s:i,s:i}",
	                      "objectId", item->objectId,
	                      "quantity", item->quantity,
	                      "pos", item->pos);


	delete item;
	return ret;
}

PyObject *tibiaauto_reader_getCharacterByTibiaId(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CTibiaCharacter *ch = CMemReader::getMemReader().getCharacterByTibiaId(arg1);
	if (!ch)
		ch = CMemReader::getMemReader().readSelfCharacter(); //avoids returning NULL

	PyObject *ret =
	        Py_BuildValue("{s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:f,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:s,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:s}",
	                      "hp", ch->hp,
	                      "mana", ch->mana,
	                      "x", ch->x,
	                      "y", ch->y,
	                      "z", ch->z,
	                      "tibiaId", ch->tibiaId,
	                      "visible", ch->visible,
	                      "outfitId", ch->outfitId,
	                      "mountId", ch->mountId,
	                      "moving", ch->moving,
	                      "monsterType", ch->monsterType,
	                      "hpPercLeft", ch->hpPercLeft,
	                      "cap", ch->cap,
	                      "stamina", ch->stamina,
	                      "maxHp", ch->maxHp,
	                      "maxMana", ch->maxMana,
	                      "exp", ch->exp,
	                      "lvl", ch->lvl,
	                      "mlvl", ch->mlvl,
	                      "mlvlPercLeft", ch->mlvlPercLeft,
	                      "soulPoints", ch->soulPoints,
	                      "skillSword", ch->skillSword,
	                      "skillAxe", ch->skillAxe,
	                      "skillClub", ch->skillClub,
	                      "skillShield", ch->skillShield,
	                      "skillDist", ch->skillDist,
	                      "skillFish", ch->skillFish,
	                      "skillFist", ch->skillFist,
	                      "skillSwordPercLeft", ch->skillSwordPercLeft,
	                      "skillAxePercLeft", ch->skillAxePercLeft,
	                      "skillClubPercLeft", ch->skillClubPercLeft,
	                      "skillShieldPercLeft", ch->skillShieldPercLeft,
	                      "skillDistPercLeft", ch->skillDistPercLeft,
	                      "skillFishPercLeft", ch->skillFishPercLeft,
	                      "skillFistPercLeft", ch->skillFistPercLeft,
	                      "name", ch->name,
	                      "nr", ch->nr,
	                      "lastAttackTm", ch->lastAttackTm,
	                      "lookDirection", ch->lookDirection,
	                      "colorHead", ch->colorHead,
	                      "colorBody", ch->colorBody,
	                      "colorLegs", ch->colorLegs,
	                      "colorFoot", ch->colorFoot,
	                      "walkSpeed", ch->walkSpeed,
	                      "skulls", ch->skulls,
	                      "shields", ch->shields,
	                      "warIcon", ch->warIcon,
	                      "blocking", ch->blocking,
	                      "voc", ch->voc);


	delete ch;
	return ret;
}

PyObject *tibiaauto_reader_mapGetPointItemsCount(PyObject *self, PyObject *args)
{
	
	struct point p;
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	p.x = arg1;
	p.y = arg2;
	p.z = arg3;

	int ret1      = CMemReader::getMemReader().mapGetPointItemsCount(p);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_mapGetPointItemId(PyObject *self, PyObject *args)
{
	
	struct point p;
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;
	p.x = arg1;
	p.y = arg2;
	p.z = arg3;

	int ret1      = CMemReader::getMemReader().mapGetPointItemId(p, arg4);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_mapSetPointItemsCount(PyObject *self, PyObject *args)
{
	
	struct point p;
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;
	p.x = arg1;
	p.y = arg2;
	p.z = arg3;

	CMemReader::getMemReader().mapSetPointItemsCount(p, arg4);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_mapSetPointItemId(PyObject *self, PyObject *args)
{
	
	struct point p;
	int arg1, arg2, arg3, arg4, arg5;
	if (!PyArg_ParseTuple(args, "iiiii", &arg1, &arg2, &arg3, &arg4, &arg5))
		return NULL;
	p.x = arg1;
	p.y = arg2;
	p.z = arg3;

	CMemReader::getMemReader().mapSetPointItemId(p, arg4, arg5);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_mapGetPointItemExtraInfo(PyObject *self, PyObject *args)
{
	
	struct point p;
	int arg1, arg2, arg3, arg4, arg5;
	if (!PyArg_ParseTuple(args, "iiiii", &arg1, &arg2, &arg3, &arg4, &arg5))
		return NULL;
	p.x = arg1;
	p.y = arg2;
	p.z = arg3;

	int ret1      = CMemReader::getMemReader().mapGetPointItemExtraInfo(p, arg4, arg5);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_mapGetPointStackIndex(PyObject *self, PyObject *args)
{
	
	struct point p;
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;
	p.x = arg1;
	p.y = arg2;
	p.z = arg3;

	int ret1      = CMemReader::getMemReader().mapGetPointStackIndex(p, arg4);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_getCurrentTm(PyObject *self, PyObject *args)
{
	
	int ret1      = CMemReader::getMemReader().getCurrentTm();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_setRemainingTilesToGo(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;

	CMemReader::getMemReader().setRemainingTilesToGo(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_setMemIntValue(PyObject *self, PyObject *args)
{
	
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
	{
		PyErr_Clear();
		arg3 = 1;
		if (!PyArg_ParseTuple(args, "ii", &arg1, &arg2))
			return NULL;
	}

	CMemUtil::SetMemIntValue(arg1, arg2, arg3);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_getMemIntValue(PyObject *self, PyObject *args)
{
	
	int arg1, arg2;
	int ret1;
	if (!PyArg_ParseTuple(args, "ii", &arg1, &arg2))
	{
		PyErr_Clear();
		arg2 = 1;
		if (!PyArg_ParseTuple(args, "i", &arg1))
			return NULL;
	}

	ret1 = CMemUtil::GetMemIntValue(arg1, arg2);

	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_writeEnableRevealCName(PyObject *self, PyObject *args)
{
	
	CMemReader::getMemReader().writeEnableRevealCName();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_writeDisableRevealCName(PyObject *self, PyObject *args)
{
	
	CMemReader::getMemReader().writeDisableRevealCName();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_getProcessId(PyObject *self, PyObject *args)
{
	
	int ret1      = CMemUtil::getGlobalProcessId();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_getBaseAddr(PyObject *self, PyObject *args)
{
	
	int ret1 = CMemUtil::getGlobalBaseAddr();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_getKernelMainVersion(PyObject *self, PyObject *args)
{
	
	int ret1      = getKernelMainVersion();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_getKernelPatchVersion(PyObject *self, PyObject *args)
{
	
	int ret1      = getKernelPatchVersion();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_writeCreatureLightPower(PyObject *self, PyObject *args)
{
	
	int arg1, arg2;
	if (!PyArg_ParseTuple(args, "ii", &arg1, &arg2))
		return NULL;
	CMemReader::getMemReader().writeCreatureLightPower(arg1, arg2);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_writeCreatureLightColor(PyObject *self, PyObject *args)
{
	
	int arg1, arg2;
	if (!PyArg_ParseTuple(args, "ii", &arg1, &arg2))
		return NULL;
	CMemReader::getMemReader().writeCreatureLightColor(arg1, arg2);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_readCreatureLightPower(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	int ret1      = CMemReader::getMemReader().readCreatureLightPower(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_readCreatureLightColor(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	int ret1      = CMemReader::getMemReader().readCreatureLightColor(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_getTibiaTile(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CTibiaTile *tile = CTileReader::getTileReader().getTile(arg1);
	if (!tile)
		return NULL;
	PyObject *ret = Py_BuildValue("{s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i}",
	                              "ground", tile->ground,
	                              "goUp", tile->goUp,
	                              "goDown", tile->goDown,
	                              "blocking", tile->blocking,
	                              "speed", tile->speed,
	                              "requireRope", tile->requireRope,
	                              "requireUse", tile->requireUse,
	                              "requireShovel", tile->requireShovel,
	                              "canWalkThrough", tile->canWalkThrough,
	                              "isDepot", tile->isDepot,
	                              "isContainer", tile->isContainer,
	                              "notMoveable", tile->notMoveable,
	                              "stackable", tile->stackable,
	                              "alwaysOnTop", tile->alwaysOnTop,
	                              "moreAlwaysOnTop", tile->moreAlwaysOnTop,
	                              "isTeleporter", tile->isTeleporter,
	                              "isReadable", tile->isReadable,
	                              "isFluid", tile->isFluid,
	                              "isRune", tile->isRune,
	                              "isUseable", tile->isUseable,
	                              "isUseableImmobile", tile->isUseableImmobile,
	                              "blockPathFind", tile->blockPathFind,
	                              "minimapColor", tile->minimapColor);

	return ret;
}

PyObject *tibiaauto_reader_setGlobalVariable(PyObject *self, PyObject *args)
{
	
	char *arg1, *arg2;
	if (!PyArg_ParseTuple(args, "ss", &arg1, &arg2))
		return NULL;
	CVariableStore::setVariable(arg1, arg2);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_getGlobalVariable(PyObject *self, PyObject *args)
{
	
	char *arg1;
	if (!PyArg_ParseTuple(args, "s", &arg1))
		return NULL;
	const char *ret1 = CVariableStore::getVariable(arg1);
	PyObject *ret    = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_sender_useWithObjectFromFloorOnFloor(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9;
	if (!PyArg_ParseTuple(args, "iiiiiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9))
	{
		PyErr_Clear();
		if (!PyArg_ParseTuple(args, "iiiiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8))
			return NULL;
		else
			CPackSender::useWithObjectFromFloorOnFloor(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}
	else
	{
		CPackSender::useWithObjectFromFloorOnFloor(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_useWithObjectFromFloorInContainer(PyObject *self, PyObject *args)
{
	
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8;
	if (!PyArg_ParseTuple(args, "iiiiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8))
	{
		PyErr_Clear();
		if (!PyArg_ParseTuple(args, "iiiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7))
			return NULL;
		else
			CPackSender::useWithObjectFromFloorInContainer(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	}
	else
	{
		CPackSender::useWithObjectFromFloorInContainer(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_useWithObjectFromContainerInContainer(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7;
	if (!PyArg_ParseTuple(args, "iiiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7))
	{
		PyErr_Clear();
		if (!PyArg_ParseTuple(args, "iiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
			return NULL;
		else
			CPackSender::useWithObjectFromContainerInContainer(arg1, arg2, arg3, arg4, arg5, arg6);
	}
	else
	{
		CPackSender::useWithObjectFromContainerInContainer(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	}


	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_useWithObjectFromContainerOnFloor(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8;
	if (!PyArg_ParseTuple(args, "iiiiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8))
	{
		PyErr_Clear();
		if (!PyArg_ParseTuple(args, "iiiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7))
			return NULL;
		else
			CPackSender::useWithObjectFromContainerOnFloor(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	}
	else
	{
		CPackSender::useWithObjectFromContainerOnFloor(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	}
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_useWithObjectOnFloor(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5, arg6;
	if (!PyArg_ParseTuple(args, "iiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
	{
		PyErr_Clear();
		if (!PyArg_ParseTuple(args, "iiiii", &arg1, &arg2, &arg3, &arg4, &arg5))
			return NULL;
		else
			CPackSender::useWithObjectOnFloor(arg1, arg2, arg3, arg4, arg5);
	}
	else
	{
		CPackSender::useWithObjectOnFloor(arg1, arg2, arg3, arg4, arg5, arg6);
	}


	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_useWithObjectInContainer(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5;
	if (!PyArg_ParseTuple(args, "iiiii", &arg1, &arg2, &arg3, &arg4, &arg5))
	{
		PyErr_Clear();
		if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
			return NULL;
		else
			CPackSender::useWithObjectInContainer(arg1, arg2, arg3, arg4);
	}
	else
	{
		CPackSender::useWithObjectInContainer(arg1, arg2, arg3, arg4, arg5);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_moveObjectFromFloorToFloor(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8;
	if (!PyArg_ParseTuple(args, "iiiiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8))
		return NULL;
	CPackSender::moveObjectFromFloorToFloor(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_moveObjectFromFloorToContainer(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7;
	if (!PyArg_ParseTuple(args, "iiiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7))
		return NULL;
	CPackSender::moveObjectFromFloorToContainer(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_moveObjectBetweenContainers(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5, arg6;
	if (!PyArg_ParseTuple(args, "iiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
		return NULL;
	CPackSender::moveObjectBetweenContainers(arg1, arg2, arg3, arg4, arg5, arg6);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_moveObjectFromContainerToFloor(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7;
	if (!PyArg_ParseTuple(args, "iiiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7))
		return NULL;
	CPackSender::moveObjectFromContainerToFloor(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_castRuneAgainstCreature(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5;
	if (!PyArg_ParseTuple(args, "iiiii", &arg1, &arg2, &arg3, &arg4, &arg5))
		return NULL;
	CPackSender::castRuneAgainstCreature(arg1, arg2, arg3, arg4, arg5);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_castRuneAgainstHuman(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7;
	if (!PyArg_ParseTuple(args, "iiiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7))
		return NULL;
	CPackSender::castRuneAgainstHuman(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_useItemOnCreature(PyObject *self, PyObject *args)
{
	int arg1, arg2;
	if (!PyArg_ParseTuple(args, "ii", &arg1, &arg2))
		return NULL;
	CPackSender::useItemOnCreature(arg1, arg2);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_useItemFromContainerOnCreature(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;
	CPackSender::useItemFromContainerOnCreature(arg1, arg2, arg3, arg4);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_useItemFromFloorOnCreature(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5;
	if (!PyArg_ParseTuple(args, "iiiii", &arg1, &arg2, &arg3, &arg4, &arg5))
		return NULL;
	CPackSender::useItemFromFloorOnCreature(arg1, arg2, arg3, arg4, arg5);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_sendTAMessage(PyObject *self, PyObject *args)
{
	char *arg1;
	if (!PyArg_ParseTuple(args, "s", &arg1))
		return NULL;
	CPackSender::sendTAMessage(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_useItem(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CPackSender::useItem(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_useItemOnFloor(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;
	CPackSender::useItemOnFloor(arg1, arg2, arg3, arg4);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_useItemInContainer(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	CPackSender::useItemInContainer(arg1, arg2, arg3);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_openAutoContainerFromFloor(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;
	int ret1      = CPackSender::openAutoContainerFromFloor(arg1, arg2, arg3, arg4);
	PyObject *ret = Py_BuildValue("i", ret1);
	return ret;
}

PyObject *tibiaauto_sender_openContainerFromFloor(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5;
	if (!PyArg_ParseTuple(args, "iiiii", &arg1, &arg2, &arg3, &arg4, &arg5))
		return NULL;
	CPackSender::openContainerFromFloor(arg1, arg2, arg3, arg4, arg5);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_openAutoContainerFromContainer(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	int ret1      = CPackSender::openAutoContainerFromContainer(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);
	return ret;
}

PyObject *tibiaauto_sender_openContainerFromContainer(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;
	CPackSender::openContainerFromContainer(arg1, arg2, arg3, arg4);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_say(PyObject *self, PyObject *args)
{
	char *arg1;
	if (!PyArg_ParseTuple(args, "s", &arg1))
		return NULL;
	CPackSender::say(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_sayWhisper(PyObject *self, PyObject *args)
{
	char *arg1;
	if (!PyArg_ParseTuple(args, "s", &arg1))
		return NULL;
	CPackSender::sayWhisper(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_sayYell(PyObject *self, PyObject *args)
{
	char *arg1;
	if (!PyArg_ParseTuple(args, "s", &arg1))
		return NULL;
	CPackSender::sayYell(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_sayNPC(PyObject *self, PyObject *args)
{
	char *arg1;
	if (!PyArg_ParseTuple(args, "s", &arg1))
		return NULL;
	CPackSender::sayNPC(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_tell(PyObject *self, PyObject *args)
{
	char *arg1, *arg2;
	if (!PyArg_ParseTuple(args, "ss", &arg1, &arg2))
		return NULL;
	CPackSender::tell(arg1, arg2);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_sayOnChan(PyObject *self, PyObject *args)
{
	char *arg1;
	int arg2;
	int arg3;
	if (!PyArg_ParseTuple(args, "sii", &arg1, &arg2, &arg3))
		return NULL;
	CPackSender::sayOnChan(arg1, arg2, arg3);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_npcBuy(PyObject *self, PyObject *args)
{
	int arg1;
	int arg2;
	int arg3;
	int arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;
	CPackSender::npcBuy(arg1, arg2, arg3, arg4);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_npcSell(PyObject *self, PyObject *args)
{
	int arg1;
	int arg2;
	if (!PyArg_ParseTuple(args, "ii", &arg1, &arg2))
		return NULL;
	CPackSender::npcSell(arg1, arg2);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_logout(PyObject *self, PyObject *args)
{
	CPackSender::logout();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_walkOnTAMap(PyObject *self, PyObject *args)
{
		int arg1, arg2, arg3, arg4;
	int path[10000];

	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
	{
		PyErr_Clear();
		arg4 = 0;
		if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
			return NULL;
	}
	CTibiaCharacter *selfCh = CMemReader::getMemReader().readSelfCharacter();


	CModuleUtil::findPathOnMap(selfCh->x, selfCh->y, selfCh->z, arg1, arg2, arg3, 0, path, arg4);
	int pathSize;
	for (pathSize = 0; pathSize < 10000 && path[pathSize]; pathSize++)
	{
	}
	if (pathSize)
		CModuleUtil::executeWalk(selfCh->x, selfCh->y, selfCh->z, path);

	delete selfCh;

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_closeContainer(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CPackSender::closeContainer(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_attackMode(PyObject *self, PyObject *args)
{
	
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
	{
		PyErr_Clear();
		arg4 = CMemReader::getMemReader().getPlayerModePVP();
		if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		{
			PyErr_Clear();
			arg3 = CMemReader::getMemReader().getPlayerModeAttackPlayers();
			if (!PyArg_ParseTuple(args, "ii", &arg1, &arg2))
				return NULL;
		}
	}
	CPackSender::attackMode(arg1, arg2, arg3, arg4);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_attack(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CPackSender::attack(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_follow(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CPackSender::follow(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_turnLeft(PyObject *self, PyObject *args)
{
	CPackSender::turnLeft();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_turnRight(PyObject *self, PyObject *args)
{
	CPackSender::turnRight();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_turnUp(PyObject *self, PyObject *args)
{
	CPackSender::turnUp();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_turnDown(PyObject *self, PyObject *args)
{
	CPackSender::turnDown();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_stopAll(PyObject *self, PyObject *args)
{
	CPackSender::stopAll();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_stepMulti(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15;
	if (!PyArg_ParseTuple(args, "iiiiiiiiiiiiiii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10, &arg11, &arg12, &arg13, &arg14, &arg15))
		return NULL;
	int size = 15;
	if (arg15 == 0)
		size = 14;
	if (arg14 == 0)
		size = 13;
	if (arg13 == 0)
		size = 12;
	if (arg12 == 0)
		size = 11;
	if (arg11 == 0)
		size = 10;
	if (arg10 == 0)
		size = 9;
	if (arg9 == 0)
		size = 8;
	if (arg8 == 0)
		size = 7;
	if (arg7 == 0)
		size = 6;
	if (arg6 == 0)
		size = 5;
	if (arg5 == 0)
		size = 4;
	if (arg4 == 0)
		size = 3;
	if (arg3 == 0)
		size = 2;
	if (arg2 == 0)
		size = 1;
	if (arg1 == 0)
		size = 0;

	int *path = new int[size];
	if (arg1 != 0)
		path[0] = arg1;
	if (arg2 != 0)
		path[1] = arg2;
	if (arg3 != 0)
		path[2] = arg3;
	if (arg4 != 0)
		path[3] = arg4;
	if (arg5 != 0)
		path[4] = arg5;
	if (arg6 != 0)
		path[5] = arg6;
	if (arg7 != 0)
		path[6] = arg7;
	if (arg8 != 0)
		path[7] = arg8;
	if (arg9 != 0)
		path[8] = arg9;
	if (arg10 != 0)
		path[9] = arg10;
	if (arg11 != 0)
		path[10] = arg11;
	if (arg12 != 0)
		path[11] = arg12;
	if (arg13 != 0)
		path[12] = arg13;
	if (arg14 != 0)
		path[13] = arg14;
	if (arg15 != 0)
		path[14] = arg15;

	CPackSender::stepMulti(path, size);
	delete path;
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_stepLeft(PyObject *self, PyObject *args)
{
	CPackSender::stepLeft();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_stepRight(PyObject *self, PyObject *args)
{
	CPackSender::stepRight();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_stepUp(PyObject *self, PyObject *args)
{
	CPackSender::stepUp();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_stepDown(PyObject *self, PyObject *args)
{
	CPackSender::stepDown();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_stepUpRight(PyObject *self, PyObject *args)
{
	CPackSender::stepUpRight();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_stepDownRight(PyObject *self, PyObject *args)
{
	CPackSender::stepDownRight();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_stepUpLeft(PyObject *self, PyObject *args)
{
	CPackSender::stepUpLeft();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_stepDownLeft(PyObject *self, PyObject *args)
{
	CPackSender::stepDownLeft();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_printText(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5;
	char* arg6;
	if (!PyArg_ParseTuple(args, "iiiiis", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
		return NULL;

	CPackSender::printText(CPoint(arg1, arg2), arg3, arg4, arg5, arg6);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_sendMount(PyObject *self, PyObject *args)
{
	CPackSender::sendMount();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_sendDismount(PyObject *self, PyObject *args)
{
	CPackSender::sendDismount();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_sendDirectPacket(PyObject *self, PyObject *args)
{
	char* arg1;
	int arg2;
	if (!PyArg_ParseTuple(args, "s#", &arg1, &arg2))
		return NULL;

	CPackSender::sendDirectPacket(arg1, arg2);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_sendCreatureInfo(PyObject *self, PyObject *args)
{
	char *arg1, *arg2, *arg3;
	if (!PyArg_ParseTuple(args, "sss", &arg1, &arg2, &arg3))
		return NULL;
	CPackSender::sendCreatureInfo(arg1, arg2, arg3);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_look(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;
	CPackSender::look(arg1, arg2, arg3, arg4);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_ignoreLook(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CPackSender::ignoreLook(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_sendAutoAimConfig(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	CPackSender::sendAutoAimConfig(arg1, arg2, arg3);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_sendClearCreatureInfo(PyObject *self, PyObject *args)
{
	CPackSender::sendClearCreatureInfo();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_enableCName(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CPackSender::enableCName(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_sender_sendAttackedCreatureToAutoAim(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CPackSender::sendAttackedCreatureToAutoAim(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_map_removePointAvailable(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	CTibiaMap::getTibiaMap().removePointAvailable(arg1, arg2, arg3);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_map_prohPointClear(PyObject *self, PyObject *args)
{
	CTibiaMap::getTibiaMap().prohPointClear();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_map_prohPointAdd(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	CTibiaMap::getTibiaMap().prohPointAdd(arg1, arg2, arg3);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_map_getPointType(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	int ret1      = CTibiaMap::getTibiaMap().getPointType(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_map_getPointTypeNoProh(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	int ret1      = CTibiaMap::getTibiaMap().getPointTypeNoProh(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_map_setPointType(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;
	CTibiaMap::getTibiaMap().setPointType(arg1, arg2, arg3, arg4);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_map_getPrevPointZ(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	int ret1      = CTibiaMap::getTibiaMap().getPrevPointZ(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_map_getPrevPointY(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	int ret1      = CTibiaMap::getTibiaMap().getPrevPointY(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_map_getPrevPointX(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	int ret1      = CTibiaMap::getTibiaMap().getPrevPointX(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_map_clearPrevPoint(PyObject *self, PyObject *args)
{
	CTibiaMap::getTibiaMap().clearPrevPoint();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_map_setPrevPoint(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4, arg5, arg6;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
		return NULL;
	CTibiaMap::getTibiaMap().setPrevPoint(arg1, arg2, arg3, arg4, arg5, arg6);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_map_clear(PyObject *self, PyObject *args)
{
	CTibiaMap::getTibiaMap().clear();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_map_setPointAsAvailable(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	CTibiaMap::getTibiaMap().setPointAsAvailable(arg1, arg2, arg3);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_map_isPointAvailable(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	int ret1      = CTibiaMap::getTibiaMap().isPointAvailable(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_map_isPointAvailableNoProh(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	int ret1      = CTibiaMap::getTibiaMap().isPointAvailableNoProh(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_map_size(PyObject *self, PyObject *args)
{
	int ret1      = CTibiaMap::getTibiaMap().size();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_map_isPointInMiniMap(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	int ret1 = CTAMiniMap::getTAMiniMap().isPointInMiniMap(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_map_getMiniMapPoint(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	CTibiaMiniMapPoint *retPoint = CTAMiniMap::getTAMiniMap().getMiniMapPoint(arg1, arg2, arg3);
	PyObject *ret                =
	        Py_BuildValue("{s:i,s:i,s:i,s:i,s:i}",
	                      "x", retPoint->x,
	                      "y", retPoint->y,
	                      "z", retPoint->z,
	                      "colour", retPoint->colour,
	                      "speed", retPoint->speed);

	delete retPoint;
	return ret;
}

PyObject *tibiaauto_map_setPointLocked(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;
	CTibiaMap::getTibiaMap().setPointLocked(arg1, arg2, arg3, arg4);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_map_isPointLocked(PyObject *self, PyObject *args)
{
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;
	int ret1      = CTibiaMap::getTibiaMap().isPointLocked(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_regexp_match(PyObject *self, PyObject *args)
{
	char *arg1, *arg2;
	if (!PyArg_ParseTuple(args, "ss", &arg1, &arg2))
		return NULL;
	int ret1      = CModuleUtil::RegexMatch(arg1, arg2);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_alice_respond(PyObject *self, PyObject *args)
{
	char *arg1, *arg2;
	if (!PyArg_ParseTuple(args, "ss", &arg1, &arg2))
		return NULL;
	char *ret1    = kernelRespond(arg1, arg2);
	PyObject *ret = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_item_getItemName(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	char *ret1    = CTibiaItem::getItemName(arg1);
	PyObject *ret = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_item_getItemId(PyObject *self, PyObject *args)
{
	char *arg1;
	if (!PyArg_ParseTuple(args, "s", &arg1))
		return NULL;
	int ret1      = CTibiaItem::getItemId(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_item_getItemIndex(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	int ret1      = CTibiaItem::getItemIndex(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_item_getFoodIndex(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	int ret1      = CTibiaItem::getFoodIndex(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_item_getLootItemIndex(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	int ret1      = CTibiaItem::getLootItemIndex(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_item_getItemIdAtIndex(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	int ret1      = CTibiaItem::getItemIdAtIndex(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_item_getFoodIdAtIndex(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	int ret1      = CTibiaItem::getFoodIdAtIndex(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_item_getLootItemIdAtIndex(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	int ret1      = CTibiaItem::getLootItemIdAtIndex(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_item_getItemNameAtIndex(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	char* ret1    = CTibiaItem::getItemNameAtIndex(arg1);
	PyObject *ret = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_item_getFoodNameAtIndex(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	char* ret1    = CTibiaItem::getFoodNameAtIndex(arg1);
	PyObject *ret = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_item_getLootItemNameAtIndex(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	char* ret1    = CTibiaItem::getLootItemNameAtIndex(arg1);
	PyObject *ret = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_item_getFoodTimeAtIndex(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	int ret1      = CTibiaItem::getFoodTimeAtIndex(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_item_addItem(PyObject *self, PyObject *args)
{
	char* arg1;
	int arg2;
	if (!PyArg_ParseTuple(args, "si", &arg1, &arg2))
		return NULL;
	CTibiaItem::addItem(arg1, arg2);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_item_addFood(PyObject *self, PyObject *args)
{
	char* arg1;
	int arg2, arg3;
	if (!PyArg_ParseTuple(args, "sii", &arg1, &arg2, &arg3))
		return NULL;
	CTibiaItem::addFood(arg1, arg2, arg3);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_item_addLootItem(PyObject *self, PyObject *args)
{
	char* arg1;
	int arg2;
	if (!PyArg_ParseTuple(args, "si", &arg1, &arg2))
		return NULL;
	CTibiaItem::addLootItem(arg1, arg2);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_item_removeItem(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CTibiaItem::removeItem(arg1);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_item_removeFood(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CTibiaItem::removeFood(arg1);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_item_removeLootItem(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CTibiaItem::removeLootItem(arg1);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_item_clearFoodList(PyObject *self, PyObject *args)
{
	CTibiaItem::clearFoodList();

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_item_getItemCount(PyObject *self, PyObject *args)
{
	int ret1      = CTibiaItem::getItemCount();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_item_getFoodCount(PyObject *self, PyObject *args)
{
	int ret1      = CTibiaItem::getFoodCount();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_item_getLootItemCount(PyObject *self, PyObject *args)
{
	int ret1      = CTibiaItem::getLootItemCount();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_item_getValueForConst(PyObject *self, PyObject *args)
{
	char *arg1;
	if (!PyArg_ParseTuple(args, "s", &arg1))
		return NULL;
	int ret1      = CTibiaItem::getValueForConst(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_item_refreshItemLists(PyObject *self, PyObject *args)
{
	CTibiaItem::refreshItemLists();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_item_saveItemLists(PyObject *self, PyObject *args)
{
	CTibiaItem::saveItemLists();
	Py_INCREF(Py_None);
	return Py_None;
}

/* Deprecated Section Start*/
PyObject *tibiaauto_item_getName(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getName has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getObjectId(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getObjectId has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getCorpseIdByCreatureName(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getCorpseIdByCreatureName has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getItemsItems(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getItemsItems has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getItemsItemsId(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getItemsItemsId has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getItemsItemsCount(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getItemsItemsCount has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getItemsFood(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getItemsFood has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getItemsFoodId(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getItemsFoodId has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getItemsFoodCount(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getItemsFoodCount has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getItemsCorpses(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getItemsCorpses has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getItemsCorpsesId(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getItemsCorpsesId has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getItemsCorpsesCount(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getItemsCorpsesCount has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getItemsLooted(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getItemsLooted has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getItemsLootedId(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getItemsLootedId has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

PyObject *tibiaauto_item_getItemsLootedCount(PyObject *self, PyObject *args)
{
	PyErr_SetString(PyExc_NameError, "Tibia Auto Function Error: tibiaauto_item_getItemsLootedCount has beed deprecated. Please see the Tibia Auto wiki for python functions.");
	return NULL;
}

/* Deprecated Section End*/

PyObject *tibiaauto_kernel_startModule(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	if (arg1 >= CModuleProxy::allModulesCount || arg1 < 0)
		return NULL;

	CModuleProxy::allModules[arg1]->start();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_kernel_stopModule(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	if (arg1 >= CModuleProxy::allModulesCount || arg1 < 0)
		return NULL;

	CModuleProxy::allModules[arg1]->stop();
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_kernel_getModuleCount(PyObject *self, PyObject *args)
{
	int ret1      = CModuleProxy::allModulesCount;
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_kernel_getModuleName(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	if (arg1 >= CModuleProxy::allModulesCount || arg1 < 0)
		return NULL;

	char *ret1    = CModuleProxy::allModules[arg1]->moduleName;
	PyObject *ret = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_kernel_getModuleDesc(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	if (arg1 >= CModuleProxy::allModulesCount || arg1 < 0)
		return NULL;

	char *ret1    = CModuleProxy::allModules[arg1]->getName();
	PyObject *ret = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_kernel_getModuleVersion(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	if (arg1 >= CModuleProxy::allModulesCount || arg1 < 0)
		return NULL;

	char *ret1    = CModuleProxy::allModules[arg1]->getVersion();
	PyObject *ret = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_kernel_isModuleStarted(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	if (arg1 >= CModuleProxy::allModulesCount || arg1 < 0)
		return NULL;

	int ret1      = CModuleProxy::allModules[arg1]->isStarted();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_kernel_startPythonModule(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	if (arg1 >= CPythonScript::getScriptCount() || arg1 < 0)
		return NULL;

	CPythonScript::getScriptByNr(arg1)->setEnabled(1);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_kernel_stopPythonModule(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	if (arg1 >= CPythonScript::getScriptCount() || arg1 < 0)
		return NULL;

	CPythonScript::getScriptByNr(arg1)->setEnabled(0);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_kernel_getPythonModuleCount(PyObject *self, PyObject *args)
{
	int ret1      = CPythonScript::getScriptCount();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_kernel_getPythonModuleName(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	if (arg1 >= CPythonScript::getScriptCount() || arg1 < 0)
		return NULL;

	char *ret1    = CPythonScript::getScriptByNr(arg1)->getFileName();
	PyObject *ret = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_kernel_getPythonModuleDesc(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	if (arg1 >= CPythonScript::getScriptCount() || arg1 < 0)
		return NULL;

	char *ret1    = CPythonScript::getScriptByNr(arg1)->getName();
	PyObject *ret = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_kernel_getPythonModuleVersion(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	if (arg1 >= CPythonScript::getScriptCount() || arg1 < 0)
		return NULL;

	char *ret1    = CPythonScript::getScriptByNr(arg1)->getVersion();
	PyObject *ret = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_kernel_isPythonModuleStarted(PyObject *self, PyObject *args)
{
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	if (arg1 >= CPythonScript::getScriptCount() || arg1 < 0)
		return NULL;

	int ret1      = CPythonScript::getScriptByNr(arg1)->isEnabled();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_readMiniMap(PyObject *self, PyObject *args)
{
		int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;

	CTibiaMiniMap *miniMap = CMemReader::getMemReader().readMiniMap(arg1);

	if (!miniMap)
		return NULL;

	PyObject *ret = Py_BuildValue("{s:i,s:i,s:i,s:i,s:i,s:i}",
	                              "x", miniMap->x,
	                              "y", miniMap->y,
	                              "z", miniMap->z,
	                              "pointsAttached", miniMap->pointsAttached,
	                              "segmentPrev", miniMap->segmentPrev,
	                              "segmentNext", miniMap->segmentNext);
	delete miniMap;

	return ret;
}

PyObject *tibiaauto_reader_readMiniMapLabel(PyObject *self, PyObject *args)
{
		int arg1, arg2;
	if (!PyArg_ParseTuple(args, "ii", &arg1, &arg2))
		return NULL;

	CTibiaMiniMapLabel *MiniMapLabel = CMemReader::getMemReader().readMiniMapLabel(arg1, arg2);

	if (!MiniMapLabel)
		return NULL;

	PyObject *ret = Py_BuildValue("{s:i,s:i,s:i,s:s}",
	                              "x", MiniMapLabel->x,
	                              "y", MiniMapLabel->y,
	                              "type", MiniMapLabel->type,
	                              "desc", MiniMapLabel->desc);
	delete MiniMapLabel;

	return ret;
}

PyObject *tibiaauto_reader_readMiniMapPoint(PyObject *self, PyObject *args)
{
		int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;

	CTibiaMiniMapPoint *MiniMapPoint = CMemReader::getMemReader().readMiniMapPoint(arg1, arg2, arg3);

	if (!MiniMapPoint)
		return NULL;

	PyObject *ret = Py_BuildValue("{s:i,s:i,s:i,s:i,s:i}",
	                              "x", MiniMapPoint->x,
	                              "y", MiniMapPoint->y,
	                              "z", MiniMapPoint->z,
	                              "colour", MiniMapPoint->colour,
	                              "speed", MiniMapPoint->speed);
	delete MiniMapPoint;

	return ret;
}

PyObject *tibiaauto_reader_writeMiniMapPoint(PyObject *self, PyObject *args)
{
	
	int arg1, arg2, arg3, arg4, arg5;
	if (!PyArg_ParseTuple(args, "iiiii", &arg1, &arg2, &arg3, &arg4, &arg5))
		return NULL;
	CMemReader::getMemReader().writeMiniMapPoint(arg1, arg2, arg3, arg4, arg5);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_crstat_count(PyObject *self, PyObject *args)
{
	CCreaturesReader cReader;
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;

	int ret1 = cReader.findCreatureStatForLocationCount(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_crstat_tibiaId(PyObject *self, PyObject *args)
{
	CCreaturesReader cReader;
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;

	int ret1 = cReader.findCreatureStatForLocationTibiaId(arg1, arg2, arg3, arg4);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_crstat_name(PyObject *self, PyObject *args)
{
	CCreaturesReader cReader;
	int arg1, arg2, arg3, arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1, &arg2, &arg3, &arg4))
		return NULL;

	char *ret1 = cReader.findCreatureStatForLocationName(arg1, arg2, arg3, arg4);
	PyObject *ret;
	if (ret1)
	{
		ret = Py_BuildValue("s", ret1);
		delete ret1;
	}
	else
	{
		ret = Py_None;
	}

	return ret;
}

PyObject *tibiaauto_crstat_inarea(PyObject *self, PyObject *args)
{
	CCreaturesReader cReader;
	int arg1, arg2, arg3, arg4, arg5;
	if (!PyArg_ParseTuple(args, "iiiii", &arg1, &arg2, &arg3, &arg4, &arg5))
		return NULL;

	char **ret1 = cReader.findCreatureStatInArea(arg1, arg2, arg3, arg4, arg5);
	int len     = 0;
	for (len = 0; ret1[len]; )
		len++;

	PyObject *ret = PyList_New(len);
	int i;
	for (i = 0; i < len; i++)
	{
		PyObject *retSub = Py_BuildValue("s", ret1[i]);
		Py_INCREF(retSub);
		PyList_SetItem(ret, i, retSub);
		free(ret1[i]);
	}
	free(ret1);


	return ret;
}

PyObject *tibiaauto_reader_setMainWindowText(PyObject *self, PyObject *args)
{
	
	char *arg1;
	if (!PyArg_ParseTuple(args, "s", &arg1))
		return NULL;
	CMemReader::getMemReader().setMainWindowText(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_setMainTrayText(PyObject *self, PyObject *args)
{
	
	char *arg1;
	if (!PyArg_ParseTuple(args, "s", &arg1))
		return NULL;
	CMemReader::getMemReader().setMainTrayText(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_getPlayerModeAttackPlayers(PyObject *self, PyObject *args)
{
	
	int ret1      = CMemReader::getMemReader().getPlayerModeAttackPlayers();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_getPlayerModeAttackType(PyObject *self, PyObject *args)
{
	
	int ret1      = CMemReader::getMemReader().getPlayerModeAttackType();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_getPlayerModeFollow(PyObject *self, PyObject *args)
{
	
	int ret1      = CMemReader::getMemReader().getPlayerModeFollow();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_getPlayerModePVP(PyObject *self, PyObject *args)
{
	
	int ret1      = CMemReader::getMemReader().getPlayerModePVP();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_getConnectionState(PyObject *self, PyObject *args)
{
	
	int ret1      = CMemReader::getMemReader().getConnectionState();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_isLoggedIn(PyObject *self, PyObject *args)
{
	
	int ret1      = CMemReader::getMemReader().isLoggedIn();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_getOpenWindowName(PyObject *self, PyObject *args)
{
	
	char *ret1    = CMemReader::getMemReader().getOpenWindowName();
	PyObject *ret = Py_BuildValue("s", ret1);

	return ret;
}

PyObject *tibiaauto_reader_setXRayValues(PyObject *self, PyObject *args)
{
	
	int arg1, arg2;
	if (!PyArg_ParseTuple(args, "ii", &arg1, &arg2))
		return NULL;

	CMemReader::getMemReader().setXRayValues(arg1, arg2);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_getXRayValue1(PyObject *self, PyObject *args)
{
	
	int ret1      = CMemReader::getMemReader().getXRayValue1();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_getXRayValue2(PyObject *self, PyObject *args)
{
	
	int ret1      = CMemReader::getMemReader().getXRayValue2();
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_writeCreatureDeltaXY(PyObject *self, PyObject *args)
{
	
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;

	CMemReader::getMemReader().writeCreatureDeltaXY(arg1, arg2, arg3);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *tibiaauto_reader_getCreatureDeltaX(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	int ret1      = CMemReader::getMemReader().getCreatureDeltaX(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_getCreatureDeltaY(PyObject *self, PyObject *args)
{
		int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	int ret1      = CMemReader::getMemReader().getCreatureDeltaY(arg1);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_readVIPEntry(PyObject *self, PyObject *args)
{
	
	int arg1;
	if (!PyArg_ParseTuple(args, "i", &arg1))
		return NULL;
	CTibiaVIPEntry *vip = CMemReader::getMemReader().readVIPEntry(arg1);

	if (!vip)
		return NULL;

	PyObject *ret =
	        Py_BuildValue("{s:i,s:s,s:i,s:i,s:s,s:i,s:i}",
	                      "id", vip->id,
	                      "name", vip->name,
	                      "status", vip->status,
	                      "icon", vip->icon,
	                      "description", vip->descr,
	                      "loginTime", vip->loginTm,
	                      "notifyAtLogin", vip->notify);

	delete vip;
	return ret;
}

PyObject *tibiaauto_reader_mapGetPointTopPos(PyObject *self, PyObject *args)
{
	
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;

	int ret1      = CMemReader::getMemReader().itemOnTopIndex(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_reader_mapGetPointSeenOnTopPos(PyObject *self, PyObject *args)
{
	
	int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;

	int ret1      = CMemReader::getMemReader().itemSeenOnTopIndex(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}

PyObject *tibiaauto_packet_first(PyObject *self, PyObject *args)
{
		int arg1, arg2, arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1, &arg2, &arg3))
		return NULL;

	int ret1      = CMemReader::getMemReader().itemSeenOnTopIndex(arg1, arg2, arg3);
	PyObject *ret = Py_BuildValue("i", ret1);

	return ret;
}
