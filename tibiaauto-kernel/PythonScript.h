// PythonScript.h: interface for the CPythonScript class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PYTHONSCRIPT_H__72796954_565E_4E4B_B8EA_B58B799CA4A0__INCLUDED_)
#define AFX_PYTHONSCRIPT_H__72796954_565E_4E4B_B8EA_B58B799CA4A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Prevent linking to python24_d.lib, unavailable in general python release
#ifdef _DEBUG
#undef _DEBUG
#include "python.h"
#define _DEBUG
#else
#include "python.h"
#endif

#include "PythonEngine.h"

enum FUNTYPE
{
	FUNTYPE_PERIODICAL = 0,
	FUNTYPE_MSG        = 1,
	FUNTYPE_TAMSG      = 2,
	FUNTYPE_INPACKET   = 3
};

struct paramType
{
	char name[128];
	char description[128];
	char value[1024];
};


struct funType
{
	int tmNextExec;
	int interval;
	PyObject *fun;
	int type;
	char name[128];
	char *matchExpr;
	int matchExprHandle;
	int tmLastExec;
	int scriptNr;
public:
	~funType()
	{
	}

	funType()
	{
		matchExpr       = NULL;
		matchExprHandle = 0;
	}

	void call()
	{
		long int tm = GetTickCount();
		tmLastExec = tm;
		if (type == FUNTYPE_PERIODICAL)
			tmNextExec = tm + interval;
	}
};


class CPythonScript
{
	friend class CPythonEngine;
public:
	void setParamValue(char *name, char *value);
	PyObject * getParamsDic();
	struct paramType *getParamDef(int nr);
	void addParamDef(char *name, char *desc);
	void setFileName(char *newFileName);
	char * getFileName();
	static int getScriptCount();
	int isEnabled();
	void setEnabled(int newVal);
	PyObject * getPluginObject();
	struct funType * getFunDef(int nr);
	static CPythonScript * getScriptByNr(int nr);
	void addFunDef(int type, int interval, PyObject *fun);
	void addFunDef(int type, char *interval, int regLen, PyObject *fun);
	CPythonScript();
	virtual ~CPythonScript();
	void setName(char *name);
	void setVersion(char *version);
	void setPluginClass(PyObject *pluginClass);
	void setPluginObject(PyObject *pluginObject);
	char *getName();
	char *getVersion();

private:
	void reallocParamDef();
	void reallocFunDef();
	char name[128];
	char version[128];
	char fileName[1024];
	PyObject *pluginClass;
	PyObject *pluginObject;
	int funDefCount;
	int funDefSize;
	struct funType *funDefTab;
	int paramDefCount;
	int paramDefSize;
	struct paramType *paramDefTab;


	int scriptNr;
	int enabled;

	static int pythonScriptCount;
	static int pythonScriptSize;
	static CPythonScript **pythonScriptTab;
	static void reallocPythonScript();
};

#endif // !defined(AFX_PYTHONSCRIPT_H__72796954_565E_4E4B_B8EA_B58B799CA4A0__INCLUDED_)
