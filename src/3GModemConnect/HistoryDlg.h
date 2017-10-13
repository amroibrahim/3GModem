#pragma once
#include "afxdtctl.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "History.h"


// CHistoryDlg dialog

class CHistoryDlg : public CDialog
{
	DECLARE_DYNAMIC(CHistoryDlg)

public:
	CHistoryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHistoryDlg();

	// Dialog Data
	enum { IDD = IDD_DIALOG_HISTORY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CDateTimeCtrl m_dtStart;
	CDateTimeCtrl m_dtEnd;
	CStatic m_stcDownloaded;
	CStatic m_stcUploaded;
	afx_msg void OnDtnDatetimechangeDatetimepickerStart(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_lstctrHistory;
	void FillHistoryList(SYSTEMTIME stStart, SYSTEMTIME stEnd);
	afx_msg void OnDtnDatetimechangeDatetimepickerEnd(NMHDR *pNMHDR, LRESULT *pResult);
	void UpdateGUI(void);
};
