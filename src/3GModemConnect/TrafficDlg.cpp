// TrafficDlg.cpp : implementation file
//

#include "stdafx.h"
#include "3GModemConnect.h"
#include "TrafficDlg.h"


// CTrafficDlg dialog

IMPLEMENT_DYNAMIC(CTrafficDlg, CDialog)

CTrafficDlg::CTrafficDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTrafficDlg::IDD, pParent)
{
	m_bTracking = FALSE;
	m_fLastDownloadSpeed = 0;
	m_fLastUploadSpeed = 0;
}

CTrafficDlg::~CTrafficDlg()
{
}

void CTrafficDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TRAFFIC, m_CtrlTraffic);
}


BEGIN_MESSAGE_MAP(CTrafficDlg, CDialog)
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CTrafficDlg message handlers

void CTrafficDlg::AddDownload(float fDownload)
{
	CString sString;
	m_fLastDownloadSpeed = fDownload;
	m_CtrlTraffic.AddDownload((int)fDownload);
	m_CtrlTraffic.SetDownload(fDownload);
	sString.Format(L"DL : %.2f kb/s   UL : %.2f kb/s", m_fLastDownloadSpeed, m_fLastUploadSpeed );
	SetWindowText(sString);
}

void CTrafficDlg::AddUpload(float fUpload)
{
	CString sString;
	m_fLastUploadSpeed = fUpload;
	m_CtrlTraffic.AddUpload((int)fUpload);
	m_CtrlTraffic.SetUpload(fUpload);
	sString.Format(L"DL : %.2f kb/s   UL : %.2f kb/s", m_fLastDownloadSpeed, m_fLastUploadSpeed );
	SetWindowText(sString);
}

BOOL CTrafficDlg::Create(UINT nID, CWnd * pWnd)
{
	return CDialog::Create(nID,pWnd);
}

BOOL CTrafficDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetLayeredWindowAttributes( NULL, 100, LWA_ALPHA);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTrafficDlg::OnMouseLeave()
{
	m_bTracking = FALSE;
	SetLayeredWindowAttributes( NULL, 100, LWA_ALPHA);

	CDialog::OnMouseLeave();
}


void CTrafficDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT trackmouseevent;
		trackmouseevent.cbSize = sizeof(TRACKMOUSEEVENT);
		trackmouseevent.dwFlags =  TME_LEAVE;
		trackmouseevent.hwndTrack = this->m_hWnd;

		if (TrackMouseEvent(&trackmouseevent))
		{
			m_bTracking = TRUE;
			SetLayeredWindowAttributes( NULL, 255, LWA_ALPHA);
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}
