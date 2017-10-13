#include "StdAfx.h"
#include "Modem.h"
#include "Log.h"

CModem::CModem(CString sPort)
{
	m_bEnableStatistics = FALSE;
	m_pStatisticsThread = NULL;
	m_pEventWnd = NULL;
	m_iUpdateRate = 2000; //2000 millisecond
	m_sPort = sPort;

}

CModem::~CModem(void)
{
}

CString CModem::GetPort(void)
{
	return m_sPort;
}

int CModem::GetUpdateRate(void)
{
	return m_iUpdateRate;
}

void CModem::SetAPN(CString sAPN)
{
	if(sAPN.IsEmpty())
	{
		return;
	}
	char szCommand[128];
	sprintf_s(szCommand, 128,"AT+CGDCONT=1,\"IP\",\"%S\"\r\n", sAPN.GetString());
	AddToWriteQueue(szCommand);
}

void CModem::SetMode(EModemMode eMode, EModemBand eBand)
{

	char szCommand[128];
	szCommand[0]= '\0';

	switch(eMode)
	{
	case CModem::eMODE_2G_ONLY:
		sprintf_s(szCommand, 128, "AT^SYSCFG=13,1,");
		break;
	case CModem::eMODE_3G_ONLY:
		sprintf_s(szCommand, 128, "AT^SYSCFG=14,2,");
		break;
	case CModem::eMODE_2G_PREF:
		sprintf_s(szCommand, 128, "AT^SYSCFG=2,1,");
		break;
	case CModem::eMODE_3G_PREF:
		sprintf_s(szCommand, 128, "AT^SYSCFG=2,2,");
		break;
	case CModem::eMODE_ANY:
		sprintf_s(szCommand, 128, "AT^SYSCFG=2,0,");
		break;
	}

	/*
	GSM(850MHz): 00080000
	EGSM(900MHz): 00000100
	PGSM(900MHz): 00000200
	DCS(1800MHz): 00000080
	PCS(1900MHz): 00200000
	WCDMA(2100MHz): 00400000
	Any band: 3FFFFFFF
	*/

	int iOffset = strlen(szCommand);

	switch(eBand)
	{
	case CModem::eBAND_GSM:
		sprintf_s(szCommand + iOffset, 128 - iOffset, "00080000,2,4\r\n");
		break;
	case CModem::eBAND_EGSM:
		sprintf_s(szCommand + iOffset, 128 - iOffset, "00000100,2,4\r\n");
		break;
	case CModem::eBAND_PGSM:
		sprintf_s(szCommand + iOffset, 128 - iOffset, "00000200,2,4\r\n");
		break;
	case CModem::eBAND_DCS:
		sprintf_s(szCommand + iOffset, 128 - iOffset, "00000080,2,4\r\n");
		break;
	case CModem::eBAND_PCS:
		sprintf_s(szCommand + iOffset, 128 - iOffset, "00200000,2,4\r\n");
		break;
	case CModem::eBAND_WCDMA:
		sprintf_s(szCommand + iOffset, 128 - iOffset, "00400000,2,4\r\n");
		break;
	case CModem::eBAND_ANY:
		sprintf_s(szCommand + iOffset, 128 - iOffset, "3FFFFFFF,2,4\r\n");
		break;
	}

	AddToWriteQueue(szCommand);
}


void CModem::AddToWriteQueue(std::string sBuffer)
{
	//Add to serial port write queue
	m_COMSerial.AddToWriteQueue(sBuffer);
}

int CModem::GetEntriesCount()
{
	return m_RAS.GetEntriesCount();
}

CString CModem::GetEntry(int iIndex)
{
	return m_RAS.GetEntry(iIndex);
}

BOOL CModem::Dial(CString sEntry, CString sUser, CString sPassword)
{
	return m_RAS.Dial(sEntry, sUser, sPassword);
}

BOOL CModem::GetDialErrorMsg(DWORD dwError, CString &sMsg)
{
	return m_RAS.GetDialErrorMsg(dwError, sMsg);
}

BOOL CModem::HangUp()
{
	return m_RAS.HangUp();
}

//int CModem::ConnectionStatus(void)
//{
//	return m_RAS.ConnectionStatus();
//}

//BOOL CModem::GetConnectionStatistics(DWORD &dwBytesXmited, DWORD &dwBytesRcved, DWORD &dwConnectDuration)
//{
//	return m_RAS.GetConnectionStatistics(dwBytesXmited, dwBytesRcved, dwConnectDuration);
//}

BOOL CModem::isConnected()
{
	return m_RAS.isConnected();
}

BOOL CModem::isDialing()
{
	return m_RAS.isDialing();
}

int CModem::CheckIfAnyExistingConnection()
{
	return m_RAS.CheckIfAnyExistingConnection();
}

