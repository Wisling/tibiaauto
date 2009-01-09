// ToolMapShow.cpp : implementation file
//

#include "stdafx.h"
#include "ToolMapShow.h"

#include "MemReaderProxy.h"
#include "TibiaCharacter.h"
#include "MapButton.h"
#include "resource.h"
#include "TibiaMapProxy.h"
#include "TibiaMapPoint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTibiaMapProxy tibiaMap;
static int prevX=0,prevY=0,prevZ=0,iter=0;

/////////////////////////////////////////////////////////////////////////////
// CONSTANTS
int MINX=-8,MAXX=9,MINY=-6,MAXY=7;

/////////////////////////////////////////////////////////////////////////////
// CToolMapShow dialog


CToolMapShow::CToolMapShow(CWnd* pParent /*=NULL*/)
	: CDialog(CToolMapShow::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolMapShow)
	//}}AFX_DATA_INIT
}


void CToolMapShow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolMapShow)
	DDX_Control(pDX, IDC_TOOL_MAPSHOW_RESEARCH, m_research);
	DDX_Control(pDX, IDC_TOOL_MAPSHOW_EXTENDED_RESEARCH, m_extendedResearch);
	DDX_Control(pDX, IDC_TILE_INFO, m_tileInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolMapShow, CDialog)
	//{{AFX_MSG_MAP(CToolMapShow)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_TOOL_MAPSHOW_CLEAR, OnToolMapshowClear)
	ON_BN_CLICKED(IDC_TOOL_MAPSHOW_RESEARCH, OnToolMapshowResearch)
	ON_BN_CLICKED(IDC_TOOL_MAPSHOW_EXTENDED_RESEARCH, OnToolMapshowExtendedResearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolMapShow message handlers

void CToolMapShow::OnClose() 
{
	ShowWindow(SW_HIDE);
}

void CToolMapShow::OnOK() 
{
	ShowWindow(SW_HIDE);
}

BOOL CToolMapShow::OnInitDialog() 
{
	CDialog::OnInitDialog();
	alt  = flicker = back= 0;
	int x;
	int y;
	RECT rect;
	GetClientRect(&rect);
	
	for (x=0;x<21;x++)
	{
		for (y =0 ;y<21;y++)
		{

			
			CMapButton *but = new CMapButton(x,y);			
			
			rect.top=20+y*20;
			rect.left=((rect.right - rect.left) / 2) - (20 * (10 - x)) - 10;
			rect.right=rect.left+20;
			rect.bottom=rect.top+20;
			
			but->Create("test",WS_CHILD|WS_VISIBLE|BS_FLAT|BS_PUSHLIKE|BS_OWNERDRAW,rect,this,IDC_MAPSHOW_FIRSTBUTTON);
			but->LoadBitmaps(IDB_MAP_EMPTY);
			but->m_value=-1;
			m_mapButtonImage[x][y]=IDB_MAP_EMPTY;						
			m_mapButtons[x][y]=but;			
			
	GetClientRect(&rect);
			
		}
	}

	refreshVisibleMap();

	SetTimer(1001,250,NULL);
	SetTimer(1004,(rand() % 360000) + 1,NULL);
	
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CToolMapShow::OnEraseBkgnd(CDC* pDC) 
{
	if(!alt) {
		CDialog::OnEraseBkgnd(pDC);	
	}
	else {
		CRect rect;
		GetClientRect(&rect);
		CDC dc;
		CDC dcBuffer;
		dc.CreateCompatibleDC(pDC);
		dcBuffer.CreateCompatibleDC(&dc);
		int bmw, bmh ;
		BITMAP bmap;
		CBitmap m_bitmap;
		m_bitmap.LoadBitmap(IDB_BACKGROUND);
		CBitmap* pOldBitmap = dcBuffer.SelectObject(&m_bitmap);
		m_bitmap.GetBitmap(&bmap);
		bmw = bmap.bmHeight;
		bmh = bmap.bmHeight;
		int xo=0, yo=0;
		
		for (yo = 0; yo < rect.Height(); yo += bmh)
		{
			for (xo = 0; xo < rect.Width(); xo += bmw)
			{
				pDC->BitBlt (xo, yo, rect.Width(), rect.Height(), &dcBuffer, 0, 0, SRCCOPY);
			}
		}
		BitBlt(dc, 0, 0, rect.Width(), rect.Height(), dcBuffer, 0, 0, SRCCOPY);
	}
	return true;
}

void CToolMapShow::refreshVisibleMap()
{
	
	CMemReaderProxy reader;	
	int x;
	int y;
		
	CTibiaCharacter *self = reader.readSelfCharacter();
	
	if (m_mapButtonImage[10][10]!=IDB_MAP_SELF)
	{
		m_mapButtons[10][10]->LoadBitmaps(IDB_MAP_SELF,IDB_MAP_SELF,IDB_MAP_SELF,IDB_MAP_SELF);
		m_mapButtons[10][10]->RedrawWindow();
		m_mapButtonImage[10][10]=IDB_MAP_SELF;
		m_mapButtons[10][10]->m_value=-2;
	}
	
	for (x=0;x<=20;x++)
	{
		for (y=0;y<=20;y++)
		{
			if (x!=10||y!=10)
			{
				int avail=tibiaMap.isPointAvailableNoProh(x+self->x-10,y+self->y-10,self->z);				
				if (avail)
				{
					int updownSel=tibiaMap.getPointUpDown(x+self->x-10,y+self->y-10,self->z);					
					switch (updownSel)
					{
					case 0:						
						if (m_mapButtonImage[x][y]!=IDB_MAP_SAMEFLOOR)
						{
							m_mapButtons[x][y]->LoadBitmaps(IDB_MAP_SAMEFLOOR,IDB_MAP_SAMEFLOOR,IDB_MAP_SAMEFLOOR,IDB_MAP_SAMEFLOOR);
							m_mapButtons[x][y]->RedrawWindow();
							m_mapButtonImage[x][y]=IDB_MAP_SAMEFLOOR;
							m_mapButtons[x][y]->m_value=0;
						}
						break;
					case 101:
						// open hole						
						if (m_mapButtonImage[x][y]!=IDB_MAP_OPENHOLE)
						{
							m_mapButtons[x][y]->LoadBitmaps(IDB_MAP_OPENHOLE,IDB_MAP_OPENHOLE,IDB_MAP_OPENHOLE,IDB_MAP_OPENHOLE);
							m_mapButtons[x][y]->RedrawWindow();
							m_mapButtonImage[x][y]=IDB_MAP_OPENHOLE;
							m_mapButtons[x][y]->m_value=101;
						}												
						break;
					case 102:
						// closed hole
						if (m_mapButtonImage[x][y]!=IDB_MAP_CLOSEDHOLE)
						{
							m_mapButtons[x][y]->LoadBitmaps(IDB_MAP_CLOSEDHOLE,IDB_MAP_CLOSEDHOLE,IDB_MAP_CLOSEDHOLE,IDB_MAP_CLOSEDHOLE);
							m_mapButtons[x][y]->RedrawWindow();
							m_mapButtonImage[x][y]=IDB_MAP_CLOSEDHOLE;
							m_mapButtons[x][y]->m_value=102;
						}												
						break;
					case 103:
						// crate
						if (m_mapButtonImage[x][y]!=IDB_MAP_CRATE)
						{
							m_mapButtons[x][y]->LoadBitmaps(IDB_MAP_CRATE,IDB_MAP_CRATE,IDB_MAP_CRATE,IDB_MAP_CRATE);
							m_mapButtons[x][y]->RedrawWindow();
							m_mapButtonImage[x][y]=IDB_MAP_CRATE;
							m_mapButtons[x][y]->m_value=103;
						}												
						break;
					case 201:
						// rope
						
						if (m_mapButtonImage[x][y]!=IDB_MAP_ROPE)
						{
							m_mapButtons[x][y]->LoadBitmaps(IDB_MAP_ROPE,IDB_MAP_ROPE,IDB_MAP_ROPE,IDB_MAP_ROPE);
							m_mapButtons[x][y]->RedrawWindow();
							m_mapButtonImage[x][y]=IDB_MAP_ROPE;
							m_mapButtons[x][y]->m_value=201;
						}							
						
						break;
					case 202:
						// magic rope
						
						if (m_mapButtonImage[x][y]!=IDB_MAP_MAGICROPE)
						{
							m_mapButtons[x][y]->LoadBitmaps(IDB_MAP_MAGICROPE,IDB_MAP_MAGICROPE,IDB_MAP_MAGICROPE,IDB_MAP_MAGICROPE);
							m_mapButtons[x][y]->RedrawWindow();
							m_mapButtonImage[x][y]=IDB_MAP_MAGICROPE;
							m_mapButtons[x][y]->m_value=202;
						}							
						
						break;
					case 203:
						// ladder
						
						if (m_mapButtonImage[x][y]!=IDB_MAP_LADDER)
						{
							m_mapButtons[x][y]->LoadBitmaps(IDB_MAP_LADDER,IDB_MAP_LADDER,IDB_MAP_LADDER,IDB_MAP_LADDER);
							m_mapButtons[x][y]->RedrawWindow();
							m_mapButtonImage[x][y]=IDB_MAP_LADDER;
							m_mapButtons[x][y]->m_value=203;
						}							
						
						break;
					case 204:
						// stairs
						
						if (m_mapButtonImage[x][y]!=IDB_MAP_STAIRS)
						{
							m_mapButtons[x][y]->LoadBitmaps(IDB_MAP_STAIRS,IDB_MAP_STAIRS,IDB_MAP_STAIRS,IDB_MAP_STAIRS);
							m_mapButtons[x][y]->RedrawWindow();
							m_mapButtonImage[x][y]=IDB_MAP_STAIRS;
							m_mapButtons[x][y]->m_value=204;
						}							
						
						break;
					case 301:
						// depot
						
						if (m_mapButtonImage[x][y]!=IDB_MAP_DEPOT)
						{
							m_mapButtons[x][y]->LoadBitmaps(IDB_MAP_DEPOT,IDB_MAP_DEPOT,IDB_MAP_DEPOT,IDB_MAP_DEPOT);
							m_mapButtons[x][y]->RedrawWindow();
							m_mapButtonImage[x][y]=IDB_MAP_DEPOT;
							m_mapButtons[x][y]->m_value=301;
						}							
						
						break;
					}
				} else {
					if (m_mapButtonImage[x][y]!=IDB_MAP_EMPTY)
					{
						m_mapButtons[x][y]->LoadBitmaps(IDB_MAP_EMPTY,IDB_MAP_EMPTY,IDB_MAP_EMPTY,IDB_MAP_EMPTY);					
						m_mapButtons[x][y]->RedrawWindow();
						m_mapButtonImage[x][y]=IDB_MAP_EMPTY;
						m_mapButtons[x][y]->m_value=-1;
					}
				}
			}
		}
	}
	delete self;
}

void CToolMapShow::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent==1001)
	{
		KillTimer(1001);
		refreshVisibleMap();		
		SetTimer(1001,1000,NULL);
	}
	if (nIDEvent==1002)
	{
		
		KillTimer(1002);
		CMemReaderProxy reader;
		CTibiaCharacter *self = reader.readSelfCharacter();

		tibiaMap.setPointAsAvailable(self->x,self->y,self->z);
	

		delete self;
		SetTimer(1002,25,NULL);
	}
	if (nIDEvent==1003)
	{
		KillTimer(1003);
		CMemReaderProxy reader;
		// get tile 0 to make sure that the framework is initialised
		reader.getTibiaTile(0);
		CTibiaCharacter *self = reader.readSelfCharacter();

		iter++;//increases every 0.5 secs
		if (self->x!=prevX||self->y!=prevY||self->z!=prevZ||iter%3==0)
		{			
			int x,y;
			int tileArrAvail[18][14];
			int tileArrUpDown[18][14];

			for (x=-8;x<=9;x++)
			{				
				for (y=-6;y<=7;y++)
				{					
					int i;
					int count=reader.mapGetPointItemsCount(point(x,y,0));

					int blocked=0;
					int updown=0;
					int ground=0;
					for (i=0;i<count;i++)
					{						
						int tileId = reader.mapGetPointItemId(point(x,y,0),i);
						if (tileId!=99)
						{
							CTibiaTile *tileData = reader.getTibiaTile(tileId);
							if (tileData->blocking)
							{
								blocked=1;
							}
							if (tileData->ground)
							{
								ground=1;
							}
							if (tileData->goDown)
							{
								if (tileData->requireShovel)
								{
									updown=102;
								} else {
									if (tileData->requireUse)
									{
										updown=103;
									} else {
										updown=101;
									}
								}
							}
							if (tileData->goUp)
							{
								if (tileData->requireRope)
								{
									updown=201;
								} else {
									if (tileData->requireUse)
									{
										updown=203;
									} else {
										updown=204;
									};
								}
							}
							if (tileData->isDepot)
							{
								updown=301;
							}
						} else if (x!=0 || y!=0) {
							tibiaMap.prohPointAdd(self->x+x,self->y+y,self->z);
						}
					} // for i

					// if tile is depot chest than treat it in a special way
					if (updown==301) blocked=0;
					// if there is not a single walkable tile than one cannot pass
					if (ground==0&&!updown) 
					{
						blocked=1;
					}

					// if count==0 then override "blocked" and "no updown"
					if (count==0)
					{
						blocked=1;
						updown=0;
					}

					if (!blocked)
					{				
						tileArrAvail[x+8][y+6]=1;
						tileArrUpDown[x+8][y+6]=updown;
					} else {
						tileArrAvail[x+8][y+6]=0;
						tileArrUpDown[x+8][y+6]=0;
					}
				} // for y
			} // for x
			CTibiaCharacter *newSelf = reader.readSelfCharacter();
			for (x=-8;x<=9;x++)
			{				
				for (y=-6;y<=7;y++)
				{	
					if (tileArrAvail[x+8][y+6])
					{
						tibiaMap.setPointAsAvailable(self->x+x,self->y+y,self->z);
						tibiaMap.setPointUpDown(self->x+x,self->y+y,self->z,tileArrUpDown[x+8][y+6]);
					} else {
						tibiaMap.setPointUpDown(self->x+x,self->y+y,self->z,0);
						tibiaMap.removePointAvailable(self->x+x,self->y+y,self->z);
					}
				}
			}
		}
		
		delete self;
		SetTimer(1003,1000,NULL);
	}
	if (nIDEvent==1004)
	{
		int fail = 0;
		KillTimer(1004);
		if (!flicker) {
			flicker = (rand() % 7) + 1;
			if (flicker == 6)
				fail = 1;
			back = flicker;
			SetTimer(1004,(rand() % 100000) + 1,NULL);
			alt = 0;
		}
		else {
			SetTimer(1004,(rand() % 200) + 1,NULL);
			if (flicker < back) {
				back--;
				alt = 0;
			}
			else {
				flicker--;
				alt = 1;
			}
		}
		if (fail == 1) {
			KillTimer(1004);
			alt = 1;
		}
		RedrawWindow();
};
	
	CDialog::OnTimer(nIDEvent);
}

