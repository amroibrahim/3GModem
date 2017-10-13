
// 3GModemConnectDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "Modem.h"
#include "TrafficDlg.h"
#include "TrayIcon.h"
#include "History.h"
#include "CustomStatic.h"

// CMy3GModemConnectDlg dialog
class CMy3GModemConnectDlg : public CDialog
{
	// Construction
public:
	CMy3GModemConnectDlg(CWnd* pParent = NULL);	// standard constructor
	
	void CreateStatusBar(void);
	void SetDlgSizeToSmall(void);
	void SetDlgSizeToLarge(void);
	void FillComboWithProfiles(void);

	afx_msg LRESULT OnRasDialEvent(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnModemReadEvent(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnTrayEvent(WPARAM wparam, LPARAM lparam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonApplySignalMode();
	afx_msg void OnBnClickedButtonApnApply();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonExpand();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnTrayShow();
	afx_msg void OnTrayHistory();
	afx_msg void OnTrayHelp();
	afx_msg void OnTrayExit();
	afx_msg void OnTrayTraffic();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);


	// Dialog Data
	enum { IDD = IDD_MY3GMODEMCONNECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Implementation
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()


private:
	void SetDuration(int iTimeInSec);
	void SetDownloadSpeed(int iSpeedInBits);
	void SetUploadSpeed(int iSpeedInBits);
	void SetUploadedData(int iSizeInBits);
	void SetDownloadedData(int iSizeInBits);
	void SetModemMode(int iMode);
	void SetBandMode(int iBand);
	void SetMode(int iMode);
	void SetRSSI(int iRSSI);
	void SetProvider(char* szProvider);
	void SetAPN(char* szAPN);
	void SetManufacturer(char* szManufacturer);
	void SetModel(char* szModel);
	void SetPort(char* szPort);
	void SetIMEI(char* szIMEI);
	void SetFirmware(char* szFirmware);
	void SetHardware(char* szHardware);
	void ShowDialError(DWORD dwError);
	void HideLogin(void);
	void ShowLogin(void);

	int m_iTotalDownloaded;
	int m_iTotalUploaded;
	
	BOOL m_bTracking;

	CModem* m_modem;

	CEdit m_edtUser;
	CEdit m_edtPassword;
	CEdit m_edtAPN;

	CStatic m_stcLogin;
	//CStatic m_stcUser;
	//CStatic m_stcPassword;
	//CStatic m_stcAPN;
	//CStatic m_stcMode;
	CStatic m_stcSignal;
	CStatic m_stcManufacturer;
	CStatic m_stcModel;
	CStatic m_stcHardware;
	CStatic m_stcFirmware;
	CStatic m_stcIMEI;
	CStatic m_stcPort;
	CStatic m_stcMode;

	CCustomStatic m_stcProvider;
	CCustomStatic m_stcTxtOperator;
	CCustomStatic m_stcTxtDownload;
	CCustomStatic m_stcTxtUpload;
	CCustomStatic m_stcDownloaded;
	CCustomStatic m_stcUploaded;
	CCustomStatic m_stcDownload;
	CCustomStatic m_stcUpload;
	CCustomStatic m_stcDuration;
	CCustomStatic m_stcTxtDuration;
	CCustomStatic m_stcTxtDownloaded;
	CCustomStatic m_stcTxtUploaded;
	CCustomStatic m_stcTxtProfile;
	CCustomStatic m_stcTxtUserName;
	CCustomStatic m_stcTxtPassword;
	CCustomStatic m_stcTxtMode;
	CCustomStatic m_stcTxtAPN;
	CCustomStatic m_stcStatus;
	CCustomStatic m_stcTxtPort;
	CCustomStatic m_stcTxtManufacturer;
	CCustomStatic m_stcTxtModel;
	CCustomStatic m_stcTxtFirmware;
	CCustomStatic m_stcTxtIMEI;
	CCustomStatic m_stcTxtHardware;

	CButton m_btnConnect;
	CButton m_btnAPNApply;
	CButton m_btnExpand;

	CComboBox m_comboModeType;
	CComboBox m_comboBandType;
	CComboBox m_comboProfiles;

	SYSTEMTIME m_stStart; 
	SYSTEMTIME m_stEnd;

	CTrayIcon *m_pTrayIcon;

	//CStatusBar m_statusbar; 

#ifdef _TRAFFIC_GRAPH
	CTrafficDlg *m_pTrafficDlg;
#endif
public:
	CStatic m_stcSignalPercentage;
};
