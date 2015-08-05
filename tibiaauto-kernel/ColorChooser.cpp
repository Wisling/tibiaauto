// ColorChooser.cpp : implementation file
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "ColorChooser.h"
#include <xercesc/util/XMLString.hpp>
#include "util.h"
#include "Skin.h"
#include <MemReader.h>
#include <tibiaauto_util/SkinLoader.h>


/////////////////////////////////////////////////////////////////////////////
// CColorChooser dialog


CColorChooser::CColorChooser(CWnd* pParent /*=NULL*/)
	: CDialog(CColorChooser::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorChooser)
	//}}AFX_DATA_INIT
}

void CColorChooser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorChooser)
	DDX_Control(pDX, IDC_EXAMPLE_EDIT, m_ExampleEditbox);
	DDX_Control(pDX, IDC_EXAMPLE_CHECK, m_ExampleCheckbox);
	DDX_Control(pDX, IDC_SECONDARY_BACKGROUND, m_SecondaryBackground);
	DDX_Control(pDX, IDC_DETAIL_PICKER, m_DetailPicker);
	DDX_Control(pDX, IDC_GREEN_SLIDER_UP, m_GreenSliderUp);
	DDX_Control(pDX, IDC_RED_SLIDER_UP, m_RedSliderUp);
	DDX_Control(pDX, IDC_GREEN_SLIDER_DOWN, m_GreenSliderDown);
	DDX_Control(pDX, IDC_BLUE_SLIDER_UP, m_BlueSliderUp);
	DDX_Control(pDX, IDC_BLUE_SLIDER_DOWN, m_BlueSliderDown);
	DDX_Control(pDX, IDC_EXAMPLE_BUTTON, m_Example);
	DDX_Control(pDX, IDC_RED_SLIDER_DOWN, m_RedSliderDown);
	DDX_Control(pDX, IDC_GREEN_SLIDER, m_GreenSlider);
	DDX_Control(pDX, IDC_BLUE_SLIDER, m_BlueSlider);
	DDX_Control(pDX, IDC_RED_SLIDER, m_RedSlider);
	DDX_Text(pDX, IDC_RED_VALUE, m_DetailRedValue);
	DDV_MinMaxInt(pDX, m_DetailRedValue, 0, 255);
	DDX_Text(pDX, IDC_GREEN_VALUE, m_DetailGreenValue);
	DDV_MinMaxInt(pDX, m_DetailGreenValue, 0, 255);
	DDX_Text(pDX, IDC_BLUE_VALUE, m_DetailBlueValue);
	DDV_MinMaxInt(pDX, m_DetailBlueValue, 0, 255);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CColorChooser, CDialog)
//{{AFX_MSG_MAP(CColorChooser)
ON_WM_HSCROLL()
ON_BN_CLICKED(IDC_RED_SLIDER_DOWN, OnRedSliderDown)
ON_BN_CLICKED(IDC_GREEN_SLIDER_DOWN, OnGreenSliderDown)
ON_BN_CLICKED(IDC_BLUE_SLIDER_DOWN, OnBlueSliderDown)
ON_BN_CLICKED(IDC_RED_SLIDER_UP, OnRedSliderUp)
ON_BN_CLICKED(IDC_GREEN_SLIDER_UP, OnGreenSliderUp)
ON_BN_CLICKED(IDC_BLUE_SLIDER_UP, OnBlueSliderUp)
ON_WM_CTLCOLOR()
ON_WM_ERASEBKGND()
ON_CBN_SELCHANGE(IDC_DETAIL_PICKER, OnSelchangeDetailPicker)
ON_BN_CLICKED(IDC_LOAD, OnLoad)
ON_BN_CLICKED(IDSAVE, OnSave)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorChooser message handlers

