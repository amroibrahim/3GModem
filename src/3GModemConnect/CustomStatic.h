#pragma once


// CColorStatic

class CCustomStatic : public CWnd
{
	DECLARE_DYNAMIC(CCustomStatic)

public:
	CCustomStatic();
	virtual ~CCustomStatic();

	void SetTextColor(COLORREF crColor);
	void SetBold(bool bBold);
	void SetItalic(bool bItalic);
	void SetHeight(int iHeight);
	void SetAlignmentCenter();
	void SetAlignmentRight();
	void SetAlignmentLeft();
	void SetWindowText(LPCTSTR lpszString);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:
	COLORREF m_crTextColor;
	CString m_sFont;
	CString m_sText;
	int m_iHeight;
	int m_iBold;
	bool m_bItalic;
	bool m_bAlignmentCenter;
	bool m_bAlignmentRight;
	bool m_bAlignmentLeft;


protected:
	DECLARE_MESSAGE_MAP()


};


