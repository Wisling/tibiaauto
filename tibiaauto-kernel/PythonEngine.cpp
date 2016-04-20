// PythonEngine.cpp: implementation of the CPythonEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto.h"
#include "PythonEngine.h"
#include <MemReader.h>
#include <PackSender.h>
#include "PythonScript.h"
#include <IPCBackPipe.h>
#include "TibiaMiniMapPoint.h"
#include <TibiaItem.h>
#include <ModuleUtil.h>
#include "PythonEngine_fun.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG


int CPythonEngine::initialised = 0;

CRITICAL_SECTION ScriptEngineCriticalSection;
#define MAX_PAYLOAD_LEN 1024

struct parcelRecvActionData {
	int handle; //the pointer to the PyObject fun is used as a unique handle.
	int totalCount;
	int countLeft;
	int len;
	char actionData[MAX_PAYLOAD_LEN - sizeof(int) * 4];
	parcelRecvActionData()
	{
		memset(actionData, 0, sizeof(actionData));
	}
};
//////////////////////////////////////////////////////////////////////
// Python tibiaauto
//////////////////////////////////////////////////////////////////////

int registerPluginCount = 0;

static PyObject *tibiaauto_tibiaauto_registerPlugin(PyObject *self, PyObject *args)
{
	registerPluginCount++;

	CPythonScript *pythonScript = new CPythonScript();
	CPythonEngine pythonEngine;


	PyObject *pluginClass = NULL;
	if (!PyArg_ParseTuple(args, "O", &pluginClass))
		return NULL;

	Py_XINCREF(pluginClass);
	pythonScript->setPluginClass(pluginClass);

	PyObject *pluginObject = PyInstance_New(pluginClass, NULL, NULL);
	if (pluginObject == NULL)
	{
		AfxMessageBox("registerObject() takes class as an argument");
		Py_XDECREF(pluginClass);
		return NULL;
	}
	Py_XINCREF(pluginObject);
	pythonScript->setPluginObject(pluginObject);

	// getName
	PyObject *result = PyObject_CallMethod(pluginObject, "getName", "()");
	pythonScript->setName(PyString_AsString(result));
	Py_XDECREF(result);

	// getVersion
	result = PyObject_CallMethod(pluginObject, "getVersion", "()");
	pythonScript->setVersion(PyString_AsString(result));
	Py_XDECREF(result);


	// getFunDef
	int nr;
	for (nr = 0;; nr++)
	{
		int type, interval;
		char *matchExpr = NULL;
		int regLen;
		PyObject *periodicalFun;

		result = PyObject_CallMethod(pluginObject, "getFunDef", "(i)", nr);
		if (result == NULL)
		{
			AfxMessageBox("getFunDef() call failed");
			return NULL;
		}

		//nested if statements are for trying to read the two possible sets of parameters
		type = -255;
		if (PyArg_ParseTuple(result, "|iiO", &type, &interval, &periodicalFun))
		{
			if (type != -255)
				pythonScript->addFunDef(type, interval, periodicalFun);
			else
				break;
		}
		else
		{
			PyErr_Clear();
			if (PyArg_ParseTuple(result, "|is#O", &type, &matchExpr, &regLen, &periodicalFun))
			{
				if (type != -255)
					pythonScript->addFunDef(type, matchExpr, regLen, periodicalFun);
				else
					break;
			}
			else
			{
				Py_XDECREF(result);
				return NULL;
			}
		}


		Py_XDECREF(result);
	}
	// getConfigParam
	for (nr = 0;; nr++)
	{
		char *name = NULL, *description = NULL;
		result = PyObject_CallMethod(pluginObject, "getConfigParam", "(i)", nr);
		if (result == NULL)
		{
			AfxMessageBox("getConfigParam() call failed");
			return NULL;
		}
		if (PyArg_ParseTuple(result, "|ss", &name, &description))
		{
			if (name != NULL && description != NULL)
				pythonScript->addParamDef(name, description);
			else
				break;
		}
		else
		{
			Py_XDECREF(result);
			return NULL;
		}
		Py_XDECREF(result);
	}


	Py_XDECREF(pluginObject);
	Py_XDECREF(pluginClass);

	Py_INCREF(Py_None);

	return Py_None;
}

/*
   //////////////////////////////////////////////////////////////////////
   // Python tareader
   //////////////////////////////////////////////////////////////////////


   static PyObject *tibiaauto_reader_setProcessId(PyObject *self, PyObject *args)
   {
        CMemReader& reader = CMemReader::getMemReader();

        int processId;

    if (!PyArg_ParseTuple(args, "i", &processId)) return NULL;
        reader.setProcessId(processId);
        Py_INCREF(Py_None);
        return Py_None;
   }

   static PyObject *tibiaauto_reader_readSelfCharacter(PyObject *self, PyObject *args)
   {
        CMemReader& reader = CMemReader::getMemReader();

        CTibiaCharacter selfCh;
         reader.readSelfCharacter(&selfCh);
        PyObject *ret =
                Py_BuildValue("{s:i,s:i,s:i,s:i,s:i}",
                "hp",selfCh->hp,
                "mana",selfCh->mana,
                "x",selfCh->x,
                "y",selfCh->y,
                "z",selfCh->z);

        delete selfCh;
        return ret;
   }

   //////////////////////////////////////////////////////////////////////
   // Python tasender
   //////////////////////////////////////////////////////////////////////

   static PyObject *tibiaauto_sender_turnUp(PyObject *self, PyObject *args)
   {
        CPackSenderProxy sender;

    CPackSender::turnUp();
        Py_INCREF(Py_None);
        return Py_None;
   }

   static PyObject *tibiaauto_sender_turnDown(PyObject *self, PyObject *args)
   {
        CPackSenderProxy sender;

    CPackSender::turnDown();
        Py_INCREF(Py_None);
        return Py_None;
   }
 */


