// SkinLoader.h: interface for the CSkinLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINLOADER_H__E5968AC4_92D5_4046_B7FB_746657C492EF__INCLUDED_)
#define AFX_SKINLOADER_H__E5968AC4_92D5_4046_B7FB_746657C492EF__INCLUDED_

#include "Skin.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSkinLoader
{
public:
	CSkinLoader();
	virtual ~CSkinLoader();

protected:
	bool saveSkin(CString, CSkin, bool);
	CSkin loadedSkin;
	CSkin loadCurrentSkin(CString);
	CSkin loadSkin(CString);
};

#endif // !defined(AFX_SKINLOADER_H__E5968AC4_92D5_4046_B7FB_746657C492EF__INCLUDED_)
