#pragma once
#include <queue>
#include "MyRAS.h"
#include "Serial.h"


class CModem
{
public:
	enum EModemMode
	{
		eMODE_2G_ONLY,
		eMODE_2G_PREF,
		eMODE_3G_ONLY,
		eMODE_3G_PREF,
		eMODE_ANY
	};

	enum EModemBand
	{
		eBAND_GSM,
		eBAND_EGSM,
		eBAND_PGSM,
		eBAND_DCS,
		eBAND_PCS,
		eBAND_WCDMA,
		eBAND_ANY
	};

	enum EModemEvent
	{
		eMODEM_EVENT_DURATION,
		eMODEM_EVENT_UPLOAD_SPEED,
		eMODEM_EVENT_DOWNLOAD_SPEED,
		eMODEM_EVENT_DOWNLOADED_DATA,
		eMODEM_EVENT_UPLOADED_DATA,
		eMODEM_EVENT_SYS_MODE,
		eMODEM_EVENT_RSSI,
		eMODEM_EVENT_PROVIDER,
		eMODEM_EVENT_APN,
		eMODEM_EVENT_MANUFACTURER,
		eMODEM_EVENT_PORT,
		eMODEM_EVENT_MODEL,
		eMODEM_EVENT_IMEI,
		eMODEM_EVENT_FIRMWARE,
		eMODEM_EVENT_HARDWARE,
		eMODEM_EVENT_MODEM_MODE,
		eMODEM_EVENT_MODEM_BAND
	};

	enum EModemSysSubMode
	{
		eMODEM_MODE_NA,
		eMODEM_MODE_GSM,
		eMODEM_MODE_GPRS,
		eMODEM_MODE_EDEG,
		eMODEM_MODE_WCDMA,
		eMODEM_MODE_HSDPA,
		eMODEM_MODE_HSUPA,
		eMODEM_MODE_HSPA,
	};

	CModem(CString sPort);
	virtual ~CModem(void);
	virtual BOOL Initialize(CWnd* pEventWnd) = 0;
	virtual void OnStatisticsTime(void); //Implement this function if u want to do something spicial every 2 sec.
	virtual void ReceivedMessage(char* szMsg, int iSize);
	virtual void ShutDown(void);


	void SetAPN(CString sAPN);
	void EnableStatistics(void);
	void SetMode(EModemMode eMode, EModemBand eBand);

	CString GetEntry(int iIndex);
	CString GetPort(void);

	int GetEntriesCount(void);
	int GetUpdateRate(void);
	int CheckIfAnyExistingConnection(void);

	BOOL HangUp(void);
	BOOL isDialing();
	BOOL isConnected(void);
	BOOL Dial(CString sEntry, CString sUser, CString sPassword);
	BOOL GetDialErrorMsg(DWORD dwError, CString &sMsg);

	//BOOL GetConnectionStatistics(DWORD &dwBytesXmited, DWORD &dwBytesRcved, DWORD &dwConnectDuration);
	//int ConnectionStatus(void);


protected:
	struct ReadOrderItem 
	{
		std::string sATCommand; //The Command needed for selecting the correct function to parse
		int iNumberOfLines; //Expected Number of lines in the reply usually 1 but let keep track just in case
	};

	//std::queue<ReadOrderItem> m_qReadOrderQueue;
	virtual void ParseMessageAndPost(char* szMsg, int iSize);
	virtual void ProcessLine(const char *szLine) = 0;
	virtual void AddToWriteQueue(std::string sBuffer);

	static UINT StatisticsThread(LPVOID pParam);

	int m_iUpdateRate;
	CString m_sPort;
	CMyRAS m_RAS;
	CWnd *m_pEventWnd;
	CSerial m_COMSerial;
	BOOL m_bEnableStatistics;

	HANDLE m_pStatisticsThread;
	OVERLAPPED m_oStatisticsThreadExit;

};