void CModem::ReceivedMessage(char* szMsg, int iSize)
{
	ParseMessageAndPost(szMsg, iSize);
	return;
}

void CModem::ParseMessageAndPost(char* szMsg, int iSize)
{ 
	std::vector<std::string> vtrString;  //We need the vector to work around strtoken multi instance
	char szMessage[1025];

	strncpy_s(szMessage, 1025, szMsg, _TRUNCATE); //copy on given size
	szMessage[iSize] = 0x0;

#if defined(_LOG) && defined(_DEBUG) 
	CString sLog(szMessage);
	CLog::Instance()->LogToFile(L"Info : MSG :\n");
	CLog::Instance()->LogToFile(sLog);
	CLog::Instance()->LogToFile(L"\n");
#endif

	char szSepsLine[] = "\r\n"; // to split string to lines
	char *pToken = NULL;
	char *pNextToken = NULL;
	pToken = strtok_s( szMessage, szSepsLine, &pNextToken );

	while( pToken != NULL )
	{
		vtrString.push_back(pToken);
		pToken = strtok_s( NULL, szSepsLine, &pNextToken );
	}

	for( std::vector<std::string>::size_type i = 0; i < vtrString.size(); i++ ) 
	{
		ProcessLine(vtrString[i].c_str());
	}
	return;
}

void CModem::EnableStatistics(void)
{
	m_bEnableStatistics = TRUE;

	m_oStatisticsThreadExit.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_oStatisticsThreadExit.hEvent)
	{
		ResetEvent(m_oStatisticsThreadExit.hEvent);
	}
	else
	{
		//TODO: ERROR
	}

	m_pStatisticsThread =  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) StatisticsThread, (LPVOID) this, 0, NULL); 
}

void CModem::ShutDown(void)
{
	//TODO: RAS should do some cleaning and close any handler m_RAS
	if(m_bEnableStatistics)
	{
		SetEvent( m_oStatisticsThreadExit.hEvent);
		if ( WAIT_TIMEOUT == WaitForSingleObject( m_pStatisticsThread, 3000 ) )
		{
			//force it closed with Terminate thread
			//Not a good choice tho
			TerminateThread( m_pStatisticsThread, -1000 );
		}

		CloseHandle(m_oStatisticsThreadExit.hEvent);
	}
	m_COMSerial.ShutDown();
}

void CModem::OnStatisticsTime(void)
{
	//Nothing to do now :) but might need it later :)
}

UINT CModem::StatisticsThread(LPVOID pParam)
{
	DWORD dwRes;
	BOOL bStatisticsThreadDone = FALSE;

	CModem *pModem = (CModem*) pParam;
	HANDLE hExit = pModem->m_oStatisticsThreadExit.hEvent;

	DWORD dwPreBytesXmited, dwPreBytesRcved, dwPreConnectDuration;
	int iUpdateRate = pModem->GetUpdateRate();
	dwPreBytesXmited = 0;
	dwPreBytesRcved = 0;
	dwPreConnectDuration = 0;


	while (!bStatisticsThreadDone)
	{
		RASCONNSTATE rasconnstate = pModem->m_RAS.GetConnectionStatus();

		if(rasconnstate == RASCS_Connected)
		{
			DWORD dwBytesXmited, dwBytesRcved, dwConnectDuration;
			pModem->m_RAS.GetConnectionStatistics(dwBytesXmited, dwBytesRcved, dwConnectDuration);
			dwConnectDuration = dwConnectDuration / 1000; //This is in milliseconds

			pModem->m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_UPLOADED_DATA, dwBytesXmited);
			pModem->m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_DOWNLOADED_DATA, dwBytesRcved);
			pModem->m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_DURATION, dwConnectDuration);
			pModem->m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_UPLOAD_SPEED, (dwBytesXmited - dwPreBytesXmited)/(iUpdateRate/1000));// u need to fined per second not per 2 sec ;)
			pModem->m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_DOWNLOAD_SPEED, (dwBytesRcved - dwPreBytesRcved)/(iUpdateRate/1000));

			dwPreBytesXmited = dwBytesXmited;
			dwPreBytesRcved = dwBytesRcved;
		}

		pModem->OnStatisticsTime();
		dwRes = WaitForSingleObject(hExit, iUpdateRate);

		switch(dwRes)
		{
		case WAIT_OBJECT_0:
			bStatisticsThreadDone = TRUE;
			break;


		case WAIT_TIMEOUT:
			// Operation isn't complete yet. fWaitingOnRead flag isn't
			// changed since I'll loop back around, and I don't want
			// to issue another read until the first one finishes.
			//
			// This is a good time to do some background work.
			break;                       

		default:
			// Error in the WaitForSingleObject; abort.
			// This indicates a problem with the OVERLAPPED structure's
			// event handle.
			break;
		}
	}
	return 0;
}