BOOL CColorChooser::OnInitDialog()
{
	CDialog::OnInitDialog();

	originalSkin = skin;

	// TODO: Add extra initialization here
	GetClientRect(m_ExampleRect);
	m_ExampleRect.top    += 7;
	m_ExampleRect.bottom -= 7;
	m_ExampleRect.right  -= 8;
	m_ExampleRect.left    = 265;

	m_ExampleEditbox.SetWindowText("Editbox");

	m_RedSlider.SetRange(0, 255, true);
	m_GreenSlider.SetRange(0, 255, true);
	m_BlueSlider.SetRange(0, 255, true);
	m_DetailPicker.SetCurSel(0);
	switch (m_DetailPicker.GetCurSel())
	{
	case 0:
		UpdateDisplay(6, skin.m_ButtonFaceRedValue, skin.m_ButtonFaceGreenValue, skin.m_ButtonFaceBlueValue);
		break;
	case 1:
		UpdateDisplay(6, skin.m_ButtonDSRedValue, skin.m_ButtonDSGreenValue, skin.m_ButtonDSBlueValue);
		break;
	case 2:
		UpdateDisplay(6, skin.m_ButtonLSRedValue, skin.m_ButtonLSGreenValue, skin.m_ButtonLSBlueValue);
		break;
	case 3:
		UpdateDisplay(6, skin.m_ButtonLightRedValue, skin.m_ButtonLightGreenValue, skin.m_ButtonLightBlueValue);
		break;
	case 4:
		UpdateDisplay(6, skin.m_ButtonHighLightRedValue, skin.m_ButtonHighLightGreenValue, skin.m_ButtonHighLightBlueValue);
		break;
	case 5:
		UpdateDisplay(6, skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue);
		break;
	case 6:
		UpdateDisplay(6, skin.m_PrimaryBackgroundRedValue, skin.m_PrimaryBackgroundGreenValue, skin.m_PrimaryBackgroundBlueValue);
		break;
	case 7:
		UpdateDisplay(6, skin.m_SecondaryBackgroundRedValue, skin.m_SecondaryBackgroundGreenValue, skin.m_SecondaryBackgroundBlueValue);
		break;
	case 8:
		UpdateDisplay(6, skin.m_EditboxRedValue, skin.m_EditboxGreenValue, skin.m_EditboxBlueValue);
		break;
	}

	SetTimer(2000, 250, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CColorChooser::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	CSliderCtrl *check = (CSliderCtrl *)pScrollBar;
	switch (nSBCode)
	{
	case TB_THUMBTRACK:
		switch (m_DetailPicker.GetCurSel())
		{
		case 0:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonFaceRedValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonFaceGreenValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonFaceBlueValue = nPos;
			break;
		case 1:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonDSRedValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonDSGreenValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonDSBlueValue = nPos;
			break;
		case 2:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonLSRedValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonLSGreenValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonLSBlueValue = nPos;
			break;
		case 3:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonLightRedValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonLightGreenValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonLightBlueValue = nPos;
			break;
		case 4:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonHighLightRedValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonHighLightGreenValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonHighLightBlueValue = nPos;
			break;
		case 5:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_TextRedValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_TextGreenValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_TextBlueValue = nPos;
			break;
		case 6:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_PrimaryBackgroundRedValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_PrimaryBackgroundGreenValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_PrimaryBackgroundBlueValue = nPos;
			break;
		case 7:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_SecondaryBackgroundRedValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_SecondaryBackgroundGreenValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_SecondaryBackgroundBlueValue = nPos;
			break;
		case 8:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_EditboxRedValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_EditboxGreenValue = nPos;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_EditboxBlueValue = nPos;
			break;
		}
		break;
	case TB_BOTTOM:
		switch (m_DetailPicker.GetCurSel())
		{
		case 0:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonFaceRedValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonFaceGreenValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonFaceBlueValue = 255;
			break;
		case 1:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonDSRedValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonDSGreenValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonDSBlueValue = 255;
			break;
		case 2:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonLSRedValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonLSGreenValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonLSBlueValue = 255;
			break;
		case 3:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonLightRedValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonLightGreenValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonLightBlueValue = 255;
			break;
		case 4:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonHighLightRedValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonHighLightGreenValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonHighLightBlueValue = 255;
			break;
		case 5:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_TextRedValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_TextGreenValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_TextBlueValue = 255;
			break;
		case 6:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_PrimaryBackgroundRedValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_PrimaryBackgroundGreenValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_PrimaryBackgroundBlueValue = 255;
			break;
		case 7:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_SecondaryBackgroundRedValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_SecondaryBackgroundGreenValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_SecondaryBackgroundBlueValue = 255;
			break;
		case 8:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_EditboxRedValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_EditboxGreenValue = 255;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_EditboxBlueValue = 255;
			break;
		}
		break;
	case TB_TOP:
		switch (m_DetailPicker.GetCurSel())
		{
		case 0:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonFaceRedValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonFaceGreenValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonFaceBlueValue = 0;
			break;
		case 1:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonDSRedValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonDSGreenValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonDSBlueValue = 0;
			break;
		case 2:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonLSRedValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonLSGreenValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonLSBlueValue = 0;
			break;
		case 3:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonLightRedValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonLightGreenValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonLightBlueValue = 0;
			break;
		case 4:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_ButtonHighLightRedValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_ButtonHighLightGreenValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_ButtonHighLightBlueValue = 0;
			break;
		case 5:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_TextRedValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_TextGreenValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_TextBlueValue = 0;
			break;
		case 6:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_PrimaryBackgroundRedValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_PrimaryBackgroundGreenValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_PrimaryBackgroundBlueValue = 0;
			break;
		case 7:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_SecondaryBackgroundRedValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_SecondaryBackgroundGreenValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_SecondaryBackgroundBlueValue = 0;
			break;
		case 8:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
				skin.m_EditboxRedValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
				skin.m_EditboxGreenValue = 0;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
				skin.m_EditboxBlueValue = 0;
			break;
		}
		break;
	case TB_LINEDOWN:
		switch (m_DetailPicker.GetCurSel())
		{
		case 0:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue < 255)
				skin.m_ButtonFaceRedValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue < 255)
				skin.m_ButtonFaceGreenValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue < 255)
				skin.m_ButtonFaceBlueValue++;
			break;
		case 1:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue < 255)
				skin.m_ButtonDSRedValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue < 255)
				skin.m_ButtonDSGreenValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue < 255)
				skin.m_ButtonDSBlueValue++;
			break;
		case 2:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue < 255)
				skin.m_ButtonLSRedValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue < 255)
				skin.m_ButtonLSGreenValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue < 255)
				skin.m_ButtonLSBlueValue++;
			break;
		case 3:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue < 255)
				skin.m_ButtonLightRedValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue < 255)
				skin.m_ButtonLightGreenValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue < 255)
				skin.m_ButtonLightBlueValue++;
			break;
		case 4:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue < 255)
				skin.m_ButtonHighLightRedValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue < 255)
				skin.m_ButtonHighLightGreenValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue < 255)
				skin.m_ButtonHighLightBlueValue++;
			break;
		case 5:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue < 255)
				skin.m_TextRedValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue < 255)
				skin.m_TextGreenValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue < 255)
				skin.m_TextBlueValue++;
			break;
		case 6:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue < 255)
				skin.m_PrimaryBackgroundRedValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue < 255)
				skin.m_PrimaryBackgroundGreenValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue < 255)
				skin.m_PrimaryBackgroundBlueValue++;
			break;
		case 7:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue < 255)
				skin.m_SecondaryBackgroundRedValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue < 255)
				skin.m_SecondaryBackgroundGreenValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue < 255)
				skin.m_SecondaryBackgroundBlueValue++;
			break;
		case 8:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue < 255)
				skin.m_EditboxRedValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue < 255)
				skin.m_EditboxGreenValue++;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue < 255)
				skin.m_EditboxBlueValue++;
			break;
		}
		break;
	case TB_LINEUP:
		switch (m_DetailPicker.GetCurSel())
		{
		case 0:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue > 0)
				skin.m_ButtonFaceRedValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue > 0)
				skin.m_ButtonFaceGreenValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue > 0)
				skin.m_ButtonFaceBlueValue--;
			break;
		case 1:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue > 0)
				skin.m_ButtonDSRedValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue > 0)
				skin.m_ButtonDSGreenValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue > 0)
				skin.m_ButtonDSBlueValue--;
			break;
		case 2:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue > 0)
				skin.m_ButtonLSRedValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue > 0)
				skin.m_ButtonLSGreenValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue > 0)
				skin.m_ButtonLSBlueValue--;
			break;
		case 3:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue > 0)
				skin.m_ButtonLightRedValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue > 0)
				skin.m_ButtonLightGreenValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue > 0)
				skin.m_ButtonLightBlueValue--;
			break;
		case 4:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue > 0)
				skin.m_ButtonHighLightRedValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue > 0)
				skin.m_ButtonHighLightGreenValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue > 0)
				skin.m_ButtonHighLightBlueValue--;
			break;
		case 5:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue > 0)
				skin.m_TextRedValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue > 0)
				skin.m_TextGreenValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue > 0)
				skin.m_TextBlueValue--;
			break;
		case 6:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue > 0)
				skin.m_PrimaryBackgroundRedValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue > 0)
				skin.m_PrimaryBackgroundGreenValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue > 0)
				skin.m_PrimaryBackgroundBlueValue--;
			break;
		case 7:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue > 0)
				skin.m_SecondaryBackgroundRedValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue > 0)
				skin.m_SecondaryBackgroundGreenValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue > 0)
				skin.m_SecondaryBackgroundBlueValue--;
			break;
		case 8:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER && m_DetailRedValue > 0)
				skin.m_EditboxRedValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER && m_DetailGreenValue > 0)
				skin.m_EditboxGreenValue--;
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER && m_DetailBlueValue > 0)
				skin.m_EditboxBlueValue--;
			break;
		}
		break;
	case TB_PAGEDOWN:
		switch (m_DetailPicker.GetCurSel())
		{
		case 0:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue < 240)
					skin.m_ButtonFaceRedValue += 16;
				else
					skin.m_ButtonFaceRedValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue < 240)
					skin.m_ButtonFaceGreenValue += 16;
				else
					skin.m_ButtonFaceGreenValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue < 240)
					skin.m_ButtonFaceBlueValue += 16;
				else
					skin.m_ButtonFaceBlueValue = 255;
			}
			break;
		case 1:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue < 240)
					skin.m_ButtonDSRedValue += 16;
				else
					skin.m_ButtonDSRedValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue < 240)
					skin.m_ButtonDSGreenValue += 16;
				else
					skin.m_ButtonDSGreenValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue < 240)
					skin.m_ButtonDSBlueValue += 16;
				else
					skin.m_ButtonDSBlueValue = 255;
			}
			break;
		case 2:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue < 240)
					skin.m_ButtonLSRedValue += 16;
				else
					skin.m_ButtonLSRedValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue < 240)
					skin.m_ButtonLSGreenValue += 16;
				else
					skin.m_ButtonLSGreenValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue < 240)
					skin.m_ButtonLSBlueValue += 16;
				else
					skin.m_ButtonLSBlueValue = 255;
			}
			break;
		case 3:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue < 240)
					skin.m_ButtonLightRedValue += 16;
				else
					skin.m_ButtonLightRedValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue < 240)
					skin.m_ButtonLightGreenValue += 16;
				else
					skin.m_ButtonLightGreenValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue < 240)
					skin.m_ButtonLightBlueValue += 16;
				else
					skin.m_ButtonLightBlueValue = 255;
			}
			break;
		case 4:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue < 240)
					skin.m_ButtonHighLightRedValue += 16;
				else
					skin.m_ButtonHighLightRedValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue < 240)
					skin.m_ButtonHighLightGreenValue += 16;
				else
					skin.m_ButtonHighLightGreenValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue < 240)
					skin.m_ButtonHighLightBlueValue += 16;
				else
					skin.m_ButtonHighLightBlueValue = 255;
			}
			break;
		case 5:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue < 240)
					skin.m_TextRedValue += 16;
				else
					skin.m_TextRedValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue < 240)
					skin.m_TextGreenValue += 16;
				else
					skin.m_TextGreenValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue < 240)
					skin.m_TextBlueValue += 16;
				else
					skin.m_TextBlueValue = 255;
			}
			break;
		case 6:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue < 240)
					skin.m_PrimaryBackgroundRedValue += 16;
				else
					skin.m_PrimaryBackgroundRedValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue < 240)
					skin.m_PrimaryBackgroundGreenValue += 16;
				else
					skin.m_PrimaryBackgroundGreenValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue < 240)
					skin.m_PrimaryBackgroundBlueValue += 16;
				else
					skin.m_PrimaryBackgroundBlueValue = 255;
			}
			break;
		case 7:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue < 240)
					skin.m_SecondaryBackgroundRedValue += 16;
				else
					skin.m_SecondaryBackgroundRedValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue < 240)
					skin.m_SecondaryBackgroundGreenValue += 16;
				else
					skin.m_SecondaryBackgroundGreenValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue < 240)
					skin.m_SecondaryBackgroundBlueValue += 16;
				else
					skin.m_SecondaryBackgroundBlueValue = 255;
			}
			break;
		case 8:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue < 240)
					skin.m_EditboxRedValue += 16;
				else
					skin.m_EditboxRedValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue < 240)
					skin.m_EditboxGreenValue += 16;
				else
					skin.m_EditboxGreenValue = 255;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue < 240)
					skin.m_EditboxBlueValue += 16;
				else
					skin.m_EditboxBlueValue = 255;
			}
			break;
		}
		break;
	case TB_PAGEUP:
		switch (m_DetailPicker.GetCurSel())
		{
		case 0:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue > 16)
					skin.m_ButtonFaceRedValue -= 16;
				else
					skin.m_ButtonFaceRedValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue > 16)
					skin.m_ButtonFaceGreenValue -= 16;
				else
					skin.m_ButtonFaceGreenValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue > 16)
					skin.m_ButtonFaceBlueValue -= 16;
				else
					skin.m_ButtonFaceBlueValue = 0;
			}
			break;
		case 1:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue > 16)
					skin.m_ButtonDSRedValue -= 16;
				else
					skin.m_ButtonDSRedValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue > 16)
					skin.m_ButtonDSGreenValue -= 16;
				else
					skin.m_ButtonDSGreenValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue > 16)
					skin.m_ButtonDSBlueValue -= 16;
				else
					skin.m_ButtonDSBlueValue = 0;
			}
			break;
		case 2:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue > 16)
					skin.m_ButtonLSRedValue -= 16;
				else
					skin.m_ButtonLSRedValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue > 16)
					skin.m_ButtonLSGreenValue -= 16;
				else
					skin.m_ButtonLSGreenValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue > 16)
					skin.m_ButtonLSBlueValue -= 16;
				else
					skin.m_ButtonLSBlueValue = 0;
			}
			break;
		case 3:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue > 16)
					skin.m_ButtonLightRedValue -= 16;
				else
					skin.m_ButtonLightRedValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue > 16)
					skin.m_ButtonLightGreenValue -= 16;
				else
					skin.m_ButtonLightGreenValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue > 16)
					skin.m_ButtonLightBlueValue -= 16;
				else
					skin.m_ButtonLightBlueValue = 0;
			}
			break;
		case 4:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue > 16)
					skin.m_ButtonHighLightRedValue -= 16;
				else
					skin.m_ButtonHighLightRedValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue > 16)
					skin.m_ButtonHighLightGreenValue -= 16;
				else
					skin.m_ButtonHighLightGreenValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue > 16)
					skin.m_ButtonHighLightBlueValue -= 16;
				else
					skin.m_ButtonHighLightBlueValue = 0;
			}
			break;
		case 5:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue > 16)
					skin.m_TextRedValue -= 16;
				else
					skin.m_TextRedValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue > 16)
					skin.m_TextGreenValue -= 16;
				else
					skin.m_TextGreenValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue > 16)
					skin.m_TextBlueValue -= 16;
				else
					skin.m_TextBlueValue = 0;
			}
			break;
		case 6:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue > 16)
					skin.m_PrimaryBackgroundRedValue -= 16;
				else
					skin.m_PrimaryBackgroundRedValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue > 16)
					skin.m_PrimaryBackgroundGreenValue -= 16;
				else
					skin.m_PrimaryBackgroundGreenValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue > 16)
					skin.m_PrimaryBackgroundBlueValue -= 16;
				else
					skin.m_PrimaryBackgroundBlueValue = 0;
			}
			break;
		case 7:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue > 16)
					skin.m_SecondaryBackgroundRedValue -= 16;
				else
					skin.m_SecondaryBackgroundRedValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue > 16)
					skin.m_SecondaryBackgroundGreenValue -= 16;
				else
					skin.m_SecondaryBackgroundGreenValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue > 16)
					skin.m_SecondaryBackgroundBlueValue -= 16;
				else
					skin.m_SecondaryBackgroundBlueValue = 0;
			}
		case 8:
			if (pScrollBar->GetDlgCtrlID() == IDC_RED_SLIDER)
			{
				if (m_DetailRedValue > 16)
					skin.m_EditboxRedValue -= 16;
				else
					skin.m_EditboxRedValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_GREEN_SLIDER)
			{
				if (m_DetailGreenValue > 16)
					skin.m_EditboxGreenValue -= 16;
				else
					skin.m_EditboxGreenValue = 0;
			}
			if (pScrollBar->GetDlgCtrlID() == IDC_BLUE_SLIDER)
			{
				if (m_DetailBlueValue > 16)
					skin.m_EditboxBlueValue -= 16;
				else
					skin.m_EditboxBlueValue = 0;
			}
			break;
		}
		break;
	}

	switch (m_DetailPicker.GetCurSel())
	{
	case 0:
		UpdateDisplay(0, skin.m_ButtonFaceRedValue, skin.m_ButtonFaceGreenValue, skin.m_ButtonFaceBlueValue);
		break;
	case 1:
		UpdateDisplay(1, skin.m_ButtonDSRedValue, skin.m_ButtonDSGreenValue, skin.m_ButtonDSBlueValue);
		break;
	case 2:
		UpdateDisplay(2, skin.m_ButtonLSRedValue, skin.m_ButtonLSGreenValue, skin.m_ButtonLSBlueValue);
		break;
	case 3:
		UpdateDisplay(3, skin.m_ButtonLightRedValue, skin.m_ButtonLightGreenValue, skin.m_ButtonLightBlueValue);
		break;
	case 4:
		UpdateDisplay(4, skin.m_ButtonHighLightRedValue, skin.m_ButtonHighLightGreenValue, skin.m_ButtonHighLightBlueValue);
		break;
	case 5:
		UpdateDisplay(5, skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue);
		break;
	case 6:
		UpdateDisplay(6, skin.m_PrimaryBackgroundRedValue, skin.m_PrimaryBackgroundGreenValue, skin.m_PrimaryBackgroundBlueValue);
		break;
	case 7:
		UpdateDisplay(7, skin.m_SecondaryBackgroundRedValue, skin.m_SecondaryBackgroundGreenValue, skin.m_SecondaryBackgroundBlueValue);
		break;
	case 8:
		UpdateDisplay(8, skin.m_EditboxRedValue, skin.m_EditboxGreenValue, skin.m_EditboxBlueValue);
		break;
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CColorChooser::OnRedSliderDown()
{
	// TODO: Add your control notification handler code here
	OnHScroll(TB_LINEUP, 0, (CScrollBar*) &m_RedSlider);
}

void CColorChooser::OnGreenSliderDown()
{
	// TODO: Add your control notification handler code here
	OnHScroll(TB_LINEUP, 0, (CScrollBar*) &m_GreenSlider);
}

void CColorChooser::OnBlueSliderDown()
{
	// TODO: Add your control notification handler code here
	OnHScroll(TB_LINEUP, 0, (CScrollBar*) &m_BlueSlider);
}

void CColorChooser::OnRedSliderUp()
{
	// TODO: Add your control notification handler code here
	OnHScroll(TB_LINEDOWN, 0, (CScrollBar*) &m_RedSlider);
}

void CColorChooser::OnGreenSliderUp()
{
	// TODO: Add your control notification handler code here
	OnHScroll(TB_LINEDOWN, 0, (CScrollBar*) &m_GreenSlider);
}

void CColorChooser::OnBlueSliderUp()
{
	// TODO: Add your control notification handler code here
	OnHScroll(TB_LINEDOWN, 0, (CScrollBar*) &m_BlueSlider);
}

HBRUSH CColorChooser::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// TODO: Change any attributes of the DC here

	// TODO: Return a different brush if the default is not desired
	CRect rect;
	pWnd->GetClientRect(&rect);
	CBrush brush;
	brush.CreateSolidBrush(RGB(skin.m_SecondaryBackgroundRedValue, skin.m_SecondaryBackgroundGreenValue, skin.m_SecondaryBackgroundBlueValue));
	if (pWnd->GetDlgCtrlID() == 9999)
	{
		rect.top    += 6;
		rect.right  -= 2;
		rect.bottom -= 2;
		pDC->FillRect(&rect, &brush);
	}

	if (pWnd->GetDlgCtrlID() == 9999)
	{
		pDC->SetTextColor(RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue));
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
	}
	else if (pWnd->GetDlgCtrlID() == IDC_EXAMPLE_CHECK)
	{
		pDC->SetTextColor(RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue));
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
	}
	else if (pWnd->GetDlgCtrlID() == IDC_EXAMPLE_STATIC)
	{
		pDC->SetTextColor(RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue));
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
	}
	else if (pWnd->GetDlgCtrlID() == IDC_EXAMPLE_EDIT)
	{
		pDC->SetTextColor(RGB(skin.m_EditboxRedValue, skin.m_EditboxGreenValue, skin.m_EditboxBlueValue));
		return (HBRUSH)brush;
	}
	else
	{
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
}

