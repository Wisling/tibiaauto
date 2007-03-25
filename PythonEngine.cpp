// PythonEngine.cpp: implementation of the CPythonEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto.h"
#include "PythonEngine.h"
#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "PythonScript.h"
#include "IPCBackPipeProxy.h"
#include "TibiaMapProxy.h"
#include "TibiaItemProxy.h"
#include "AliceProxy.h"
#include "RegexpProxy.h"
#include "ModuleUtil.h"
  
  
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


int CPythonEngine::initialised=0;

CRITICAL_SECTION ScriptEngineCriticalSection;


//////////////////////////////////////////////////////////////////////
// Python tibiaauto
//////////////////////////////////////////////////////////////////////

int registerPluginCount=0;

 
static PyObject *tibiaauto_tibiaauto_registerPlugin(PyObject *self, PyObject *args)    
{					
	
	registerPluginCount++;

	CPythonScript *pythonScript = new CPythonScript();
	CPythonEngine pythonEngine;

	
	PyObject *pluginClass=NULL;
	if (!PyArg_ParseTuple(args,"O",&pluginClass)) return NULL;
		
	Py_XINCREF(pluginClass);	
	pythonScript->setPluginClass(pluginClass);
	
	PyObject *pluginObject = PyInstance_New(pluginClass,NULL,NULL);			
	if (pluginObject==NULL)
	{ 
		AfxMessageBox("registerObject() takes class as an argument");
		Py_XDECREF(pluginClass);
		return NULL;
	}
	Py_XINCREF(pluginObject);	 
	pythonScript->setPluginObject(pluginObject);		
	
	// getName			
    PyObject *result = PyObject_CallMethod(pluginObject, "getName","()");
	pythonScript->setName(PyString_AsString(result));	
	Py_XDECREF(result);	

	// getVersion		
    result = PyObject_CallMethod(pluginObject, "getVersion","()");
	pythonScript->setVersion(PyString_AsString(result));		
	Py_XDECREF(result);		


	// getFunDef			
	int nr;
	for (nr=0;;nr++)
	{						
		int type,interval; 
		PyObject *periodicalFun;				
		
		result = PyObject_CallMethod(pluginObject, "getFunDef","(i)",nr);		
		if (result==NULL)
		{
			AfxMessageBox("getFunDef() call failed");
			return NULL;
		}		
			
		type=-255;
		if (PyArg_ParseTuple(result,"|iiO",&type,&interval,&periodicalFun))
		{				
			if (type!=-255)
			{
				pythonScript->addFunDef(type,interval,periodicalFun);			
			} else {
				break;
			}
		} else {			
			Py_XDECREF(result);
			return NULL;			
		}		
		
		
		Py_XDECREF(result);		
	}
	// getConfigParam
	for (nr=0;;nr++)
	{
		char *name=NULL,*description=NULL;
		result = PyObject_CallMethod(pluginObject,"getConfigParam","(i)",nr);
		if (result==NULL)
		{
			AfxMessageBox("getConfigParam() call failed");
			return NULL;
		}		
		if (PyArg_ParseTuple(result,"|ss",&name,&description))
		{
			if (name!=NULL&&description!=NULL)
			{
				pythonScript->addParamDef(name,description);
			} else {
				break;
			}
		} else {
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
*/
 
#include "PythonEngine_fun.cpp"

//////////////////////////////////////////////////////////////////////
// Python function list definitions
//////////////////////////////////////////////////////////////////////


static PyMethodDef Methods_tibiaauto[] = {    
	{"registerPlugin", tibiaauto_tibiaauto_registerPlugin, METH_VARARGS},
    {NULL,      NULL}        /* Sentinel */
};

static PyMethodDef Methods_tareader[] = {
    {"setProcessId", tibiaauto_reader_setProcessId, METH_VARARGS},    
	{"readSelfCharacter", tibiaauto_reader_readSelfCharacter, METH_VARARGS},	
	{"readContainer", tibiaauto_reader_readContainer, METH_VARARGS},
	{"readContainerItem", tibiaauto_reader_readContainerItem,METH_VARARGS},
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
	{"setMainWindowText", tibiaauto_reader_setMainWindowText, METH_VARARGS},
	{"setMainTrayText", tibiaauto_reader_setMainTrayText, METH_VARARGS},
	{"getPlayerModeAttackPlayers", tibiaauto_reader_getPlayerModeAttackPlayers, METH_VARARGS},
	{"getPlayerModeAttackType", tibiaauto_reader_getPlayerModeAttackType, METH_VARARGS},
	{"getPlayerModeFollow", tibiaauto_reader_getPlayerModeFollow, METH_VARARGS},
	{"getOpenWindowName", tibiaauto_reader_getOpenWindowName, METH_VARARGS},
	{"getConnectionState", tibiaauto_reader_getConnectionState, METH_VARARGS},
    {NULL,      NULL}        /* Sentinel */
};

static PyMethodDef Methods_tasender[] = {    
	{"moveObjectBetweenContainers", tibiaauto_sender_moveObjectBetweenContainers, METH_VARARGS},
	{"useWithObjectFromContainerOnFloor", tibiaauto_sender_useWithObjectFromContainerOnFloor, METH_VARARGS},
	{"moveObjectFromFloorToContainer", tibiaauto_sender_moveObjectFromFloorToContainer, METH_VARARGS},
	{"moveObjectFromContainerToFloor", tibiaauto_sender_moveObjectFromContainerToFloor, METH_VARARGS},
	{"moveObjectFromFloorToFloor", tibiaauto_sender_moveObjectFromFloorToFloor, METH_VARARGS},
	{"castRuneAgainstCreature", tibiaauto_sender_castRuneAgainstCreature, METH_VARARGS},
	{"castRuneAgainstHuman", tibiaauto_sender_castRuneAgainstHuman, METH_VARARGS},
	{"sendTAMessage", tibiaauto_sender_sendTAMessage, METH_VARARGS},
	{"useItemOnFloor", tibiaauto_sender_useItemOnFloor, METH_VARARGS},
	{"useItemInContainer", tibiaauto_sender_useItemInContainer, METH_VARARGS},
	{"openContainerFromFloor", tibiaauto_sender_openContainerFromFloor, METH_VARARGS},
	{"openContainerFromContainer", tibiaauto_sender_openContainerFromContainer, METH_VARARGS},
	{"say", tibiaauto_sender_say, METH_VARARGS},
	{"sayWhisper", tibiaauto_sender_sayWhisper, METH_VARARGS},
	{"sayYell", tibiaauto_sender_sayYell, METH_VARARGS},
	{"tell", tibiaauto_sender_tell, METH_VARARGS},
	{"sayOnChan", tibiaauto_sender_sayOnChan, METH_VARARGS},
	{"logout", tibiaauto_sender_logout, METH_VARARGS},
	{"walkOnTAMap", tibiaauto_sender_walkOnTAMap, METH_VARARGS},
	{"closeContainer", tibiaauto_sender_closeContainer, METH_VARARGS},
	{"attackMode", tibiaauto_sender_attackMode, METH_VARARGS},
	{"attack", tibiaauto_sender_attack, METH_VARARGS},
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
	
    {NULL,      NULL}        /* Sentinel */
};


static PyMethodDef Methods_tamap[] = {    
	{"removePointAvailable", tibiaauto_map_removePointAvailable, METH_VARARGS},
	{"prohPointClear", tibiaauto_map_prohPointClear, METH_VARARGS},
	{"prohPointAdd", tibiaauto_map_prohPointAdd, METH_VARARGS},
	{"getPointUpDown", tibiaauto_map_getPointUpDown, METH_VARARGS},
	{"getPointUpDownNoProh", tibiaauto_map_getPointUpDownNoProh, METH_VARARGS},
	{"setPointUpDown", tibiaauto_map_setPointUpDown, METH_VARARGS},
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
	
	
    {NULL,      NULL}        /* Sentinel */
};


static PyMethodDef Methods_taregexp[] = {    
	{"match", tibiaauto_regexp_match, METH_VARARGS},
    {NULL,      NULL}        /* Sentinel */
};


static PyMethodDef Methods_taalice[] = { 
	{"respond", tibiaauto_alice_respond, METH_VARARGS},
		
    {NULL,      NULL}        /* Sentinel */
};

static PyMethodDef Methods_tacrstat[] = { 
	{"findCreatureStatForLocationTibiaId", tibiaauto_crstat_tibiaId, METH_VARARGS},
	{"findCreatureStatForLocationCount", tibiaauto_crstat_count, METH_VARARGS},
	{"findCreatureStatForLocationName", tibiaauto_crstat_name, METH_VARARGS},
	{"findCreatureStatInArea", tibiaauto_crstat_inarea,METH_VARARGS},
		
    {NULL,      NULL}        /* Sentinel */
};

static PyMethodDef Methods_taitem[] = {
	{"getName", tibiaauto_item_getName, METH_VARARGS},
	{"getObjectId", tibiaauto_item_getObjectId, METH_VARARGS},
	{"getValueForConst", tibiaauto_item_getValueForConst, METH_VARARGS},
	{"refreshItemLists", tibiaauto_item_refreshItemLists, METH_VARARGS},
	{"getCorpseIdByCreatureName", tibiaauto_item_getCorpseIdByCreatureName, METH_VARARGS},
	{"getItemsItems", tibiaauto_item_getItemsItems, METH_VARARGS},
	{"getItemsItemsId", tibiaauto_item_getItemsItemsId, METH_VARARGS},
	{"getItemsItemsCount", tibiaauto_item_getItemsItemsCount, METH_VARARGS},
	{"getItemsFood", tibiaauto_item_getItemsFood, METH_VARARGS},
	{"getItemsFoodId", tibiaauto_item_getItemsFoodId, METH_VARARGS},
	{"getItemsFoodCount", tibiaauto_item_getItemsFoodCount, METH_VARARGS},
	{"getItemsCorpses", tibiaauto_item_getItemsCorpses, METH_VARARGS},
	{"getItemsCorpsesId", tibiaauto_item_getItemsCorpsesId, METH_VARARGS},
	{"getItemsCorpsesCount", tibiaauto_item_getItemsCorpsesCount, METH_VARARGS},
	{"getItemsLooted", tibiaauto_item_getItemsLooted, METH_VARARGS},
	{"getItemsLootedId", tibiaauto_item_getItemsLootedId, METH_VARARGS},
	{"getItemsLootedCount", tibiaauto_item_getItemsLootedCount, METH_VARARGS},
	
	
    {NULL,      NULL}        /* Sentinel */
};


static PyMethodDef Methods_takernel[] = { 
	{"startModule", tibiaauto_kernel_startModule, METH_VARARGS},
	{"stopModule", tibiaauto_kernel_stopModule, METH_VARARGS},
	{"getModuleCount", tibiaauto_kernel_getModuleCount, METH_VARARGS},
	{"getModuleName", tibiaauto_kernel_getModuleName, METH_VARARGS},
	{"getModuleDesc", tibiaauto_kernel_getModuleDesc, METH_VARARGS},
	{"isModuleStarted", tibiaauto_kernel_isModuleStarted, METH_VARARGS},
	{"getModuleVersion", tibiaauto_kernel_getModuleVersion, METH_VARARGS},

	{"startPythonModule", tibiaauto_kernel_startPythonModule, METH_VARARGS},
	{"stopPythonModule", tibiaauto_kernel_stopPythonModule, METH_VARARGS},
	{"getPythonModuleCount", tibiaauto_kernel_getPythonModuleCount, METH_VARARGS},
	{"getPythonModuleName", tibiaauto_kernel_getPythonModuleName, METH_VARARGS},
	{"isPythonModuleStarted", tibiaauto_kernel_isPythonModuleStarted, METH_VARARGS},
	{"getPythonModuleDesc", tibiaauto_kernel_getPythonModuleDesc, METH_VARARGS},
	{"getPythonModuleVersion", tibiaauto_kernel_getPythonModuleVersion, METH_VARARGS},

		
    {NULL,      NULL}        /* Sentinel */
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
		initialised=1;
		
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

		
		
		char installPath[1024];
		unsigned long installPathLen=1023;
		installPath[0]='\0';
		HKEY hkey=NULL;
		if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey))
		{
			RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
			RegCloseKey(hkey);
		}
		if (!strlen(installPath))
		{
			::MessageBox(0,"ERROR! Unable to read TA install directory! Please reinstall!","ERROR",0);
			exit(1);
		}

		char pathBuf[2048];
		sprintf(pathBuf,"%s\\tascripts\\tautil.py",installPath);

		// load tautil.py
		FILE *f=fopen(pathBuf,"r");
		if (f)
		{		
			fseek(f,0,SEEK_END);
			int fileSize = ftell(f);
			fseek(f,0,SEEK_SET);
			char *fileBuf = (char *)malloc(fileSize+1);
			memset(fileBuf,0,fileSize+1);
			fread(fileBuf,1,fileSize,f);
									
			int ret=PyRun_SimpleString(fileBuf);	
			if (ret==-1) AfxMessageBox("Loading tautil.py script failed!");
			fclose(f);
		} else {
			AfxMessageBox("Unable to find tascripts\\tautil.py!");
		}
		
		PyGILState_Release(gstate);		

		InitializeCriticalSection(&ScriptEngineCriticalSection);		
						

		
		// now load all scripts from 'tascripts' subdirectory
		sprintf(pathBuf,"%s\\tascripts\\*.py",installPath);
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = FindFirstFile(pathBuf,&findFileData);		
		if (hFind!=INVALID_HANDLE_VALUE)
		{
			char buf[1024];			

			snprintf(buf,1023,"%s\\tascripts\\%s",installPath,findFileData.cFileName);
			loadScript(buf);

			while (FindNextFile(hFind,&findFileData))
			{				
				snprintf(buf,1023,"%s\\tascripts\\%s",installPath,findFileData.cFileName);
				// tautil.py will be loaded in a special way
				if (!strstr(buf,"tascripts\\tautil.py"))
				{					
					loadScript(buf);
				}
			}
			FindClose(hFind);
		}					
	}
}

