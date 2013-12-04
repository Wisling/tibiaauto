// ToolMapShow.cpp : implementation file
//

#include "stdafx.h"
#include "ToolMapShow.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
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

/////////////////////////////////////////////////////////////////////////////
// CONSTANTS
int MINX=-8,MAXX=9,MINY=-6,MAXY=7;

/////////////////////////////////////////////////////////////////////////////
// CToolMapShow dialog


CToolMapShow::CToolMapShow(CWnd* pParent /*=NULL*/)
	: MyDialog(CToolMapShow::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolMapShow)
	//}}AFX_DATA_INIT
}
CToolMapShow::~CToolMapShow() {
	for (int x=0;x<21;x++) {
		for (int y =0 ;y<21;y++) {
			delete m_mapButtons[x][y];
		}
	}
}

void CToolMapShow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolMapShow)
	DDX_Control(pDX, IDC_TOOL_MAPSHOW_CLEAR, m_ClearMap);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_FRAME_MAPPING_OPTIONS, m_MappingOptionFrame);
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
	ON_WM_CTLCOLOR()
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

void CToolMapShow::DoSetButtonSkin(){
	skin.SetButtonSkin(	m_ClearMap);
	skin.SetButtonSkin(	m_OK);
}

BOOL CToolMapShow::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

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

	OnToolMapshowExtendedResearch();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CToolMapShow::OnEraseBkgnd(CDC* pDC)
{
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
	dcBuffer.SelectObject(pOldBitmap);
	
	return true;
}