void CColorChooser::UpdateDisplay(int updateID, int redValue, int greenValue, int blueValue)
{
	m_DetailRedValue   = redValue;
	m_DetailGreenValue = greenValue;
	m_DetailBlueValue  = blueValue;

	m_RedSlider.SetPos(m_DetailRedValue);
	m_GreenSlider.SetPos(m_DetailGreenValue);
	m_BlueSlider.SetPos(m_DetailBlueValue);
	m_RedSlider.AddColor(0, 255, RGB(0, m_DetailGreenValue, m_DetailBlueValue), RGB(255, m_DetailGreenValue, m_DetailBlueValue));
	m_RedSlider.Refresh();
	m_GreenSlider.AddColor(0, 255, RGB(m_DetailRedValue, 0, m_DetailBlueValue), RGB(m_DetailRedValue, 255, m_DetailBlueValue));
	m_GreenSlider.Refresh();
	m_BlueSlider.AddColor(0, 255, RGB(m_DetailRedValue, m_DetailGreenValue, 0), RGB(m_DetailRedValue, m_DetailGreenValue, 255));
	m_BlueSlider.Refresh();

	if (updateID == 6)
		InvalidateRect(m_ExampleRect);
	m_SecondaryBackground.Invalidate();
	skin.SetButtonSkin(m_Example);
	m_ExampleEditbox.Invalidate();
	m_ExampleCheckbox.Invalidate();

	UpdateData(false);
}

