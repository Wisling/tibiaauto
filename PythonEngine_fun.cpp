#include "ModuleProxy.h"
#include "PythonScript.h"
#include "CreaturesReaderProxy.h"
#include <commons.h>

static PyObject *tibiaauto_reader_setProcessId(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int processId;
    if (!PyArg_ParseTuple(args, "i", &processId)) return NULL;
	reader.setProcessId(processId);    
	Py_INCREF(Py_None);
	return Py_None;   
}
static PyObject *tibiaauto_reader_readSelfCharacter(PyObject *self, PyObject *args)
{ 
	CMemReaderProxy reader;
	
	CTibiaCharacter *selfCh = reader.readSelfCharacter();
	PyObject *ret = 
		Py_BuildValue("{s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:f,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:s,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i}",
		"hp",selfCh->hp,
		"mana",selfCh->mana,
		"x",selfCh->x,
		"y",selfCh->y,
		"z",selfCh->z,
		"tibiaId",selfCh->tibiaId,
		"visible",selfCh->visible,
		"outfitId",selfCh->outfitId,
		"monsterType",selfCh->monsterType,
		"hpPercLeft",selfCh->hpPercLeft,
		"cap",selfCh->cap,
		"stamina",selfCh->stamina,
		"maxHp",selfCh->maxHp,
		"maxMana",selfCh->maxMana,
		"exp",selfCh->exp,
		"lvl",selfCh->lvl,
		"mlvl",selfCh->mlvl,
		"mlvlPercLeft",selfCh->mlvlPercLeft,
		"soulPoints",selfCh->soulPoints,
		"skillSword",selfCh->skillSword,
		"skillAxe",selfCh->skillAxe,
		"skillClub",selfCh->skillClub,
		"skillShield",selfCh->skillShield,
		"skillDist",selfCh->skillDist,
		"skillFish",selfCh->skillFish,
		"skillFist",selfCh->skillFist,
		"skillSwordPercLeft",selfCh->skillSwordPercLeft,
		"skillAxePercLeft",selfCh->skillAxePercLeft,
		"skillClubPercLeft",selfCh->skillClubPercLeft,
		"skillShieldPercLeft",selfCh->skillShieldPercLeft,
		"skillDistPercLeft",selfCh->skillDistPercLeft,
		"skillFishPercLeft",selfCh->skillFishPercLeft,
		"skillFistPercLeft",selfCh->skillFistPercLeft,
		"name",selfCh->name,
		"nr",selfCh->nr,
		"lastAttackTm",selfCh->lastAttackTm,
		"lookDirection",selfCh->lookDirection,
		"colorHead",selfCh->colorHead,
		"colorBody",selfCh->colorBody,
		"colorLegs",selfCh->colorLegs,
		"colorFoot",selfCh->colorFoot,
		"walkSpeed",selfCh->walkSpeed,
		"skulls",selfCh->skulls,
		"shields",selfCh->shields);	

	delete selfCh;
	return ret;
}
static PyObject *tibiaauto_reader_readContainer(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int contNr;
    if (!PyArg_ParseTuple(args, "i", &contNr)) return NULL;
	CTibiaContainer *cont = reader.readContainer(contNr);
	PyObject *ret = 
		Py_BuildValue("{s:i,s:i,s:i,s:i,s:i}",
		"flagOnOff",cont->flagOnOff,
		"objectId",cont->objectId,
		"size",cont->size,
		"number",cont->number,
		"itemsInside",cont->itemsInside);

	

	delete cont;
	return ret;
}

