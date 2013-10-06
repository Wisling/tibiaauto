// TibiaMiniMapLabel.h: interface for the CTibiaMiniMapLabel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIBIAMiniMapLabel_H__8571B5CD_B0D8_4F43_B4A0_D523C9730686__INCLUDED_)
#define AFX_TIBIAMiniMapLabel_H__8571B5CD_B0D8_4F43_B4A0_D523C9730686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTibiaMiniMapLabel  
{
public:
	CTibiaMiniMapLabel();
	virtual ~CTibiaMiniMapLabel();
	int x;
	int y;
	int type;
	char desc[100];

};

#endif // !defined(AFX_TIBIAMiniMapLabel_H__8571B5CD_B0D8_4F43_B4A0_D523C9730686__INCLUDED_)