void CToolMapShow::OnToolMapshowClear() 
{
	tibiaMap.clear();	
}

void CToolMapShow::OnToolMapshowResearch() 
{
	if (m_research.GetCheck())
	{
		// enabling
		SetTimer(1002,25,NULL);
	} else {
		// disabling
		KillTimer(1002);
	}
}

void CToolMapShow::OnToolMapshowExtendedResearch()
{
	if (m_extendedResearch.GetCheck())
	{		
		SetTimer(1003,1003,NULL);
	} else {
		KillTimer(1003);
	}
}

void CToolMapShow::mapPointClicked(int posX, int posY, int pos)
{
	CMemReaderProxy reader;
	
	CTibiaCharacter *self = reader.readSelfCharacter();

	int realX=self->x+posX-10;
	int realY=self->y+posY-10;
	int realZ=self->z;

	delete self;	

	if (pos>=0)
	{
		// point added/updated		
		tibiaMap.setPointAsAvailable(realX,realY,realZ);				
		tibiaMap.setPointUpDown(realX,realY,realZ,pos);
	} else {
		// point removed
		tibiaMap.removePointAvailable(realX,realY,realZ);
	}
	
	refreshVisibleMap();
}

BOOL CToolMapShow::OnCommand(WPARAM wParam, LPARAM lParam) 
{

	
	return CDialog::OnCommand(wParam, lParam);
}