static PyObject *tibiaauto_reader_readContainerItem(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int contNr;
	int itemNr;
    if (!PyArg_ParseTuple(args, "ii", &contNr,&itemNr)) return NULL;	
	CTibiaContainer *cont = reader.readContainer(contNr);
	if (itemNr>=cont->itemsInside) 
	{
		delete cont;
		return NULL;
	}
	CTibiaItem *item = (CTibiaItem *)cont->items.GetAt(itemNr);
	PyObject *ret = 
		Py_BuildValue("{s:i,s:i,s:i}",
		"objectId",item->objectId,
		"quantity",item->quantity,
		"pos",item->pos);

		
 
	delete cont; 
	return ret;
}
static PyObject *tibiaauto_reader_writeSelfLightPower(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	reader.writeSelfLightPower(arg1);
	Py_INCREF(Py_None);
	return Py_None;   
}
static PyObject *tibiaauto_reader_writeSelfLightColor(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	reader.writeSelfLightColor(arg1);
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_reader_readSelfLightPower(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1=reader.readSelfLightPower();
	PyObject *ret = Py_BuildValue("i",arg1);
	
	return ret;
}
static PyObject *tibiaauto_reader_readSelfLightColor(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1=reader.readSelfLightColor();
	PyObject *ret = Py_BuildValue("i",arg1);
	
	return ret;
}
static PyObject *tibiaauto_reader_cancelAttackCoords(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	    
	reader.cancelAttackCoords();
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_reader_writeGotoCoords(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1,arg2,arg3;
    if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;
	reader.writeGotoCoords(arg1,arg2,arg3);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_reader_getLoggedCharNr(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1=reader.getLoggedCharNr();
	PyObject *ret = Py_BuildValue("i",arg1);
	
	return ret;
}
static PyObject *tibiaauto_reader_getSelfEventFlags(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1=reader.getSelfEventFlags();
	PyObject *ret = Py_BuildValue("i",arg1);
	
	return ret;
}
static PyObject *tibiaauto_reader_writeVisibleCreatureName(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1;
	char *arg2;
    if (!PyArg_ParseTuple(args, "is", &arg1,&arg2)) return NULL;
	reader.writeVisibleCreatureName(arg1,arg2);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_reader_getTradeItemPartner(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;
	
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;		
	CTibiaItem *item = reader.getTradeItemPartner(arg1);
	PyObject *ret = 
		Py_BuildValue("{s:i,s:i,s:i}",
		"objectId",item->objectId,
		"quantity",item->quantity,
		"pos",item->pos);
	
	

	delete item;
	return ret;
}
static PyObject *tibiaauto_reader_getTradeItemSelf(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;		
	CTibiaItem *item = reader.getTradeItemSelf(arg1);
	PyObject *ret = 
		Py_BuildValue("{s:i,s:i,s:i}",
		"objectId",item->objectId,
		"quantity",item->quantity,
		"pos",item->pos);

	
	

	delete item;
	return ret;
}
static PyObject *tibiaauto_reader_getTradeCountPartner(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1=reader.getTradeCountPartner();
	PyObject *ret = Py_BuildValue("i",arg1);
	
	return ret;
}
static PyObject *tibiaauto_reader_getTradeCountSelf(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1=reader.getTradeCountSelf();
	PyObject *ret = Py_BuildValue("i",arg1);
	
	return ret;
}
static PyObject *tibiaauto_reader_getAttackedCreature(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1=reader.getAttackedCreature();
	PyObject *ret = Py_BuildValue("i",arg1);
	
	return ret;
}
static PyObject *tibiaauto_reader_GetLoggedChar(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	char *name=reader.GetLoggedChar(arg1);
	PyObject *ret = Py_BuildValue("s",name);	
	
	return ret;
}
static PyObject *tibiaauto_reader_readBattleListMax(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1=reader.readBattleListMax();
	PyObject *ret = Py_BuildValue("i",arg1);
	
	return ret;
}
static PyObject *tibiaauto_reader_readBattleListMin(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1=reader.readBattleListMin();
	PyObject *ret = Py_BuildValue("i",arg1);
	
	return ret;
}
static PyObject *tibiaauto_reader_readVisibleCreature(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	CTibiaCharacter *ch = reader.readVisibleCreature(arg1);
	PyObject *ret = 
		Py_BuildValue("{s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:f,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:s,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i}",
		"hp",ch->hp,
		"mana",ch->mana,
		"x",ch->x,
		"y",ch->y,
		"z",ch->z,
		"tibiaId",ch->tibiaId,
		"visible",ch->visible,
		"outfitId",ch->outfitId,
		"monsterType",ch->monsterType,
		"hpPercLeft",ch->hpPercLeft,
		"cap",ch->cap,
		"stamina",ch->stamina,
		"maxHp",ch->maxHp,
		"maxMana",ch->maxMana,
		"exp",ch->exp,
		"lvl",ch->lvl,
		"mlvl",ch->mlvl,
		"mlvlPercLeft",ch->mlvlPercLeft,
		"soulPoints",ch->soulPoints,
		"skillSword",ch->skillSword,
		"skillAxe",ch->skillAxe,
		"skillClub",ch->skillClub,
		"skillShield",ch->skillShield,
		"skillDist",ch->skillDist,
		"skillFish",ch->skillFish,
		"skillFist",ch->skillFist,
		"skillSwordPercLeft",ch->skillSwordPercLeft,
		"skillAxePercLeft",ch->skillAxePercLeft,
		"skillClubPercLeft",ch->skillClubPercLeft,
		"skillShieldPercLeft",ch->skillShieldPercLeft,
		"skillDistPercLeft",ch->skillDistPercLeft,
		"skillFishPercLeft",ch->skillFishPercLeft,
		"skillFistPercLeft",ch->skillFistPercLeft,
		"name",ch->name,
		"nr",ch->nr,
		"lastAttackTm",ch->lastAttackTm,
		"lookDirection",ch->lookDirection,
		"colorHead",ch->colorHead,
		"colorBody",ch->colorBody,
		"colorLegs",ch->colorLegs,
		"colorFoot",ch->colorFoot,
		"walkSpeed",ch->walkSpeed,
		"skulls",ch->skulls,
		"shields",ch->shields);

	

	delete ch;
	return ret; 
}
static PyObject *tibiaauto_reader_readItem(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;		
	CTibiaItem *item = reader.readItem(arg1);
	PyObject *ret = 
		Py_BuildValue("{s:i,s:i,s:i}",
		"objectId",item->objectId,
		"quantity",item->quantity,
		"pos",item->pos);

	
	

	delete item;
	return ret;
}
static PyObject *tibiaauto_reader_getCharacterByTibiaId(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;
	
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	CTibiaCharacter *ch = reader.getCharacterByTibiaId(arg1);
	if (!ch) return NULL;
	PyObject *ret = 
		Py_BuildValue("{s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:f,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:s,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i}",
		"hp",ch->hp,
		"mana",ch->mana,
		"x",ch->x,
		"y",ch->y,
		"z",ch->z,
		"tibiaId",ch->tibiaId,
		"visible",ch->visible,
		"outfitId",ch->outfitId,
		"monsterType",ch->monsterType,
		"hpPercLeft",ch->hpPercLeft,
		"cap",ch->cap,
		"stamina",ch->stamina,
		"maxHp",ch->maxHp,
		"maxMana",ch->maxMana,
		"exp",ch->exp,
		"lvl",ch->lvl,
		"mlvl",ch->mlvl,
		"mlvlPercLeft",ch->mlvlPercLeft,
		"soulPoints",ch->soulPoints,
		"skillSword",ch->skillSword,
		"skillAxe",ch->skillAxe,
		"skillClub",ch->skillClub,
		"skillShield",ch->skillShield,
		"skillDist",ch->skillDist,
		"skillFish",ch->skillFish,
		"skillFist",ch->skillFist,
		"skillSwordPercLeft",ch->skillSwordPercLeft,
		"skillAxePercLeft",ch->skillAxePercLeft,
		"skillClubPercLeft",ch->skillClubPercLeft,
		"skillShieldPercLeft",ch->skillShieldPercLeft,
		"skillDistPercLeft",ch->skillDistPercLeft,
		"skillFishPercLeft",ch->skillFishPercLeft,
		"skillFistPercLeft",ch->skillFistPercLeft,
		"name",ch->name,
		"nr",ch->nr,
		"lastAttackTm",ch->lastAttackTm,
		"lookDirection",ch->lookDirection,
		"colorHead",ch->colorHead,
		"colorBody",ch->colorBody,
		"colorLegs",ch->colorLegs,
		"colorFoot",ch->colorFoot,
		"walkSpeed",ch->walkSpeed,
		"skulls",ch->skulls,
		"shields",ch->shields);

	

	delete ch;
	return ret; 
}

static PyObject *tibiaauto_reader_mapGetPointItemsCount(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	struct point p;	
	int arg1,arg2,arg3;
    if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;
	p.x=arg1;
	p.y=arg2;
	p.z=arg3;

	int ret1=reader.mapGetPointItemsCount(p);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_mapGetPointItemId(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	struct point p;	
	int arg1,arg2,arg3,arg4;
    if (!PyArg_ParseTuple(args, "iiii", &arg1,&arg2,&arg3,&arg4)) return NULL;
	p.x=arg1;
	p.y=arg2;
	p.z=arg3;

	int ret1=reader.mapGetPointItemId(p,arg4);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_mapSetPointItemsCount(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	struct point p;	
	int arg1,arg2,arg3,arg4;
    if (!PyArg_ParseTuple(args, "iiii", &arg1,&arg2,&arg3,&arg4)) return NULL;
	p.x=arg1;
	p.y=arg2;
	p.z=arg3;

	reader.mapSetPointItemsCount(p,arg4);	

	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_reader_mapSetPointItemId(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	struct point p;	
	int arg1,arg2,arg3,arg4,arg5;
    if (!PyArg_ParseTuple(args, "iiiii", &arg1,&arg2,&arg3,&arg4,&arg5)) return NULL;
	p.x=arg1;
	p.y=arg2;
	p.z=arg3;

	reader.mapSetPointItemId(p,arg4,arg5);	

	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_reader_mapGetPointItemExtraInfo(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	struct point p;	
	int arg1,arg2,arg3,arg4,arg5;
    if (!PyArg_ParseTuple(args, "iiiii", &arg1,&arg2,&arg3,&arg4,&arg5)) return NULL;
	p.x=arg1;
	p.y=arg2;
	p.z=arg3;

	int ret1=reader.mapGetPointItemExtraInfo(p,arg4,arg5);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_mapGetPointStackIndex(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	struct point p;	
	int arg1,arg2,arg3,arg4;
    if (!PyArg_ParseTuple(args, "iiii", &arg1,&arg2,&arg3,&arg4)) return NULL;
	p.x=arg1;
	p.y=arg2;
	p.z=arg3;

	int ret1=reader.mapGetPointStackIndex(p,arg4);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_getCurrentTm(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;

	int ret1=reader.getCurrentTm();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_setRemainingTilesToGo(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	

	reader.setRemainingTilesToGo(arg1);
	Py_INCREF(Py_None);
	return Py_None; 
}

static PyObject *tibiaauto_reader_setMemIntValue(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1,arg2;
    if (!PyArg_ParseTuple(args, "ii", &arg1,&arg2)) return NULL;	

	reader.setMemIntValue(arg1,arg2);
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_reader_getMemIntValue(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	
	int ret1=reader.getMemIntValue(arg1);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_writeEnableRevealCName(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	reader.writeEnableRevealCName();
	Py_INCREF(Py_None);
	return Py_None;  
}
static PyObject *tibiaauto_reader_writeDisableRevealCName(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	reader.writeDisableRevealCName();
	Py_INCREF(Py_None);
	return Py_None;  
}
static PyObject *tibiaauto_reader_getProcessId(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;

	int ret1=reader.getProcessId();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_getKernelMainVersion(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int ret1=reader.getKernelMainVersion();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_getKernelPatchVersion(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int ret1=reader.getKernelPatchVersion();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_writeCreatureLightPower(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1,arg2;
    if (!PyArg_ParseTuple(args, "ii", &arg1,&arg2)) return NULL;	
	reader.writeCreatureLightPower(arg1,arg2);	
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_reader_writeCreatureLightColor(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1,arg2;
    if (!PyArg_ParseTuple(args, "ii", &arg1,&arg2)) return NULL;	
	reader.writeCreatureLightColor(arg1,arg2);	
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_reader_readCreatureLightPower(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	
	int ret1=reader.readCreatureLightPower(arg1);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_readCreatureLightColor(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	
	int ret1=reader.readCreatureLightColor(arg1);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_getTibiaTile(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	
	CTibiaTile *tile=reader.getTibiaTile(arg1);
	PyObject *ret = Py_BuildValue("{s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i}",
		"ground",tile->ground,
		"goUp",tile->goUp,
		"goDown",tile->goDown,
		"blocking",tile->blocking,
		"speed",tile->speed,
		"requireRope",tile->requireRope,
		"requireUse",tile->requireUse,
		"requireShovel",tile->requireShovel,
		"canWalkThrough",tile->canWalkThrough,
		"isDepot",tile->isDepot,
		"isContainer",tile->isContainer,
		"notMoveable",tile->notMoveable,
		"stackable",tile->stackable,
		"alwaysOnTop",tile->alwaysOnTop,
		"moreAlwaysOnTop",tile->moreAlwaysOnTop);
	
	return ret;
}
static PyObject *tibiaauto_reader_setGlobalVariable(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	char *arg1,*arg2;
    if (!PyArg_ParseTuple(args, "ss", &arg1,&arg2)) return NULL;	
	reader.setGlobalVariable(arg1,arg2);
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_reader_getGlobalVariable(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	char *arg1;
    if (!PyArg_ParseTuple(args, "s", &arg1)) return NULL;	
	char *ret1=reader.getGlobalVariable(arg1);	
	PyObject *ret = Py_BuildValue("s",ret1);
		
	return ret; 
}
static PyObject *tibiaauto_sender_useWithObjectFromFloorOnFloor(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9;
    if (!PyArg_ParseTuple(args, "iiiiiiiii", &arg1,&arg2,&arg3,&arg4,&arg5,&arg6,&arg7,&arg8,&arg9)) return NULL;	
	sender.useWithObjectFromFloorOnFloor(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_useWithObjectFromFloorInContainer(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8;
    if (!PyArg_ParseTuple(args, "iiiiiiii", &arg1,&arg2,&arg3,&arg4,&arg5,&arg6,&arg7,&arg8)) return NULL;	
	sender.useWithObjectFromFloorInContainer(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_useWithObjectFromContainerInContainer(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5,arg6,arg7;
    if (!PyArg_ParseTuple(args, "iiiiiii", &arg1,&arg2,&arg3,&arg4,&arg5,&arg6,&arg7)) return NULL;	
	sender.useWithObjectFromContainerInContainer(arg1,arg2,arg3,arg4,arg5,arg6,arg7);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_useWithObjectFromContainerOnFloor(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8;
    if (!PyArg_ParseTuple(args, "iiiiiiii", &arg1,&arg2,&arg3,&arg4,&arg5,&arg6,&arg7,&arg8)) return NULL;	
	sender.useWithObjectFromContainerOnFloor(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_useWithObjectOnFloor(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5,arg6;
    if (!PyArg_ParseTuple(args, "iiiiii", &arg1,&arg2,&arg3,&arg4,&arg5,&arg6)) return NULL;	
	sender.useWithObjectOnFloor(arg1,arg2,arg3,arg4,arg5,arg6);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_useWithObjectInContainer(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5;
    if (!PyArg_ParseTuple(args, "iiiii", &arg1,&arg2,&arg3,&arg4,&arg5)) return NULL;	
	sender.useWithObjectInContainer(arg1,arg2,arg3,arg4,arg5);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *tibiaauto_sender_moveObjectFromFloorToFloor(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8;
    if (!PyArg_ParseTuple(args, "iiiiiiii", &arg1,&arg2,&arg3,&arg4,&arg5,&arg6,&arg7,&arg8)) return NULL;	
	sender.moveObjectFromFloorToFloor(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_moveObjectFromFloorToContainer(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5,arg6,arg7;
    if (!PyArg_ParseTuple(args, "iiiiiii", &arg1,&arg2,&arg3,&arg4,&arg5,&arg6,&arg7)) return NULL;	
	sender.moveObjectFromFloorToContainer(arg1,arg2,arg3,arg4,arg5,arg6,arg7);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_moveObjectBetweenContainers(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5,arg6;
    if (!PyArg_ParseTuple(args, "iiiiii", &arg1,&arg2,&arg3,&arg4,&arg5,&arg6)) return NULL;	
	sender.moveObjectBetweenContainers(arg1,arg2,arg3,arg4,arg5,arg6);
	Py_INCREF(Py_None);
	return Py_None; 
}

static PyObject *tibiaauto_sender_moveObjectFromContainerToFloor(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5,arg6,arg7;
    if (!PyArg_ParseTuple(args, "iiiiiii", &arg1,&arg2,&arg3,&arg4,&arg5,&arg6,&arg7)) return NULL;	
	sender.moveObjectFromContainerToFloor(arg1,arg2,arg3,arg4,arg5,arg6,arg7);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_castRuneAgainstCreature(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5;
    if (!PyArg_ParseTuple(args, "iiiii", &arg1,&arg2,&arg3,&arg4,&arg5)) return NULL;	
	sender.castRuneAgainstCreature(arg1,arg2,arg3,arg4,arg5);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *tibiaauto_sender_castRuneAgainstHuman(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5,arg6,arg7;
    if (!PyArg_ParseTuple(args, "iiiiiii", &arg1,&arg2,&arg3,&arg4,&arg5,&arg6,&arg7)) return NULL;	
	sender.castRuneAgainstHuman(arg1,arg2,arg3,arg4,arg5,arg6,arg7);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_useItemOnCreature(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2;
    if (!PyArg_ParseTuple(args, "ii", &arg1,&arg2)) return NULL;	
	sender.useItemOnCreature(arg1,arg2);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_useItemFromContainerOnCreature(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4;
    if (!PyArg_ParseTuple(args, "iiii", &arg1,&arg2,&arg3,&arg4)) return NULL;	
	sender.useItemFromContainerOnCreature(arg1,arg2,arg3,arg4);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_useItemFromFloorOnCreature(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5;
    if (!PyArg_ParseTuple(args, "iiiii", &arg1,&arg2,&arg3,&arg4,&arg5)) return NULL;	
	sender.useItemFromFloorOnCreature(arg1,arg2,arg3,arg4,arg5);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *tibiaauto_sender_sendTAMessage(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	char *arg1;
    if (!PyArg_ParseTuple(args, "s", &arg1)) return NULL;	
	sender.sendTAMessage(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_useItem(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	
	sender.useItem(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_useItemOnFloor(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4;
    if (!PyArg_ParseTuple(args, "iiii", &arg1,&arg2,&arg3,&arg4)) return NULL;	
	sender.useItemOnFloor(arg1,arg2,arg3,arg4);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_useItemInContainer(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3;
    if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	sender.useItemInContainer(arg1,arg2,arg3);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_openAutoContainerFromFloor(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4;
    if (!PyArg_ParseTuple(args, "iiii", &arg1,&arg2,&arg3,&arg4)) return NULL;
	int ret1 = sender.openAutoContainerFromFloor(arg1,arg2,arg3,arg4);
	PyObject *ret = Py_BuildValue("i",ret1);
	return ret;
}
static PyObject *tibiaauto_sender_openContainerFromFloor(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5;
    if (!PyArg_ParseTuple(args, "iiiii", &arg1,&arg2,&arg3,&arg4,&arg5)) return NULL;	
	sender.openContainerFromFloor(arg1,arg2,arg3,arg4,arg5);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_openAutoContainerFromContainer(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3;
    if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	int ret1 = sender.openAutoContainerFromContainer(arg1,arg2,arg3);
	PyObject *ret = Py_BuildValue("i",ret1);
	return ret;
}
static PyObject *tibiaauto_sender_openContainerFromContainer(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4;
    if (!PyArg_ParseTuple(args, "iiii", &arg1,&arg2,&arg3,&arg4)) return NULL;	
	sender.openContainerFromContainer(arg1,arg2,arg3,arg4);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_say(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	char *arg1;
    if (!PyArg_ParseTuple(args, "s", &arg1)) return NULL;	
	sender.say(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_sayWhisper(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	char *arg1;
    if (!PyArg_ParseTuple(args, "s", &arg1)) return NULL;	
	sender.sayWhisper(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_sayYell(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	char *arg1;
    if (!PyArg_ParseTuple(args, "s", &arg1)) return NULL;	
	sender.sayYell(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_sayNPC(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	char *arg1;
    if (!PyArg_ParseTuple(args, "s", &arg1)) return NULL;	
	sender.sayNPC(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_tell(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	char *arg1,*arg2;
    if (!PyArg_ParseTuple(args, "ss", &arg1,&arg2)) return NULL;	
	sender.tell(arg1,arg2);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_sayOnChan(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	char *arg1;
	int arg2;
	int arg3;
    if (!PyArg_ParseTuple(args, "sii", &arg1,&arg2,&arg3)) return NULL;	
	sender.sayOnChan(arg1,arg2,arg3);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_npcBuy(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;
	
	int arg1;
	int arg2;
	int arg3;
	int arg4;
    if (!PyArg_ParseTuple(args, "iiii", &arg1,&arg2,&arg3,&arg4)) return NULL;	
	sender.npcBuy(arg1,arg2,arg3,arg4);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_npcSell(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;
	
	int arg1;
	int arg2;
    if (!PyArg_ParseTuple(args, "ii", &arg1,&arg2)) return NULL;	
	sender.npcSell(arg1,arg2);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_logout(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;
	
	sender.logout();
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_walkOnTAMap(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;
	CMemReaderProxy reader;
	int arg1,arg2,arg3;
	int path[10000];

    if (!PyArg_ParseTuple(args, "iii",&arg1,&arg2,&arg3)) return NULL;	
	CTibiaCharacter *selfCh = reader.readSelfCharacter();	
	
	
	CModuleUtil::findPathOnMap(selfCh->x,selfCh->y,selfCh->z,arg1,arg2,arg3,0,path);											
	int pathSize;
	for (pathSize=0;pathSize<10000&&path[pathSize];pathSize++){}										
	if (pathSize){
		CModuleUtil::executeWalk(selfCh->x,selfCh->y,selfCh->z,path);		
	}
	
	delete selfCh;

	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_sender_closeContainer(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1;
    if (!PyArg_ParseTuple(args, "i",&arg1)) return NULL;	
	sender.closeContainer(arg1);
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_sender_attackMode(PyObject *self, PyObject *args)
{	
	CPackSenderProxy sender;

	int arg1,arg2;
    if (!PyArg_ParseTuple(args, "ii",&arg1,&arg2)) return NULL;	
	sender.attackMode(arg1,arg2);
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_sender_attack(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1;
    if (!PyArg_ParseTuple(args, "i",&arg1)) return NULL;	
	sender.attack(arg1);
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_sender_follow(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1;
    if (!PyArg_ParseTuple(args, "i",&arg1)) return NULL;	
	sender.follow(arg1);
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_sender_turnLeft(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;
	
	sender.turnLeft();
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_sender_turnRight(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;
	
	sender.turnRight();
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_sender_turnUp(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;
	
	sender.turnUp();
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_sender_turnDown(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;
	
	sender.turnDown();
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_sender_stopAll(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;
	
	sender.stopAll();
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_sender_stepMulti(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15;
    if (!PyArg_ParseTuple(args, "iiiiiiiiiiiiiii",&arg1,&arg2,&arg3,&arg4,&arg5,&arg6,&arg7,&arg8,&arg9,&arg10,&arg11,&arg12,&arg13,&arg14,&arg15)) return NULL;	
	int size=15;
	if(arg15==0) size=14;
	if(arg14==0) size=13;
	if(arg13==0) size=12;
	if(arg12==0) size=11;
	if(arg11==0) size=10;
	if(arg10==0) size=9;
	if(arg9==0) size=8;
	if(arg8==0) size=7;
	if(arg7==0) size=6;
	if(arg6==0) size=5;
	if(arg5==0) size=4;
	if(arg4==0) size=3;
	if(arg3==0) size=2;
	if(arg2==0) size=1;
	if(arg1==0) size=0;

	int *path=new int[size];
	if(arg1!=0) path[0]=arg1;
	if(arg2!=0) path[1]=arg2;
	if(arg3!=0) path[2]=arg3;
	if(arg4!=0) path[3]=arg4;
	if(arg5!=0) path[4]=arg5;
	if(arg6!=0) path[5]=arg6;
	if(arg7!=0) path[6]=arg7;
	if(arg8!=0) path[7]=arg8;
	if(arg9!=0) path[8]=arg9;
	if(arg10!=0) path[9]=arg10;
	if(arg11!=0) path[10]=arg11;
	if(arg12!=0) path[11]=arg12;
	if(arg13!=0) path[12]=arg13;
	if(arg14!=0) path[13]=arg14;
	if(arg15!=0) path[14]=arg15;

	sender.stepMulti(path,size);
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_sender_sendCreatureInfo(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	char *arg1,*arg2,*arg3;
    if (!PyArg_ParseTuple(args, "sss",&arg1,&arg2,&arg3)) return NULL;	
	sender.sendCreatureInfo(arg1,arg2,arg3);
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_sender_look(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3,arg4;
    if (!PyArg_ParseTuple(args, "iiii", &arg1,&arg2,&arg3,&arg4)) return NULL;	
	sender.look(arg1,arg2,arg3,arg4);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_ignoreLook(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	
	sender.ignoreLook(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_sendAutoAimConfig(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1,arg2,arg3;
    if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	sender.sendAutoAimConfig(arg1,arg2,arg3);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_sendClearCreatureInfo(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;
	
	sender.sendClearCreatureInfo();
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_enableCName(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	
	sender.enableCName(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_sender_sendAttackedCreatureToAutoAim(PyObject *self, PyObject *args)
{
	CPackSenderProxy sender;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	
	sender.sendAttackedCreatureToAutoAim(arg1);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_map_removePointAvailable(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;

	int arg1,arg2,arg3;
    if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	tibiaMap.removePointAvailable(arg1,arg2,arg3);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_map_prohPointClear(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;
	
	tibiaMap.prohPointClear();
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_map_prohPointAdd(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;

	int arg1,arg2,arg3;
    	if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	tibiaMap.prohPointAdd(arg1,arg2,arg3);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_map_getPointUpDown(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;

	int arg1,arg2,arg3;
   	 if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	int ret1=tibiaMap.getPointUpDown(arg1,arg2,arg3);	
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_map_getPointUpDownNoProh(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;

	int arg1,arg2,arg3;
   	 if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	int ret1=tibiaMap.getPointUpDownNoProh(arg1,arg2,arg3);	
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_map_setPointUpDown(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;

	int arg1,arg2,arg3,arg4;
   	 if (!PyArg_ParseTuple(args, "iiii", &arg1,&arg2,&arg3,&arg4)) return NULL;	
	tibiaMap.setPointUpDown(arg1,arg2,arg3,arg4);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_map_getPrevPointZ(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;

	int arg1,arg2,arg3;
    	if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	int ret1=tibiaMap.getPrevPointZ(arg1,arg2,arg3);	
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_map_getPrevPointY(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;

	int arg1,arg2,arg3;
   	 if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	int ret1=tibiaMap.getPrevPointY(arg1,arg2,arg3);	
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret; 
}
static PyObject *tibiaauto_map_getPrevPointX(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;

	int arg1,arg2,arg3;
   	 if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	int ret1=tibiaMap.getPrevPointX(arg1,arg2,arg3);	
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_map_clearPrevPoint(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;
    	
	tibiaMap.clearPrevPoint();		
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_map_setPrevPoint(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;

	int arg1,arg2,arg3,arg4,arg5,arg6;
  	  if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3,&arg4,&arg5,&arg6)) return NULL;	
	tibiaMap.setPrevPoint(arg1,arg2,arg3,arg4,arg5,arg6);	
	Py_INCREF(Py_None);
	return Py_None; 
}

static PyObject *tibiaauto_map_clear(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;
	
	tibiaMap.clear();	
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_map_setPointAsAvailable(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;

	int arg1,arg2,arg3;
   	 if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	tibiaMap.setPointAsAvailable(arg1,arg2,arg3);
	Py_INCREF(Py_None);
	return Py_None;
}
static PyObject *tibiaauto_map_isPointAvailable(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;

	int arg1,arg2,arg3;
   	 if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	int ret1=tibiaMap.isPointAvailable(arg1,arg2,arg3);	
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_map_isPointAvailableNoProh(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;

	int arg1,arg2,arg3;
  	 if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	int ret1=tibiaMap.isPointAvailableNoProh(arg1,arg2,arg3);	
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_map_size(PyObject *self, PyObject *args)
{
	CTibiaMapProxy tibiaMap;
	
	int ret1=tibiaMap.size();	
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_map_isPointInMiniMap(PyObject *self, PyObject *args)
{
	CTAMiniMapProxy taMiniMap;
	
	int arg1,arg2,arg3;
  	 if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	int ret1=taMiniMap.isPointInMiniMap(arg1,arg2,arg3);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_map_getMiniMapPoint(PyObject *self, PyObject *args)
{
	CTAMiniMapProxy taMiniMap;
	
	int arg1,arg2,arg3;
  	 if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;
	CTibiaMiniMapPoint *retPoint = taMiniMap.getMiniMapPoint(arg1,arg2,arg3);
	PyObject *ret = 
		Py_BuildValue("{s:i,s:i,s:i,s:i,s:i}",
		"x",retPoint->x,
		"y",retPoint->y,
		"z",retPoint->z,
		"colour",retPoint->colour,
		"speed",retPoint->speed);	

	delete retPoint;
	return ret;
}
static PyObject *tibiaauto_regexp_match(PyObject *self, PyObject *args)
{
	CRegexpProxy regexpProxy;

	char *arg1,*arg2;
  	  if (!PyArg_ParseTuple(args, "ss", &arg1,&arg2)) return NULL;	
	int ret1=regexpProxy.match(arg1,arg2);	
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_alice_respond(PyObject *self, PyObject *args)
{
	CAliceProxy alice;

	char *arg1,*arg2;
 	   if (!PyArg_ParseTuple(args, "ss", &arg1,&arg2)) return NULL;	
	char *ret1=alice.respond(arg1,arg2);	
	PyObject *ret = Py_BuildValue("s",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_getName(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;

	int arg1;
  	  if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	
	char *ret1=itemProxy.getName(arg1);	
	PyObject *ret = Py_BuildValue("s",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_getObjectId(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;

	char *arg1;
    if (!PyArg_ParseTuple(args, "s", &arg1)) return NULL;
	int ret1=itemProxy.getObjectId(arg1);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_getValueForConst(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;

	char *arg1;
    if (!PyArg_ParseTuple(args, "s", &arg1)) return NULL;
	int ret1=itemProxy.getValueForConst(arg1);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_refreshItemLists(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;
	
	itemProxy.refreshItemLists();
	Py_INCREF(Py_None);
	return Py_None; 
}
static PyObject *tibiaauto_item_getCorpseIdByCreatureName(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;

	char *arg1;
    if (!PyArg_ParseTuple(args, "s", &arg1)) return NULL;
	int ret1=itemProxy.getCorpseIdByCreatureName(arg1);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}

static PyObject *tibiaauto_item_getItemsItems(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	char *ret1=itemProxy.getItemsItems(arg1);
	PyObject *ret = Py_BuildValue("s",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_getItemsItemsId(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	int ret1=itemProxy.getItemsItemsId(arg1);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_getItemsItemsCount(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;
	
	int ret1=itemProxy.getItemsItemsCount();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_getItemsFood(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	char *ret1=itemProxy.getItemsFood(arg1);
	PyObject *ret = Py_BuildValue("s",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_getItemsFoodId(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	int ret1=itemProxy.getItemsFoodId(arg1);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret; 
}
static PyObject *tibiaauto_item_getItemsFoodCount(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;
	
	int ret1=itemProxy.getItemsFoodCount();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_getItemsCorpses(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	char *ret1=itemProxy.getItemsCorpses(arg1);
	PyObject *ret = Py_BuildValue("s",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_getItemsCorpsesId(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	int ret1=itemProxy.getItemsCorpsesId(arg1);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_getItemsCorpsesCount(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;
	
	int ret1=itemProxy.getItemsCorpsesCount();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_getItemsLooted(PyObject *self, PyObject *args)
{	
	CTibiaItemProxy itemProxy;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	char *ret1=itemProxy.getItemsLooted(arg1);
	PyObject *ret = Py_BuildValue("",ret1);
	
	return ret;
}
static PyObject *tibiaauto_item_getItemsLootedId(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	int ret1=itemProxy.getItemsLootedId(arg1);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret; 
}
static PyObject *tibiaauto_item_getItemsLootedCount(PyObject *self, PyObject *args)
{
	CTibiaItemProxy itemProxy;
	
	int ret1=itemProxy.getItemsLootedCount();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}

static PyObject *tibiaauto_kernel_startModule(PyObject *self,PyObject *args)
{
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	if (arg1>=CModuleProxy::allModulesCount||arg1<0) return NULL;

	CModuleProxy::allModules[arg1]->start();
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *tibiaauto_kernel_stopModule(PyObject *self,PyObject *args)
{
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	if (arg1>=CModuleProxy::allModulesCount||arg1<0) return NULL;

	CModuleProxy::allModules[arg1]->stop();
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *tibiaauto_kernel_getModuleCount(PyObject *self,PyObject *args)
{
	int ret1=CModuleProxy::allModulesCount;
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}

static PyObject *tibiaauto_kernel_getModuleName(PyObject *self,PyObject *args)
{
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	if (arg1>=CModuleProxy::allModulesCount||arg1<0) return NULL;

	char *ret1=CModuleProxy::allModules[arg1]->moduleName;
	PyObject *ret = Py_BuildValue("s",ret1);
	
	return ret;
}

static PyObject *tibiaauto_kernel_getModuleDesc(PyObject *self,PyObject *args)
{
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	if (arg1>=CModuleProxy::allModulesCount||arg1<0) return NULL;

	char *ret1=CModuleProxy::allModules[arg1]->getName();
	PyObject *ret = Py_BuildValue("s",ret1);
	
	return ret;
}

static PyObject *tibiaauto_kernel_getModuleVersion(PyObject *self,PyObject *args)
{
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	if (arg1>=CModuleProxy::allModulesCount||arg1<0) return NULL;

	char *ret1=CModuleProxy::allModules[arg1]->getVersion();
	PyObject *ret = Py_BuildValue("s",ret1);
	
	return ret;
}

static PyObject *tibiaauto_kernel_isModuleStarted(PyObject *self,PyObject *args)
{
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	if (arg1>=CModuleProxy::allModulesCount||arg1<0) return NULL;

	int ret1=CModuleProxy::allModules[arg1]->isStarted();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}


static PyObject *tibiaauto_kernel_startPythonModule(PyObject *self,PyObject *args)
{
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	if (arg1>=CPythonScript::getScriptCount()||arg1<0) return NULL;

	CPythonScript::getScriptByNr(arg1)->setEnabled(1);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *tibiaauto_kernel_stopPythonModule(PyObject *self,PyObject *args)
{
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	if (arg1>=CPythonScript::getScriptCount()||arg1<0) return NULL;

	CPythonScript::getScriptByNr(arg1)->setEnabled(0);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *tibiaauto_kernel_getPythonModuleCount(PyObject *self,PyObject *args)
{
	int ret1=CPythonScript::getScriptCount();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}

static PyObject *tibiaauto_kernel_getPythonModuleName(PyObject *self,PyObject *args)
{
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	if (arg1>=CPythonScript::getScriptCount()||arg1<0) return NULL;

	char *ret1=CPythonScript::getScriptByNr(arg1)->getFileName();
	PyObject *ret = Py_BuildValue("s",ret1);
	
	return ret;
}

static PyObject *tibiaauto_kernel_getPythonModuleDesc(PyObject *self,PyObject *args)
{
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	if (arg1>=CPythonScript::getScriptCount()||arg1<0) return NULL;

	char *ret1=CPythonScript::getScriptByNr(arg1)->getName();
	PyObject *ret = Py_BuildValue("s",ret1);
	
	return ret;
}

static PyObject *tibiaauto_kernel_getPythonModuleVersion(PyObject *self,PyObject *args)
{
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	if (arg1>=CPythonScript::getScriptCount()||arg1<0) return NULL;

	char *ret1=CPythonScript::getScriptByNr(arg1)->getVersion();
	PyObject *ret = Py_BuildValue("s",ret1);
	
	return ret;
}



static PyObject *tibiaauto_kernel_isPythonModuleStarted(PyObject *self,PyObject *args)
{
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	if (arg1>=CPythonScript::getScriptCount()||arg1<0) return NULL;

	int ret1=CPythonScript::getScriptByNr(arg1)->isEnabled();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}

static PyObject *tibiaauto_reader_readMiniMap(PyObject *self,PyObject *args)
{
	CMemReaderProxy reader;
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	

	CTibiaMiniMap *miniMap = reader.readMiniMap(arg1);

	if (!miniMap) return NULL;
	
	PyObject *ret = Py_BuildValue("{s:i,s:i,s:i,s:i,s:i,s:i}",
		"x",miniMap->x,
		"y",miniMap->y,
		"z",miniMap->z,
		"pointsAttached",miniMap->pointsAttached,
		"segmentPrev",miniMap->segmentPrev,
		"segmentNext",miniMap->segmentNext);
	delete miniMap;
	
	return ret;
}

static PyObject *tibiaauto_reader_readMiniMapLabel(PyObject *self,PyObject *args)
{
	CMemReaderProxy reader;
	int arg1,arg2;
    if (!PyArg_ParseTuple(args, "ii", &arg1,&arg2)) return NULL;	

	CTibiaMiniMapLabel *MiniMapLabel = reader.readMiniMapLabel(arg1,arg2);

	if (!MiniMapLabel) return NULL;
	
	PyObject *ret = Py_BuildValue("{s:i,s:i,s:i,s:s}",
		"x",MiniMapLabel->x,
		"y",MiniMapLabel->y,
		"type",MiniMapLabel->type,
		"desc",MiniMapLabel->desc);
	delete MiniMapLabel;
	
	return ret;
}

static PyObject *tibiaauto_reader_readMiniMapPoint(PyObject *self,PyObject *args)
{
	CMemReaderProxy reader;
	int arg1,arg2,arg3;
    if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;

	CTibiaMiniMapPoint *MiniMapPoint = reader.readMiniMapPoint(arg1,arg2,arg3);

	if (!MiniMapPoint) return NULL;
	
	PyObject *ret = Py_BuildValue("{s:i,s:i,s:i,s:i,s:i}",
		"x",MiniMapPoint->x,
		"y",MiniMapPoint->y,
		"z",MiniMapPoint->z,
		"colour",MiniMapPoint->colour,
		"speed",MiniMapPoint->speed);
	delete MiniMapPoint;
	
	return ret;
}

static PyObject *tibiaauto_crstat_count(PyObject *self, PyObject *args)
{
	CCreaturesReaderProxy cReader;
	int arg1,arg2,arg3;
	if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	
	
	int ret1=cReader.findCreatureStatForLocationCount(arg1,arg2,arg3);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}

static PyObject *tibiaauto_crstat_tibiaId(PyObject *self, PyObject *args)
{
	CCreaturesReaderProxy cReader;
	int arg1,arg2,arg3,arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1,&arg2,&arg3,&arg4)) return NULL;	
	
	int ret1=cReader.findCreatureStatForLocationTibiaId(arg1,arg2,arg3,arg4);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}

static PyObject *tibiaauto_crstat_name(PyObject *self, PyObject *args)
{
	CCreaturesReaderProxy cReader;
	int arg1,arg2,arg3,arg4;
	if (!PyArg_ParseTuple(args, "iiii", &arg1,&arg2,&arg3,&arg4)) return NULL;	
	
	char *ret1=cReader.findCreatureStatForLocationName(arg1,arg2,arg3,arg4);
	PyObject *ret;
	if (ret1)
	{
		ret = Py_BuildValue("s",ret1);
		delete ret1;
	} else {
		ret=Py_None;
	}
	
	return ret;
}

static PyObject *tibiaauto_crstat_inarea(PyObject *self, PyObject *args)
{
	CCreaturesReaderProxy cReader;
	int arg1,arg2,arg3,arg4,arg5;
	if (!PyArg_ParseTuple(args, "iiiii", &arg1,&arg2,&arg3,&arg4,&arg5)) return NULL;	
	
	char **ret1=cReader.findCreatureStatInArea(arg1,arg2,arg3,arg4,arg5);
	int len=0;
	for (len=0;ret1[len];) len++;

	PyObject *ret=PyList_New(len);
	int i;
	for (i=0;i<len;i++)
	{
		PyObject *retSub=Py_BuildValue("s",ret1[i]);
		Py_INCREF(retSub);
		PyList_SetItem(ret,i,retSub);
		free(ret1[i]);
	}
	free(ret1);
		
	
	return ret;
}

static PyObject *tibiaauto_reader_setMainWindowText(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	char *arg1;
    if (!PyArg_ParseTuple(args, "s", &arg1)) return NULL;	
	reader.setMainWindowText(arg1);
	Py_INCREF(Py_None);
	return Py_None; 
}

static PyObject *tibiaauto_reader_setMainTrayText(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	char *arg1;
    if (!PyArg_ParseTuple(args, "s", &arg1)) return NULL;	
	reader.setMainTrayText(arg1);
	Py_INCREF(Py_None);
	return Py_None; 
}

static PyObject *tibiaauto_reader_getPlayerModeAttackPlayers(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;

	int ret1=reader.getPlayerModeAttackPlayers();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_getPlayerModeAttackType(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;

	int ret1=reader.getPlayerModeAttackType();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_getPlayerModeFollow(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;

	int ret1=reader.getPlayerModeFollow();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}

static PyObject *tibiaauto_reader_getConnectionState(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;

	int ret1=reader.getConnectionState();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}

static PyObject *tibiaauto_reader_getOpenWindowName(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;

	char *ret1=reader.getOpenWindowName();
	PyObject *ret = Py_BuildValue("s",ret1);
	
	return ret;
}

static PyObject *tibiaauto_reader_setXRayValues(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1,arg2;
    if (!PyArg_ParseTuple(args, "ii", &arg1,&arg2)) return NULL;	

	reader.setXRayValues(arg1,arg2);
	Py_INCREF(Py_None);
	return Py_None; 
}


static PyObject *tibiaauto_reader_getXRayValue1(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;

	int ret1=reader.getXRayValue1();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}


static PyObject *tibiaauto_reader_getXRayValue2(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;

	int ret1=reader.getXRayValue2();
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}


static PyObject *tibiaauto_reader_writeCreatureDeltaXY(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1,arg2,arg3;
    if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;	

	reader.writeCreatureDeltaXY(arg1,arg2,arg3);
	Py_INCREF(Py_None);
	return Py_None; 
}


static PyObject *tibiaauto_reader_setAttackedCreature(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	

	reader.setAttackedCreature(arg1);
	Py_INCREF(Py_None);
	return Py_None; 
}

static PyObject *tibiaauto_reader_getCreatureDeltaX(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;

	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	
	int ret1=reader.getCreatureDeltaX(arg1);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}

static PyObject *tibiaauto_reader_getCreatureDeltaY(PyObject *self, PyObject *args)
{	
	CMemReaderProxy reader;
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;	
	int ret1=reader.getCreatureDeltaY(arg1);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}

static PyObject *tibiaauto_reader_readVIPEntry(PyObject *self, PyObject *args)
{ 
	CMemReaderProxy reader;
	
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	CTibiaVIPEntry *vip = reader.readVIPEntry(arg1);

	if (!vip) return NULL;

	PyObject *ret = 
		Py_BuildValue("{s:i,s:s,s:i,s:i}",
		"id",vip->id,
		"name",vip->name,
		"status",vip->status,
		"icon",vip->icon);

	delete vip;
	return ret;
}
static PyObject *tibiaauto_reader_mapGetPointTopPos(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1,arg2,arg3;
    if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;

	int ret1=itemOnTopIndex(arg1,arg2,arg3);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
static PyObject *tibiaauto_reader_mapGetPointSeenOnTopPos(PyObject *self, PyObject *args)
{
	CMemReaderProxy reader;

	int arg1,arg2,arg3;
    if (!PyArg_ParseTuple(args, "iii", &arg1,&arg2,&arg3)) return NULL;

	int ret1=itemSeenOnTopIndex(arg1,arg2,arg3);
	PyObject *ret = Py_BuildValue("i",ret1);
	
	return ret;
}
