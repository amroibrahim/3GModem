// Traffic.cpp : implementation file
//

#include "stdafx.h"
#include "3GModemConnect.h"
#include "CtrlTraffic.h"

#define BAR_SIZE 1
#define BAR_FRAME_SIZE 0

// CTraffic

IMPLEMENT_DYNAMIC(CCtrlTraffic, CStatic)

CCtrlTraffic::CCtrlTraffic()
{
	m_vDownloads.clear();
	m_vUploads.clear();

	m_fDownloadSpeed = 0;
	m_fUploadSpeed = 0;
}

CCtrlTraffic::~CCtrlTraffic()
{
}


BEGIN_MESSAGE_MAP(CCtrlTraffic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CTraffic message handlers



void CCtrlTraffic::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	int iMax = GetMaxSpeed();
	CRect rect;
	GetClientRect(&rect);

	Graphics graphics(dc);

	//Draw background
	LinearGradientBrush lineargradientbrush(
		Point(rect.left, rect.top), 
		Point(rect.right, rect.bottom),
		Color(255, 65, 120, 165),
		Color(255, 0, 0, 0));

	Pen PenDownload(Color(150, 65, 95, 120));
	Pen PenUpload(Color(255, 135, 135, 135)); //230, 155, 55));
	Pen PenWhite(Color(150, 255, 255, 255)); 


	SolidBrush BrushDownload(Color(150, 65, 95, 120));
	SolidBrush BrushUpload(Color(150, 135, 135, 135)); // 185, 125, 55));
	SolidBrush BrushWhite(Color(150, 255, 255, 255));
	
	Gdiplus::Font fFont(L"Segoe UI", 10);

	PointF pointfDown(0.0f, rect.Height()/2 - 10);
	PointF pointfUp(rect.Width()/2 , rect.Height()/2 - 10);
	//DrawBack ground
	graphics.FillRectangle(&lineargradientbrush, rect.left, rect.top, rect.right, rect.bottom);

	graphics.DrawLine(&PenWhite, rect.left, rect.Height()/2 - 10, (int)rect.right, (int)rect.Height()/2 - 10 );
	graphics.DrawLine(&PenWhite, rect.left, rect.Height()/2 + 10, (int)rect.right, (int)rect.Height()/2 + 10 );
	graphics.DrawLine(&PenWhite, rect.Width()/2, rect.Height()/2 - 10, (int)rect.Width()/2, (int)rect.Height()/2 + 10 );

	CString sDonwload;
	sDonwload.Format(L"Down: %.2f kb/s", m_fDownloadSpeed);

	CString sUpload;
	sUpload.Format(L"Up: %.2f kb/s", m_fUploadSpeed );


	graphics.DrawString(sDonwload, sDonwload.GetLength(), &fFont, pointfDown, &BrushWhite);
	graphics.DrawString(sUpload, sUpload.GetLength(), &fFont, pointfUp, &BrushWhite);

	int iSize = m_vDownloads.size();
	int iNumberOfBars = rect.Width() / BAR_SIZE;

	std::vector<int>::iterator iItrDownload;
	std::vector<int>::iterator iItrUpload;

	iItrDownload = m_vDownloads.begin();
	iItrUpload = m_vUploads.begin(); 

	while(iItrUpload!= m_vUploads.end() && iItrDownload!= m_vDownloads.end())
	{

		int iHightUpload = (*iItrUpload)*(rect.Height()/2 - 10) / iMax;
		int iHightDownload = (*iItrDownload)*(rect.Height()/2 - 10) / iMax;
		if (iHightDownload > iHightUpload)
		{
			graphics.FillRectangle(&BrushDownload, rect.right - BAR_SIZE *(iSize) , (rect.Height()/2-11) - iHightDownload, BAR_SIZE, iHightDownload);		
			graphics.DrawRectangle(&PenDownload, rect.right - BAR_SIZE*(iSize) + BAR_FRAME_SIZE, (rect.Height()/2-11) - iHightDownload + BAR_FRAME_SIZE * 2 , BAR_SIZE - BAR_FRAME_SIZE*2, iHightDownload - BAR_FRAME_SIZE*2);

			graphics.FillRectangle(&BrushUpload, rect.right - BAR_SIZE *(iSize) , (rect.Height()/2+11) , BAR_SIZE, iHightUpload);
			graphics.DrawRectangle(&PenUpload, rect.right - BAR_SIZE*(iSize) + BAR_FRAME_SIZE, (rect.Height()/2+11) + BAR_FRAME_SIZE * 2 , BAR_SIZE - BAR_FRAME_SIZE*2, iHightUpload - BAR_FRAME_SIZE*2);			
		}
		else
		{
			graphics.FillRectangle(&BrushUpload, rect.right - BAR_SIZE *(iSize) , (rect.Height()/2+11), BAR_SIZE, iHightUpload);
			graphics.DrawRectangle(&PenUpload, rect.right - BAR_SIZE*(iSize) + BAR_FRAME_SIZE, (rect.Height()/2+11) + BAR_FRAME_SIZE * 2 , BAR_SIZE - BAR_FRAME_SIZE*2, iHightUpload - BAR_FRAME_SIZE*2);			

			graphics.FillRectangle(&BrushDownload, rect.right - BAR_SIZE *(iSize) , (rect.Height()/2-11) - iHightDownload, BAR_SIZE, iHightDownload);		
			graphics.DrawRectangle(&PenDownload, rect.right - BAR_SIZE*(iSize) + BAR_FRAME_SIZE, (rect.Height()/2-11) - iHightDownload + BAR_FRAME_SIZE * 2 , BAR_SIZE - BAR_FRAME_SIZE*2, iHightDownload - BAR_FRAME_SIZE*2);
		}

		iItrDownload++;
		iItrUpload++;
		iSize--;
	}
}