//////////////////////////////////////////////////////////////////////
// Python function list definitions
//////////////////////////////////////////////////////////////////////


static PyMethodDef Methods_tibiaauto[] = {
	{"registerPlugin", tibiaauto_tibiaauto_registerPlugin, METH_VARARGS},
	{NULL, NULL}         /* Sentinel */
};

static PyMethodDef Methods_tareader[] = {
	{"setProcessId", tibiaauto_reader_setProcessId, METH_VARARGS},
	{"readSelfCharacter", tibiaauto_reader_readSelfCharacter, METH_VARARGS},
	{"readOpenContainerCount", tibiaauto_reader_readOpenContainerCount, METH_VARARGS},
	{"readContainer", tibiaauto_reader_readContainer, METH_VARARGS},
	{"readContainerItem", tibiaauto_reader_readContainerItem, METH_VARARGS},
	{"writeSelfLightPower", tibiaauto_reader_writeSelfLightPower, METH_VARARGS},
	{"writeSelfLightColor", tibiaauto_reader_writeSelfLightColor, METH_VARARGS},
	{"readSelfLightPower", tibiaauto_reader_readSelfLightPower, METH_VARARGS},
	{"readSelfLightColor", tibiaauto_reader_readSelfLightColor, METH_VARARGS},
	{"cancelAttackCoords", tibiaauto_reader_cancelAttackCoords, METH_VARARGS},
	{"writeGotoCoords", tibiaauto_reader_writeGotoCoords, METH_VARARGS},
	{"getLoggedCharNr", tibiaauto_reader_getLoggedCharNr, METH_VARARGS},
	{"getSelfEventFlags", tibiaauto_reader_getSelfEventFlags, METH_VARARGS},
	{"writeVisibleCreatureName", tibiaauto_reader_writeVisibleCreatureName, METH_VARARGS},
	{"getTradeItemPartner", tibiaauto_reader_getTradeItemPartner, METH_VARARGS},
	{"getTradeItemSelf", tibiaauto_reader_getTradeItemSelf, METH_VARARGS},
	{"getTradeCountPartner", tibiaauto_reader_getTradeCountPartner, METH_VARARGS},
	{"getTradeCountSelf", tibiaauto_reader_getTradeCountSelf, METH_VARARGS},
	{"getAttackedCreature", tibiaauto_reader_getAttackedCreature, METH_VARARGS},
	{"setAttackedCreature", tibiaauto_reader_setAttackedCreature, METH_VARARGS},
	{"getFollowedCreature", tibiaauto_reader_getFollowedCreature, METH_VARARGS},
	{"setFollowedCreature", tibiaauto_reader_setFollowedCreature, METH_VARARGS},
	{"GetLoggedChar", tibiaauto_reader_GetLoggedChar, METH_VARARGS},
	{"readBattleListMax", tibiaauto_reader_readBattleListMax, METH_VARARGS},
	{"readBattleListMin", tibiaauto_reader_readBattleListMin, METH_VARARGS},
	{"readVisibleCreature", tibiaauto_reader_readVisibleCreature, METH_VARARGS},
	{"readItem", tibiaauto_reader_readItem, METH_VARARGS},
	{"getCharacterByTibiaId", tibiaauto_reader_getCharacterByTibiaId, METH_VARARGS},
	{"mapGetPointItemsCount", tibiaauto_reader_mapGetPointItemsCount, METH_VARARGS},
	{"mapGetPointItemId", tibiaauto_reader_mapGetPointItemId, METH_VARARGS},
	{"mapSetPointItemsCount", tibiaauto_reader_mapSetPointItemsCount, METH_VARARGS},
	{"mapSetPointItemId", tibiaauto_reader_mapSetPointItemId, METH_VARARGS},
	{"mapGetPointItemExtraInfo", tibiaauto_reader_mapGetPointItemExtraInfo, METH_VARARGS},
	{"mapGetPointStackIndex", tibiaauto_reader_mapGetPointStackIndex, METH_VARARGS},
	{"getCurrentTm", tibiaauto_reader_getCurrentTm, METH_VARARGS},
	{"setRemainingTilesToGo", tibiaauto_reader_setRemainingTilesToGo, METH_VARARGS},
	{"setMemIntValue", tibiaauto_reader_setMemIntValue, METH_VARARGS},
	{"getMemIntValue", tibiaauto_reader_getMemIntValue, METH_VARARGS},
	{"writeEnableRevealCName", tibiaauto_reader_writeEnableRevealCName, METH_VARARGS},
	{"writeDisableRevealCName", tibiaauto_reader_writeDisableRevealCName, METH_VARARGS},
	{"getProcessId", tibiaauto_reader_getProcessId, METH_VARARGS},
	{"getKernelMainVersion", tibiaauto_reader_getKernelMainVersion, METH_VARARGS},
	{"getKernelPatchVersion", tibiaauto_reader_getKernelPatchVersion, METH_VARARGS},
	{"writeCreatureLightPower", tibiaauto_reader_writeCreatureLightPower, METH_VARARGS},
	{"writeCreatureLightColor", tibiaauto_reader_writeCreatureLightColor, METH_VARARGS},
	{"readCreatureLightPower", tibiaauto_reader_readCreatureLightPower, METH_VARARGS},
	{"readCreatureLightColor", tibiaauto_reader_readCreatureLightColor, METH_VARARGS},
	{"getTibiaTile", tibiaauto_reader_getTibiaTile, METH_VARARGS},
	{"setGlobalVariable", tibiaauto_reader_setGlobalVariable, METH_VARARGS},
	{"getGlobalVariable", tibiaauto_reader_getGlobalVariable, METH_VARARGS},
	{"readMiniMap", tibiaauto_reader_readMiniMap, METH_VARARGS},
	{"readMiniMapPoint", tibiaauto_reader_readMiniMapPoint, METH_VARARGS},
	{"readMiniMapLabel", tibiaauto_reader_readMiniMapLabel, METH_VARARGS},
	{"writeMiniMapPoint", tibiaauto_reader_writeMiniMapPoint, METH_VARARGS},
	{"setMainWindowText", tibiaauto_reader_setMainWindowText, METH_VARARGS},
	{"setMainTrayText", tibiaauto_reader_setMainTrayText, METH_VARARGS},
	{"getPlayerModeAttackPlayers", tibiaauto_reader_getPlayerModeAttackPlayers, METH_VARARGS},
	{"getPlayerModeAttackType", tibiaauto_reader_getPlayerModeAttackType, METH_VARARGS},
	{"getPlayerModeFollow", tibiaauto_reader_getPlayerModeFollow, METH_VARARGS},
	{"getPlayerModePVP", tibiaauto_reader_getPlayerModePVP, METH_VARARGS},
	{"getOpenWindowName", tibiaauto_reader_getOpenWindowName, METH_VARARGS},
	{"getConnectionState", tibiaauto_reader_getConnectionState, METH_VARARGS},
	{"isLoggedIn", tibiaauto_reader_isLoggedIn, METH_VARARGS},
	{"setXRayValues", tibiaauto_reader_setXRayValues, METH_VARARGS},
	{"getXRayValue1", tibiaauto_reader_getXRayValue1, METH_VARARGS},
	{"getXRayValue2", tibiaauto_reader_getXRayValue2, METH_VARARGS},
	{"writeCreatureDeltaXY", tibiaauto_reader_writeCreatureDeltaXY, METH_VARARGS},
	{"getCreatureDeltaX", tibiaauto_reader_getCreatureDeltaX, METH_VARARGS},
	{"getCreatureDeltaY", tibiaauto_reader_getCreatureDeltaY, METH_VARARGS},
	{"readVIPEntry", tibiaauto_reader_readVIPEntry, METH_VARARGS},
	{"mapGetPointTopPos", tibiaauto_reader_mapGetPointTopPos, METH_VARARGS},
	{"mapGetPointSeenOnTopPos", tibiaauto_reader_mapGetPointSeenOnTopPos, METH_VARARGS},
	{"getBaseAddr", tibiaauto_reader_getBaseAddr, METH_VARARGS},
	{NULL, NULL}         /* Sentinel */
};

