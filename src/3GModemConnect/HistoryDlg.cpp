// HistoryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "3GModemConnect.h"
#include "HistoryDlg.h"


// CHistoryDlg dialog

IMPLEMENT_DYNAMIC(CHistoryDlg, CDialog)

CHistoryDlg::CHistoryDlg(CWnd* pParent /*=NULL*/)
: CDialog(CHistoryDlg::IDD, pParent)
{

}

CHistoryDlg::~CHistoryDlg()
{
}

void CHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_dtStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_dtEnd);
	DDX_Control(pDX, IDC_STATIC_HISTORY_DOWNLOADED, m_stcDownloaded);
	DDX_Control(pDX, IDC_STATIC_HISTORY_UPLOADED, m_stcUploaded);
	DDX_Control(pDX, IDC_LIST_HISTORY, m_lstctrHistory);
}


BEGIN_MESSAGE_MAP(CHistoryDlg, CDialog)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_START, &CHistoryDlg::OnDtnDatetimechangeDatetimepickerStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_END, &CHistoryDlg::OnDtnDatetimechangeDatetimepickerEnd)
END_MESSAGE_MAP()


// CHistoryDlg message handlers

BOOL CHistoryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_lstctrHistory.InsertColumn(0, L"Date", LVCFMT_LEFT, 100);
	m_lstctrHistory.InsertColumn(1, L"Start Time", LVCFMT_LEFT, 80);
	m_lstctrHistory.InsertColumn(2, L"End Time", LVCFMT_LEFT, 80);
	m_lstctrHistory.InsertColumn(3, L"Downloaded", LVCFMT_LEFT, 80);
	m_lstctrHistory.InsertColumn(4, L"Uploaded", LVCFMT_LEFT, 80);


	SYSTEMTIME stStart, stEnd;

	m_dtStart.GetTime(&stStart);
	stStart.wDay = 1; //Move to beginning of the month
	m_dtStart.SetTime(&stStart);

	m_dtEnd.GetTime(&stEnd);

	FillHistoryList(stStart, stEnd);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CHistoryDlg::FillHistoryList(SYSTEMTIME stStart, SYSTEMTIME stEnd)
{
	int iReturnStart, iReturnEnd;
	float iTotalDownloaded = 0;
	float iTotalUploaded = 0;


	SYSTEMTIME stStartReturn, stEndReturn;
	int iDownloaded, iUploaded;

	m_lstctrHistory.DeleteAllItems();
	m_stcDownloaded.SetWindowText(L"Total Downloaded : 0");
	m_stcUploaded.SetWindowText(L"Total Uploaded : 0");

	CHistory::Instance()->GetIndexForTimeRange(stStart, stEnd, iReturnStart, iReturnEnd);

	if (iReturnStart < 0 || iReturnEnd < 0 )
	{
		return;
	}

	for (int iItr = iReturnStart; iItr <= iReturnEnd; iItr++)
	{
		CHistory::Instance()->GetItemAt(iItr, stStartReturn, stEndReturn, iDownloaded, iUploaded);
		CString sData;
		sData.Format(L"%02d/%02d/%04d", 
			stStartReturn.wDay,
			stStartReturn.wMonth,
			stStartReturn.wYear);

		int Index = m_lstctrHistory.InsertItem(LVIF_TEXT, 0, sData, 0, 0, 0, NULL);

		sData.Format(L"%02d:%02d:%02d",
			stStartReturn.wHour,
			stStartReturn.wMinute,
			stStartReturn.wSecond);

		m_lstctrHistory.SetItem(Index, 1, LVIF_TEXT, sData, 0, 0, 0, NULL);

		sData.Format(L"%02d:%02d:%02d",
			stEndReturn.wHour,
			stEndReturn.wMinute,
			stEndReturn.wSecond);

		m_lstctrHistory.SetItem(Index, 2, LVIF_TEXT, sData, 0, 0, 0, NULL);

		sData.Format(L"%.2f MB", (float)iDownloaded / 1024 / 1024);
		m_lstctrHistory.SetItem(Index, 3, LVIF_TEXT, sData, 0, 0, 0, NULL);


		sData.Format(L"%.2f MB", (float)iUploaded / 1024 / 1024);
		m_lstctrHistory.SetItem(Index, 4, LVIF_TEXT, sData, 0, 0, 0, NULL);

		iTotalDownloaded += iDownloaded;
		iTotalUploaded += iUploaded;
	}
	CString sTxt;
	sTxt.Format(L"Total Downloaded : %.2f MB", iTotalDownloaded / 1024 / 1024);
	m_stcDownloaded.SetWindowText(sTxt);

	sTxt.Format(L"Total Uploaded : %.2f MB", iTotalUploaded / 1024 / 1024);
	m_stcUploaded.SetWindowText(sTxt);

}
void CHistoryDlg::OnDtnDatetimechangeDatetimepickerStart(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	*pResult = 0;
	UpdateGUI();
}

void CHistoryDlg::OnDtnDatetimechangeDatetimepickerEnd(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	*pResult = 0;
	UpdateGUI();
}

void CHistoryDlg::UpdateGUI()
{

	FILETIME ftStart, ftEnd;
	SYSTEMTIME stStart, stEnd;

	m_dtStart.GetTime(&stStart);
	m_dtEnd.GetTime(&stEnd);

	SystemTimeToFileTime(&stStart, &ftStart);
	SystemTimeToFileTime(&stEnd, &ftEnd);

	int iCompare = CompareFileTime(&ftStart, &ftEnd);

	if (iCompare > 0)
	{
		::MessageBox(NULL, L"Invalid date range", L"Error", MB_OK);
		return;
	}

	FillHistoryList(stStart, stEnd);

}
