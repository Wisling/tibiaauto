// WhiteList.cpp : implementation file
//

#include "stdafx.h"
#include "mod_showmap.h"
#include "ConfigWindow.h"
#include "TibiaTile.h"
#include "memReaderProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapConfig dialog


CMapConfig::CMapConfig(CTibiaTile *memTilesForConfig[10], int x, int y, CWnd* pParent /*=NULL*/)
: MyDialog(CMapConfig::IDD, pParent)
{
	xMem = x;
	yMem = y;
	currentStackPos = 0;
	//{{AFX_DATA_INIT(CMapConfig)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMapConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapConfig)
	DDX_Control(pDX, IDC_FRAME_USE, m_UseFrame);
	DDX_Control(pDX, IDC_FRAME_TILE_TYPE, m_TileTypeFrame);
	DDX_Control(pDX, IDC_FRAME_MOVEMENT, m_MovementFrame);
	DDX_Control(pDX, IDC_FRAME_CHARACTERISTICS, m_CharacteristicsFrame);
	DDX_Control(pDX, IDC_COMMIT, m_Commit);
	DDX_Control(pDX, IDC_TILE_ID, m_TileId);
	DDX_Control(pDX, IDC_SPEED, m_Speed);
	DDX_Control(pDX, IDC_ALWAYS_ON_TOP, m_AlwaysOnTop);
	DDX_Control(pDX, IDC_BLOCKING, m_Blocking);
	DDX_Control(pDX, IDC_WALKABLE, m_Walkable);
	DDX_Control(pDX, IDC_GO_DOWN, m_GoDown);
	DDX_Control(pDX, IDC_GO_UP, m_GoUp);
	DDX_Control(pDX, IDC_GROUND, m_Ground);
	DDX_Control(pDX, IDC_CONTAINER, m_Container);
	DDX_Control(pDX, IDC_DEPOT, m_Depot);
	DDX_Control(pDX, IDC_MOVABLE, m_Moveable);
	DDX_Control(pDX, IDC_ROPE, m_RequireRope);
	DDX_Control(pDX, IDC_SHOVEL, m_RequireShovel);
	DDX_Control(pDX, IDC_USE_TILE, m_RequireUse);
	DDX_Control(pDX, IDC_STACKABLE, m_Stackable);
	DDX_Control(pDX, IDC_TELEPORTER, m_Teleporter);
	DDX_Control(pDX, IDC_PREVIOUS_CONFIG_TILE, m_Previous);
	DDX_Control(pDX, IDC_NEXT_CONFIG_TILE, m_Next);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMapConfig, CDialog)
	//{{AFX_MSG_MAP(CMapConfig)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_COMMIT, OnCommit)
	ON_BN_CLICKED(IDC_PREVIOUS_CONFIG_TILE, OnPreviousTile)
	ON_BN_CLICKED(IDC_NEXT_CONFIG_TILE, OnNextTile)
	ON_BN_CLICKED(IDC_GO_DOWN, OnGoDown)
	ON_BN_CLICKED(IDC_GO_UP, OnGoUp)
	ON_BN_CLICKED(IDC_ROPE, OnRope)
	ON_BN_CLICKED(IDC_USE_TILE, OnClick)
	ON_BN_CLICKED(IDC_SHOVEL, OnShovel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapConfig message handlers

void CMapConfig::OnCommit() {
	CMemReaderProxy reader;
	saveTile();
	for (int pos = 0; pos < 10; pos++) {
		if (tileId[pos]) {
			reader.setTibiaTile(tileId[pos], memTilesForConfig[pos]);
		}
	}
	this->EndDialog(IDOK);
}

void CMapConfig::Mem2Config() {
	CMemReaderProxy reader;
	int outOfRange = 0;
	// make (x,y) relative to the center
	xMem -= 10;
	yMem -= 10;

	for (int i = 0; i < 10; i++)
		tileId[i] = 0;
	
	if (xMem < -8 || xMem > 9 || yMem < -6 || yMem > 7) outOfRange = 1;
	if (!outOfRange) {		
		int count=reader.mapGetPointItemsCount(point(xMem, yMem, 0));
		if (count > 10) count = 10;
		for (int pos = 0; pos < count; pos++) {
			tileId[pos] = reader.mapGetPointItemId(point(xMem, yMem, 0),pos);
			memTilesForConfig[pos] = reader.getTibiaTile(tileId[pos]);
		}
	
	}
	currentStackPos = 0;
	loadTile(tileId[currentStackPos], currentStackPos);
	if (!currentStackPos) 
		m_Previous.EnableWindow(false);
	else
		m_Previous.EnableWindow(true);
	if (!tileId[currentStackPos + 1])
		m_Next.EnableWindow(false);
	else
		m_Next.EnableWindow(true);
}

void CMapConfig::loadTile(int tileId, int pos) {
	char buf[32];

	sprintf(buf, "%d", tileId);
	m_TileId.SetWindowText(buf);
	sprintf(buf, "%d", memTilesForConfig[pos]->speed);
	m_Speed.SetWindowText(buf);
	m_AlwaysOnTop.SetCheck(memTilesForConfig[pos]->alwaysOnTop);
	m_Blocking.SetCheck(memTilesForConfig[pos]->blocking);
	m_Walkable.SetCheck(memTilesForConfig[pos]->canWalkThrough);
	m_GoDown.SetCheck(memTilesForConfig[pos]->goDown);
	m_GoUp.SetCheck(memTilesForConfig[pos]->goUp);
	m_Ground.SetCheck(memTilesForConfig[pos]->ground);
	m_Container.SetCheck(memTilesForConfig[pos]->isContainer);
	m_Depot.SetCheck(memTilesForConfig[pos]->isDepot);
	m_Moveable.SetCheck(memTilesForConfig[pos]->notMoveable?0:1);
	m_RequireRope.SetCheck(memTilesForConfig[pos]->requireRope);
	m_RequireShovel.SetCheck(memTilesForConfig[pos]->requireShovel);
	m_RequireUse.SetCheck(memTilesForConfig[pos]->requireUse);
	m_Stackable.SetCheck(memTilesForConfig[pos]->stackable);
	m_Teleporter.SetCheck(memTilesForConfig[pos]->isTeleporter);
}

void CMapConfig::saveTile() {
	char buf[32];
	m_TileId.GetWindowText(buf,31); tileId[currentStackPos] = atoi(buf);
	m_Speed.GetWindowText(buf, 31); memTilesForConfig[currentStackPos]->speed = atoi(buf);
	

	memTilesForConfig[currentStackPos]->alwaysOnTop = m_AlwaysOnTop.GetCheck();
	memTilesForConfig[currentStackPos]->blocking = m_Blocking.GetCheck();
	memTilesForConfig[currentStackPos]->canWalkThrough = m_Walkable.GetCheck();
	memTilesForConfig[currentStackPos]->goDown = m_GoDown.GetCheck();
	memTilesForConfig[currentStackPos]->goUp = m_GoUp.GetCheck();
	memTilesForConfig[currentStackPos]->ground = m_Ground.GetCheck();
	memTilesForConfig[currentStackPos]->isContainer = m_Container.GetCheck();
	memTilesForConfig[currentStackPos]->isDepot = m_Depot.GetCheck();
	memTilesForConfig[currentStackPos]->notMoveable = m_Moveable.GetCheck()?0:1;
	memTilesForConfig[currentStackPos]->requireRope = m_RequireRope.GetCheck();
	memTilesForConfig[currentStackPos]->requireShovel = m_RequireShovel.GetCheck();
	memTilesForConfig[currentStackPos]->requireUse = m_RequireUse.GetCheck();
	memTilesForConfig[currentStackPos]->stackable = m_Stackable.GetCheck();
	memTilesForConfig[currentStackPos]->isTeleporter = m_Teleporter.GetCheck();
}

BOOL CMapConfig::OnInitDialog() {
	CDialog::OnInitDialog();
	skin.SetButtonSkin( m_Previous);
	skin.SetButtonSkin(	m_Commit);
	skin.SetButtonSkin( m_Next);
	
	Mem2Config();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CMapConfig::OnPreviousTile() {
	saveTile();
	if (currentStackPos) {
		currentStackPos--;
		loadTile(tileId[currentStackPos], currentStackPos);
	}
	else return;
	if (!currentStackPos) 
		m_Previous.EnableWindow(false);
	else
		m_Previous.EnableWindow(true);
	if (!tileId[currentStackPos + 1])
		m_Next.EnableWindow(false);
	else
		m_Next.EnableWindow(true);
}

void CMapConfig::OnNextTile() {
	saveTile();
	if (currentStackPos < 9) {
		currentStackPos++;
		loadTile(tileId[currentStackPos], currentStackPos);
	}
	else return;
	if (!currentStackPos) 
		m_Previous.EnableWindow(false);
	else
		m_Previous.EnableWindow(true);
	if (!tileId[currentStackPos + 1])
		m_Next.EnableWindow(false);
	else
		m_Next.EnableWindow(true);
}


void CMapConfig::OnRope() {
	if (m_RequireRope.GetCheck()) {
			m_GoUp.SetCheck(true);
			m_RequireShovel.SetCheck(false);
			m_RequireUse.SetCheck(false);
	}
}

void CMapConfig::OnClick() {
	if (m_GoDown.GetCheck()) {
			m_GoUp.SetCheck(true);
			m_RequireRope.SetCheck(false);
			m_RequireShovel.SetCheck(false);
	}
}

void CMapConfig::OnShovel() {
	if (m_GoDown.GetCheck()) {
			m_GoDown.SetCheck(true);
			m_RequireRope.SetCheck(false);
			m_RequireUse.SetCheck(false);
	}
}

void CMapConfig::OnGoDown() {
	if (m_GoDown.GetCheck()) {
			m_GoUp.SetCheck(false);
			m_RequireRope.SetCheck(false);
			m_RequireUse.SetCheck(false);
	}
}


void CMapConfig::OnGoUp() {
	if (m_GoUp.GetCheck()) {
			m_GoDown.SetCheck(false);
			m_RequireShovel.SetCheck(false);
	}
}