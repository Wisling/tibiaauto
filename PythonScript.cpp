// PythonScript.cpp: implementation of the CPythonScript class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto.h"
#include "PythonScript.h"
#include "PackSenderProxy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////

int CPythonScript::pythonScriptCount=0;
int CPythonScript::pythonScriptSize=0;
CPythonScript ** CPythonScript::pythonScriptTab=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPythonScript::CPythonScript()
{
	name[0]=version[0]=fileName[0]='\0';
	pluginObject=pluginClass=NULL;

	funDefCount=funDefSize=0;
	funDefTab=NULL;
	paramDefCount=paramDefSize=0;
	paramDefTab=NULL;
	enabled=0;
	scriptNr=pythonScriptCount;
	

	
	reallocFunDef();
	reallocParamDef();
	reallocPythonScript();

	pythonScriptTab[pythonScriptCount++]=this;	
}

CPythonScript::~CPythonScript()
{
	int i;
	Py_XDECREF(pluginObject);	Py_XDECREF(pluginClass);

	for (i=0;i<funDefCount;i++)
	{
		Py_XDECREF(funDefTab[i].fun);
	}
	free(funDefTab);
	if (paramDefTab) delete paramDefTab;
}


void CPythonScript::setName(char *name)
{
	strncpy(this->name,name,127);
}

void CPythonScript::setVersion(char *version)
{
	strncpy(this->version,version,127);
}

char * CPythonScript::getVersion()
{
	return version;
}


char * CPythonScript::getName()
{
	return name;
}

void CPythonScript::setPluginClass(PyObject *pluginClass)
{
	Py_XDECREF(pluginClass);	
	this->pluginClass=pluginClass;
	Py_XINCREF(pluginClass);	
}

void CPythonScript::setPluginObject(PyObject *pluginObject)
{
	Py_XDECREF(pluginObject);	
	this->pluginObject=pluginObject;
	Py_XINCREF(pluginObject);	
}

void CPythonScript::addFunDef(int type,int interval, PyObject *fun)
{
	reallocFunDef();
	
	PyObject *py_name = PyObject_GetAttrString(fun,"__name__");	
	Py_XINCREF(py_name);
	strncpy(funDefTab[funDefCount].name,PyString_AsString(py_name),127);
	Py_XDECREF(py_name);

	funDefTab[funDefCount].type=type;
	funDefTab[funDefCount].interval=interval;	
	funDefTab[funDefCount].fun=fun;
	funDefTab[funDefCount].tmNextExec=0;
	funDefTab[funDefCount].tmLastExec=0;
	funDefTab[funDefCount].scriptNr=scriptNr;
	Py_XINCREF(fun);
	funDefCount++;
}

void CPythonScript::addFunDef(int type,char *matchExpr, int regLen, PyObject *fun)
{
	static int handleNum = 1;
	CPackSenderProxy sender;
	reallocFunDef();
	
	PyObject *py_name = PyObject_GetAttrString(fun,"__name__");
	Py_XINCREF(py_name);
	strncpy(funDefTab[funDefCount].name,PyString_AsString(py_name),127);
	Py_XDECREF(py_name);

	funDefTab[funDefCount].type=type;
	funDefTab[funDefCount].matchExpr=matchExpr; //wis: may need to be copied into local char array, but I am unsure when string is freed.
	funDefTab[funDefCount].matchExprHandle = (int)fun; //Using the function here has the added advantage of not changing too often when reloading the script or TA
	funDefTab[funDefCount].fun=fun;
	funDefTab[funDefCount].tmNextExec=0;
	funDefTab[funDefCount].tmLastExec=0;
	funDefTab[funDefCount].scriptNr=scriptNr;
	sender.registerInpacketRegex(funDefTab[funDefCount].matchExprHandle, matchExpr, regLen);
	handleNum++;

	Py_XINCREF(fun);
	funDefCount++;
}

void CPythonScript::reallocFunDef()
{
	if (funDefCount+1>=funDefSize)
	{
		funDefSize+=5;
		funDefSize*=2;
		funDefTab=(struct funType *)realloc(funDefTab,funDefSize*sizeof(struct funType));
	}
}

void CPythonScript::reallocPythonScript()
{
	if (pythonScriptCount+1>=pythonScriptSize)
	{
		pythonScriptSize+=5;
		pythonScriptSize*=2;
		pythonScriptTab=(CPythonScript **)realloc(pythonScriptTab,pythonScriptSize*sizeof(CPythonScript *));
	}
}

CPythonScript * CPythonScript::getScriptByNr(int nr)
{
	if (nr>=pythonScriptCount) return NULL;
	return pythonScriptTab[nr];	
}

struct funType * CPythonScript::getFunDef(int nr)
{
	if (nr>=funDefCount) return NULL;
	return &funDefTab[nr];
}

PyObject * CPythonScript::getPluginObject()
{
	return pluginObject;
}

void CPythonScript::setEnabled(int newVal)
{
	enabled=newVal;
}

int CPythonScript::isEnabled()
{
	return enabled;
}

int CPythonScript::getScriptCount()
{
	return pythonScriptCount;
}

char * CPythonScript::getFileName()
{
	return fileName;
}

void CPythonScript::setFileName(char *newFileName)
{
	strcpy(fileName,newFileName);
}

void CPythonScript::addParamDef(char *name, char *desc)
{
	reallocParamDef();		
		
	strncpy(paramDefTab[paramDefCount].name,name,127);
	strncpy(paramDefTab[paramDefCount].description,desc,127);
	paramDefTab[paramDefCount].value[0]='\0';
	
	paramDefCount++;
}

struct paramType *CPythonScript::getParamDef(int nr)
{
	if (nr>=paramDefCount) return NULL;
	return &paramDefTab[nr];
}

void CPythonScript::reallocParamDef()
{
	if (paramDefCount+1>=paramDefSize)
	{
		paramDefSize+=5;
		paramDefSize*=2;
		paramDefTab=(struct paramType *)realloc(paramDefTab,paramDefSize*sizeof(struct paramType));
	}
}

PyObject * CPythonScript::getParamsDic()
{
	int nr=0;
	PyObject * dict = PyDict_New();
	for (;;)
	{
		struct paramType *param = getParamDef(nr++);
		if (!param) break;

		PyDict_SetItemString(dict,param->name,PyString_FromString(param->value));
	}
	return dict;
}


void CPythonScript::setParamValue(char *name, char *value)
{
	int nr;
	for (nr=0;nr<paramDefCount;nr++)
	{
		if (!strcmp(name,paramDefTab[nr].name))
		{
			strncpy(paramDefTab[nr].value,value,1024);
		}
	}
}
