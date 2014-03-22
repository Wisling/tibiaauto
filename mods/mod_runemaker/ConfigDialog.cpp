// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_runemaker.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"
#include "ToolContainerContent.h"
#include "TibiaItemProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_runemakerApp *app,CWnd* pParent /*=NULL*/)
	: MyDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app=app;
}


void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_FRAME_STATISTICS, m_StatisticsFrame);
	DDX_Control(pDX, IDC_FRAME_RUNE_OPTIONS, m_RuneOptionsFrame);
	DDX_Control(pDX, IDC_FRAME_GENERAL_OPTIONS, m_GeneralOptionsFrame);
	DDX_Control(pDX, IDC_FRAME_BACKPACK_VIEWER, m_BackpackViewerFrame);
	DDX_Control(pDX, IDC_TOOL_MAXMANAUSE, m_maxUse);
	DDX_Control(pDX, IDC_TOOL_PREMIUM, m_premium);
	DDX_Control(pDX, IDC_TOOL_RUNEMAKER_MANALIMIT, m_manaLimit);
	DDX_Control(pDX, IDC_TOOL_RUNEMAKER_MANA, m_mana);
	DDX_Control(pDX, IDC_TOOL_RUNEMAKER_CONT, m_safe);
	DDX_Control(pDX, IDC_TOOL_RUNEMAKER_SPELLS, m_spells);
	DDX_Control(pDX, IDC_TOOL_RUNEMAKER_SOUL, m_soulPoints);
	DDX_Control(pDX, IDC_TOOL_RUNEMAKER_RUNESPELL, m_spell);
	DDX_Control(pDX, IDC_TOOL_RUNEMAKER_INFO_RUNETIME, m_infoRuneTime);
	DDX_Control(pDX, IDC_TOOL_RUNEMAKER_INFO_SOULPOINTSTIME, m_infoSoulpointsTime);
	DDX_Control(pDX, IDC_TOOL_RUNEMAKER_INFO_FOOD, m_infoFood);
	DDX_Control(pDX, IDC_TOOL_RUNEMAKER_INFO_BLANKS, m_infoBlanks);
	DDX_Control(pDX, IDC_ADDSPELLTOLIST, m_addSpell);
	DDX_Control(pDX, IDC_DELETEFROMLIST, m_deleteSpell);
	DDX_Control(pDX, IDC_LOADSPELLFROMLIST, m_loadSpell);
	DDX_Control(pDX, IDC_MAKENOW, m_makeNow);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDC_TOOL_RANDOMCAST,m_randomCast);
	DDX_Control(pDX, IDC_TOOL_USESPEAR,m_useSpear);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_TOOL_RUNEMAKER_CONT, OnDblclkToolRunemakerCont)
	ON_BN_CLICKED(IDC_ADDSPELLTOLIST, OnAddspelltolist)
	ON_BN_CLICKED(IDC_LOADSPELLFROMLIST, OnLoadspellfromlist)
	ON_BN_CLICKED(IDC_DELETEFROMLIST, OnDeletefromlist)
	ON_BN_CLICKED(IDC_MAKENOW, OnMakenow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog message handlers

void CConfigDialog::OnOK()
{
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnClose()
{
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnEnable()
{
	if (m_enable.GetCheck())
	{
		m_app->controlsToConfig();
		if (m_app->validateConfig(1))
		{
			m_app->start();
		} else {
			m_enable.SetCheck(0);
		}
	} else {
		m_app->stop();
	}
}

void CConfigDialog::disableControls()
{
	m_mana.EnableWindow(false);
	m_manaLimit.EnableWindow(false);
	m_soulPoints.EnableWindow(false);
	m_spell.EnableWindow(false);
	m_premium.EnableWindow(false);
	m_maxUse.EnableWindow(false);
	m_addSpell.EnableWindow(false);
	m_loadSpell.EnableWindow(false);
	m_deleteSpell.EnableWindow(false);
	m_useSpear.EnableWindow(false);
	m_randomCast.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_mana.EnableWindow(true);
	m_manaLimit.EnableWindow(true);
	m_soulPoints.EnableWindow(true);
	m_spell.EnableWindow(true);
	m_premium.EnableWindow(true);
	m_maxUse.EnableWindow(true);
	m_addSpell.EnableWindow(true);
	m_loadSpell.EnableWindow(true);
	m_deleteSpell.EnableWindow(true);
	m_useSpear.EnableWindow(true);
	m_randomCast.EnableWindow(true);
}


void CConfigDialog::configToControls(CConfigData *configData)
{
	char buf[1024];

	m_spell.SetWindowText(configData->spell);
	sprintf(buf,"%d",configData->mana);m_mana.SetWindowText(buf);
	sprintf(buf,"%d",configData->manaLimit);m_manaLimit.SetWindowText(buf);
	sprintf(buf,"%d",configData->soulPoints);m_soulPoints.SetWindowText(buf);
	m_premium.SetCheck(configData->premium);
	m_maxUse.SetCheck(configData->maxUse);
	m_randomCast.SetCheck(configData->randomCast);
	m_useSpear.SetCheck(configData->useSpear);

	m_spells.DeleteAllItems();
	for (int i=0; i<15; i++) {
		if (configData->listSpells[i].words[0] != '0') {
			int c=m_spells.GetItemCount();

			sprintf(buf,"%d",i+1);

			m_spells.InsertItem(c,buf);
			m_spells.SetItemData(c,i);

			sprintf(buf,"%s",configData->listSpells[i].words);
			m_spells.SetItemText(c,1,buf);

			sprintf(buf,"%d",configData->listSpells[i].soulPoints);
			m_spells.SetItemText(c,2,buf);

			sprintf(buf,"%d",configData->listSpells[i].mana);
			m_spells.SetItemText(c,3,buf);

		}
	}
}

CConfigData * CConfigDialog::controlsToConfig()
{
	char buf[1024];
	CConfigData *newConfigData = new CConfigData();

	m_spell.GetWindowText(newConfigData->spell,127);
	m_mana.GetWindowText(buf,127);newConfigData->mana=atoi(buf);
	m_manaLimit.GetWindowText(buf,127);newConfigData->manaLimit=atoi(buf);
	m_soulPoints.GetWindowText(buf,127);newConfigData->soulPoints=atoi(buf);
	newConfigData->premium	= m_premium.GetCheck();
	newConfigData->maxUse	= m_maxUse.GetCheck();
	newConfigData->randomCast	= m_randomCast.GetCheck();
	newConfigData->useSpear	= m_useSpear.GetCheck();

	for (int i=0; i < 15; i++) {
		if (i >= m_spells.GetItemCount()) {
			newConfigData->listSpells[i].mana = 0;
			newConfigData->listSpells[i].words[0] = '0';
			newConfigData->listSpells[i].soulPoints = 5;
		} else {
			sprintf(newConfigData->listSpells[i].words,"%s",m_spells.GetItemText(i,1));
			newConfigData->listSpells[i].soulPoints = atoi(m_spells.GetItemText(i,2));
			newConfigData->listSpells[i].mana = atoi(m_spells.GetItemText(i,3));
		}
	}

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	CTibiaItemProxy itemProxy;
	switch (nIDEvent)
	{
	case 1001:
		{
			KillTimer(1001);
			// container refresh event
			CMemReaderProxy reader;
			CMemConstData memConstData = reader.getMemConstData();
			CTibiaCharacter *myself = reader.readSelfCharacter();
			int i;
			char buf[256];
			
			
			// check whether some container should be added
			int openContNr=0;
			int openContMax=reader.readOpenContainerCount();
			for (i=0;i<memConstData.m_memMaxContainers && openContNr<openContMax;i++)
			{
				
				CTibiaContainer *container = reader.readContainer(i);

				if (container->flagOnOff)
				{
					openContNr++;
					int j;
					int foundContainer=0;
					
					for (j=0;j<m_safe.GetItemCount();j++)
					{
						if (int(m_safe.GetItemData(j))==i)
						{
							foundContainer=1;
														
							updateListItem(&m_safe,container,j);
							// we have found, but refresh is needed
						}
					}
					if (!foundContainer)
					{
						int c=m_safe.GetItemCount();

						sprintf(buf,"%d",i+1);
						m_safe.InsertItem(c,buf);
						m_safe.SetItemData(c,i);

						updateListItem(&m_safe,container,c);
					};
				};
				delete container;
			}

			
			// check whether some container should be deleted
			int mustRun=1;
			while (mustRun)
			{
				mustRun=0;
				for (i=0;i<m_safe.GetItemCount();i++)
				{
					int containerNr=m_safe.GetItemData(i);
					CTibiaContainer *container = reader.readContainer(containerNr);
					
					if (!container->flagOnOff)
					{
						m_safe.DeleteItem(i);
						mustRun=1;
						delete container;
						break;
					}
					
					delete container;
				};
			}
			

			// now refresh info fields
			int blanksCount=0;
			int foodFishCount=0;
			for (i=m_safe.GetItemCount()-1;i>=0;i--)
			{
				int containerNr=m_safe.GetItemData(i);
				CTibiaContainer *container = reader.readContainer(containerNr);
				if (container->flagOnOff)
				{
					blanksCount+=container->countItemsOfType(itemProxy.getValueForConst("runeBlank"));
					foodFishCount+=container->countItemsOfType(itemProxy.getValueForConst("fish"));
				};
				delete container;
			}
			sprintf(buf,"Total blank runes: %d", blanksCount);
			m_infoBlanks.SetWindowText(buf);
			
			int fishTimeH = foodFishCount*120/3600;
			int fishTimeMin = (foodFishCount*120/60)%60;
			int fishTimeSec = foodFishCount*120%60;
									
			sprintf(buf,"Food for: %dh %dmin %ds",fishTimeH,fishTimeMin,fishTimeSec);
			m_infoFood.SetWindowText(buf);

			m_mana.GetWindowText(buf,127);
			int blanksTime = int(blanksCount * atoi(buf)*(m_premium.GetCheck()?1:1.5));
			int blanksTimeH = blanksTime/3600;
			int blanksTimeMin = (blanksTime/60)%60;
			int blanksTimeSec = blanksTime%60;

			sprintf(buf,"Blanks for: %dh %dmin %ds",blanksTimeH,blanksTimeMin,blanksTimeSec);
			m_infoRuneTime.SetWindowText(buf);

			m_soulPoints.GetWindowText(buf,127);
			int spTime = 0;
			if (blanksCount&&atoi(buf))
			{
				spTime = (myself->soulPoints / atoi(buf)) * (blanksTime / blanksCount);
			}
			int spTimeH = spTime/3600;
			int spTimeMin = (spTime/60)%60;
			int spTimeSec = spTime%60;
						
			sprintf(buf,"Soulpoints for: %dh %dmin %ds",spTimeH,spTimeMin,spTimeSec);
			m_infoSoulpointsTime.SetWindowText(buf);
			
			
			SetTimer(1001,250,NULL);
			delete myself;
								
		}
		break;
	};
	
	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin(){
	skin.SetButtonSkin( m_addSpell);
	skin.SetButtonSkin( m_deleteSpell);
	skin.SetButtonSkin( m_loadSpell);
	skin.SetButtonSkin( m_makeNow);
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_enable);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	initialiseContainers();
	
	SetTimer(1001,250,NULL);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CConfigDialog::activateEnableButton(int enable)
{
	m_enable.SetCheck(enable);
}

void CConfigDialog::initialiseContainers()
{
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	int i;
	
	m_safe.InsertColumn(0,"Nr",LVCFMT_LEFT,25);
	m_safe.InsertColumn(1,"Name",LVCFMT_LEFT,80);
	m_safe.InsertColumn(2,"Items",LVCFMT_LEFT,40);
	m_safe.InsertColumn(3,"Size",LVCFMT_LEFT,40);
	m_safe.InsertColumn(4,"Blanks",LVCFMT_LEFT,50);
	
	m_safe.SetExtendedStyle(m_safe.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	int openContNr=0;
	int openContMax=reader.readOpenContainerCount();
	for (i=0;i<memConstData.m_memMaxContainers && openContNr<openContMax;i++)
	{
		char buf[256];
		CTibiaContainer *container = reader.readContainer(i);
		
		if (container->flagOnOff)
		{
			openContNr++;
			int itemNr=m_safe.GetItemCount();

			sprintf(buf,"%d",i+1);
			m_safe.InsertItem(itemNr,buf);
			m_safe.SetItemData(itemNr,i);

			updateListItem(&m_safe,container,itemNr);
		}
		

		delete container;
	}
	

	m_spells.InsertColumn(0,"Nr",LVCFMT_LEFT,30);
	m_spells.InsertColumn(1,"Words",LVCFMT_LEFT,160);
	m_spells.InsertColumn(2,"SP",LVCFMT_LEFT,35);
	m_spells.InsertColumn(3,"Mana",LVCFMT_LEFT,50);

	m_spells.SetExtendedStyle(m_spells.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

}

void CConfigDialog::updateListItem(CListCtrl *list, CTibiaContainer *container, int itemNr)
{
	CTibiaItemProxy itemProxy;
	char buf[256];
	
	sprintf(buf,"%s",itemProxy.getItemName(container->objectId));
	list->SetItemText(itemNr,1,buf);
	sprintf(buf,"%d",container->itemsInside);
	list->SetItemText(itemNr,2,buf);
	sprintf(buf,"%d",container->size);
	list->SetItemText(itemNr,3,buf);
	sprintf(buf,"%d",container->countItemsOfType(itemProxy.getValueForConst("runeBlank")));
	list->SetItemText(itemNr,4,buf);

}

void CConfigDialog::OnDblclkToolRunemakerCont(NMHDR* pNMHDR, LRESULT* pResult)
{
	POSITION pos = m_safe.GetFirstSelectedItemPosition();
	int itemNr;
	if (pos)
	{
		itemNr=m_safe.GetNextSelectedItem(pos);

		CToolContainerContent *dialog = new CToolContainerContent(m_safe.GetItemData(itemNr));
		dialog->DoModal();
		delete dialog;
	}
	

	*pResult = 0;
}

void CConfigDialog::OnAddspelltolist()
{
	char buf[1025];

	int c=m_spells.GetItemCount();
	int i=c;

	sprintf(buf,"%d",i+1);

	m_spells.InsertItem(c,buf);
	m_spells.SetItemData(c,i);

	m_spell.GetWindowText(buf,1024);
	m_spells.SetItemText(c,1,buf);

	m_soulPoints.GetWindowText(buf,1024);
	m_spells.SetItemText(c,2,buf);

	m_mana.GetWindowText(buf,1024);
	m_spells.SetItemText(c,3,buf);
	
}

void CConfigDialog::OnLoadspellfromlist()
{

	char buf[1025];

	int c = m_spells.GetSelectionMark();

	sprintf(buf,"%s",m_spells.GetItemText(c,1));
	m_spell.SetWindowText(buf);

	sprintf(buf,"%s",m_spells.GetItemText(c,2));
	m_soulPoints.SetWindowText(buf);

	sprintf(buf,"%s",m_spells.GetItemText(c,3));
	m_mana.SetWindowText(buf);

}

void CConfigDialog::OnDeletefromlist()
{
	
	m_spells.DeleteItem(m_spells.GetSelectionMark());
	
}

void CConfigDialog::OnMakenow()
{

	int enable=m_makeNow.GetCheck();
	m_app->makeNow(enable);
	
}