static PyMethodDef Methods_tasender[] = {
	{"useWithObjectFromFloorOnFloor", tibiaauto_sender_useWithObjectFromFloorOnFloor, METH_VARARGS},
	{"useWithObjectFromFloorInContainer", tibiaauto_sender_useWithObjectFromFloorInContainer, METH_VARARGS},
	{"useWithObjectFromContainerInContainer", tibiaauto_sender_useWithObjectFromContainerInContainer, METH_VARARGS},
	{"useWithObjectFromContainerOnFloor", tibiaauto_sender_useWithObjectFromContainerOnFloor, METH_VARARGS},
	{"useWithObjectOnFloor", tibiaauto_sender_useWithObjectOnFloor, METH_VARARGS},
	{"useWithObjectInContainer", tibiaauto_sender_useWithObjectInContainer, METH_VARARGS},
	{"moveObjectFromFloorToFloor", tibiaauto_sender_moveObjectFromFloorToFloor, METH_VARARGS},
	{"moveObjectFromFloorToContainer", tibiaauto_sender_moveObjectFromFloorToContainer, METH_VARARGS},
	{"moveObjectBetweenContainers", tibiaauto_sender_moveObjectBetweenContainers, METH_VARARGS},
	{"moveObjectFromContainerToFloor", tibiaauto_sender_moveObjectFromContainerToFloor, METH_VARARGS},
	{"castRuneAgainstCreature", tibiaauto_sender_castRuneAgainstCreature, METH_VARARGS},
	{"castRuneAgainstHuman", tibiaauto_sender_castRuneAgainstHuman, METH_VARARGS},
	{"useItemOnCreature", tibiaauto_sender_useItemOnCreature, METH_VARARGS},
	{"useItemFromContainerOnCreature", tibiaauto_sender_useItemFromContainerOnCreature, METH_VARARGS},
	{"useItemFromFloorOnCreature", tibiaauto_sender_useItemFromFloorOnCreature, METH_VARARGS},
	{"sendTAMessage", tibiaauto_sender_sendTAMessage, METH_VARARGS},
	{"useItem", tibiaauto_sender_useItem, METH_VARARGS},
	{"useItemOnFloor", tibiaauto_sender_useItemOnFloor, METH_VARARGS},
	{"useItemInContainer", tibiaauto_sender_useItemInContainer, METH_VARARGS},
	{"openAutoContainerFromFloor", tibiaauto_sender_openAutoContainerFromFloor, METH_VARARGS},
	{"openContainerFromFloor", tibiaauto_sender_openContainerFromFloor, METH_VARARGS},
	{"openAutoContainerFromContainer", tibiaauto_sender_openAutoContainerFromContainer, METH_VARARGS},
	{"openContainerFromContainer", tibiaauto_sender_openContainerFromContainer, METH_VARARGS},
	{"say", tibiaauto_sender_say, METH_VARARGS},
	{"sayWhisper", tibiaauto_sender_sayWhisper, METH_VARARGS},
	{"sayYell", tibiaauto_sender_sayYell, METH_VARARGS},
	{"sayNPC", tibiaauto_sender_sayNPC, METH_VARARGS},
	{"npcBuy", tibiaauto_sender_npcBuy, METH_VARARGS},
	{"npcSell", tibiaauto_sender_npcSell, METH_VARARGS},
	{"tell", tibiaauto_sender_tell, METH_VARARGS},
	{"sayOnChan", tibiaauto_sender_sayOnChan, METH_VARARGS},
	{"logout", tibiaauto_sender_logout, METH_VARARGS},
	{"walkOnTAMap", tibiaauto_sender_walkOnTAMap, METH_VARARGS},
	{"closeContainer", tibiaauto_sender_closeContainer, METH_VARARGS},
	{"attackMode", tibiaauto_sender_attackMode, METH_VARARGS},
	{"attack", tibiaauto_sender_attack, METH_VARARGS},
	{"follow", tibiaauto_sender_follow, METH_VARARGS},
	{"turnLeft", tibiaauto_sender_turnLeft, METH_VARARGS},
	{"turnRight", tibiaauto_sender_turnRight, METH_VARARGS},
	{"turnUp", tibiaauto_sender_turnUp, METH_VARARGS},
	{"turnDown", tibiaauto_sender_turnDown, METH_VARARGS},
	{"sendCreatureInfo", tibiaauto_sender_sendCreatureInfo, METH_VARARGS},
	{"look", tibiaauto_sender_look, METH_VARARGS},
	{"ignoreLook", tibiaauto_sender_ignoreLook, METH_VARARGS},
	{"sendAutoAimConfig", tibiaauto_sender_sendAutoAimConfig, METH_VARARGS},
	{"sendClearCreatureInfo", tibiaauto_sender_sendClearCreatureInfo, METH_VARARGS},
	{"enableCName", tibiaauto_sender_enableCName, METH_VARARGS},
	{"sendAttackedCreatureToAutoAim", tibiaauto_sender_sendAttackedCreatureToAutoAim, METH_VARARGS},
	{"stopAll", tibiaauto_sender_stopAll, METH_VARARGS},
	{"stepMulti", tibiaauto_sender_stepMulti, METH_VARARGS},
	{"stepLeft", tibiaauto_sender_stepLeft, METH_VARARGS},
	{"stepRight", tibiaauto_sender_stepRight, METH_VARARGS},
	{"stepUp", tibiaauto_sender_stepUp, METH_VARARGS},
	{"stepDown", tibiaauto_sender_stepDown, METH_VARARGS},
	{"stepUpRight", tibiaauto_sender_stepUpRight, METH_VARARGS},
	{"stepDownRight", tibiaauto_sender_stepDownRight, METH_VARARGS},
	{"stepUpLeft", tibiaauto_sender_stepUpLeft, METH_VARARGS},
	{"stepDownLeft", tibiaauto_sender_stepDownLeft, METH_VARARGS},
	{"sendMount", tibiaauto_sender_sendMount, METH_VARARGS},
	{"sendDismount", tibiaauto_sender_sendDismount, METH_VARARGS},
	{"sendDirectPacket", tibiaauto_sender_sendDirectPacket, METH_VARARGS},
	{"printText", tibiaauto_sender_printText, METH_VARARGS},


	{NULL, NULL}         /* Sentinel */
};


