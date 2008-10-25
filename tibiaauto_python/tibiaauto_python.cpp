// tibiaauto_python.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"


#include "Python.h"
#include "tibiaauto_python.h"
#include "MemReaderProxy.h"
#include "PackSenderProxy.h"



static PyObject *tibiaauto_setProcessId(PyObject *self, PyObject *args)    
{
	CMemReaderProxy reader;

	int processId;

    if (!PyArg_ParseTuple(args, "i", &processId)) return NULL;
	reader.setProcessId(processId);    
	Py_INCREF(Py_None);
	return Py_None;    
}

static PyObject *tibiaauto_readSelfCharacter(PyObject *self, PyObject *args)
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

static PyObject *tibiaauto_turnUp(PyObject *self, PyObject *args)    
{
	MessageBox(0,"turnUp()","turnUp()",0);
	CPackSenderProxy sender;

    sender.turnUp();
	Py_INCREF(Py_None);
	return Py_None;    
}

static PyObject *tibiaauto_reader_readVIPEntry(PyObject *self, PyObject *args)
{ 
	CMemReaderProxy reader;
	
	int arg1;
    if (!PyArg_ParseTuple(args, "i", &arg1)) return NULL;
	CTibiaCharacter *vip = reader.readVIPEntry(arg1);

	PyObject *ret = 
		Py_BuildValue("{s:i,s:i,s:i,s:i}",
		"id",vip->id,
		"name",vip->name,
		"status",vip->status,
		"icon",vip->icon);

	delete vip;
	return ret;
}

static PyMethodDef NoMethods[] = {    
    {NULL,      NULL}        /* Sentinel */
};


static PyMethodDef TAMethods[] = {
    {"setProcessId", tibiaauto_setProcessId, METH_VARARGS},    
	{"readSelfCharacter", tibiaauto_readSelfCharacter, METH_VARARGS},
	{"turnUp", tibiaauto_turnUp, METH_VARARGS},
    {NULL,      NULL}        /* Sentinel */
};

void inittibiaauto_python()
{	
	MessageBox(0,"init","init",0);
    Py_InitModule("tibiaauto_python", NoMethods);
	Py_InitModule("tibiaauto", TAMethods);
}
  