// ToolMonsterShow.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ToolMonsterShow.h"
#include "MonsterButton.h"
#include "MemReaderProxy.h"
#include "TibiaCharacter.h"
#include "MemConstData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolMonsterShow dialog


CToolMonsterShow::CToolMonsterShow(CWnd* pParent /*=NULL*/)
	: MyDialog(CToolMonsterShow::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolMonsterShow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	CMemReaderProxy reader;
	memConstData = reader.getMemConstData();
}

CToolMonsterShow::~CToolMonsterShow(){
	for (int x=0;x<2*10+1;x++){
		for (int y=0;y<2*8+1;y++){
			delete m_monsterButtons[x][y];
		}
	}
}

void CToolMonsterShow::DoDataExchange(CDataExchange* pDX)
{
	MyDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolMonsterShow)
	DDX_Control(pDX, IDC_FRAME_OPTIONS, m_OptionsFrame);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_FRAME_MONSTER_MAP, m_MonsterMapFrame);
	DDX_Control(pDX, IDC_MONSTERSHOW_REVEALSLIME, m_revealSlime);
	DDX_Control(pDX, IDC_MONSTERSHOW_FREEZE, m_freeze);
	DDX_Control(pDX, IDC_MONSTERSHOW_ONERELLVL, m_showOneRelLvl);
	DDX_Control(pDX, IDC_MONSTERSHOW_MONSTERNIFO, m_monsterInfo);
	DDX_Control(pDX, IDC_MONSTERSHOW_MONSTERNIFO_BELOW, m_monsterInfoBelow);
	DDX_Control(pDX, IDC_MONSTERSHOW_MONSTERNIFO_SAME, m_monsterInfoSame);
	DDX_Control(pDX, IDC_MONSTERSHOW_MONSTERNIFO_ABOVE, m_monsterInfoAbove);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolMonsterShow, CDialog)
	//{{AFX_MSG_MAP(CToolMonsterShow)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_MONSTERSHOW_FREEZE, OnMonstershowFreeze)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_MONSTERSHOW_REVEALSLIME, OnMonstershowRevealslime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolMonsterShow message handlers

void CToolMonsterShow::DoSetButtonSkin(){
	skin.SetButtonSkin(	m_OK);
}

BOOL CToolMonsterShow::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();
	
	int x;
	int y;

	for (x=0;x<2*10+1;x++)
	{
		for (y=0;y<2*8+1;y++)
		{

			CButton *but = new CMonsterButton(x,y);
			RECT rect;
			rect.top=29+y*20;
			rect.left=20+x*20;
			rect.right=rect.left+14;
			rect.bottom=rect.top+14;
			
			but->Create("test",WS_CHILD|WS_VISIBLE|BS_RADIOBUTTON|BS_FLAT|BS_3STATE,rect,this,IDC_MONSTERSHOW_FIRSTBUTTON);
						
			m_monsterButtons[x][y]=but;
		}
	}

	refreshVisibleCreatures();

	SetTimer(1001,500,NULL);
	
	return TRUE;
}

void CToolMonsterShow::OnMouseMove(UINT nFlags, CPoint point)
{
	showMonsterDetails(-1,-1);

	CDialog::OnMouseMove(nFlags, point);
}

void CToolMonsterShow::OnOK()
{
	ShowWindow(SW_HIDE);
}

BOOL CToolMonsterShow::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CToolMonsterShow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return CDialog::OnCommand(wParam, lParam);
}

void CToolMonsterShow::showMonsterDetails(int x, int y)
{
	CMemReaderProxy reader;
	char buf[1024];
	char buf2[1024];
	
	int creatureNr;

	CTibiaCharacter *self = reader.readSelfCharacter();
	

	sprintf(buf,"creatures:");

	for (creatureNr=0;creatureNr<memConstData.m_memMaxCreatures;creatureNr++)
	{
		CTibiaCharacter *ch=reader.readVisibleCreature(creatureNr);
		if (ch->tibiaId == 0){
			delete ch;
			break;
		}
		int relX=10+(ch->x-self->x);
		int relY=8+(ch->y-self->y);
		int relZ=self->z-ch->z;

		if (ch->visible&&relX==x&&relY==y)
		{
			sprintf(buf2,"%s [z=%d|hp=%d%%|nr=%d] ",ch->name,relZ,ch->hpPercLeft,creatureNr);
			strcat(buf,buf2);
		};
		
		delete ch;
	}

	m_monsterInfo.SetWindowText(buf);

	delete self;
}