static PyMethodDef Methods_tamap[] = {
	{"removePointAvailable", tibiaauto_map_removePointAvailable, METH_VARARGS},
	{"prohPointClear", tibiaauto_map_prohPointClear, METH_VARARGS},
	{"prohPointAdd", tibiaauto_map_prohPointAdd, METH_VARARGS},
	{"getPointUpDown", tibiaauto_map_getPointType, METH_VARARGS},
	{"getPointUpDownNoProh", tibiaauto_map_getPointTypeNoProh, METH_VARARGS},
	{"setPointUpDown", tibiaauto_map_setPointType, METH_VARARGS},
	{"getPointType", tibiaauto_map_getPointType, METH_VARARGS},
	{"getPointTypeNoProh", tibiaauto_map_getPointTypeNoProh, METH_VARARGS},
	{"setPointType", tibiaauto_map_setPointType, METH_VARARGS},
	{"getPrevPointZ", tibiaauto_map_getPrevPointZ, METH_VARARGS},
	{"getPrevPointY", tibiaauto_map_getPrevPointY, METH_VARARGS},
	{"getPrevPointX", tibiaauto_map_getPrevPointX, METH_VARARGS},
	{"clearPrevPoint", tibiaauto_map_clearPrevPoint, METH_VARARGS},
	{"setPrevPoint", tibiaauto_map_setPrevPoint, METH_VARARGS},
	{"clear", tibiaauto_map_clear, METH_VARARGS},
	{"setPointAsAvailable", tibiaauto_map_setPointAsAvailable, METH_VARARGS},
	{"isPointAvailable", tibiaauto_map_isPointAvailable, METH_VARARGS},
	{"isPointAvailableNoProh", tibiaauto_map_isPointAvailableNoProh, METH_VARARGS},
	{"size", tibiaauto_map_size, METH_VARARGS},
	{"isPointInMiniMap", tibiaauto_map_isPointInMiniMap, METH_VARARGS},
	{"getMiniMapPoint", tibiaauto_map_getMiniMapPoint, METH_VARARGS},
	{"isPointLocked", tibiaauto_map_isPointLocked, METH_VARARGS},
	{"setPointLocked", tibiaauto_map_setPointLocked, METH_VARARGS},


	{NULL, NULL}         /* Sentinel */
};