void CToolMapShow::refreshVisibleMap()
{
	CMemReaderProxy reader;
	int x;
	int y;

	if (!IsWindowVisible()) return;
		
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
					case 302:
						// teleporter
						int tilePic;
						if(tibiaMap.getDestPoint(x+self->x-10,y+self->y-10,self->z).x==0) tilePic=IDB_MAP_UNKTELE;
						else tilePic=IDB_MAP_TELE;

						if (m_mapButtonImage[x][y]!=tilePic)
						{
							m_mapButtons[x][y]->LoadBitmaps(tilePic,tilePic,tilePic,tilePic);
							m_mapButtons[x][y]->RedrawWindow();
							m_mapButtonImage[x][y]=tilePic;
							m_mapButtons[x][y]->m_value=302;
						}
						
						break;
					case 303:
						// permanent block
						if (m_mapButtonImage[x][y]!=IDB_MAP_BLOCK)
						{
							m_mapButtons[x][y]->LoadBitmaps(IDB_MAP_BLOCK,IDB_MAP_BLOCK,IDB_MAP_BLOCK,IDB_MAP_BLOCK);
							m_mapButtons[x][y]->RedrawWindow();
							m_mapButtonImage[x][y]=IDB_MAP_BLOCK;
							m_mapButtons[x][y]->m_value=303;
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

int mod(int i,int m){
	int ans=i%m;
	if (ans<0) ans+=m;
	return ans;
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

		if (tibiaMap.getPointUpDownNoProh(self->x,self->y,self->z)==0)
		{
			tibiaMap.setPointAsAvailable(self->x,self->y,self->z);
			tibiaMap.setPointSpeed(self->x,self->y,self->z,130);//130 default( is >255/2 and <70*2)
		}
	

		delete self;
		SetTimer(1002,25,NULL);
	}
	if (nIDEvent==1003)
	{
		static int prevX=0,prevY=0,prevZ=0,iter=0;
		KillTimer(1003);
		CMemReaderProxy reader;
		// get tile 0 to make sure that the framework is initialised
		reader.getTibiaTile(0);
		CTibiaCharacter *self = reader.readSelfCharacter();

		iter++;//increases every 0.5 secs
		if (self->x!=prevX||self->y!=prevY||self->z!=prevZ||iter%6==0)//reading takes about 100ms +- 100
		{

			int x,y;
			int tileArrAvail[18][14];
			int tileArrUpDown[18][14];
			int tileArrSpd[18][14];
			int relToCell=reader.mapGetSelfCellNr();// the present location of self in map memory range 0-2016
//			char buf[111];
//			sprintf(buf,"%d-%d %d",relToCell,relToCell%18,relToCell%(14*18)/18);
//			AfxMessageBox(buf);

			for (x=-8;x<=9;x++)
			{
				for (y=-6;y<=7;y++)
				{
					int i;
					int count=reader.mapGetPointItemsCount(point(x,y,0),relToCell);

					int blocked=0;
					int updown=0;
					int ground=0;
					int speed=0;
					for (i=0;i<count;i++)
					{
						int tileId = reader.mapGetPointItemId(point(x,y,0),i,relToCell);
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
							if (tileData->speed)
							{
								speed=tileData->speed;
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
							if (tileData->isTeleporter)
							{
								updown=302;
							}
						} else if (x!=0 || y!=0) {
							tibiaMap.prohPointAdd(self->x+x,self->y+y,self->z);
						}
					} // for i

					int prevUpDown=tibiaMap.getPointUpDown(self->x+x,self->y+y,self->z);

					//blocking tiles and teleporters are marked permanently until changed manually
					if (prevUpDown==302 || prevUpDown==301 || prevUpDown==303) updown=prevUpDown;

					// if tile is depot chest or teleporter then treat it in a special way
					if (updown==301 || updown==302) blocked=0;
					// if there is not a single walkable tile then one cannot pass
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

					//303 is handled as blocking in the pathfind algorithm
					if (!blocked)
					{
						tileArrSpd[x+8][y+6]=speed;
						tileArrAvail[x+8][y+6]=1;
						tileArrUpDown[x+8][y+6]=updown;
					} else {
						tileArrSpd[x+8][y+6]=0;
						tileArrAvail[x+8][y+6]=0;
						tileArrUpDown[x+8][y+6]=0;
					}
				} // for y
			} // for x
			CTibiaCharacter *newSelf = reader.readSelfCharacter();

			int cellNr=reader.mapGetSelfCellNr();
			//if change in cellNr does not match with change in (x,y) then teleported  ( mod func returns >= 0 )
			int wasTeleported=(cellNr%18!=mod((relToCell%18+(newSelf->x-self->x)),18))||(cellNr%(14*18)/18!=mod((relToCell%(14*18)/18+(newSelf->y-self->y)),14));

			//since we used relToCell only changing floors and teleports while reading drastically matter
			if (newSelf->z==self->z && !wasTeleported)
			{
				prevX=self->x;
				prevY=self->y;
				prevZ=self->z;
				// tiles changed between read from tibia and write to map are excluded
				for (x=-8+max(0,newSelf->x-self->x);x<=9+min(0,newSelf->x-self->x);x++)
				{
					for (y=-6+max(0,newSelf->y-self->y);y<=7+min(0,newSelf->y-self->y);y++)
					{

						if (tileArrAvail[x+8][y+6])
						{
							tibiaMap.setPointAsAvailable(self->x+x,self->y+y,self->z);
							tibiaMap.setPointUpDown(self->x+x,self->y+y,self->z,tileArrUpDown[x+8][y+6]);
							if (tileArrSpd[x+8][y+6]==0) tibiaMap.setPointSpeed(self->x+x,self->y+y,self->z,130);//130 default( is >255/2 and <70*2)
							else tibiaMap.setPointSpeed(self->x+x,self->y+y,self->z,tileArrSpd[x+8][y+6]);
						} else {
							tibiaMap.setPointUpDown(self->x+x,self->y+y,self->z,0);
							tibiaMap.setPointSpeed(self->x+x,self->y+y,self->z,0);
							tibiaMap.removePointAvailable(self->x+x,self->y+y,self->z);
						}
					}
				}
			}
			delete newSelf;
		}

		
		delete self;
		SetTimer(1003,500,NULL);
	}
	if (nIDEvent==1004)
	{
		static int prevXTele=0,prevYTele=0,prevZTele=0;
		KillTimer(1004);
		CMemReaderProxy reader;
		// get tile 0 to make sure that the framework is initialised
		reader.getTibiaTile(0);
		CTibiaCharacter *self = reader.readSelfCharacter();

		int sensitivity=2;//sqm to check for teleporter and #sqm travellable between checks
		if (self->x!=prevXTele||self->y!=prevYTele||self->z!=prevZTele){
			if (abs(self->x-prevXTele)>sensitivity||abs(self->y-prevYTele)>sensitivity||abs(self->z-prevZTele)>0){
				int x=0,y=0;
				int xSwitch=0;
				int ySwitch=0;
				while (x!=sensitivity+1 && y !=sensitivity+1){
					//manage x and y coords for spiraling
					if (xSwitch==0 && ySwitch==0){xSwitch=1;}
					else if (x==y && x>=0 && xSwitch==1 && ySwitch==0) {x++; xSwitch=0;ySwitch=-1;}
					else if (!xSwitch && !(x==y && x>=0) && abs(x)==abs(y)){xSwitch=ySwitch;ySwitch=0;x+=xSwitch;y+=ySwitch;}
					else if (!ySwitch && abs(x)==abs(y)){ySwitch=-xSwitch;xSwitch=0;x+=xSwitch;y+=ySwitch;}
					else {x+=xSwitch;y+=ySwitch;}

					int upDown=tibiaMap.getPointUpDown(prevXTele+x,prevYTele+y,prevZTele);
					if(upDown==302){//teleporter
						CPackSenderProxy sender;
						if (tibiaMap.getDestPoint(prevXTele+x,prevYTele+y,prevZTele).x==0){
							tibiaMap.setDestPoint(prevXTele+x,prevYTele+y,prevZTele,self->x,self->y,self->z);
							char buf[128];
							sprintf(buf,"Assigned Teleporter Dest(%d,%d,%d)->(%d,%d,%d)",prevXTele+x,prevYTele+y,prevZTele,self->x,self->y,self->z);
							sender.sendTAMessage(buf);
							break;
						}
					}else if(upDown>0) break;//other updown is closer and probably used
				}
			}
			prevXTele=self->x;
			prevYTele=self->y;
			prevZTele=self->z;
		}
		delete self;
		SetTimer(1004,200,NULL);
	}
	
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

void CToolMapShow::RefreshExtendedResearchMap(){
	if (m_extendedResearch.GetCheck())
	{
		SetTimer(1003,1000,NULL);
		SetTimer(1004,300,NULL);
	} else {
		KillTimer(1003);
		KillTimer(1004);
	}
}
void CToolMapShow::OnToolMapshowExtendedResearch()
{
	RefreshExtendedResearchMap();
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
		tibiaMap.setPointSpeed(realX,realY,realZ,130);//130 default( is >255/2 and <70*2)
	} else {
		// point removed
		tibiaMap.removePointAvailable(realX,realY,realZ);
		tibiaMap.setPointSpeed(realX,realY,realZ,0);
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
	CTibiaMapProxy tibiaMap;
	int outOfRange=0;
	char buf[2560];

	CTibiaCharacter* self=reader.readSelfCharacter();
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
		
		sprintf(buf,"Tile info: %d [x=%d y=%d]",tibiaMap.getPointSpeed(self->x+x,self->y+y,self->z),x,y);
		for (pos=0;pos<count;pos++)
		{
			sprintf(subbuf," id=%d[%d:%d,%d]",reader.mapGetPointStackIndex(point(x,y,0),pos),reader.mapGetPointItemId(point(x,y,0),pos),reader.mapGetPointItemExtraInfo(point(x,y,0),pos,1),reader.mapGetPointItemExtraInfo(point(x,y,0),pos,2));
			strcat(buf,subbuf);
		}
		m_tileInfo.SetWindowText(buf);
	} else {
		sprintf(buf,"Tile info: n/a");
		m_tileInfo.SetWindowText(buf);
	}
	delete self;
	
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

