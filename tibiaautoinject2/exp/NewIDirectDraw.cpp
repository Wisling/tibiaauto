#include "stdafx.h"

#ifdef INCLUDE_PROXY_CLASSES

//GUID IID_IDirectDraw4_ = (0x9c59509a,0x39bd,0x11d1,0x8c,0x4a,0x00,0xc0,0x4f,0xd9,0x30,0xc5 );

class NewIDirectDraw : public IDirectDraw
{
public:
	NewIDirectDraw(IDirectDraw *real,char *subtype)
	{
		realIDirectDraw = real;
		realIDirectDraw->AddRef();		

		sprintf(this->subtype,"%s",subtype);
	}
	~NewIDirectDraw()
	{
		 realIDirectDraw->Release();		 
	}

	 /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR * ppvObj) 
	{						
		char buf[1024];		
		sprintf(buf,"[%s] QueryInterface(riid=%x;%x;%x;%x;%x,%x,%x,%x,%x,%x,%x, ppvObj=%x)",subtype,riid.Data1,riid.Data2,riid.Data3,riid.Data4[0],riid.Data4[1],riid.Data4[2],riid.Data4[3],riid.Data4[4],riid.Data4[5],riid.Data4[6],riid.Data4[7],ppvObj);
		MessageBox(NULL,buf,"",0);
		IDirectDraw4 *dDraw4= (IDirectDraw4 *)*ppvObj;		
		//if (IsEqualIID(IID_IDirectDraw4,riid))
		//{
			MessageBox(NULL,"QueryInterface: IDirectDraw4","",0);
			
			IDirectDraw4 *realIDirectDraw4;
			realIDirectDraw->QueryInterface(riid,(void **)&realIDirectDraw4);
			*ppvObj = new NewIDirectDraw4(realIDirectDraw4,"IDirectDraw4");
			return S_OK;
		//} else {		
		//	return realIDirectDraw->QueryInterface(riid,ppvObj);
		//}
	}
    STDMETHOD_(ULONG,AddRef) (THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] AddRef",subtype);
		MessageBox(NULL,buf,"",0);		
		return realIDirectDraw->AddRef();
	}
    STDMETHOD_(ULONG,Release) (THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Release",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->Release();
	}
    /*** IDirectDraw methods ***/
    STDMETHOD(Compact)(THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Compact",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->Compact();
	}
    STDMETHOD(CreateClipper)(THIS_ DWORD p1, LPDIRECTDRAWCLIPPER FAR* p2, IUnknown FAR * p3) 
	{
		char buf[1024];
		sprintf(buf,"[%s] CreateClipper",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->CreateClipper(p1,p2,p3);
	}
    STDMETHOD(CreatePalette)(THIS_ DWORD p1, LPPALETTEENTRY p2, LPDIRECTDRAWPALETTE FAR* p3, IUnknown FAR * p4) 
	{
		char buf[1024];
		sprintf(buf,"[%s] CreatePalette",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->CreatePalette(p1,p2,p3,p4);
	}
    STDMETHOD(CreateSurface)(THIS_  LPDDSURFACEDESC p1, LPDIRECTDRAWSURFACE FAR *p2, IUnknown FAR *p3) 
	{
		char buf[1024];
		sprintf(buf,"[%s] CreateSurface",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->CreateSurface(p1,p2,p3);
	}
    STDMETHOD(DuplicateSurface)( THIS_ LPDIRECTDRAWSURFACE p1, LPDIRECTDRAWSURFACE FAR * p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] DuplicateSurface",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->DuplicateSurface(p1,p2);
	}
    STDMETHOD(EnumDisplayModes)( THIS_ DWORD p1, LPDDSURFACEDESC p2, LPVOID p3, LPDDENUMMODESCALLBACK p4) 
	{
		char buf[1024];
		sprintf(buf,"[%s] EnumDisplayModes",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->EnumDisplayModes(p1,p2,p3,p4);
	}
    STDMETHOD(EnumSurfaces)(THIS_ DWORD p1, LPDDSURFACEDESC p2, LPVOID p3,LPDDENUMSURFACESCALLBACK p4) 
	{
		char buf[1024];
		sprintf(buf,"[%s] EnumSurfaces",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->EnumSurfaces(p1,p2,p3,p4);
	}
    STDMETHOD(FlipToGDISurface)(THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] FlipToGDISurface",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->FlipToGDISurface();
	}
    STDMETHOD(GetCaps)( THIS_ LPDDCAPS p1, LPDDCAPS p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetCaps",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->GetCaps(p1,p2);
	}
    STDMETHOD(GetDisplayMode)( THIS_ LPDDSURFACEDESC p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetDisplayMode",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->GetDisplayMode(p1);
	}
    STDMETHOD(GetFourCCCodes)(THIS_  LPDWORD p1, LPDWORD p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetFourCCCodes",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->GetFourCCCodes(p1,p2);
	}
    STDMETHOD(GetGDISurface)(THIS_ LPDIRECTDRAWSURFACE FAR *p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetGDISurface",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->GetGDISurface(p1);
	}
    STDMETHOD(GetMonitorFrequency)(THIS_ LPDWORD p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetMonitorFrequency",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->GetMonitorFrequency(p1);
	}
    STDMETHOD(GetScanLine)(THIS_ LPDWORD p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetScanLine",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->GetScanLine(p1);
	}
    STDMETHOD(GetVerticalBlankStatus)(THIS_ LPBOOL p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetVerticalBlankStatus",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->GetVerticalBlankStatus(p1);
	}
    STDMETHOD(Initialize)(THIS_ GUID FAR *p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Initialize",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->Initialize(p1);
	}
    STDMETHOD(RestoreDisplayMode)(THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] RestoreDisplayMode",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->RestoreDisplayMode();
	}
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND p1, DWORD p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] SetCooperativeLevel",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->SetCooperativeLevel(p1,p2);
	}
    STDMETHOD(SetDisplayMode)(THIS_ DWORD p1, DWORD p2,DWORD p3) 
	{
		char buf[1024];
		sprintf(buf,"[%s] SetDisplayMode",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->SetDisplayMode(p1,p2,p3);
	}
    STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD p1, HANDLE p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] WaitForVerticalBlank",subtype);
		MessageBox(NULL,buf,"",0);
		return realIDirectDraw->WaitForVerticalBlank(p1,p2);
	}


private:
	IDirectDraw *realIDirectDraw;	
	char subtype[128];

};
#endif