static PyMethodDef Methods_taregexp[] = {
	{"match", tibiaauto_regexp_match, METH_VARARGS},
	{NULL, NULL}         /* Sentinel */
};


static PyMethodDef Methods_taalice[] = {
	{"respond", tibiaauto_alice_respond, METH_VARARGS},

	{NULL, NULL}         /* Sentinel */
};

static PyMethodDef Methods_tacrstat[] = {
	{"findCreatureStatForLocationTibiaId", tibiaauto_crstat_tibiaId, METH_VARARGS},
	{"findCreatureStatForLocationCount", tibiaauto_crstat_count, METH_VARARGS},
	{"findCreatureStatForLocationName", tibiaauto_crstat_name, METH_VARARGS},
	{"findCreatureStatInArea", tibiaauto_crstat_inarea, METH_VARARGS},

	{NULL, NULL}         /* Sentinel */
};

static PyMethodDef Methods_taitem[] = {
	{"getItemName", tibiaauto_item_getItemName, METH_VARARGS},
	{"getItemId", tibiaauto_item_getItemId, METH_VARARGS},
	{"getItemIndex", tibiaauto_item_getItemIndex, METH_VARARGS},
	{"getFoodIndex", tibiaauto_item_getFoodIndex, METH_VARARGS},
	{"getLootItemIndex", tibiaauto_item_getLootItemIndex, METH_VARARGS},
	{"getItemIdAtIndex", tibiaauto_item_getItemIdAtIndex, METH_VARARGS},
	{"getFoodIdAtIndex", tibiaauto_item_getFoodIdAtIndex, METH_VARARGS},
	{"getLootItemIdAtIndex", tibiaauto_item_getLootItemIdAtIndex, METH_VARARGS},
	{"getItemNameAtIndex", tibiaauto_item_getItemNameAtIndex, METH_VARARGS},
	{"getFoodNameAtIndex", tibiaauto_item_getFoodNameAtIndex, METH_VARARGS},
	{"getLootItemNameAtIndex", tibiaauto_item_getLootItemNameAtIndex, METH_VARARGS},
	{"getFoodTimeAtIndex", tibiaauto_item_getFoodTimeAtIndex, METH_VARARGS},
	{"addItem", tibiaauto_item_addItem, METH_VARARGS},
	{"addFood", tibiaauto_item_addFood, METH_VARARGS},
	{"addLootItem", tibiaauto_item_addLootItem, METH_VARARGS},
	{"removeItem", tibiaauto_item_removeItem, METH_VARARGS},
	{"removeFood", tibiaauto_item_removeFood, METH_VARARGS},
	{"removeLootItem", tibiaauto_item_removeLootItem, METH_VARARGS},
	{"clearFoodList", tibiaauto_item_clearFoodList, METH_VARARGS},
	{"getItemCount", tibiaauto_item_getItemCount, METH_VARARGS},
	{"getFoodCount", tibiaauto_item_getFoodCount, METH_VARARGS},
	{"getLootItemCount", tibiaauto_item_getLootItemCount, METH_VARARGS},
	{"getValueForConst", tibiaauto_item_getValueForConst, METH_VARARGS},
	{"refreshItemLists", tibiaauto_item_refreshItemLists, METH_VARARGS},
	{"saveItemLists", tibiaauto_item_saveItemLists, METH_VARARGS},

	/* Deprecated Section Start*/
	{"getName", tibiaauto_item_getName, METH_VARARGS},
	{"getObjectId", tibiaauto_item_getObjectId, METH_VARARGS},
	{"getCorpseIdByCreatureName", tibiaauto_item_getCorpseIdByCreatureName, METH_VARARGS},
	{"getItemsItems", tibiaauto_item_getItemsItems, METH_VARARGS},
	{"getItemsItemsId", tibiaauto_item_getItemsItemsId, METH_VARARGS},
	{"getItemsFood", tibiaauto_item_getItemsFood, METH_VARARGS},
	{"getItemsCorpses", tibiaauto_item_getItemsCorpses, METH_VARARGS},
	{"getItemsCorpsesId", tibiaauto_item_getItemsCorpsesId, METH_VARARGS},
	{"getItemsCorpsesCount", tibiaauto_item_getItemsCorpsesCount, METH_VARARGS},
	{"getItemsLooted", tibiaauto_item_getItemsLooted, METH_VARARGS},
	{"getItemsLootedId", tibiaauto_item_getItemsLootedId, METH_VARARGS},
	{"getItemsItemsCount", tibiaauto_item_getItemsItemsCount, METH_VARARGS},
	{"getItemsFoodCount", tibiaauto_item_getItemsFoodCount, METH_VARARGS},
	{"getItemsLootedCount", tibiaauto_item_getItemsLootedCount, METH_VARARGS},
	/* Deprecated Section End*/


	{NULL, NULL}         /* Sentinel */
};

