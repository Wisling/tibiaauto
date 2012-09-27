/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/


// mod_addressfinder.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_addressfinder.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "ModuleUtil.h"
#include "TibiaMapProxy.h"
#include <Tlhelp32.h>
#include <List>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CMod_addressfinderApp

BEGIN_MESSAGE_MAP(CMod_addressfinderApp, CWinApp)
//{{AFX_MSG_MAP(CMod_addressfinderApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool functions

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc( LPVOID lpParam ) {		
	CMemReaderProxy reader;
	CConfigData *config = (CConfigData *)lpParam;
	while (!toolThreadShouldStop) {
		int loop = 0;
		list <byte> tibiaFile;
		list<byte>::iterator fileIterator;
		list <byte> comparison;
		list<byte>::iterator comparisonIterator;
		list <int> mask;
		list<int>::iterator maskIterator;
		
		comparison.push_back(0xa1);
		mask.push_back(1);
		comparison.push_back(0x00);
		mask.push_back(0);
		comparison.push_back(0x00);
		mask.push_back(0);
		comparison.push_back(0x00);
		mask.push_back(0);
		comparison.push_back(0x00);
		mask.push_back(0);
		comparison.push_back(0x40);
		mask.push_back(1);
		comparison.push_back(0x83);
		mask.push_back(1);
		comparison.push_back(0xf8);
		mask.push_back(1);
		comparison.push_back(0x01);
		mask.push_back(1);
		comparison.push_back(0x7d);
		mask.push_back(1);
		comparison.push_back(0x07);
		mask.push_back(1);
		comparison.push_back(0xb8);
		mask.push_back(1);
		comparison.push_back(0xff);
		mask.push_back(1);
		comparison.push_back(0xff);
		mask.push_back(1);
		comparison.push_back(0xff);
		mask.push_back(1);
		comparison.push_back(0x7f);
		mask.push_back(1);
		comparison.push_back(0xeb);
		mask.push_back(1);
		comparison.push_back(0x2e);
		mask.push_back(1);
		comparison.push_back(0x3d);
		mask.push_back(1);
		comparison.push_back(0xf4);
		mask.push_back(1);
		comparison.push_back(0x01);
		mask.push_back(1);
		comparison.push_back(0x00);
		mask.push_back(1);
		comparison.push_back(0x00);
		mask.push_back(1);
		comparison.push_back(0x7e);
		mask.push_back(1);
		comparison.push_back(0x07);
		mask.push_back(1);
		comparison.push_back(0xb8);
		mask.push_back(1);
		comparison.push_back(0xff);
		mask.push_back(1);
		comparison.push_back(0xff);
		mask.push_back(1);
		comparison.push_back(0xff);
		mask.push_back(1);
		comparison.push_back(0x7f);
		mask.push_back(1);
		comparison.push_back(0xeb);
		mask.push_back(1);
		comparison.push_back(0x20);
		mask.push_back(1);
		comparison.push_back(0x8d);
		mask.push_back(1);
		comparison.push_back(0x48);
		mask.push_back(1);
		comparison.push_back(0xfa);
		mask.push_back(1);
		comparison.push_back(0x0f);
		mask.push_back(1);
		comparison.push_back(0xaf);
		mask.push_back(1);
		comparison.push_back(0xc8);
		mask.push_back(1);
		comparison.push_back(0x83);
		mask.push_back(1);
		comparison.push_back(0xc1);
		mask.push_back(1);
		comparison.push_back(0x11);
		mask.push_back(1);
		comparison.push_back(0x0f);
		mask.push_back(1);
		comparison.push_back(0xaf);
		mask.push_back(1);
		comparison.push_back(0xc8);
		mask.push_back(1);
		comparison.push_back(0x83);
		mask.push_back(1);
		comparison.push_back(0xe9);
		mask.push_back(1);
		comparison.push_back(0x0c);
		mask.push_back(1);
		comparison.push_back(0xb8);
		mask.push_back(1);
		comparison.push_back(0xab);
		mask.push_back(1);
		comparison.push_back(0xaa);
		mask.push_back(1);
		comparison.push_back(0xaa);
		mask.push_back(1);
		comparison.push_back(0x2a);
		mask.push_back(1);
		comparison.push_back(0xf7);
		mask.push_back(1);
		comparison.push_back(0xe9);
		mask.push_back(1);
		comparison.push_back(0x8b);
		mask.push_back(1);
		comparison.push_back(0xc2);
		mask.push_back(1);
		comparison.push_back(0xc1);
		mask.push_back(1);
		comparison.push_back(0xe8);
		mask.push_back(1);
		comparison.push_back(0x1f);
		mask.push_back(1);
		comparison.push_back(0x03);
		mask.push_back(1);
		comparison.push_back(0xc2);
		mask.push_back(1);
		comparison.push_back(0x6b);
		mask.push_back(1);
		comparison.push_back(0xc0);
		mask.push_back(1);
		comparison.push_back(0x64);
		mask.push_back(1);
		comparison.push_back(0x56);
		mask.push_back(1);
		comparison.push_back(0x2b);
		mask.push_back(1);
		comparison.push_back(0x05);
		mask.push_back(1);

		for (; loop < comparison.size(); loop++) {
			tibiaFile.push_back(reader.getMemIntValue(0x401000 + loop));
		}

		while(loop < 0x5b0ffe) {
			fileIterator = tibiaFile.begin();
			comparisonIterator = comparison.begin();
			maskIterator = mask.begin();
			int countSuccess = 0;
			while (fileIterator != tibiaFile.end() && comparisonIterator != comparison.end() && maskIterator != mask.end() && *maskIterator * *fileIterator == *comparisonIterator){
				countSuccess++;
				fileIterator++;
				comparisonIterator++;
				maskIterator++;
			}
			
			if (countSuccess == comparison.size()) {
				AfxMessageBox("Success");
				config->experienceAddress = reader.getMemIntValue(0x401000 + loop + 1);
				config->experience = reader.getMemIntValue(config->experienceAddress);
				toolThreadShouldStop=1;
				break;
			}
			else {
				tibiaFile.pop_front();
				tibiaFile.push_back(reader.getMemIntValue(0x401000 + ++loop));
			}
		}
		char buf[32];
		sprintf(buf, "Xp Address: 0x%x", 0x401000 + loop);
		AfxMessageBox(buf);
	}
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_addressfinderApp construction

CMod_addressfinderApp::CMod_addressfinderApp() {
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_addressfinderApp::~CMod_addressfinderApp() {
	if (m_configDialog)
		delete m_configDialog;
	delete m_configData;	
}

char * CMod_addressfinderApp::getName() {
	return "Auto addressfinder";
}

int CMod_addressfinderApp::isStarted() {
	return m_started;
}

void CMod_addressfinderApp::start() {	
	superStart();
	if (m_configDialog) 	{
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}
	
	DWORD threadId;
	
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);				
	m_started=1;
}

void CMod_addressfinderApp::stop() {
	toolThreadShouldStop=1;
	while (toolThreadShouldStop) {
		Sleep(50);
	};
	m_started=0;
	
	if (m_configDialog)	{
		m_configDialog->enableControls();
		m_configDialog->activateEnableButton(false);
	}
} 

void CMod_addressfinderApp::showConfigDialog() {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	
	if (!m_configDialog) {
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
		if (m_started) disableControls();
		else enableControls();
		m_configDialog->m_enable.SetCheck(m_started);
	}
	m_configDialog->ShowWindow(SW_SHOW);
}

void CMod_addressfinderApp::configToControls() {
	if (m_configDialog) {	
		m_configDialog->configToControls(m_configData);
	}
}

void CMod_addressfinderApp::controlsToConfig() {
	if (m_configDialog)	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}

void CMod_addressfinderApp::disableControls() {
	if (m_configDialog)
		m_configDialog->disableControls();
}

void CMod_addressfinderApp::enableControls() {
	if (m_configDialog)
		m_configDialog->enableControls();
}


char *CMod_addressfinderApp::getVersion() {
	return "1.0";
}

int CMod_addressfinderApp::validateConfig(int showAlerts) {	
	return 1;
}

void CMod_addressfinderApp::resetConfig() {
	m_configData = new CConfigData();
}

void CMod_addressfinderApp::loadConfigParam(char *paramName,char *paramValue) {
	if (!strcmp(paramName, "ExperienceAddress")) m_configData->experienceAddress = atoi(paramValue);
	if (!strcmp(paramName, "Experience")) m_configData->experience = atoi(paramValue);
}

char *CMod_addressfinderApp::saveConfigParam(char *paramName) {
	static char buf[1024];
	buf[0]='\0';
	if (!strcmp(paramName, "ExperienceAddress")) sprintf(buf,"%d",m_configData->experienceAddress);
	if (!strcmp(paramName, "Experience")) sprintf(buf,"%d",m_configData->experience);

	return buf;
}

char *CMod_addressfinderApp::getConfigParamName(int nr) {
	switch (nr) {
	case 0: return "ExperienceAddress";
	case 1: return "Experience";
	default:
		return NULL;
	}
}

void CMod_addressfinderApp::getNewSkin(CSkin newSkin) {
	skin = newSkin;
	
	if (m_configDialog){
		m_configDialog->DoSetButtonSkin();
		m_configDialog->Invalidate();
	}
}