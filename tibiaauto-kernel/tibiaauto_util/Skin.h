#pragma once

#include "tibiaauto_util.h"
#include "BtnST.h"

class TIBIAAUTOUTIL_API CSkin
{
public:
	void SetButtonSkin(CButtonST&);
	bool operator==(const CSkin &) const;
	bool CSkin::operator!=(const CSkin &other) const;

	CSkin();
	virtual ~CSkin();
	int m_ButtonFaceRedValue;
	int m_ButtonFaceGreenValue;
	int m_ButtonFaceBlueValue;
	int m_ButtonDSRedValue;
	int m_ButtonDSGreenValue;
	int m_ButtonDSBlueValue;
	int m_ButtonLSRedValue;
	int m_ButtonLSGreenValue;
	int m_ButtonLSBlueValue;
	int m_ButtonLightRedValue;
	int m_ButtonLightGreenValue;
	int m_ButtonLightBlueValue;
	int m_ButtonHighLightRedValue;
	int m_ButtonHighLightGreenValue;
	int m_ButtonHighLightBlueValue;
	int m_TextRedValue;
	int m_TextGreenValue;
	int m_TextBlueValue;
	int m_PrimaryBackgroundRedValue;
	int m_PrimaryBackgroundGreenValue;
	int m_PrimaryBackgroundBlueValue;
	int m_SecondaryBackgroundRedValue;
	int m_SecondaryBackgroundGreenValue;
	int m_SecondaryBackgroundBlueValue;
	int m_EditboxRedValue;
	int m_EditboxGreenValue;
	int m_EditboxBlueValue;
};
TIBIAAUTOUTIL_API extern CSkin skin;
