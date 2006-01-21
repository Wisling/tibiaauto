#include "stdafx.h"

#ifdef INCLUDE_PROXY_CLASSES
class NewIDirectDraw4 : public IDirectDraw4
{
public:
	NewIDirectDraw4(IDirectDraw4 *real,char *subtype)
	{
		realIDirectDraw4 = real;
		realIDirectDraw4->AddRef();		

		sprintf(this->subtype,"%s",subtype);
	}
	~NewIDirectDraw4()
	{
		 realIDirectDraw4->Release();		 
	}

	 /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR * ppvObj) 
	{						
		char buf[1024];		
		sprintf(buf,"[%s] QueryInterface(riid=%x;%x;%x;%x;%x,%x,%x,%x,%x,%x,%x, ppvObj=%x)",subtype,riid.Data1,riid.Data2,riid.Data3,riid.Data4[0],riid.Data4[1],riid.Data4[2],riid.Data4[3],riid.Data4[4],riid.Data4[5],riid.Data4[6],riid.Data4[7],ppvObj);
		MessageBox(NULL,buf,"",0);		
		

		return realIDirectDraw4->QueryInterface(riid,ppvObj);
	}
    STDMETHOD_(ULONG,AddRef) (THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] AddRef",subtype);
		MessageBox(NULL,buf,"",0);		
		return realIDirectDraw4->AddRef();
	}
    STDMETHOD_(ULONG,Release) (THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Release",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->Release();
	}
    /*** IDirectDraw methods ***/
    STDMETHOD(Compact)(THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Compact",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->Compact();
	}
    STDMETHOD(CreateClipper)(THIS_ DWORD p1, LPDIRECTDRAWCLIPPER FAR* p2, IUnknown FAR * p3) 
	{
		char buf[1024];
		sprintf(buf,"[%s] CreateClipper",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->CreateClipper(p1,p2,p3);
	}
    STDMETHOD(CreatePalette)(THIS_ DWORD p1, LPPALETTEENTRY p2, LPDIRECTDRAWPALETTE FAR* p3, IUnknown FAR * p4) 
	{
		char buf[1024];
		sprintf(buf,"[%s] CreatePalette",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->CreatePalette(p1,p2,p3,p4);
	}
    STDMETHOD(CreateSurface)(THIS_  LPDDSURFACEDESC2 p1, LPDIRECTDRAWSURFACE4 FAR *p2, IUnknown FAR *p3) 
	{
		char buf[1024];
		sprintf(buf,"[%s] CreateSurface; p1=%x p2=%x",subtype,p1,p2);
		MessageBox(NULL,buf,"",0);

		IDirectDrawSurface4 *realIDirectDrawSurface4;
		realIDirectDraw4->CreateSurface(p1,&realIDirectDrawSurface4,p3);
		*p2 = new NewIDirectDrawSurface4(realIDirectDrawSurface4,"IDirectDrafSurface4");
		return S_OK;
		
		return realIDirectDraw4->CreateSurface(p1,p2,p3);
	}
    STDMETHOD(DuplicateSurface)( THIS_ LPDIRECTDRAWSURFACE4 p1, LPDIRECTDRAWSURFACE4 FAR * p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] DuplicateSurface",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->DuplicateSurface(p1,p2);
	}
    STDMETHOD(EnumDisplayModes)( THIS_ DWORD p1, LPDDSURFACEDESC2 p2, LPVOID p3, LPDDENUMMODESCALLBACK2 p4) 
	{
		char buf[1024];
		sprintf(buf,"[%s] EnumDisplayModes",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->EnumDisplayModes(p1,p2,p3,p4);
	}
    STDMETHOD(EnumSurfaces)(THIS_ DWORD p1, LPDDSURFACEDESC2 p2, LPVOID p3,LPDDENUMSURFACESCALLBACK2 p4) 
	{
		char buf[1024];
		sprintf(buf,"[%s] EnumSurfaces",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->EnumSurfaces(p1,p2,p3,p4);
	}
    STDMETHOD(FlipToGDISurface)(THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] FlipToGDISurface",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->FlipToGDISurface();
	}
    STDMETHOD(GetCaps)( THIS_ LPDDCAPS p1, LPDDCAPS p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetCaps",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->GetCaps(p1,p2);
	}
    STDMETHOD(GetDisplayMode)( THIS_ LPDDSURFACEDESC2 p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetDisplayMode",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->GetDisplayMode(p1);
	}
    STDMETHOD(GetFourCCCodes)(THIS_  LPDWORD p1, LPDWORD p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetFourCCCodes",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->GetFourCCCodes(p1,p2);
	}
    STDMETHOD(GetGDISurface)(THIS_ LPDIRECTDRAWSURFACE4 FAR *p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetGDISurface",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->GetGDISurface(p1);
	}
    STDMETHOD(GetMonitorFrequency)(THIS_ LPDWORD p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetMonitorFrequency",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->GetMonitorFrequency(p1);
	}
    STDMETHOD(GetScanLine)(THIS_ LPDWORD p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetScanLine",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->GetScanLine(p1);
	}
    STDMETHOD(GetVerticalBlankStatus)(THIS_ LPBOOL p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetVerticalBlankStatus",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->GetVerticalBlankStatus(p1);
	}
    STDMETHOD(Initialize)(THIS_ GUID FAR *p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Initialize",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->Initialize(p1);
	}
    STDMETHOD(RestoreDisplayMode)(THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] RestoreDisplayMode",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->RestoreDisplayMode();
	}
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND p1, DWORD p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] SetCooperativeLevel",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->SetCooperativeLevel(p1,p2);
	}
    STDMETHOD(SetDisplayMode)(THIS_ DWORD p1, DWORD p2,DWORD p3, DWORD p4, DWORD p5) 
	{
		char buf[1024];
		sprintf(buf,"[%s] SetDisplayMode",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->SetDisplayMode(p1,p2,p3,p4,p5);
	}
    STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD p1, HANDLE p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] WaitForVerticalBlank",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->WaitForVerticalBlank(p1,p2);
	}

	/*** Added in the v2 interface ***/
    STDMETHOD(GetAvailableVidMem)(THIS_ LPDDSCAPS2 p1, LPDWORD p2, LPDWORD p3) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetAvailableVidMem",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->GetAvailableVidMem(p1,p2,p3);
	}
    /*** Added in the V4 Interface ***/
    STDMETHOD(GetSurfaceFromDC) (THIS_ HDC p1, LPDIRECTDRAWSURFACE4 *p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetSurfaceFromDC",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->GetSurfaceFromDC(p1,p2);
	}
    STDMETHOD(RestoreAllSurfaces)(THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] RestoreAllSurfaces",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->RestoreAllSurfaces();
	}
    STDMETHOD(TestCooperativeLevel)(THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] TestCooperativeLevel",subtype);
		//MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->TestCooperativeLevel();
	}
    STDMETHOD(GetDeviceIdentifier)(THIS_ LPDDDEVICEIDENTIFIER p1, DWORD p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetDeviceIdentifier",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw4->GetDeviceIdentifier(p1,p2);
	}



private:
	IDirectDraw4 *realIDirectDraw4;	
	char subtype[128];

};
#endif