void CColorChooser::OnSelchangeDetailPicker()
{
	// TODO: Add your control notification handler code here
	switch (m_DetailPicker.GetCurSel())
	{
	case 0:
		UpdateDisplay(0, skin.m_ButtonFaceRedValue, skin.m_ButtonFaceGreenValue, skin.m_ButtonFaceBlueValue);
		break;
	case 1:
		UpdateDisplay(1, skin.m_ButtonDSRedValue, skin.m_ButtonDSGreenValue, skin.m_ButtonDSBlueValue);
		break;
	case 2:
		UpdateDisplay(2, skin.m_ButtonLSRedValue, skin.m_ButtonLSGreenValue, skin.m_ButtonLSBlueValue);
		break;
	case 3:
		UpdateDisplay(3, skin.m_ButtonLightRedValue, skin.m_ButtonLightGreenValue, skin.m_ButtonLightBlueValue);
		break;
	case 4:
		UpdateDisplay(4, skin.m_ButtonHighLightRedValue, skin.m_ButtonHighLightGreenValue, skin.m_ButtonHighLightBlueValue);
		break;
	case 5:
		UpdateDisplay(5, skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue);
		break;
	case 6:
		UpdateDisplay(6, skin.m_PrimaryBackgroundRedValue, skin.m_PrimaryBackgroundGreenValue, skin.m_PrimaryBackgroundBlueValue);
		break;
	case 7:
		UpdateDisplay(7, skin.m_SecondaryBackgroundRedValue, skin.m_SecondaryBackgroundGreenValue, skin.m_SecondaryBackgroundBlueValue);
		break;
	case 8:
		UpdateDisplay(8, skin.m_EditboxRedValue, skin.m_EditboxGreenValue, skin.m_EditboxBlueValue);
		break;
	}
}