static PyMethodDef Methods_takernel[] = {
	{"startModule", tibiaauto_kernel_startModule, METH_VARARGS},
	{"stopModule", tibiaauto_kernel_stopModule, METH_VARARGS},
	{"getModuleList", tibiaauto_kernel_getModuleList, METH_VARARGS},
	{"getModuleDesc", tibiaauto_kernel_getModuleDesc, METH_VARARGS},
	{"getModuleVersion", tibiaauto_kernel_getModuleVersion, METH_VARARGS},
	{"isModuleStarted", tibiaauto_kernel_isModuleStarted, METH_VARARGS},
	{"getModuleParam", tibiaauto_kernel_getModuleParam, METH_VARARGS},
	{"setModuleParam", tibiaauto_kernel_setModuleParam, METH_VARARGS},

	{"startPythonModule", tibiaauto_kernel_startPythonModule, METH_VARARGS},
	{"stopPythonModule", tibiaauto_kernel_stopPythonModule, METH_VARARGS},
	{"getPythonModuleCount", tibiaauto_kernel_getPythonModuleCount, METH_VARARGS},
	{"getPythonModuleName", tibiaauto_kernel_getPythonModuleName, METH_VARARGS},
	{"isPythonModuleStarted", tibiaauto_kernel_isPythonModuleStarted, METH_VARARGS},
	{"getPythonModuleDesc", tibiaauto_kernel_getPythonModuleDesc, METH_VARARGS},
	{"getPythonModuleVersion", tibiaauto_kernel_getPythonModuleVersion, METH_VARARGS},


	{NULL, NULL}         /* Sentinel */
};

