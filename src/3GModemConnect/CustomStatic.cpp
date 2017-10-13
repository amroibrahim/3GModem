// ColorStatic.cpp : implementation file
//

#include "stdafx.h"
#include "3GModemConnect.h"
#include "CustomStatic.h"


// CColorStatic

IMPLEMENT_DYNAMIC(CCustomStatic, CWnd)

CCustomStatic::CCustomStatic()
{
	m_crTextColor = RGB(0, 0 , 0);
	m_sFont = "Segoe UI";
	m_iHeight = 15;
	m_iBold = FW_NORMAL;;
	m_bItalic = false;

	m_bAlignmentCenter = false;
	m_bAlignmentRight = false;
	m_bAlignmentLeft = true;
}

CCustomStatic::~CCustomStatic()
{
}


BEGIN_MESSAGE_MAP(CCustomStatic, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CColorStatic message handlers

void CCustomStatic::SetHeight(int iHeight)
{
	m_iHeight = iHeight;
}

void CCustomStatic::SetTextColor(COLORREF crColor)
{
	m_crTextColor = crColor;
}

void CCustomStatic::SetBold(bool bBold)
{
	if(bBold)
	{
		m_iBold  = FW_BLACK;
	}
	else
	{
		m_iBold = FW_NORMAL;
	}

}

void CCustomStatic::SetItalic(bool bItalic)
{
	m_bItalic = bItalic;
}

void CCustomStatic::SetAlignmentCenter()
{
	m_bAlignmentCenter = true;
	m_bAlignmentRight = false;
	m_bAlignmentLeft = false;
}

void CCustomStatic::SetAlignmentRight()
{
	m_bAlignmentCenter = false;
	m_bAlignmentRight = true;
	m_bAlignmentLeft = false;
}

void CCustomStatic::SetAlignmentLeft()
{
	m_bAlignmentCenter = false;
	m_bAlignmentRight = false;
	m_bAlignmentLeft = true;
}

void CCustomStatic::SetWindowText(LPCTSTR lpszString)
{
	CWnd::SetWindowText(L"");
	m_sText = lpszString;
	Invalidate(1);
}

void CCustomStatic::OnPaint()
{
	CPaintDC dc(this);

	

	CFont fFont;
	CFont *fOldFont;
	CRect rect;
	//CString sText;

	//GetWindowText(sText);
	GetClientRect(rect);

	UINT uiFormat = 0;

	if(m_bAlignmentCenter)
	{
		uiFormat|=DT_CENTER;
	}
	else if(m_bAlignmentRight)
	{
		uiFormat|=DT_RIGHT;
	}
	else if(m_bAlignmentLeft)
	{	
		uiFormat|=DT_LEFT;
	}

	uiFormat|=DT_NOPREFIX;
	uiFormat|=DT_WORDBREAK;



	dc.SetTextColor(m_crTextColor);

	fFont.Detach();


	fFont.CreateFont(m_iHeight, 
		0,
		0,
		0, 
		m_iBold, 
		m_bItalic, 
		FALSE, 
		FALSE, 
		DEFAULT_CHARSET, 
		OUT_CHARACTER_PRECIS, 
		CLIP_CHARACTER_PRECIS, 
		DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, 
		m_sFont);

	fOldFont = dc.SelectObject(&fFont);
	dc.SetBkMode(TRANSPARENT);

	//dc.TextOut(0, 0, sText);
	dc.DrawText(m_sText, rect, uiFormat);

	dc.SelectObject(fOldFont);

	fFont.DeleteObject(); 
}


BOOL CCustomStatic::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}
