
// 3GModemConnectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "3GModemConnect.h"
#include "3GModemConnectDlg.h"
#include "HistoryDlg.h"
#include "ModemFactory.h"
#include "Log.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WND_SMALL_SIZE_X 335
#define WND_SMALL_SIZE_Y 120

#define WND_LARGE_SIZE_X 335
#define WND_LARGE_SIZE_Y 475

static UINT BASED_CODE statuspanes[] =
{
	ID_STATUSBAR_CONSTATUS,
	ID_STATUSBAR_MODE,
	ID_STATUSBAR_SIGNAL
};
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMy3GModemConnectDlg dialog




CMy3GModemConnectDlg::CMy3GModemConnectDlg(CWnd* pParent /*=NULL*/)
: CDialog(CMy3GModemConnectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_modem = NULL;
	m_pTrayIcon = NULL;
#ifdef _TRAFFIC_GRAPH
	m_pTrafficDlg = NULL;
#endif
}

void CMy3GModemConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_EXPAND, m_btnExpand);
	DDX_Control(pDX, IDC_COMBO_PROFILES, m_comboProfiles);
	DDX_Control(pDX, IDC_STATIC_LOGIN, m_stcLogin);
	DDX_Control(pDX, IDC_EDIT_USER, m_edtUser);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_edtPassword);
	DDX_Control(pDX, IDC_EDIT_APN, m_edtAPN);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_btnConnect);
	DDX_Control(pDX, IDC_STATIC_DURATION, m_stcDuration);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD, m_stcDownload);
	DDX_Control(pDX, IDC_STATIC_UPLOAD, m_stcUpload);
	DDX_Control(pDX, IDC_STATIC_DOWNLOADED, m_stcDownloaded);
	DDX_Control(pDX, IDC_STATIC_UPLOADED, m_stcUploaded);
	DDX_Control(pDX, IDC_STATIC_SIGNAL, m_stcSignal);
	DDX_Control(pDX, IDC_STATIC_PROVIDER, m_stcProvider);
	DDX_Control(pDX, IDC_BUTTON_APN_APPLY, m_btnAPNApply);
	DDX_Control(pDX, IDC_STATIC_MANUFACTURER, m_stcManufacturer);
	DDX_Control(pDX, IDC_STATIC_MODEL, m_stcModel);
	DDX_Control(pDX, IDC_STATIC_HARDWARE, m_stcHardware);
	DDX_Control(pDX, IDC_STATIC_FIRMWARE, m_stcFirmware);
	DDX_Control(pDX, IDC_STATIC_IMEI, m_stcIMEI);
	DDX_Control(pDX, IDC_STATIC_PORT, m_stcPort);
	DDX_Control(pDX, IDC_COMBO_CON_MODE, m_comboModeType);
	DDX_Control(pDX, IDC_COMBO_CON_BAND, m_comboBandType);
	DDX_Control(pDX, IDC_STATIC_TXT_OPERATOR, m_stcTxtOperator);
	DDX_Control(pDX, IDC_STATIC_TXT_DOWNLOAD, m_stcTxtDownload);
	DDX_Control(pDX, IDC_STATIC_TXT_UPLOAD, m_stcTxtUpload);
	DDX_Control(pDX, IDC_STATIC_TXT_DURATION, m_stcTxtDuration);
	DDX_Control(pDX, IDC_STATIC_TXT_DOWNLOADED, m_stcTxtDownloaded);
	DDX_Control(pDX, IDC_STATIC_TXT_UPLOADED, m_stcTxtUploaded);
	DDX_Control(pDX, IDC_STATIC_TXT_PROFILE, m_stcTxtProfile);
	DDX_Control(pDX, IDC_STATIC_TXT_USER, m_stcTxtUserName);
	DDX_Control(pDX, IDC_STATIC_TXT_PASSWORD, m_stcTxtPassword);
	DDX_Control(pDX, IDC_STATIC_TXT_MODE, m_stcTxtMode);
	DDX_Control(pDX, IDC_STATIC_TXT_APN, m_stcTxtAPN);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_stcStatus);
	DDX_Control(pDX, IDC_STATIC_MODE, m_stcMode);
	DDX_Control(pDX, IDC_STATIC_TXT_PORT, m_stcTxtPort);
	DDX_Control(pDX, IDC_STATIC_TXT_MANUFACTURER, m_stcTxtManufacturer);
	DDX_Control(pDX, IDC_STATIC_TXT_MODEL, m_stcTxtModel);
	DDX_Control(pDX, IDC_STATIC_TXT_FIRMWARE, m_stcTxtFirmware);
	DDX_Control(pDX, IDC_STATIC_TXT_IMEI, m_stcTxtIMEI);
	DDX_Control(pDX, IDC_STATIC_TXT_HARDWARE, m_stcTxtHardware);
	DDX_Control(pDX, IDC_STATIC_PERCENTAGE, m_stcSignalPercentage);
}

