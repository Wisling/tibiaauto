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


// mod_team.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_team.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include <MemReader.h>
#include <PackSender.h>
#include <TibiaItem.h>
#include <ModuleUtil.h>
#include "ConnectedNode.h"
#include "ConnectedNodes.h"
#include <IPCBackPipe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG


/////////////////////////////////////////////////////////////////////////////
// CMod_teamApp

BEGIN_MESSAGE_MAP(CMod_teamApp, CWinApp)
//{{AFX_MSG_MAP(CMod_teamApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CConnectedNodes connectedNodes;


/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop = 0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc(LPVOID lpParam)
{
	int i;
	char buf[1024];

	CMemReader& reader = CMemReader::getMemReader();

	
	
	CConfigData *config        = (CConfigData *)lpParam;

	connectedNodes.disconnect();

	connectedNodes.setMasterNode(config->masterChar);

	int iter = -1;

	while (!toolThreadShouldStop)
	{
		iter++;
		Sleep(100);
		CTibiaCharacter self;
		reader.readSelfCharacter(&self);

		if (iter % 50 == 0)
		{
			// try to login/report status every 5 seconds
			if (strlen(connectedNodes.getMasterNode()))
			{
				// report our state to the master
				if (!connectedNodes.isConnected())
				{
					// no connected node -> try to login to the master
					sprintf(buf, "`TA login %d,%d,%d,%d,%d,%d,%d", self.x, self.y, self.z, self.hp, self.maxHp, self.mana, self.maxMana);
					CPackSender::tell(buf, connectedNodes.getMasterNode());
				}
				else
				{
					if (strlen(connectedNodes.getMasterNode()) && !connectedNodes.isCharConnected(connectedNodes.getMasterNode()))
						connectedNodes.findNewMasterNode();
					sprintf(buf, "`TA ping %d,%d,%d,%d,%d,%d,%d", self.x, self.y, self.z, self.hp, self.maxHp, self.mana, self.maxMana);
					CPackSender::tell(buf, connectedNodes.getMasterNode());
				}
			}

			sprintf(buf, "`TA data %s,%d,%d,%d,%d,%d,%d,%d,0", self.name, self.x, self.y, self.z, self.hp, self.maxHp, self.mana, self.maxMana);
			for (i = 0; i < connectedNodes.getMaxNodeCount(); i++)
			{
				CConnectedNode *connectedNode = connectedNodes.getNodeByNr(i);
				if (connectedNode->connected)
				{
					char buf2[128];
					sprintf(buf2, " %s,%d,%d,%d,%d,%d,%d,%d,%d", connectedNode->charName, connectedNode->x, connectedNode->y, connectedNode->z, connectedNode->hp, connectedNode->maxHp, connectedNode->mana, connectedNode->maxMana, connectedNode->distance);
					strcat(buf, buf2);
				}
			}

			for (i = 0; i < connectedNodes.getMaxNodeCount(); i++)
			{
				CConnectedNode *connectedNode = connectedNodes.getNodeByNr(i);
				if (connectedNode->connected && connectedNode->isSlave)
				{
					// send our position to all the slaves
					//sprintf(buf,"`TA ping %d,%d,%d,%d,%d,%d,%d",self.x,self.y,self.z,self.hp,self.maxHp,self.mana,self.maxMana);
					//CPackSender::tell(buf,connectedNode->charName);

					CPackSender::tell(buf, connectedNode->charName);
				}
			}
			CPackSender::tell(buf, connectedNodes.getMasterNode());
		}

		CIpcMessage mess;
		// now read messages from the master
		while (CIPCBackPipe::readFromPipe(&mess, 1005))
		{
			int infoType;
			int chanType;
			int nickLen;
			int msgLen;
			char nickBuf[16384];
			char msgBuf[16384];

			memset(nickBuf, 0, 16384);
			memset(msgBuf, 0, 16384);
			memcpy(&infoType, mess.payload, sizeof(int));
			memcpy(&chanType, mess.payload + 4, sizeof(int));
			memcpy(&nickLen, mess.payload + 8, sizeof(int));
			memcpy(&msgLen, mess.payload + 12, sizeof(int));
			memcpy(nickBuf, mess.payload + 16, nickLen);
			memcpy(msgBuf, mess.payload + 16 + nickLen, msgLen);

			if (msgBuf[0] == '`' && msgBuf[1] == 'T' && msgBuf[2] == 'A' && (connectedNodes.shouldAcceptMessage(nickBuf) || !strncmp(msgBuf, "`TA login", strlen("`TA login"))))
			{
				if (!strncmp(msgBuf, "`TA login", strlen("`TA login")))
				{
					// process login message
					int x, y, z, hp, mana, maxHp, maxMana;
					sscanf(msgBuf, "`TA login %d,%d,%d,%d,%d,%d,%d", &x, &y, &z, &hp, &maxHp, &mana, &maxMana);
					connectedNodes.refreshNodeInfo(nickBuf, hp, mana, maxHp, maxMana, x, y, z, 1, 1);

					sprintf(buf, "`TA ping %d,%d,%d,%d,%d,%d,%d", self.x, self.y, self.z, self.hp, self.maxHp, self.mana, self.maxMana);
					CPackSender::tell(buf, nickBuf);
				}
				if (!strncmp(msgBuf, "`TA ping", strlen("`TA ping")))
				{
					// process ping message (info with own data)
					int x, y, z, hp, mana, maxHp, maxMana;
					sscanf(msgBuf, "`TA ping %d,%d,%d,%d,%d,%d,%d", &x, &y, &z, &hp, &maxHp, &mana, &maxMana);
					connectedNodes.refreshNodeInfo(nickBuf, hp, mana, maxHp, maxMana, x, y, z, -1, 1);
				}

				if (!strncmp(msgBuf, "`TA data", strlen("`TA data")))
				{
					// process data message (info about the other player in the net)
					char *startBuf = msgBuf;
					startBuf += 9; // skip "`TA "
					while (strlen(startBuf))
					{
						int x, y, z, hp, mana, maxHp, maxMana, distance;
						char charName[1000];
						charName[0] = '\0';
						sscanf(startBuf, "%[^,],%d,%d,%d,%d,%d,%d,%d,%d", charName, &x, &y, &z, &hp, &maxHp, &mana, &maxMana, &distance);
						charName[32] = '\0';
						if (strlen(charName))
							connectedNodes.refreshNodeInfo(charName, hp, mana, maxHp, maxMana, x, y, z, -1, distance + 1);
						// move at least strlen(charName) - to make sure that ' '
						// in the char name does not interfere
						startBuf += strlen(charName);
						while (*startBuf != ' ' && *startBuf != '\0')
							startBuf++;
						while (*startBuf == ' ')
							startBuf++;
						while (*startBuf == '\r')
							startBuf++;
						while (*startBuf == '\n')
							startBuf++;
					}
				}
			}
		}

	}
	toolThreadShouldStop = 0;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMod_teamApp construction

CMod_teamApp::CMod_teamApp()
{
	m_configDialog = NULL;
	m_started      = 0;
	m_configData   = new CConfigData();
}

CMod_teamApp::~CMod_teamApp()
{
	if (m_configDialog)
	{
		m_configDialog->DestroyWindow();
		delete m_configDialog;
	}
	delete m_configData;
}

char * CMod_teamApp::getName()
{
	return "Team cooperation";
}

int CMod_teamApp::isStarted()
{
	return m_started;
}

void CMod_teamApp::start()
{
	superStart();
	if (m_configDialog)
	{
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}

	DWORD threadId;

	toolThreadShouldStop = 0;
	toolThreadHandle     = ::CreateThread(NULL, 0, toolThreadProc, m_configData, 0, &threadId);
	m_started            = 1;
}

void CMod_teamApp::stop()
{
	toolThreadShouldStop = 1;
	while (toolThreadShouldStop)
	{
		Sleep(50);
	};
	m_started = 0;

	if (m_configDialog)
	{
		m_configDialog->enableControls();
		m_configDialog->activateEnableButton(false);
	}
}

void CMod_teamApp::showConfigDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!m_configDialog)
	{
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
		if (m_started)
			disableControls();
		else
			enableControls();
		m_configDialog->m_enable.SetCheck(m_started);
	}
	m_configDialog->ShowWindow(SW_SHOW);
}

void CMod_teamApp::configToControls()
{
	if (m_configDialog)

		m_configDialog->configToControls(m_configData);
}

void CMod_teamApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}

void CMod_teamApp::disableControls()
{
	if (m_configDialog)
		m_configDialog->disableControls();
}

void CMod_teamApp::enableControls()
{
	if (m_configDialog)
		m_configDialog->enableControls();
}

char *CMod_teamApp::getVersion()
{
	return "1.0";
}

int CMod_teamApp::validateConfig(int showAlerts)
{
	return 1;
}

void CMod_teamApp::resetConfig()
{
	if (m_configData)
	{
		delete m_configData;
		m_configData = NULL;
	}
	m_configData = new CConfigData();
}

void CMod_teamApp::loadConfigParam(char *paramName, char *paramValue)
{
}

char *CMod_teamApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0] = 0;


	return buf;
}

char *CMod_teamApp::getConfigParamName(int nr)
{
	return NULL;
}
