// PythonEngine.h: interface for the CPythonEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PYTHONENGINE_H__5114AA39_4F36_479F_91D9_820A9C5034B4__INCLUDED_)
#define AFX_PYTHONENGINE_H__5114AA39_4F36_479F_91D9_820A9C5034B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPythonEngine
{
public:
	static void backpipeTamsgTick();
	static void leaveCriticalSection();
	static void enterCriticalSection();
	static void unloadScript(int scriptNr);
	static void backpipeMsgTick();
	static void backpipeInpacketTick();
	static void periodicalTick();
	static void loadScript(char *path);
	static void init();
	static void threadGILAcquire(int ms);
	CPythonEngine();
	virtual ~CPythonEngine();

private:
	static int initialised;
};

#endif // !defined(AFX_PYTHONENGINE_H__5114AA39_4F36_479F_91D9_820A9C5034B4__INCLUDED_)