void CPythonEngine::loadScript(char *path)
{		
	init();
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();
	
	FILE *f=fopen(path,"rb");
	if (f)
	{			
		int oldRegisterPluginCount=registerPluginCount;				
		char buf[1024];
		sprintf(buf,"execfile('%s')",path);		
		int pos,len;
		len=strlen(buf);
		for (pos=0;pos<len;pos++) if (buf[pos]=='\\') buf[pos]='/';		
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

		if (registerPluginCount==oldRegisterPluginCount)
		{
			char buf[1024];
			sprintf(buf,"Loading script failed: there must be a call to tibiaauto.registerPlugin() (%d/%d)!",registerPluginCount,oldRegisterPluginCount);
			AfxMessageBox(buf);
		} else {
			// set the just loaded script file name
			CPythonScript::getScriptByNr(CPythonScript::getScriptCount()-1)->setFileName(path);
		}
	}
	PyGILState_Release(gstate);
}

void CPythonEngine::periodicalTick()
{		
	init();
	enterCriticalSection();
	long int tm=GetTickCount();
	int scriptNr;
	for (scriptNr=0;;scriptNr++)
	{
		CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
		if (!pythonScript) break;
		if (!pythonScript->isEnabled()) continue;
		int funNr;
		for (funNr=0;;funNr++)
		{
			struct funType *fun=pythonScript->getFunDef(funNr);
			if (!fun) break;

			if (fun->type==FUNTYPE_PERIODICAL&&tm>=fun->tmNextExec)
			{				
				CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
				
				PyGILState_STATE gstate;
				gstate = PyGILState_Ensure();
				PyObject *params = pythonScript->getParamsDic();								
				PyObject *result = PyObject_CallMethod(pythonScript->getPluginObject(), fun->name,"(O)",params);
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
	long int tm=GetTickCount();
	CMemReaderProxy reader;
	CIPCBackPipeProxy backPipe;
	struct ipcMessage mess;

	if (backPipe.readFromPipe(&mess,1006)){
		int infoType;
		int nickLen;
		int msgLen;
		char nickBuf[16384];
		char msgBuf[16384];
		char chanBuf[16384];		
		
		memset(nickBuf,0,16384);
		memset(msgBuf,0,16384);
		memcpy(&infoType,mess.payload,sizeof(int));
		memcpy(&nickLen,mess.payload+4,sizeof(int));
		memcpy(&msgLen,mess.payload+8,sizeof(int));
		memcpy(nickBuf,mess.payload+12,nickLen);
		memcpy(msgBuf,mess.payload+12+nickLen,msgLen);
		switch (infoType)
		{
		case 1: sprintf(chanBuf,"say");break;
		case 2: sprintf(chanBuf,"whisper");break;
		case 3: sprintf(chanBuf,"yell");break;
		case 4: sprintf(chanBuf,"private");break;
		case 5: sprintf(chanBuf,"channel");break;
		default: sprintf(chanBuf,"other[%d]",infoType);break;
		}			
						
		int scriptNr;
		for (scriptNr=0;;scriptNr++)
		{
			CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);			
			if (!pythonScript) break;
			if (!pythonScript->isEnabled()) continue;
			int funNr;
			for (funNr=0;;funNr++)
			{
				struct funType *fun=pythonScript->getFunDef(funNr);
				if (!fun) break;
				
				if (fun->type==FUNTYPE_MSG)
				{

					PyGILState_STATE gstate;
					gstate = PyGILState_Ensure();
					PyObject *params = pythonScript->getParamsDic();									
					PyObject *result = PyObject_CallMethod(pythonScript->getPluginObject(), fun->name,"(O(isss))",params,infoType,chanBuf,nickBuf,msgBuf);
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
	CPythonScript::pythonScriptTab[scriptNr]=CPythonScript::pythonScriptTab[CPythonScript::pythonScriptCount-1];	
	CPythonScript::pythonScriptCount--;
	// this is commented out by purpose - there would be too much threads 
	// playing to cleanup this memory here
	//delete pythonScript;
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
	long int tm=GetTickCount();
	CMemReaderProxy reader;
	CIPCBackPipeProxy backPipe;
	struct ipcMessage mess;

	if (backPipe.readFromPipe(&mess,1007)){		
		int msgLen;		
		char msgBuf[16384];		
		
		memset(msgBuf,0,16384);		
		memcpy(&msgLen,mess.payload,sizeof(int));		
		memcpy(msgBuf,mess.payload+4,msgLen);					
						
		int scriptNr;
		for (scriptNr=0;;scriptNr++)
		{
			CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);			
			if (!pythonScript) break;
			if (!pythonScript->isEnabled()) continue;
			int funNr;
			for (funNr=0;;funNr++)
			{
				struct funType *fun=pythonScript->getFunDef(funNr);
				if (!fun) break;
				
				if (fun->type==FUNTYPE_TAMSG)
				{

					PyGILState_STATE gstate;
					gstate = PyGILState_Ensure();
					PyObject *params = pythonScript->getParamsDic();									
					PyObject *result = PyObject_CallMethod(pythonScript->getPluginObject(), fun->name,"(Os)",params,msgBuf);
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
