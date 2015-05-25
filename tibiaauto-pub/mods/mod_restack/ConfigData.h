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

	int ammoType;
	int ammoAt;
	int ammoTo;
	int throwableType;
	int throwableAt;
	int throwableTo;
	int pickupSpears;
	int pickupToHand;
	int pickupUL, pickupUC, pickupUR;
	int pickupCL, pickupCC, pickupCR;
	int pickupBL, pickupBC, pickupBR;
	int moveCovering;
	int restackToRight;
	int periodFrom, periodTo;
	int capLimit;
};

#endif // !defined(AFX_CONFIGDATA_H__3C69E44D_D036_45EB_BCE8_5358CF410631__INCLUDED_)