static PyMethodDef Methods_tapacket[] = {
	{"first", tibiaauto_packet_first, METH_VARARGS},// create a regex to recieve from


	{NULL, NULL}         /* Sentinel */
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPythonEngine::CPythonEngine()
{
	init();
}

CPythonEngine::~CPythonEngine()
{
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////


void CPythonEngine::init()
{
	if (!initialised)
	{
		initialised = 1;

		/* Initialize the Python interpreter.*/
		Py_Initialize();
		PyEval_InitThreads();


		PyGILState_STATE gstate;
		gstate = PyGILState_Ensure();

		/* Initialize TA Python env. */
		Py_InitModule("tibiaauto", Methods_tibiaauto);
		Py_InitModule("tareader", Methods_tareader);
		Py_InitModule("tasender", Methods_tasender);
		Py_InitModule("tamap", Methods_tamap);
		Py_InitModule("taregexp", Methods_taregexp);
		Py_InitModule("taalice", Methods_taalice);
		Py_InitModule("taitem", Methods_taitem);
		Py_InitModule("tacrstat", Methods_tacrstat);
		Py_InitModule("takernel", Methods_takernel);
		Py_InitModule("tapacket", Methods_tapacket);
		PyRun_SimpleString("import tibiaauto");
		PyRun_SimpleString("import tareader");
		PyRun_SimpleString("import tasender");
		PyRun_SimpleString("import tamap");
		PyRun_SimpleString("import taregexp");
		PyRun_SimpleString("import taalice");
		PyRun_SimpleString("import taitem");
		PyRun_SimpleString("import tacrstat");
		PyRun_SimpleString("import takernel");
		PyRun_SimpleString("import sys");
		PyRun_SimpleString("import time");

		char pathBuf[2048];
		sprintf(pathBuf, "%s\\tascripts\\tautil.py", CInstallPath::getInstallPath().c_str());

		// load tautil.py
		FILE *f = fopen(pathBuf, "r");
		if (f)
		{
			fseek(f, 0, SEEK_END);
			int fileSize = ftell(f);
			fseek(f, 0, SEEK_SET);
			char *fileBuf = (char *)malloc(fileSize + 1);
			memset(fileBuf, 0, fileSize + 1);
			fread(fileBuf, 1, fileSize, f);

			int ret = PyRun_SimpleString(fileBuf);
			if (ret == -1)
				AfxMessageBox("Loading tautil.py script failed!");
			fclose(f);
		}
		else
		{
			AfxMessageBox("Unable to find tascripts\\tautil.py!");
		}

		PyGILState_Release(gstate);

		InitializeCriticalSection(&ScriptEngineCriticalSection);


		// now load all scripts from 'tascripts' subdirectory
		sprintf(pathBuf, "%s\\tascripts\\*.py", CInstallPath::getInstallPath().c_str());
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile(pathBuf, &findFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			char buf[1024];

			do
			{
				snprintf(buf, 1023, "%s\\tascripts\\%s", CInstallPath::getInstallPath().c_str(), findFileData.cFileName);
				// tautil.py will be loaded in a special way
				if (!strstr(buf, "tascripts\\tautil.py"))
					loadScript(buf);
			}
			while (FindNextFile(hFind, &findFileData));
			FindClose(hFind);
		}
	}
}

void CPythonEngine::loadScript(char *path)
{
	init();
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();

	FILE *f = fopen(path, "rb");
	if (f)
	{
		int oldRegisterPluginCount = registerPluginCount;
		char buf[1024];
		sprintf(buf, "execfile('%s')", path);
		int pos, len;
		len = strlen(buf);
		for (pos = 0; pos < len; pos++)
		{
			if (buf[pos] == '\\')
				buf[pos] = '/';
		}
		//AfxMessageBox(buf);
		PyRun_SimpleString(buf);
		//AfxMessageBox("2");
		/*
		   AfxMessageBox("1");
		   fseek(f,0,SEEK_END);
		   int fileSize = ftell(f);
		   fseek(f,0,SEEK_SET);
		   char *fileBuf = (char *)malloc(fileSize+1);
		   memset(fileBuf,0,fileSize+1);
		   AfxMessageBox("2");
		   int l=fread(fileBuf,1,fileSize,f);
		   char bbb[128];
		   sprintf(bbb,"len=%d/fileSize=%d/delta=%d",l,fileSize,fileSize-l);
		   AfxMessageBox(bbb);
		   FILE *ff=fopen("a.txt","a+");
		   fwrite(fileBuf,1,fileSize,ff);
		   fclose(ff);

		   AfxMessageBox("3");


		   int ret=PyRun_SimpleString(fileBuf);
		   AfxMessageBox("4");
		   if (ret==-1)
		   {
		        char buf[1024];
		        sprintf(buf,"Loading script failed: python error (%s)!",path);
		        AfxMessageBox(buf);
		   }

		   free(fileBuf);
		   fclose(f);
		 */

		if (registerPluginCount == oldRegisterPluginCount)
		{
			char buf[1024];
			sprintf(buf, "Loading script failed: there must be a call to tibiaauto.registerPlugin() (%d/%d)!", registerPluginCount, oldRegisterPluginCount);
			AfxMessageBox(buf);
		}
		else
		{
			// set the just loaded script file name
			CPythonScript::getScriptByNr(CPythonScript::getScriptCount() - 1)->setFileName(path);
		}
	}
	PyGILState_Release(gstate);
}

void CPythonEngine::periodicalTick()
{
	init();
	enterCriticalSection();
	long int tm = GetTickCount();
	int scriptNr;
	for (scriptNr = 0;; scriptNr++)
	{
		CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
		if (!pythonScript)
			break;
		if (!pythonScript->isEnabled())
			continue;
		int funNr;
		for (funNr = 0;; funNr++)
		{
			struct funType *fun = pythonScript->getFunDef(funNr);
			if (!fun)
				break;

			if (fun->type == FUNTYPE_PERIODICAL && tm >= fun->tmNextExec)
			{
				CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);

				PyGILState_STATE gstate;
				gstate = PyGILState_Ensure();
				PyObject *params = pythonScript->getParamsDic();
				PyObject *result = PyObject_CallMethod(pythonScript->getPluginObject(), fun->name, "(O)", params);
				Py_XDECREF(params);
				Py_XDECREF(result);

				fun->call();

				PyGILState_Release(gstate);
			}
		}
	}
	leaveCriticalSection();
}

void CPythonEngine::backpipeMsgTick()
{
	init();
	enterCriticalSection();
	long int tm = GetTickCount();
	CMemReader& reader = CMemReader::getMemReader();
	CIpcMessage mess;

	if (CIPCBackPipe::readFromPipe(&mess, 1006))
	{
		int infoType;
		int chanType;
		int nickLen;
		int msgLen;
		char nickBuf[16384];
		char msgBuf[16384];
		char chanBuf[16384];

		memset(nickBuf, 0, 16384);
		memset(msgBuf, 0, 16384);
		memcpy(&infoType, mess.payload, sizeof(int));
		memcpy(&chanType, mess.payload + 4, sizeof(int));
		memcpy(&nickLen, mess.payload + 8, sizeof(int));
		memcpy(&msgLen, mess.payload + 12, sizeof(int));
		memcpy(nickBuf, mess.payload + 16, nickLen);
		memcpy(msgBuf, mess.payload + 16 + nickLen, msgLen);
		switch (infoType)
		{
		case 1:
			sprintf(chanBuf, "say");
			break;
		case 2:
			sprintf(chanBuf, "whisper");
			break;
		case 3:
			sprintf(chanBuf, "yell");
			break;
		case 11:
			sprintf(chanBuf, "NPC");
			break;
		case 6:
			sprintf(chanBuf, "private");
			break;
		case 7:
			sprintf(chanBuf, "channel");
			break;
		default:
			sprintf(chanBuf, "other[%d]", infoType);
			break;
		}
		//Channel IDs
		//1-Party
		//2-Own Chat Channel(premium)
		//3-Game Chat
		//4-English Channel
		//5-Trade
		//6-Rook Trade
		//7-Help

		int scriptNr;
		for (scriptNr = 0;; scriptNr++)
		{
			CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
			if (!pythonScript)
				break;
			if (!pythonScript->isEnabled())
				continue;
			int funNr;
			for (funNr = 0;; funNr++)
			{
				struct funType *fun = pythonScript->getFunDef(funNr);
				if (!fun)
					break;

				if (fun->type == FUNTYPE_MSG)
				{
					PyGILState_STATE gstate;
					gstate = PyGILState_Ensure();
					PyObject *params = pythonScript->getParamsDic();
					PyObject *result = PyObject_CallMethod(pythonScript->getPluginObject(), fun->name, "(O(isss))", params, infoType, chanBuf, nickBuf, msgBuf);
					Py_XDECREF(params);
					Py_XDECREF(result);
					fun->call();
					PyGILState_Release(gstate);
				}
			}
		}
	}
	leaveCriticalSection();
}

void CPythonEngine::unloadScript(int scriptNr)
{


	init();
	enterCriticalSection();
	CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
	pythonScript->setEnabled(false);
	struct funType *fun = NULL;
	for (int funNr = 0; (fun = pythonScript->getFunDef(funNr)); funNr++)
	{
		if (fun->type == FUNTYPE_INPACKET)
			CPackSender::unregisterInpacketRegex(fun->matchExprHandle);
	}

	CPythonScript::pythonScriptTab[scriptNr] = CPythonScript::pythonScriptTab[CPythonScript::pythonScriptCount - 1];
	CPythonScript::pythonScriptCount--;
	registerPluginCount--;
	// this is commented out by purpose - there would be too much threads
	// playing to cleanup this memory here
	//delete pythonScript;//crashes TA on reloading script possibly fix later!
	leaveCriticalSection();
}

void CPythonEngine::enterCriticalSection()
{
	init();
	EnterCriticalSection(&ScriptEngineCriticalSection);
}

void CPythonEngine::leaveCriticalSection()
{
	init();
	LeaveCriticalSection(&ScriptEngineCriticalSection);
}

void CPythonEngine::backpipeTamsgTick()
{
	init();
	enterCriticalSection();
	long int tm = GetTickCount();
	CMemReader& reader = CMemReader::getMemReader();
	CIpcMessage mess;

	if (CIPCBackPipe::readFromPipe(&mess, 1007))
	{
		int msgLen;
		char msgBuf[16384];

		memset(msgBuf, 0, 16384);
		memcpy(&msgLen, mess.payload, sizeof(int));
		memcpy(msgBuf, mess.payload + 4, msgLen);

		int scriptNr;
		for (scriptNr = 0;; scriptNr++)
		{
			CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
			if (!pythonScript)
				break;
			if (!pythonScript->isEnabled())
				continue;
			int funNr;
			for (funNr = 0;; funNr++)
			{
				struct funType *fun = pythonScript->getFunDef(funNr);
				if (!fun)
					break;

				if (fun->type == FUNTYPE_TAMSG)
				{
					PyGILState_STATE gstate;
					gstate = PyGILState_Ensure();
					PyObject *params = pythonScript->getParamsDic();
					PyObject *result = PyObject_CallMethod(pythonScript->getPluginObject(), fun->name, "(Os)", params, msgBuf);
					Py_XDECREF(params);
					Py_XDECREF(result);
					fun->call();
					PyGILState_Release(gstate);
				}
			}
		}
	}
	leaveCriticalSection();
}

// Calls python functions of type 3 when we receive notice of an incoming packet
void CPythonEngine::backpipeInpacketTick()
{
	init();
	enterCriticalSection();
	CMemReader& reader = CMemReader::getMemReader();
	CIpcMessage mess;
	struct parcelRecvActionData* pd;

	//runs at most 'maxtimes' python functions each tick
	for (int maxtimes = 5; CIPCBackPipe::readFromPipe(&mess, 1010) && maxtimes > 0; maxtimes--)
	{
		char buf[1024];

		long int tm = GetTickCount();
		pd = (parcelRecvActionData*)&(mess.payload);

		int discardIncompletePacket = pd->countLeft + 1 != pd->totalCount; //the initial packets were too old and were discarded

		int countLeft = pd->countLeft;
		int msgLen    = pd->len;
		int handle    = pd->handle;
		char* msgBuf  = (char*)malloc(sizeof(pd->actionData) * (countLeft + 1));
		memcpy(msgBuf, pd->actionData, pd->len);
		while (countLeft && GetTickCount() - tm < 200)  // wait for remaining sections to come in for up to 200ms(they should always be on their way)
		{
			if (CIPCBackPipe::readFromPipe(&mess, 1010))
			{
				if (pd->handle != handle || pd->countLeft != countLeft - 1)
					sprintf(buf, "ERROR in backpipeInpacketTick: handle %d should be %d and count %d should be %d", pd->handle, handle, pd->countLeft, countLeft - 1);
				//AfxMessageBox(buf);
				memcpy(msgBuf + msgLen, pd->actionData, pd->len);
				msgLen   += pd->len;
				countLeft = pd->countLeft;
			}
		}
		if (countLeft)
		{
			sprintf(buf, "ERROR in backpipeInpacketTick: did not finish reading data");
			//AfxMessageBox(buf);
		}
		else if (discardIncompletePacket)
		{
			int a = 0;// discard data and continue
		}
		else     //everything is OK
		{
			sprintf(buf, "One packet took %dms", GetTickCount() - tm);
			//if (GetTickCount() - tm>20) MessageBox(NULL, buf,"huaeotnre",0);
			CPythonScript *pythonScript = NULL;
			for (int scriptNr = 0; (pythonScript = CPythonScript::getScriptByNr(scriptNr)); scriptNr++)
			{
				if (!pythonScript->isEnabled())
					continue;
				struct funType *fun = NULL;
				for (int funNr = 0; (fun = pythonScript->getFunDef(funNr)); funNr++)
				{
					if (fun->type == FUNTYPE_INPACKET && fun->matchExprHandle == pd->handle)
					{
						PyGILState_STATE gstate;
						gstate = PyGILState_Ensure();
						PyObject *result = PyObject_CallMethod(pythonScript->getPluginObject(), fun->name, "(s#)", msgBuf, msgLen);
						Py_XDECREF(result);
						fun->call();
						PyGILState_Release(gstate);
						goto inpacketTickCleanup;
					}
				}
			}
		}
inpacketTickCleanup:
		msgBuf[0] = 0;
		free(msgBuf);
	}
	leaveCriticalSection();
}

void CPythonEngine::threadGILAcquire(int ms)
{
	init();
	enterCriticalSection();

	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();
	char buf[128];
	sprintf(buf, "time.sleep(%d.%03d)", ms / 1000, ms % 1000);
	PyRun_SimpleString(buf);

	PyGILState_Release(gstate);
	leaveCriticalSection();
}