BOOL CColorChooser::OnEraseBkgnd(CDC* pDC)
{
	CDialog::OnEraseBkgnd(pDC);

	CBrush primaryBackground;
	primaryBackground.CreateSolidBrush(RGB(skin.m_PrimaryBackgroundRedValue, skin.m_PrimaryBackgroundGreenValue, skin.m_PrimaryBackgroundBlueValue));

	pDC->FillRect(m_ExampleRect, &primaryBackground);
	return true;
}

void CColorChooser::OnOK()
{
	// TODO: Add extra validation here
	if (loadedSkin != skin)
	{
		char szFilters[] =
		        "Tibia Auto Skin (*.skin)|*.skin|All Files (*.*)|*.*||";

		CFileDialog fd(false, "", NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);

		if (fd.DoModal() == IDOK)
			pathBuf = fd.GetPathName();
		else
			return;
	}

	if (skin != originalSkin)
		saveSkin(pathBuf, skin, loadedSkin != skin);
	CDialog::OnOK();
}

void CColorChooser::OnLoad()
{
	// TODO: Add your control notification handler code here
	char szFilters[] =
	        "Tibia Auto Skin (*.skin)|*.skin|All Files (*.*)|*.*||";

	CFileDialog fd(true, "", NULL, OFN_FILEMUSTEXIST, szFilters, this);
	if (fd.DoModal() == IDOK)
	{
		pathBuf = (LPCSTR)fd.GetPathName();
		skin    = loadedSkin = loadSkin(pathBuf);
	}

	UpdateDisplay(6, skin.m_ButtonFaceRedValue, skin.m_ButtonFaceGreenValue, skin.m_ButtonFaceBlueValue);
}

void CColorChooser::OnCancel()
{
	// TODO: Add extra cleanup here
	skin = originalSkin;
	CDialog::OnCancel();
}

void CColorChooser::OnSave()
{
	// TODO: Add your control notification handler code here
	char szFilters[] =
	        "Tibia Auto Skin (*.skin)|*.skin|All Files (*.*)|*.*||";

	CFileDialog fd(false, "", NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);

	if (fd.DoModal() == IDOK)
		pathBuf = fd.GetPathName();
	else
		return;

	saveSkin(pathBuf, skin, true);
}
