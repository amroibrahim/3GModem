#pragma once
#include "CtrlTraffic.h"


// CTrafficDlg dialog

class CTrafficDlg : public CDialog
{
	DECLARE_DYNAMIC(CTrafficDlg)

public:
	CTrafficDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTrafficDlg();

	BOOL Create(UINT nID, CWnd * pWnd);

	virtual BOOL OnInitDialog();

	void AddDownload(float fDownload);
	void AddUpload(float fUpload);

	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	

	// Dialog Data
	enum { IDD = IDD_DIALOG_TRAFFIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()


private:
	BOOL m_bTracking;
	CCtrlTraffic m_CtrlTraffic;
	float m_fLastDownloadSpeed;
	float m_fLastUploadSpeed;
	
};
