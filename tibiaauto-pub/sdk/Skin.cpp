// Skin.cpp: implementation of the CSkin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Skin.h"
#include "BtnST.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkin::CSkin()
{
	m_ButtonFaceRedValue            = 0x72;
	m_ButtonFaceGreenValue          = 0;
	m_ButtonFaceBlueValue           = 0;
	m_ButtonDSRedValue              = 0x52;
	m_ButtonDSGreenValue            = 0;
	m_ButtonDSBlueValue             = 0;
	m_ButtonLSRedValue              = 0x62;
	m_ButtonLSGreenValue            = 0;
	m_ButtonLSBlueValue             = 0;
	m_ButtonLightRedValue           = 0x82;
	m_ButtonLightGreenValue         = 0;
	m_ButtonLightBlueValue          = 0;
	m_ButtonHighLightRedValue       = 0x92;
	m_ButtonHighLightGreenValue     = 0;
	m_ButtonHighLightBlueValue      = 0;
	m_TextRedValue                  = 0xff;
	m_TextGreenValue                = 0xff;
	m_TextBlueValue                 = 0xff;
	m_PrimaryBackgroundRedValue     = 0x72;
	m_PrimaryBackgroundGreenValue   = 0x5e;
	m_PrimaryBackgroundBlueValue    = 0x3d;
	m_SecondaryBackgroundRedValue   = 0x72;
	m_SecondaryBackgroundGreenValue = 0;
	m_SecondaryBackgroundBlueValue  = 0;
	m_EditboxRedValue               = 0x92;
	m_EditboxGreenValue             = 0;
	m_EditboxBlueValue              = 0;
}

CSkin::~CSkin()
{
}

bool CSkin::operator==(const CSkin &other) const
{
	bool result = false;
	if (other.m_ButtonDSBlueValue == this->m_ButtonDSBlueValue && other.m_ButtonDSGreenValue == this->m_ButtonDSGreenValue && other.m_ButtonDSRedValue == this->m_ButtonDSRedValue)
	{
		if (other.m_ButtonLSBlueValue == this->m_ButtonLSBlueValue && other.m_ButtonLSGreenValue == this->m_ButtonLSGreenValue && other.m_ButtonLSRedValue == this->m_ButtonLSRedValue)
		{
			if (other.m_ButtonLightBlueValue == this->m_ButtonLightBlueValue && other.m_ButtonLightGreenValue == this->m_ButtonLightGreenValue && other.m_ButtonLightRedValue == this->m_ButtonLightRedValue)
			{
				if (other.m_ButtonHighLightBlueValue == this->m_ButtonHighLightBlueValue && other.m_ButtonHighLightGreenValue == this->m_ButtonHighLightGreenValue && other.m_ButtonHighLightRedValue == this->m_ButtonHighLightRedValue)
				{
					if (other.m_ButtonFaceBlueValue == this->m_ButtonFaceBlueValue && other.m_ButtonFaceGreenValue == this->m_ButtonFaceGreenValue && other.m_ButtonFaceRedValue == this->m_ButtonFaceRedValue)
					{
						if (other.m_TextBlueValue == this->m_TextBlueValue && other.m_TextGreenValue == this->m_TextGreenValue && other.m_TextRedValue == this->m_TextRedValue)
						{
							if (other.m_PrimaryBackgroundBlueValue == this->m_PrimaryBackgroundBlueValue && other.m_PrimaryBackgroundGreenValue == this->m_PrimaryBackgroundGreenValue && other.m_PrimaryBackgroundRedValue == this->m_PrimaryBackgroundRedValue)
							{
								if (other.m_SecondaryBackgroundBlueValue == this->m_SecondaryBackgroundBlueValue && other.m_SecondaryBackgroundGreenValue == this->m_SecondaryBackgroundGreenValue && other.m_SecondaryBackgroundRedValue == this->m_SecondaryBackgroundRedValue)
									if (other.m_EditboxBlueValue == this->m_EditboxBlueValue && other.m_EditboxGreenValue == this->m_EditboxGreenValue && other.m_EditboxRedValue == this->m_EditboxRedValue)
										result = true;
							}
						}
					}
				}
			}
		}
	}
	return result;
}

bool CSkin::operator!=(const CSkin &other) const
{
	return !(*this == other);
}

CSkin skin;

void CSkin::SetButtonSkin(CButtonST &newButton)
{
	newButton.SetFlat(FALSE, FALSE);
	newButton.SetColor(0, RGB(min(255, skin.m_ButtonFaceRedValue + 30), min(255, skin.m_ButtonFaceGreenValue + 30), min(255, skin.m_ButtonFaceBlueValue + 30)), FALSE);
	newButton.SetColor(1, RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue), FALSE);
	newButton.SetColor(2, RGB(skin.m_ButtonFaceRedValue, skin.m_ButtonFaceGreenValue, skin.m_ButtonFaceBlueValue), FALSE);
	newButton.SetColor(3, RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue), FALSE);
	newButton.SetColor(4, RGB(skin.m_ButtonFaceRedValue, skin.m_ButtonFaceGreenValue, skin.m_ButtonFaceBlueValue), FALSE);
	newButton.SetColor(5, RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue), FALSE);
	newButton.SetColor(6, RGB(skin.m_ButtonLightRedValue, skin.m_ButtonLightGreenValue, skin.m_ButtonLightBlueValue), FALSE);
	newButton.SetColor(7, RGB(skin.m_ButtonHighLightRedValue, skin.m_ButtonHighLightGreenValue, skin.m_ButtonHighLightBlueValue), FALSE);
	newButton.SetColor(8, RGB(skin.m_ButtonLSRedValue, skin.m_ButtonLSGreenValue, skin.m_ButtonLSBlueValue), FALSE);
	newButton.SetColor(9, RGB(skin.m_ButtonDSRedValue, skin.m_ButtonDSGreenValue, skin.m_ButtonDSBlueValue));
}
