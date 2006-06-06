// ConfigData.h: interface for the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
#define AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConfigData  
{
public:
	CConfigData();
	virtual ~CConfigData();

	int	m_lootWorms;
	int	m_lootGp;
	int	m_lootFood;
	int	m_lootCustom;
	int	m_autoOpen;	
	int m_mode10;
	int	m_mode9;
	int	m_mode8;
	int	m_mode7;
	int	m_mode6;
	int	m_mode5;
	int	m_mode4;
	int	m_mode3;
	int	m_mode2;
	int	m_mode1;
};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
