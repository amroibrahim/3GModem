#pragma once


// CTrayIcon

class CTrayIcon : public CWnd
{
	DECLARE_DYNAMIC(CTrayIcon)

public:
	CTrayIcon();
	virtual ~CTrayIcon();
	BOOL Create(CWnd* pParent);

	void SetSignalStrength(int iSignalStrength);
	void SetNetworkMode(int iNetworkMode);
	void UpdateTrayIcon(void);
	void ShowMenu(void);

protected:
	DECLARE_MESSAGE_MAP()

private:

	NOTIFYICONDATA m_notifyicondata;
	CWnd* m_pParentWnd;
	int m_iNetworkMode;
	int m_iSignalStrength;

	CMenu m_TaryMenu;

	HICON m_hIconNA;

	HICON m_hIconEDEG1;
	HICON m_hIconEDEG2;
	HICON m_hIconEDEG3;
	HICON m_hIconEDEG4;
	HICON m_hIconEDEG5;

	HICON m_hIconWCDMA1;
	HICON m_hIconWCDMA2;
	HICON m_hIconWCDMA3;
	HICON m_hIconWCDMA4;
	HICON m_hIconWCDMA5;

	HICON m_hIconHSDPA1;
	HICON m_hIconHSDPA2;
	HICON m_hIconHSDPA3;
	HICON m_hIconHSDPA4;
	HICON m_hIconHSDPA5;

};


