// MonsterButton.cpp : implementation file
//

#include "stdafx.h"
#include "MonsterButton.h"
#include "ToolMonsterShow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMonsterButton

CMonsterButton::CMonsterButton(int x, int y)
{
	m_x = x;
	m_y = y;
}

CMonsterButton::~CMonsterButton()
{
}


BEGIN_MESSAGE_MAP(CMonsterButton, CButton)
//{{AFX_MSG_MAP(CMonsterButton)
ON_WM_MOUSEMOVE()
ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonsterButton message handlers

void CMonsterButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CToolMonsterShow *parent = (CToolMonsterShow *)GetParent();

	parent->showMonsterDetails(m_x, m_y);

	CButton::OnMouseMove(nFlags, point);
}

BOOL CMonsterButton::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CMonsterButton::OnClicked()
{
}