BEGIN_MESSAGE_MAP(CMy3GModemConnectDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_EXPAND, &CMy3GModemConnectDlg::OnBnClickedButtonExpand)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CMy3GModemConnectDlg::OnBnClickedButtonConnect)
	ON_MESSAGE(WM_DIAL_EVNT, &CMy3GModemConnectDlg::OnRasDialEvent)
	ON_MESSAGE(WM_MODEM_READ_EVNT, &CMy3GModemConnectDlg::OnModemReadEvent)
	ON_MESSAGE(WM_TRAY_EVNT, &CMy3GModemConnectDlg::OnTrayEvent)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_APPLY_SIGNAL_MODE, &CMy3GModemConnectDlg::OnBnClickedButtonApplySignalMode)
	ON_BN_CLICKED(IDC_BUTTON_APN_APPLY, &CMy3GModemConnectDlg::OnBnClickedButtonApnApply)
	ON_WM_SIZE()
	ON_COMMAND(ID_TRAY_SHOW, &CMy3GModemConnectDlg::OnTrayShow)
	ON_COMMAND(ID_TRAY_HISTORY, &CMy3GModemConnectDlg::OnTrayHistory)
	ON_COMMAND(ID_TRAY_HELP, &CMy3GModemConnectDlg::OnTrayHelp)
	ON_COMMAND(ID_TRAY_EXIT, &CMy3GModemConnectDlg::OnTrayExit)
	ON_COMMAND(ID_TRAY_TRAFFIC, &CMy3GModemConnectDlg::OnTrayTraffic)
	ON_WM_TIMER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CMy3GModemConnectDlg message handlers

BOOL CMy3GModemConnectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_bTracking = FALSE;

	SetTimer(1, 5000, 0);

	m_stcTxtOperator.SetTextColor(RGB(0, 100,  0));
	m_stcTxtOperator.SetWindowText(L"Operator:");

	m_stcProvider.SetHeight(30);
	m_stcProvider.SetBold(true);
	m_stcProvider.SetWindowText(L"NA");

	m_stcTxtDownload.SetAlignmentRight();
	m_stcTxtDownload.SetWindowText(L"Download:");
	
	m_stcDownload.SetHeight(30);
	m_stcDownload.SetAlignmentRight();
	m_stcDownload.SetBold(true);
	m_stcDownload.SetTextColor(RGB(0, 128,  0));
	m_stcDownload.SetWindowText(L"0");

	m_stcTxtUpload.SetAlignmentRight();
	m_stcTxtUpload.SetWindowText(L"Upload");

	m_stcUpload.SetHeight(30);
	m_stcUpload.SetAlignmentRight();
	m_stcUpload.SetBold(true);
	m_stcUpload.SetTextColor(RGB(15, 15,  120));
	m_stcUpload.SetWindowText(L"0");

	m_stcDownloaded.SetWindowText(L"0 KB");

	m_stcUploaded.SetWindowText(L"0 KB");

	m_stcStatus.SetTextColor(RGB(19, 19, 57));
	m_stcStatus.SetAlignmentRight();
	m_stcStatus.SetBold(true);
	m_stcStatus.SetWindowText(L"Disconnected");

	m_stcTxtDownloaded.SetTextColor(RGB(0, 128,  0));
	m_stcTxtDownloaded.SetWindowText(L"Downloaded:");

	m_stcTxtUploaded.SetTextColor(RGB(0, 128,  0));
	m_stcTxtUploaded.SetWindowText(L"Uploaded:");

	m_stcTxtProfile.SetTextColor(RGB(0, 128,  0));
	m_stcTxtProfile.SetWindowText(L"Profile:");

	m_stcTxtUserName.SetTextColor(RGB(0, 128,  0));
	m_stcTxtUserName.SetWindowText(L"User Name:");

	m_stcTxtPassword.SetTextColor(RGB(0, 128,  0));
	m_stcTxtPassword.SetWindowText(L"Password:");

	m_stcTxtMode.SetTextColor(RGB(0, 128,  0));
	m_stcTxtMode.SetWindowText(L"Mode:");

	m_stcTxtAPN.SetTextColor(RGB(0, 128,  0));
	m_stcTxtAPN.SetWindowText(L"APN:");
	//m_stcDuration.SetBold(true);

	//m_stcTxtDuration.SetBold(true);

	m_stcTxtPort.SetTextColor(RGB(0, 128,  0));
	m_stcTxtPort.SetWindowText(L"Port:");

	m_stcTxtManufacturer.SetTextColor(RGB(0, 128,  0));
	m_stcTxtManufacturer.SetWindowText(L"Manufacturer:");

	m_stcTxtModel.SetTextColor(RGB(0, 128,  0));
	m_stcTxtModel.SetWindowText(L"Model:");

	m_stcTxtFirmware.SetTextColor(RGB(0, 128,  0));
	m_stcTxtFirmware.SetWindowText(L"Firmware:");

	m_stcTxtIMEI.SetTextColor(RGB(0, 128,  0));
	m_stcTxtIMEI.SetWindowText(L"IMEI:");

	m_stcTxtHardware.SetTextColor(RGB(0, 128,  0));
	m_stcTxtHardware.SetWindowText(L"Hardware:");