int CCtrlTraffic::GetMaxSpeed(void)
{
	int iMax = 0;
	std::vector<int>::iterator iItrDownload;
	std::vector<int>::iterator iItrUpload;

	for(iItrDownload = m_vDownloads.begin(); iItrDownload!= m_vDownloads.end(); iItrDownload++)
	{
		if(iMax < *iItrDownload)
		{
			iMax = *iItrDownload;
		}
	}

	for(iItrUpload = m_vUploads.begin(); iItrUpload!= m_vUploads.end(); iItrUpload++)
	{
		if( iMax < *iItrUpload)
		{
			iMax = *iItrUpload;
		}
	}

	if (iMax == 0)
	{
		iMax = 1;
	}
	return iMax;
}


void  CCtrlTraffic::AddDownload(int iDownload)
{
	CRect rect;
	GetClientRect(&rect);
	std::vector<int>::size_type iNumberOfBars = rect.Width() / BAR_SIZE;

	m_vDownloads.push_back(iDownload);

	if(m_vDownloads.size() > iNumberOfBars)
	{
		m_vDownloads.erase(m_vDownloads.begin(), m_vDownloads.begin()+1);
	}

	Invalidate();
}

void  CCtrlTraffic::AddUpload(int iUpload)
{
	CRect rect;
	GetClientRect(&rect);
	std::vector<int>::size_type iNumberOfBars = rect.Width() / BAR_SIZE;

	m_vUploads.push_back(iUpload);

	if(m_vUploads.size() > iNumberOfBars)
	{
		m_vUploads.erase(m_vUploads.begin(), m_vUploads.begin()+1);
	}

	Invalidate();
}

void CCtrlTraffic::SetUpload(float m_fUpload)
{
	m_fUploadSpeed =  m_fUpload;
}


void CCtrlTraffic::SetDownload(float m_fDownload)
{
	m_fDownloadSpeed = m_fDownload;
}