void CToolMonsterShow::refreshVisibleCreatures()
{
	CMemReaderProxy reader;
	int creatureNr;
	int x;
	int y;
	int z;
	int relZ;

/*m_freeze.GetCheck()*/

	char listBufBelow[1024];
	char listBufSame[1024];
	char listBufAbove[1024];

	CTibiaCharacter *self = reader.readSelfCharacter();

	sprintf(listBufBelow,"");
	sprintf(listBufSame,"");
	sprintf(listBufAbove,"");

	for (x=0;x<2*10+1;x++)
	{
		for (y=0;y<2*8+1;y++)
		{
			m_monsterButtons[x][y]->SetCheck(0);
		}
	}

	for (creatureNr=0;creatureNr<memConstData.m_memMaxCreatures;creatureNr++)
	{
		CTibiaCharacter *ch=reader.readVisibleCreature(creatureNr);
		if (ch->tibiaId == 0){
			delete ch;
			break;
		}
		if (ch->visible)
		{
			x=10+(ch->x-self->x);
			y=8+(ch->y-self->y);
			z=ch->z;

			relZ=self->z-ch->z;
			
			if (x<0||y<0||x>20||y>16)
			{
				// monster out of range - it might happen sometimes
				delete ch;
				continue;
			};
			if (self->z==z&&m_monsterButtons[x][y]->GetCheck()==0)
			{
				m_monsterButtons[x][y]->SetCheck(1);
			} else {
				m_monsterButtons[x][y]->SetCheck(2);
			};
			
			if (relZ < 0) { //below player
				if ((m_showOneRelLvl.GetCheck() == 0) || (relZ == -1)) {
					sprintf(listBufBelow,"%s%s (%d,%d%%)\r\n", listBufBelow, ch->name, relZ, ch->hpPercLeft);
				}
			}
			else if (relZ > 0) { //above player
				if ((m_showOneRelLvl.GetCheck() == 0) || (relZ == 1)) {
					sprintf(listBufAbove,"%s%s (%d,%d%%)\r\n", listBufAbove, ch->name, relZ, ch->hpPercLeft);
				}
			}
			else { //same level
				sprintf(listBufSame,"%s%s (%d,%d%%)\r\n", listBufSame, ch->name, relZ, ch->hpPercLeft);
			}

		};

		delete ch;
	};

	m_monsterInfoBelow.SetWindowText(listBufBelow);
	m_monsterInfoSame.SetWindowText(listBufSame);
	m_monsterInfoAbove.SetWindowText(listBufAbove);

	delete self;
}

void CToolMonsterShow::OnTimer(UINT nIDEvent)
{
	CMemReaderProxy reader;
	if (nIDEvent==1001)
	{
		KillTimer(1001);
		refreshVisibleCreatures();
		if (!m_freeze.GetCheck())
			SetTimer(1001,500,NULL);
	};
	if (nIDEvent==1002)
	{
		int creatureNr;
		KillTimer(1002);

		for (creatureNr=0;creatureNr<memConstData.m_memMaxCreatures;creatureNr++)
		{
			CTibiaCharacter *ch=reader.readVisibleCreature(creatureNr);
			if (ch->tibiaId == 0){
				delete ch;
				break;
			}
			if (ch->visible)
			{
				if (ch->monsterType==20)
				{
					// this is a summoned slime - rename it
					reader.writeVisibleCreatureName(creatureNr,"Summoned Slime");
				}
			}

			delete ch;
		}

		if (m_revealSlime.GetCheck())
			SetTimer(1002,200,NULL);
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CToolMonsterShow::OnMonstershowFreeze()
{
	if (m_freeze.GetCheck())
		KillTimer(1001); else
		SetTimer(1001,500,NULL);
}

void CToolMonsterShow::OnClose()
{
	ShowWindow(SW_HIDE);
}

void CToolMonsterShow::OnMonstershowRevealslime()
{
	if (m_revealSlime.GetCheck())
	{
		SetTimer(1002,200,NULL);
	} else {
		KillTimer(1002);
	}
}