#ifdef _LOG
	CLog::Instance()->LogToFile(L"Info : Initializing main dialog\n");
#endif
	//CreateStatusBar();
	SetDlgSizeToLarge();

#ifdef _TRAFFIC_GRAPH
	    //Traffic Modeless Dialog
		if (!m_pTrafficDlg)
		{
			m_pTrafficDlg = new CTrafficDlg;
		}

		if (!::IsWindow(m_pTrafficDlg->GetSafeHwnd()))
		{
			m_pTrafficDlg->Create(IDD_DIALOG_TRAFFIC, GetDesktopWindow());
		}
#endif
		CRect rectDialer,rectTrafic, rectTaskbar; 
		int iScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
		int iScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
		
		CWnd *pWndTaskbar = CWnd::FindWindow(L"Shell_TrayWnd", NULL);

		pWndTaskbar->GetWindowRect(rectTaskbar);

		GetWindowRect(&rectDialer);

		SetWindowPos(NULL, iScreenWidth - rectDialer.Width() - 5, iScreenHeight - rectDialer.Height() - rectTaskbar.Height() - 5, 0,0, SWP_NOZORDER | SWP_NOSIZE );


#ifdef _TRAFFIC_GRAPH
		m_pTrafficDlg->GetWindowRect(&rectTrafic);
		m_pTrafficDlg->SetWindowPos(NULL, iScreenWidth - rectTrafic.Width(), iScreenHeight - rectTrafic.Height() - rectTaskbar.Height(), 0,0, SWP_NOZORDER | SWP_NOSIZE );
		//TODO: Needs performance improvements 
		m_pTrafficDlg->ShowWindow(SW_SHOW); 

#endif


		m_pTrayIcon = new CTrayIcon();
		m_pTrayIcon->Create(this);



		CModemFactory modemfactory;
		//TODO: FIX MEMORY LEAK
		m_modem = modemfactory.SearchForModem();
		if(m_modem == NULL)
		{
			MessageBox(L"No Modem Found!!", L"Error", MB_OK);
			return TRUE;
		}
		m_stcPort.SetWindowText(m_modem->GetPort());

		if(m_modem->Initialize(this))
		{
			//m_modem->CheckIfAnyExistingConnection();
			//Need to fill the profile box
			m_comboProfiles.ResetContent();
			FillComboWithProfiles();

		}
		else
		{
			//Something is wrong
			//Maybe Should show the create profile dialog 
		}

		return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMy3GModemConnectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy3GModemConnectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy3GModemConnectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*