void CToolMapShow::showTileDetails(int x, int y)
{		
	CMemReaderProxy reader;
	int outOfRange=0;
	char buf[2560];	

	// make (x,y) relative to the center
	x-=10;
	y-=10;

	if (x<-8||x>9||y<-6||y>7) outOfRange=1;
	
	if (!outOfRange)	
	{		
		char subbuf[256];
		int count=reader.mapGetPointItemsCount(point(x,y,0));
		int pos=0;
		if (count>10) count=10;
		
		sprintf(buf,"Tile info: [x=%d y=%d]",x,y);
		for (pos=0;pos<count;pos++)
		{			
			sprintf(subbuf," id=%d [%d,%d]",reader.mapGetPointItemId(point(x,y,0),pos),reader.mapGetPointItemExtraInfo(point(x,y,0),pos,1),reader.mapGetPointItemExtraInfo(point(x,y,0),pos,2));
			strcat(buf,subbuf);
		}		
		m_tileInfo.SetWindowText(buf);
	} else {
		sprintf(buf,"Tile info: n/a");
		m_tileInfo.SetWindowText(buf);
	}
	
}

void CToolMapShow::setTileDetails(int x, int y, int setOption) {		
	try {	
	}
	catch (...) {
		AfxMessageBox("Failed During setTileDetails");
	}
}

void CToolMapShow::ShowMapConfig(int x, int y) {
	CMapConfig *dialog = new CMapConfig(memTilesForConfig, x, y);
	dialog->DoModal();
	delete dialog;
}

