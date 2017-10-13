// TrayIcon.cpp : implementation file
//

#include "stdafx.h"
#include "3GModemConnect.h"
#include "TrayIcon.h"


// CTrayIcon

IMPLEMENT_DYNAMIC(CTrayIcon, CWnd)

CTrayIcon::CTrayIcon()
{
	m_iNetworkMode = 4;
	m_iSignalStrength = 0;

	m_hIconNA = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_NA)); 

	m_hIconEDEG1 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_EDEG1)); 
	m_hIconEDEG2 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_EDEG2)); 
	m_hIconEDEG3 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_EDEG3)); 
	m_hIconEDEG4 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_EDEG4)); 
	m_hIconEDEG5 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_EDEG5)); 

	m_hIconWCDMA1 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_WCDMA1)); 
	m_hIconWCDMA2 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_WCDMA2)); 
	m_hIconWCDMA3 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_WCDMA3)); 
	m_hIconWCDMA4 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_WCDMA4)); 
	m_hIconWCDMA5 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_WCDMA5)); 

	m_hIconHSDPA1 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_HSDPA1)); 
	m_hIconHSDPA2 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_HSDPA2)); 
	m_hIconHSDPA3 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_HSDPA3)); 
	m_hIconHSDPA4 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_HSDPA4)); 
	m_hIconHSDPA5 = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ICON_HSDPA5)); 

}

CTrayIcon::~CTrayIcon()
{
	Shell_NotifyIcon(NIM_DELETE, &m_notifyicondata);
}

BEGIN_MESSAGE_MAP(CTrayIcon, CWnd)
END_MESSAGE_MAP()



// CTrayIcon message handlers

BOOL CTrayIcon::Create(CWnd* pParent)
{
	m_pParentWnd = pParent;
	ZeroMemory(&m_notifyicondata, sizeof(NOTIFYICONDATA));
	m_notifyicondata.cbSize = sizeof(m_notifyicondata);
	m_notifyicondata.hWnd = m_pParentWnd->GetSafeHwnd();
	m_notifyicondata.uID = 100;
	m_notifyicondata.uCallbackMessage = WM_TRAY_EVNT;
	m_notifyicondata.uFlags = NIF_ICON | NIF_MESSAGE;
	m_notifyicondata.hIcon = m_hIconNA;
	Shell_NotifyIcon(NIM_ADD, &m_notifyicondata);

	m_TaryMenu.LoadMenu(IDR_MENU_TRAY);

	return TRUE;
}

void CTrayIcon::SetSignalStrength(int iSignalStrength)
{
	m_iSignalStrength = iSignalStrength;
	UpdateTrayIcon();
}

void CTrayIcon::SetNetworkMode(int iNetworkMode)
{
	m_iNetworkMode = iNetworkMode;
	UpdateTrayIcon();
}

void CTrayIcon::UpdateTrayIcon(void)
{
	switch(m_iNetworkMode)
	{
	case 0: //NA
		m_notifyicondata.hIcon = m_hIconNA;
		break;

	case 1: //GSM
	case 2: //GPRS
	case 3: //EDEG
		if (m_iSignalStrength <= 100 && m_iSignalStrength >= 80)
		{
			m_notifyicondata.hIcon = m_hIconEDEG5;
		}
		else if (m_iSignalStrength <= 79 && m_iSignalStrength >= 60)
		{
			m_notifyicondata.hIcon = m_hIconEDEG4;
		}
		else if (m_iSignalStrength <= 59 && m_iSignalStrength >= 40)
		{
			m_notifyicondata.hIcon = m_hIconEDEG3;
		}
		else if (m_iSignalStrength <= 39 && m_iSignalStrength >= 20)
		{
			m_notifyicondata.hIcon = m_hIconEDEG2;
		}
		else
		{
			m_notifyicondata.hIcon = m_hIconEDEG1;
		}

		break;


	case 4: //WCDMA
		if (m_iSignalStrength <= 100 && m_iSignalStrength >= 80)
		{
			m_notifyicondata.hIcon = m_hIconWCDMA5;
		}
		else if (m_iSignalStrength <= 79 && m_iSignalStrength >= 60)
		{
			m_notifyicondata.hIcon = m_hIconWCDMA4;
		}
		else if (m_iSignalStrength <= 59 && m_iSignalStrength >= 40)
		{
			m_notifyicondata.hIcon = m_hIconWCDMA3;
		}
		else if (m_iSignalStrength <= 39 && m_iSignalStrength >= 20)
		{
			m_notifyicondata.hIcon = m_hIconWCDMA2;
		}
		else
		{
			m_notifyicondata.hIcon = m_hIconWCDMA1;
		}
		break;


	case 5: //HSDPA
	case 6: //HSUPA
	case 7:	//HSPA
		if (m_iSignalStrength <= 100 && m_iSignalStrength >= 80)
		{
			m_notifyicondata.hIcon = m_hIconHSDPA5;
		}
		else if (m_iSignalStrength <= 79 && m_iSignalStrength >= 60)
		{
			m_notifyicondata.hIcon = m_hIconHSDPA4;
		}
		else if (m_iSignalStrength <= 59 && m_iSignalStrength >= 40)
		{
			m_notifyicondata.hIcon = m_hIconHSDPA3;
		}
		else if (m_iSignalStrength <= 39 && m_iSignalStrength >= 20)
		{
			m_notifyicondata.hIcon = m_hIconHSDPA2;
		}
		else
		{
			m_notifyicondata.hIcon = m_hIconHSDPA1;
		}
		break;
	default:
		m_notifyicondata.hIcon = m_hIconNA;
		break;
	}

	m_notifyicondata.uFlags = NIF_ICON;
	Shell_NotifyIcon(NIM_MODIFY, &m_notifyicondata);
	// Remove the notification from the tray.

}

void CTrayIcon::ShowMenu(void)
{
	POINT point;
	GetCursorPos(&point);
	m_TaryMenu.GetSubMenu(0)->TrackPopupMenu( TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN, point.x, point.y, m_pParentWnd);
}