void CMy3GModemConnectDlg::CreateStatusBar(void)
{
	m_statusbar.Create(this);
	m_statusbar.SetIndicators(statuspanes,3);
	CRect rect;
	GetClientRect(&rect);
	m_statusbar.SetPaneInfo(0,ID_STATUSBAR_CONSTATUS, SBPS_NORMAL, rect.Width()-100);      
	m_statusbar.SetPaneInfo(1,ID_STATUSBAR_MODE, SBPS_STRETCH, 0);
	m_statusbar.SetPaneInfo(2,ID_STATUSBAR_SIGNAL, SBPS_STRETCH, 0);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST,ID_STATUSBAR_SIGNAL);

}
*/
void CMy3GModemConnectDlg::SetDlgSizeToSmall(void)
{
	SetWindowPos(NULL, 0, 0, WND_SMALL_SIZE_X, WND_SMALL_SIZE_Y, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

/*
	CRect rect;
	GetClientRect(&rect);
	m_statusbar.SetPaneInfo(0,ID_STATUSBAR_CONSTATUS, SBPS_NORMAL, rect.Width()-100);      
	m_statusbar.SetPaneInfo(1,ID_STATUSBAR_MODE, SBPS_STRETCH, 0);
	m_statusbar.SetPaneInfo(2,ID_STATUSBAR_SIGNAL, SBPS_STRETCH, 0);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST,ID_STATUSBAR_SIGNAL);
*/
	HideLogin();

	m_btnExpand.SetWindowText(L"More");
}

void CMy3GModemConnectDlg::OnBnClickedButtonExpand()
{
	CString sbtnTxt;
	m_btnExpand.GetWindowText(sbtnTxt);
	if(sbtnTxt == "Less")
	{
		SetDlgSizeToSmall();
	}
	else
	{
		SetDlgSizeToLarge();
	}
}

void CMy3GModemConnectDlg::SetDlgSizeToLarge(void)
{
	SetWindowPos(NULL, 0, 0, WND_LARGE_SIZE_X, WND_LARGE_SIZE_Y, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

/*
	CRect rect;
	GetClientRect(&rect);
	m_statusbar.SetPaneInfo(0,ID_STATUSBAR_CONSTATUS, SBPS_NORMAL, rect.Width()-100);      
	m_statusbar.SetPaneInfo(1,ID_STATUSBAR_MODE, SBPS_STRETCH, 0);
	m_statusbar.SetPaneInfo(2,ID_STATUSBAR_SIGNAL, SBPS_STRETCH, 0);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST,ID_STATUSBAR_SIGNAL);
*/
	ShowLogin();

	m_btnExpand.SetWindowText(L"▲");
}

void CMy3GModemConnectDlg::OnBnClickedButtonConnect()
{
	if(m_modem->isConnected())
	{
#ifdef _LOG
		CLog::Instance()->LogToFile(L"Info : Disconnecting\n");
#endif
		m_modem->HangUp();
		return;
	}

	CString sEntry, sUser, sPassword;

	//TODO: m_modem->SetAPN
	m_comboProfiles.GetWindowText(sEntry);
	m_edtUser.GetWindowText(sUser);
	m_edtPassword.GetWindowText(sPassword);

	if(!m_modem->isConnected() && !m_modem->isDialing())
	{
#ifdef _LOG
		CLog::Instance()->LogToFile(L"Info : Dialing\n");
#endif

		if(m_modem->Dial(sEntry, sUser, sPassword))
		{
			m_btnConnect.EnableWindow(FALSE);
		}
		else
		{
#ifdef _LOG
			CLog::Instance()->LogToFile(L"Error : Dialing Failed\n");
#endif
		}

	}

}

void CMy3GModemConnectDlg::OnDestroy()
{
#ifdef _LOG
	CLog::Instance()->LogToFile(L"Info : Application Shutting down\n");
#endif
	CDialog::OnDestroy();

#ifdef _TRAFFIC_GRAPH
	if (m_pTrafficDlg)
	{
		if (::IsWindow(m_pTrafficDlg->GetSafeHwnd()))
		{
			m_pTrafficDlg->EndDialog(IDCANCEL);
		}
		m_pTrafficDlg->DestroyWindow();
		delete m_pTrafficDlg;
	}
#endif

	if (m_pTrayIcon)
	{
		m_pTrayIcon->DestroyWindow();
		delete m_pTrayIcon; 
	}

	if(m_modem != NULL)
	{
		delete m_modem;
	}

#ifdef _LOG
	CLog::ShutDown();
#endif 

	CHistory::ShutDown();
}

void CMy3GModemConnectDlg::FillComboWithProfiles(void)
{
	int iCount = m_modem->GetEntriesCount();
	for(int i = 0; i < iCount; i++)
	{
		m_comboProfiles.InsertString(i, m_modem->GetEntry(i));
	}

	//I should select the default value for the combo box
	if(iCount > 0)
	{
		m_comboProfiles.SetCurSel(0);
	}
}

void CMy3GModemConnectDlg::HideLogin(void)
{
	m_stcLogin.ShowWindow(SW_HIDE);

	m_stcTxtUserName.ShowWindow(SW_HIDE);
	m_stcTxtPassword.ShowWindow(SW_HIDE);
	m_stcTxtAPN.ShowWindow(SW_HIDE);

	m_edtUser.ShowWindow(SW_HIDE);
	m_edtPassword.ShowWindow(SW_HIDE);
	m_edtAPN.ShowWindow(SW_HIDE);

}

void CMy3GModemConnectDlg::ShowLogin(void)
{
	m_stcLogin.ShowWindow(SW_SHOW);

	m_stcTxtUserName.ShowWindow(SW_SHOW);
	m_stcTxtPassword.ShowWindow(SW_SHOW);
	m_stcTxtAPN.ShowWindow(SW_SHOW);

	m_edtUser.ShowWindow(SW_SHOW);
	m_edtPassword.ShowWindow(SW_SHOW);
	m_edtAPN.ShowWindow(SW_SHOW);
}

LRESULT CMy3GModemConnectDlg::OnModemReadEvent(WPARAM wparam, LPARAM lparam)
{

	switch(wparam)
	{
	case CModem::eMODEM_EVENT_DURATION: 
		SetDuration((int)lparam);
		break;

	case CModem::eMODEM_EVENT_DOWNLOAD_SPEED:
		SetDownloadSpeed((int)lparam);
		break;

	case CModem::eMODEM_EVENT_UPLOAD_SPEED:
		SetUploadSpeed((int)lparam);
		break;

	case CModem::eMODEM_EVENT_UPLOADED_DATA:
		SetUploadedData((int)lparam);
		break;

	case CModem::eMODEM_EVENT_DOWNLOADED_DATA:
		SetDownloadedData((int)lparam);
		break;

	case CModem::eMODEM_EVENT_SYS_MODE:
		SetMode((int)lparam);
		break;

	case CModem::eMODEM_EVENT_RSSI:
		SetRSSI((int)lparam);
		break;

	case CModem::eMODEM_EVENT_PROVIDER:
		SetProvider((char*)lparam);
		break;

	case CModem::eMODEM_EVENT_APN:
		SetAPN((char*)lparam);
		break;

	case CModem::eMODEM_EVENT_MANUFACTURER:
		SetManufacturer((char*)lparam);
		break;

	case CModem::eMODEM_EVENT_PORT:
		SetPort((char*)lparam);
		break;

	case CModem::eMODEM_EVENT_MODEL:
		SetModel((char*)lparam);
		break;

	case CModem::eMODEM_EVENT_IMEI:
		SetIMEI((char*)lparam);
		break;

	case CModem::eMODEM_EVENT_FIRMWARE:
		SetFirmware((char*)lparam);
		break;

	case CModem::eMODEM_EVENT_HARDWARE:
		SetHardware((char*)lparam);
		break;

	case CModem::eMODEM_EVENT_MODEM_MODE:
		SetModemMode((int)lparam);
		break;

	case CModem::eMODEM_EVENT_MODEM_BAND:
		SetBandMode((int)lparam);
		break;
	}

	return 0;
}

LRESULT CMy3GModemConnectDlg::OnRasDialEvent (WPARAM wparam, LPARAM lparam)
{

	RASCONNSTATE rasconnstate;
	DWORD dwError;

	rasconnstate = (RASCONNSTATE) wparam; 
	dwError = (DWORD) lparam;

	switch(rasconnstate)
	{
	case RASCS_OpenPort:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_OpenPort\n");
#endif
		//m_statusbar.SetPaneText(0, L"Opening port");
		m_stcStatus.SetWindowText(L"Opening port");
		break;

	case RASCS_PortOpened:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_PortOpened\n");
#endif
		//m_statusbar.SetPaneText(0, L"Port opened");
		m_stcStatus.SetWindowText(L"Port opened");
		break;

	case RASCS_ConnectDevice:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_ConnectDevice\n");
#endif
		//m_statusbar.SetPaneText(0, L"Device is about to be connected");
		m_stcStatus.SetWindowText(L"Connecting");
		break;

	case RASCS_DeviceConnected:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_DeviceConnected\n");
#endif
		//m_statusbar.SetPaneText(0, L"Device connected");
		m_stcStatus.SetWindowText(L"Device connected");
		break;

	case RASCS_AllDevicesConnected:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_AllDevicesConnected\n");
#endif
		//m_statusbar.SetPaneText(0, L"Link established");
		m_stcStatus.SetWindowText(L"Link established");
		break;

	case RASCS_Authenticate:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_Authenticate\n");
#endif
		//m_statusbar.SetPaneText(0, L"Starting authentication");
		m_stcStatus.SetWindowText(L"Authenticating");
		break;

	case RASCS_AuthNotify: 
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_AuthNotify\n");
#endif
		//m_statusbar.SetPaneText(0, L"Authenticated");
		m_stcStatus.SetWindowText(L"Authenticated");
		break;

	case RASCS_AuthRetry:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_AuthRetry\n");
#endif
		//m_statusbar.SetPaneText(0, L"Retry authentication");
		m_stcStatus.SetWindowText(L"Retry authentication");
		break;

	case RASCS_AuthCallback:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_AuthCallback\n");
#endif
		//m_statusbar.SetPaneText(0, L"RASCS_AuthCallback");
		break;

	case RASCS_AuthChangePassword:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_AuthChangePassword\n");
#endif
		//m_statusbar.SetPaneText(0, L"RASCS_AuthChangePassword");
		break;

	case RASCS_AuthProject:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_AuthProject\n"); 
#endif
		//m_statusbar.SetPaneText(0, L"Projection starting");
		break;

	case RASCS_AuthLinkSpeed:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_AuthLinkSpeed\n");
#endif
		//m_statusbar.SetPaneText(0, L"Link-speed calculation starting");
		m_stcStatus.SetWindowText(L"Link-speed calculation starting");
		break;

	case RASCS_AuthAck:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_AuthAck\n");
#endif
		//m_statusbar.SetPaneText(0, L"Authentication acknowledged.");
		m_stcStatus.SetWindowText(L"Authentication acknowledged.");
		break;

	case RASCS_ReAuthenticate:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_ReAuthenticate\n");
#endif
		//m_statusbar.SetPaneText(0, L"RASCS_ReAuthenticate");
		break;

	case RASCS_Authenticated:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_Authenticated\n");
#endif
		//m_statusbar.SetPaneText(0, L"Authenticated");
		m_stcStatus.SetWindowText(L"Authenticated");
		break;

	case RASCS_PrepareForCallback:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_PrepareForCallback\n");
#endif
		//m_statusbar.SetPaneText(0, L"RASCS_PrepareForCallback");
		break;

	case RASCS_WaitForModemReset:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_WaitForModemReset\n");
#endif
		//m_statusbar.SetPaneText(0, L"RASCS_WaitForModemReset");
		break;

	case RASCS_WaitForCallback:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_WaitForCallback\n");
#endif
		//m_statusbar.SetPaneText(0, L"RASCS_WaitForCallback");
		break;

	case RASCS_Projected:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_Projected\n");
#endif
		//m_statusbar.SetPaneText(0, L"Projected");
		break;

	case RASCS_Interactive:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_Interactive\n");
#endif
		//m_statusbar.SetPaneText(0, L"RASCS_Interactive");
		break;

	case RASCS_RetryAuthentication:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_RetryAuthentication\n");
#endif
		//m_statusbar.SetPaneText(0, L"Retry authentication");
		m_stcStatus.SetWindowText(L"Retry authentication");
		break;

	case RASCS_CallbackSetByCaller:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_CallbackSetByCaller\n");
#endif
		//m_statusbar.SetPaneText(0, L"RASCS_CallbackSetByCaller");
		break;

	case RASCS_PasswordExpired:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_PasswordExpired\n");
#endif
		//m_statusbar.SetPaneText(0, L"RASCS_PasswordExpired");
		break;

	case RASCS_Connected:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_Connected\n"); 
#endif
		m_btnConnect.SetWindowText(L"Disconnect");
		//m_statusbar.SetPaneText(0, L"Connected");
		m_stcStatus.SetWindowText(L"Connected");
		m_btnConnect.EnableWindow(TRUE);
		GetSystemTime(&m_stStart);
		m_iTotalDownloaded = 0;
		m_iTotalUploaded = 0;
		break;

	case RASCS_Disconnected:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_Disconnected\n");
#endif
		m_btnConnect.SetWindowText(L"Connect");
		//m_statusbar.SetPaneText(0, L"Disconnected");
		m_stcStatus.SetWindowText(L"Disconnected");
		m_btnConnect.EnableWindow(TRUE);
		GetSystemTime(&m_stEnd);
		CHistory::Instance()->Add(m_stStart, m_stEnd, m_iTotalDownloaded, m_iTotalUploaded);
		break;

	default:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Warning : RAS default\n"); 
#endif
		//m_statusbar.SetPaneText(0, L"default");
		break;
	}

	if (dwError)
	{
		//Disconnect and show Error message to user
		//Some error
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Error : Dial event Error\n");
#endif
		m_btnConnect.SetWindowText(L"Connect");
		//m_statusbar.SetPaneText(0, L"Disconnected");
		m_stcStatus.SetWindowText(L"Disconnected");
		m_btnConnect.EnableWindow(TRUE);
		GetSystemTime(&m_stEnd);
		CHistory::Instance()->Add(m_stStart, m_stEnd, m_iTotalDownloaded, m_iTotalUploaded);
		
		ShowDialError(dwError);
	}
	return 0;

}

void  CMy3GModemConnectDlg::ShowDialError(DWORD dwError)
{
	CString sMsg;
	m_modem->GetDialErrorMsg(dwError, sMsg);
#ifdef _LOG
	CLog::Instance()->LogToFile(L"Error : ");
	CLog::Instance()->LogToFile(sMsg);
	CLog::Instance()->LogToFile(L"\n");
#endif
	::MessageBox(NULL, sMsg, L"Error", MB_OK);
	return;
}


LRESULT CMy3GModemConnectDlg::OnTrayEvent(WPARAM wparam, LPARAM lparam)
{
	switch(lparam)
	{
	case WM_LBUTTONDBLCLK:
		OnTrayShow();
		break;

	case WM_RBUTTONDOWN:
		m_pTrayIcon->ShowMenu();
		break;
	}
	return 0;
}

void CMy3GModemConnectDlg::SetDuration(int iTimeInSec)
{
	CString sString;
	int iHours=iTimeInSec /3600;
	iTimeInSec =iTimeInSec %3600;
	int iMin=iTimeInSec /60;
	iTimeInSec =iTimeInSec %60;
	int iSec =iTimeInSec;
	sString.Format(L"%02d:%02d:%02d", iHours,iMin,iSec);
	m_stcDuration.SetWindowText((LPCTSTR)sString);

}

void CMy3GModemConnectDlg::SetDownloadSpeed(int iSpeedInBits)
{

	float iDownloadSpeed = (float)iSpeedInBits;
	CString sString;
	if(iDownloadSpeed >= 0)
	{
		iDownloadSpeed  = iDownloadSpeed / 1024;
		sString.Format(L"%.2f kb/s", iDownloadSpeed );
	}
	else
	{
		sString = "0 kb/s";
	}
	m_stcDownload.SetWindowText((LPCTSTR)sString);
#ifdef _TRAFFIC_GRAPH
	m_pTrafficDlg->AddDownload(iDownloadSpeed);
#endif
}

void CMy3GModemConnectDlg::SetUploadSpeed(int iSpeedInBits)
{
	float iUploadSpeed = (float)iSpeedInBits;
	CString sString;
	if(iUploadSpeed  >= 0)
	{
		iUploadSpeed   = iUploadSpeed  / 1024;
		sString.Format(L"%.2f kb/s", iUploadSpeed  );
	}
	else
	{
		sString = "0 kb/s";
	}

	m_stcUpload.SetWindowText((LPCTSTR)sString);
#ifdef _TRAFFIC_GRAPH
	m_pTrafficDlg->AddUpload(iUploadSpeed);
#endif
}

void CMy3GModemConnectDlg::SetUploadedData(int iSizeInBits)
{
	CString sString;
	float iDataUploaded = (float)iSizeInBits; //Hex to decimal
	if(iDataUploaded >= 0)
	{
		m_iTotalUploaded = (int)iDataUploaded;
		iDataUploaded = iDataUploaded / 1024 / 1024;
		sString.Format(L"%.2f MB", iDataUploaded );
		m_stcUploaded.SetWindowText((LPCTSTR)sString);
	}
}
void CMy3GModemConnectDlg::SetProvider(char* szProvider)
{
	CString sString;
	sString = szProvider;
	sString.Remove('\"');
	sString.Trim();
	m_stcProvider.SetWindowText((LPCTSTR)sString);
}

void CMy3GModemConnectDlg::SetManufacturer(char* szManufacturer)
{
	CString sString;
	sString = szManufacturer;
	sString.Trim();
	m_stcManufacturer.SetWindowText((LPCTSTR)sString);
}

void CMy3GModemConnectDlg::SetModel(char* szModel)
{
	CString sString;
	sString = szModel;
	sString.Trim();
	m_stcModel.SetWindowText((LPCTSTR)sString);

}

void CMy3GModemConnectDlg::SetPort(char* szPort)
{
	CString sString;
	sString = szPort;
	sString.Trim();
	m_stcPort.SetWindowText((LPCTSTR)sString);

}

void CMy3GModemConnectDlg::SetIMEI(char* szIMEI)
{
	CString sString;
	sString = szIMEI;
	sString.Trim();
	m_stcIMEI.SetWindowText((LPCTSTR)sString);

}

void CMy3GModemConnectDlg::SetFirmware(char* szFirmware)
{
	CString sString;
	sString = szFirmware;
	sString.Trim();
	m_stcFirmware.SetWindowText((LPCTSTR)sString);

}

void CMy3GModemConnectDlg::SetHardware(char* szHardware)
{
	CString sString;
	sString = szHardware;
	sString.Remove('\"');
	sString.Trim();
	m_stcHardware.SetWindowText((LPCTSTR)sString);

}

void CMy3GModemConnectDlg::SetAPN(char* szAPN)
{
	CString sString;
	sString = szAPN;
	sString.Remove('\"');
	sString.Trim();
	m_edtAPN.SetWindowText((LPCTSTR)sString);
}

void CMy3GModemConnectDlg::SetRSSI(int iRSSI)
{
	if(iRSSI <= 31 && iRSSI >= 26 )
	{
		m_stcSignal.SetWindowText(L"Excellent");
	}
	else if(iRSSI <= 25 && iRSSI >= 20 )
	{
		m_stcSignal.SetWindowText(L"Good");
	}
	else if(iRSSI <= 19 && iRSSI >= 14 )
	{
		m_stcSignal.SetWindowText(L"Fair");
	}
	else if(iRSSI <= 13 && iRSSI >= 8 )
	{
		m_stcSignal.SetWindowText(L"Poor");
	}
	else if(iRSSI <= 7 && iRSSI >= 2 )
	{
		m_stcSignal.SetWindowText(L"Very Poor");
	}
	else
	{
		m_stcSignal.SetWindowText(L"No Signal");
	}
	CString sString;
	int iVal = iRSSI * 100 / 32;
	m_pTrayIcon->SetSignalStrength(iVal);
	sString.Format(L"%d%%", iVal);
	m_stcSignalPercentage.SetWindowText(sString);
	//m_statusbar.SetPaneText(2, (LPCTSTR)sString);
	//TODO: Not Compleate
}

void CMy3GModemConnectDlg::SetMode(int iMode)
{
	CString sString;
	switch(iMode)
	{
	case CModem::eMODEM_MODE_NA:
		sString = "NA";
		break;
	case CModem::eMODEM_MODE_GSM:
		sString = "GSM";
		break;
	case CModem::eMODEM_MODE_GPRS:
		sString = "GPRS";
		break;
	case CModem::eMODEM_MODE_EDEG:
		sString = "EDEG";
		break;
	case CModem::eMODEM_MODE_WCDMA:
		sString = "WCDMA";
		break;
	case CModem::eMODEM_MODE_HSDPA:
		sString = "HSDPA";
		break;
	case CModem::eMODEM_MODE_HSUPA:
		sString = "HSUPA";
		break;
	case CModem::eMODEM_MODE_HSPA:
		sString = "HSPA";
		break;
	default:
		sString = "NA";
		break;
	}
	m_pTrayIcon->SetNetworkMode(iMode);
	m_stcTxtMode.SetWindowText((LPCTSTR)sString);
	//m_statusbar.SetPaneText(1, (LPCTSTR)sString);
	m_stcMode.SetWindowText(sString);
}

void CMy3GModemConnectDlg::SetDownloadedData(int iSizeInBits)
{
	CString sString;
	float iDataDownloaded = (float)iSizeInBits;
	if(iDataDownloaded >=0 )
	{
		m_iTotalDownloaded = (int)iDataDownloaded;
		iDataDownloaded = iDataDownloaded / 1024 / 1024;
		sString.Format(L"%.2f MB", iDataDownloaded );
		m_stcDownloaded.SetWindowText((LPCTSTR)sString);
	}
}

void CMy3GModemConnectDlg::OnClose()
{
	//TODO: DO any cleaning
	//Add if he want to disconnects first
	int iRet = 0;
	if(m_modem->isConnected())
	{
		if(MessageBox(L"Disconnect on exit?", L"Warning", MB_YESNO) == IDYES)
		{
			m_modem->HangUp();
		}
	}

	if(m_modem)
	{
		m_modem->ShutDown();
	}

	CDialog::OnClose();
}

void CMy3GModemConnectDlg::OnBnClickedButtonApplySignalMode()
{
	if(m_modem)
	{
		int iModeType = m_comboModeType.GetCurSel();
		int iBandType = m_comboBandType.GetCurSel();
		CModem::EModemMode eMode = CModem::eMODE_ANY;
		CModem::EModemBand eBand = CModem::eBAND_ANY; 

		switch(iModeType)
		{
		case 0:
			eMode = CModem::eMODE_2G_ONLY;
			break;

		case 1:
			eMode = CModem::eMODE_2G_PREF;
			break;

		case 2:
			eMode = CModem::eMODE_3G_ONLY;
			break;

		case 3:
			eMode = CModem::eMODE_3G_PREF;
			break;
		}

		//Any band;GSM(850MHz);EGSM(900MHz);PGSM(900MHz);DCS(1800MHz);PCS(1900MHz);WCDMA(2100MHz)
		switch(iBandType)
		{
		case 0:
			eBand = CModem::eBAND_GSM;
			break;

		case 1:
			eBand = CModem::eBAND_EGSM;
			break;

		case 2:
			eBand = CModem::eBAND_PGSM;
			break;

		case 3:
			eBand = CModem::eBAND_DCS;
			break;

		case 4:
			eBand = CModem::eBAND_PCS;
			break;

		case 5:
			eBand = CModem::eBAND_WCDMA;
			break;

		case 6:
			eBand = CModem::eBAND_ANY;
			break;
		}

		m_modem->SetMode(eMode, eBand);
	}
}

void CMy3GModemConnectDlg::OnBnClickedButtonApnApply()
{
	CString sAPN;
	m_edtAPN.GetWindowText(sAPN);
	if(!sAPN.IsEmpty() && m_modem)
	{
		m_modem->SetAPN(sAPN);
	}
}

void CMy3GModemConnectDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	WINDOWPLACEMENT windowplacement;
	GetWindowPlacement(&windowplacement);
	if(windowplacement.showCmd == SW_SHOWMINIMIZED)
	{
		ShowWindow(SW_HIDE);
	}
}

void CMy3GModemConnectDlg::SetModemMode(int iMode)
{
	if(m_modem)
	{
		switch(iMode)
		{		
		case CModem::eMODE_2G_ONLY:
			m_comboModeType.SetCurSel(0);
			break;

		case CModem::eMODE_2G_PREF:
			m_comboModeType.SetCurSel(1);
			break;

		case CModem::eMODE_3G_ONLY:
			m_comboModeType.SetCurSel(2);
			break;

		case CModem::eMODE_3G_PREF:
			m_comboModeType.SetCurSel(3);
			break;

		case CModem::eMODE_ANY:
			m_comboModeType.SetCurSel(4);
			break;

		}
	}
}

void CMy3GModemConnectDlg::SetBandMode(int iBand)
{
	if(m_modem)
	{
		//GSM(850MHz);EGSM(900MHz);PGSM(900MHz);DCS(1800MHz);PCS(1900MHz);WCDMA(2100MHz);Any
		switch(iBand)
		{
		case CModem::eBAND_GSM:
			m_comboBandType.SetCurSel(0);
			break;

		case CModem::eBAND_EGSM:
			m_comboBandType.SetCurSel(1);
			break;

		case CModem::eBAND_PGSM:
			m_comboBandType.SetCurSel(2);
			break;

		case CModem::eBAND_DCS:
			m_comboBandType.SetCurSel(3);
			break;

		case CModem::eBAND_PCS:
			m_comboBandType.SetCurSel(4);
			break;

		case CModem::eBAND_WCDMA:
			m_comboBandType.SetCurSel(5);
			break;

		case CModem::eBAND_ANY:
			m_comboBandType.SetCurSel(6);
			break;
		}

	}
}

void CMy3GModemConnectDlg::OnTrayShow()
{
	ShowWindow(SW_SHOW);
	ShowWindow(SW_RESTORE);
}

void CMy3GModemConnectDlg::OnTrayHistory()
{
	CHistoryDlg dlgHistory;
	dlgHistory.DoModal();
}

void CMy3GModemConnectDlg::OnTrayHelp()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CMy3GModemConnectDlg::OnTrayExit()
{
	PostMessage(WM_CLOSE);
}

void CMy3GModemConnectDlg::OnTrayTraffic()
{
#ifdef _TRAFFIC_GRAPH
	m_pTrafficDlg->ShowWindow(SW_SHOW);
#endif
}


void CMy3GModemConnectDlg::OnTimer(UINT_PTR nIDEvent)
{
	ShowWindow(SW_HIDE);
	CDialog::OnTimer(nIDEvent);
	KillTimer(1);
}


void CMy3GModemConnectDlg::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	m_bTracking = FALSE;
	SetTimer(1, 5000, 0);
	CDialog::OnMouseLeave();
}


void CMy3GModemConnectDlg::OnMouseMove(UINT nFlags, CPoint point)
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
			KillTimer(1);
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}
