#include "stdafx.h"


#ifdef INCLUDE_PROXY_CLASSES

static int c=0;
class NewIDirectDrawSurface4 : public IDirectDrawSurface4
{
private:
	FILE *debugFile;
public:
	NewIDirectDrawSurface4(IDirectDrawSurface4 *real,char *subtype)
	{
		realIDirectDrawSurface4 = real;
		realIDirectDrawSurface4->AddRef();		

		sprintf(this->subtype,"%s-%d",subtype,c++);
		char b[128];
		sprintf(b,"f:\\temp\\dx\\debugFile-dx-%s",this->subtype);
		debugFile=fopen(b,"w");
		if (!debugFile)
			MessageBox(NULL,"Cannot open debugFile!","",0);
	}
	~NewIDirectDrawSurface4()
	{
		 realIDirectDrawSurface4->Release();		 
		 fclose(debugFile);
	}

	 /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR * ppvObj) 
	{						
		char buf[1024];		
		sprintf(buf,"[%s] QueryInterface(riid=%x;%x;%x;%x;%x,%x,%x,%x,%x,%x,%x, ppvObj=%x)",subtype,riid.Data1,riid.Data2,riid.Data3,riid.Data4[0],riid.Data4[1],riid.Data4[2],riid.Data4[3],riid.Data4[4],riid.Data4[5],riid.Data4[6],riid.Data4[7],ppvObj);
		fprintf(debugFile,"%s\n",buf);		
		

		return realIDirectDrawSurface4->QueryInterface(riid,ppvObj);
	}
    STDMETHOD_(ULONG,AddRef) (THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] AddRef",subtype);
		fprintf(debugFile,"%s\n",buf);		
		return realIDirectDrawSurface4->AddRef();
	}
    STDMETHOD_(ULONG,Release) (THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Release",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->Release();
	}

	  /*** IDirectDrawSurface methods ***/
    STDMETHOD(AddAttachedSurface)(THIS_ LPDIRECTDRAWSURFACE4 p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] AddAttachedSurface",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->AddAttachedSurface(p1);
	}
    STDMETHOD(AddOverlayDirtyRect)(THIS_ LPRECT p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] AddOverlayDirtyRect",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->AddOverlayDirtyRect(p1);
	}
    STDMETHOD(Blt)(THIS_ LPRECT p1,LPDIRECTDRAWSURFACE4 p2, LPRECT p3,DWORD p4, LPDDBLTFX p5) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Blt",subtype);
		//fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->Blt(p1,p2,p3,p4,p5);
	}
    STDMETHOD(BltBatch)(THIS_ LPDDBLTBATCH p1, DWORD p2, DWORD p3) 
	{
		char buf[1024];
		sprintf(buf,"[%s] BltBatch",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->BltBatch(p1,p2,p3);
	}
    STDMETHOD(BltFast)(THIS_ DWORD p1,DWORD p2,LPDIRECTDRAWSURFACE4 p3, LPRECT p4,DWORD p5) 
	{
		char buf[1024];
		sprintf(buf,"[%s] BltFast",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->BltFast(p1,p2,p3,p4,p5);
	}
    STDMETHOD(DeleteAttachedSurface)(THIS_ DWORD p1,LPDIRECTDRAWSURFACE4 p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] DeleteAttachedSurface",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->DeleteAttachedSurface(p1,p2);
	}
    STDMETHOD(EnumAttachedSurfaces)(THIS_ LPVOID p1,LPDDENUMSURFACESCALLBACK2 p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] EnumAttachedSurfaces",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->EnumAttachedSurfaces(p1,p2);
	};
    STDMETHOD(EnumOverlayZOrders)(THIS_ DWORD p1 ,LPVOID p2,LPDDENUMSURFACESCALLBACK2 p3) 
	{
		char buf[1024];
		sprintf(buf,"[%s] EnumOverlayZOrders",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->EnumOverlayZOrders(p1,p2,p3);
	};
    STDMETHOD(Flip)(THIS_ LPDIRECTDRAWSURFACE4 p1, DWORD p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Flip",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->Flip(p1,p2);
	};
    STDMETHOD(GetAttachedSurface)(THIS_ LPDDSCAPS2 p1, LPDIRECTDRAWSURFACE4 FAR *p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetAttachedSurface",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetAttachedSurface(p1,p2);
	};
    STDMETHOD(GetBltStatus)(THIS_ DWORD p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetBltStatus",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetBltStatus(p1);
	};
    STDMETHOD(GetCaps)(THIS_ LPDDSCAPS2 p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetCaps",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetCaps(p1);
	};
    STDMETHOD(GetClipper)(THIS_ LPDIRECTDRAWCLIPPER FAR* p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetClipper",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetClipper(p1);
	};
    STDMETHOD(GetColorKey)(THIS_ DWORD p1, LPDDCOLORKEY p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetColorKey",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetColorKey(p1,p2);
	};
    STDMETHOD(GetDC)(THIS_ HDC FAR * p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetDC",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetDC(p1);
	};
    STDMETHOD(GetFlipStatus)(THIS_ DWORD p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetFlipStatus",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetFlipStatus(p1);
	};
    STDMETHOD(GetOverlayPosition)(THIS_ LPLONG p1, LPLONG p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetOverlayPosition",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetOverlayPosition(p1,p2);
	};
    STDMETHOD(GetPalette)(THIS_ LPDIRECTDRAWPALETTE FAR* p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetPalette",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetPalette(p1);
	};
    STDMETHOD(GetPixelFormat)(THIS_ LPDDPIXELFORMAT p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetPixelFormat",subtype);
		//fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetPixelFormat(p1);
	};
    STDMETHOD(GetSurfaceDesc)(THIS_ LPDDSURFACEDESC2 p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetSurfaceDesc",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetSurfaceDesc(p1);
	};
    STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW p1, LPDDSURFACEDESC2 p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Initialize",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->Initialize(p1,p2);
	};
    STDMETHOD(IsLost)(THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] IsLost",subtype);
		//fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->IsLost();
	};
    STDMETHOD(Lock)(THIS_ LPRECT p1,LPDDSURFACEDESC2 p2,DWORD p3,HANDLE p4) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Lock",subtype);
		//fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->Lock(p1,p2,p3,p4);
	};
    STDMETHOD(ReleaseDC)(THIS_ HDC p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] ReleaseDC",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->ReleaseDC(p1);
	};
    STDMETHOD(Restore)(THIS) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Restore",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->Restore();
	};
    STDMETHOD(SetClipper)(THIS_ LPDIRECTDRAWCLIPPER p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] SetClipper",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->SetClipper(p1);
	};
    STDMETHOD(SetColorKey)(THIS_ DWORD p1, LPDDCOLORKEY p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] SetColorKey",subtype);
		//fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->SetColorKey(p1, p2);
	};
    STDMETHOD(SetOverlayPosition)(THIS_ LONG p1, LONG p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] SetOverlayPosition",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->SetOverlayPosition(p1,p2);
	};
    STDMETHOD(SetPalette)(THIS_ LPDIRECTDRAWPALETTE p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] SetPalette",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->SetPalette(p1);
	};
    STDMETHOD(Unlock)(THIS_ LPRECT p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] Unlock",subtype);
		//fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->Unlock(p1);
	};
    STDMETHOD(UpdateOverlay)(THIS_ LPRECT p1, LPDIRECTDRAWSURFACE4 p2,LPRECT p3,DWORD p4, LPDDOVERLAYFX p5)  
	{
		char buf[1024];
		sprintf(buf,"[%s] UpdateOverlay",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->UpdateOverlay(p1,p2,p3,p4,p5);
	}
    STDMETHOD(UpdateOverlayDisplay)(THIS_ DWORD p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] UpdateOverlayDisplay",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->UpdateOverlayDisplay(p1);
	}
    STDMETHOD(UpdateOverlayZOrder)(THIS_ DWORD p1, LPDIRECTDRAWSURFACE4 p2) 
	{
		char buf[1024];
		sprintf(buf,"[%s] UpdateOverlayZOrder",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->UpdateOverlayZOrder(p1,p2);
	}
    /*** Added in the v2 interface ***/
    STDMETHOD(GetDDInterface)(THIS_ LPVOID FAR *p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] GetDDInterface",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetDDInterface(p1);
	}
    STDMETHOD(PageLock)(THIS_ DWORD p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] PageLock",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->PageLock(p1);
	}
    STDMETHOD(PageUnlock)(THIS_ DWORD p1) 
	{
		char buf[1024];
		sprintf(buf,"[%s] PageUnlock",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->PageUnlock(p1);
	}

	/*** Added in the v3 interface ***/
    STDMETHOD(SetSurfaceDesc)(THIS_ LPDDSURFACEDESC2 p1, DWORD p2)  
	{
		char buf[1024];
		sprintf(buf,"[%s] SetSurfaceDesc",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->SetSurfaceDesc(p1,p2);
	}
    /*** Added in the v4 interface ***/
    STDMETHOD(SetPrivateData)(THIS_ REFGUID p1, LPVOID p2, DWORD p3, DWORD p4)  
	{
		char buf[1024];
		sprintf(buf,"[%s] SetPrivateData",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->SetPrivateData(p1,p2,p3,p4);
	};
    STDMETHOD(GetPrivateData)(THIS_ REFGUID p1, LPVOID p2, LPDWORD p3)  
	{
		char buf[1024];
		sprintf(buf,"[%s] GetPrivateData",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetPrivateData(p1,p2,p3);
	};
    STDMETHOD(FreePrivateData)(THIS_ REFGUID p1)  
	{
		char buf[1024];
		sprintf(buf,"[%s] FreePrivateData",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->FreePrivateData(p1);
	};
    STDMETHOD(GetUniquenessValue)(THIS_ LPDWORD p1)  
	{
		char buf[1024];
		sprintf(buf,"[%s] GetUniquenessValue",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->GetUniquenessValue(p1);
	};
    STDMETHOD(ChangeUniquenessValue)(THIS)  
	{
		char buf[1024];
		sprintf(buf,"[%s] ChangeUniquenessValue",subtype);
		fprintf(debugFile,"%s\n",buf);
		return realIDirectDrawSurface4->ChangeUniquenessValue();
	};


 private:
	IDirectDrawSurface4 *realIDirectDrawSurface4;	
	char subtype[128];

};
#endif