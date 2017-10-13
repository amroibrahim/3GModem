#include "StdAfx.h"
#include "ZTEModem.h"
#include <vector>
#include <string>

#include "Log.h"

CZTEModem::CZTEModem(CString sPort)
:CModem(sPort)
{
	m_iUpdateRate = 1000; //2000 millisecond
}

CZTEModem::~CZTEModem(void)
{
}


BOOL CZTEModem::Initialize(CWnd* pEventWnd)
{

#ifdef _LOG
	CLog::Instance()->LogToFile(L"Info : Initialize ZTE modem\n");
#endif

	m_pEventWnd = pEventWnd;

	if(!m_COMSerial.Initialize(this))
	{
#ifdef _LOG
		CLog::Instance()->LogToFile(L"Error : Failed to Initialize COM Port\n");
#endif
		return FALSE;
	}

	if(!m_RAS.Initialize(m_pEventWnd))
	{
#ifdef _LOG
		CLog::Instance()->LogToFile(L"Error : Failed to Initialize RAS\n");
#endif
		return FALSE;
	}

	if(!m_COMSerial.Open(m_sPort))
	{

#ifdef _LOG
		CLog::Instance()->LogToFile(L"Error : Failed to Open Port : ");
		CLog::Instance()->LogToFile(m_sPort);
		CLog::Instance()->LogToFile(L"\n");
#endif
		return FALSE;
	}

	AddToWriteQueue("ATZ\r\n");
	AddToWriteQueue("AT+ZOPRT=5\r\n");
	AddToWriteQueue("AT+CSQ\r\n");  //Signal
	AddToWriteQueue("AT+COPS?\r\n");
	AddToWriteQueue("AT+CGDCONT?\r\n"); //APN
	AddToWriteQueue("ATI\r\n"); //Modem info
	AddToWriteQueue("AT+ZSNT?\r\n"); //Get Connection mode 3G, 2G, 3G only, 2G only
	AddToWriteQueue("AT+ZPAS?\r\n"); //GPRS, GSM, UMTS, EDGE, HSDPA 

	EnableStatistics();

	return TRUE;
}

void CZTEModem::ProcessLine(const char *sLine)
{
	if (!sLine)
	{
		return;
	}

	char szMessage[1025];
	strcpy_s(szMessage,sLine);

	if(strstr(szMessage,"OK")!=0)
	{
		//to do need to set some flag here
		m_COMSerial.ExecuteNextCommand();
		return;
	}

	if(strstr(szMessage,"ERROR")!=0)
	{
		//to do need to set some flag here
		m_COMSerial.ExecuteNextCommand();
		return;
	}

	if(strstr(szMessage,"^BOOT:")!=0)
	{
		//Ignor this message I really dont know what it means
		return;
	}

	if(strstr(szMessage,"^SRVST:")!=0)
	{
		//Ignor this message I really dont know what it means
		return;
	}

	if(strcmp(szMessage,"AT")==0 || strstr(szMessage,"AT+") || strstr(szMessage,"AT^") || strstr(szMessage,"ATI") )
	{
		//Those are commands ignor them
		return;
	}
	if(strstr(szMessage,"^DSFLOWRPT:")!=0)
	{ 
		Process_DSFLOWRPT_Msg(szMessage);
		return;
	}

	if(strstr(szMessage,"+ZPAS:")!=0)
	{
		Process_ZPAS_Msg(szMessage);
		return;
	}

	if(strstr(szMessage,"Manufacturer:")!=0)
	{
		Process_Manufacturer(szMessage);
		return;
	}

	if(strstr(szMessage,"^RSSI:")!=0)
	{
		Process_RSSI_Msg(szMessage);
		return;
	}

	if(strstr(szMessage,"Model:")!=0)
	{
		Process_Model(szMessage);
		return;
	}

	if(strstr(szMessage,"Revision:")!=0)
	{
		Process_Revision(szMessage);
		return;
	}

	if(strstr(szMessage,"IMEI:")!=0)
	{
		Process_IMEI(szMessage);
		return;
	}

	if(strstr(szMessage,"+CSQ:")!=0)
	{
		Process_CSQ_Msg(szMessage);
		return;
	}

	if(strstr(szMessage,"+CGDCONT:")!=0)
	{
		Process_CGDCONT_Msg(szMessage);
		return;
	}

	if(strstr(szMessage,"+COPS:")!=0)
	{
		Process_COPS_Msg(szMessage);
		return;
	}

	if(strstr(szMessage,"+COPN:")!=0)
	{
		Process_COPN_Msg(szMessage);
		return;
	}

	if(strstr(szMessage,"+ZSNT:")!=0)
	{
		Process_ZSNT_Msg(szMessage);
		return;
	}

	//At this point mostly this message has nothing special so we can filter it
	//Need to check Queue and see if any command should be processed
	//if (m_qReadOrderQueue.size())
	//{
	//	ReadOrderItem readorderitem;
	//	readorderitem = m_qReadOrderQueue.front();
	//	if(strstr(readorderitem.sATCommand.c_str(),"AT+GMI")!=0)
	//	{
	//		//Process_Manufacturer(szMessage);
	//	}
	//	else if (strstr(readorderitem.sATCommand.c_str(),"AT+GMM")!=0)
	//	{
	//		//Process_Model(szMessage);
	//	}
	//	else if (strstr(readorderitem.sATCommand.c_str(),"AT+GMR")!=0)
	//	{
	//		//Process_Revision(szMessage);
	//	}

	//	readorderitem.iNumberOfLines--;
	//	
	//	if (readorderitem.iNumberOfLines == 0)
	//	{
	//		m_qReadOrderQueue.pop();
	//	}
	//	else
	//	{
	//		m_qReadOrderQueue.pop(); //Pop outdated element
	//		m_qReadOrderQueue.push(readorderitem); //Push an updated one with updated number of lines
	//	}
	//}
	//TRACE("Unhandeled message %s : ",szMessage);
}

void CZTEModem::Process_RSSI_Msg(char* szMsg)
{
	/*

	/----------------------------------------------------------\ 
	| Wording  |   Blocks   | Percentages | RSSI | Decibels  |
	|----------|------------|-------------|------|-----------|
	|Excellent | [][][][][] |     100     |  31  |   >-51    |
	|----------|------------|-------------|------|-----------|
	|          | [][][][][] |     97      |  30  |    -53    |
	|----------|------------|-------------|------|-----------|
	|          | [][][][][] |     94      |  29  |    -55    |
	|----------|------------|-------------|------|-----------|
	|          | [][][][][] |     90      |  28  |    -57    |
	|----------|------------|-------------|------|-----------|
	|          | [][][][][] |     87      |  27  |    -59    |
	|----------|------------|-------------|------|-----------|
	|          | [][][][][] |     84      |  26  |    -61    |
	|----------|------------|-------------|------|-----------|
	|Good      | [][][][]   |     81      |  25  |    -63    |
	|----------|------------|-------------|------|-----------|
	|          | [][][][]   |     77      |  24  |    -65    |
	|----------|------------|-------------|------|-----------|
	|          | [][][][]   |     74      |  23  |    -67    |
	|----------|------------|-------------|------|-----------|
	|          | [][][][]   |     71      |  22  |    -69    |
	|----------|------------|-------------|------|-----------|
	|          | [][][][]   |     68      |  21  |    -71    |
	|----------|------------|-------------|------|-----------|
	|          | [][][][]   |     65      |  20  |    -73    |
	|----------|------------|-------------|------|-----------|
	|Fair      | [][][]     |     61      |  19  |    -75    |
	|----------|------------|-------------|------|-----------|
	|          | [][][]     |     58      |  18  |    -77    |
	|----------|------------|-------------|------|-----------|
	|          | [][][]     |     55      |  17  |    -79    |
	|----------|------------|-------------|------|-----------|
	|          | [][][]     |     52      |  16  |    -81    |
	|----------|------------|-------------|------|-----------|
	|          | [][][]     |     48      |  15  |    -83    |
	|----------|------------|-------------|------|-----------|
	|          | [][][]     |     45      |  14  |    -85    |
	|----------|------------|-------------|------|-----------|
	|Poor      | [][]       |     42      |  13  |    -87    |
	|----------|------------|-------------|------|-----------|
	|          | [][]       |     39      |  12  |    -89    |
	|----------|------------|-------------|------|-----------|
	|          | [][]       |     35      |  11  |    -91    |
	|----------|------------|-------------|------|-----------|
	|          | [][]       |     32      |  10  |    -93    |
	|----------|------------|-------------|------|-----------|
	|          | [][]       |     29      |  9   |    -95    |
	|----------|------------|-------------|------|-----------|
	|          | [][]       |     26      |  8   |    -97    |
	|----------|------------|-------------|------|-----------|
	|Very Poor | []         |     23      |  7   |    -99    |
	|----------|------------|-------------|------|-----------|
	|          | []         |     19      |  6   |    -101   |
	|----------|------------|-------------|------|-----------|
	|          | []         |     16      |  5   |    -103   |
	|----------|------------|-------------|------|-----------|
	|          | []         |     13      |  4   |    -105   |
	|----------|------------|-------------|------|-----------|
	|          | []         |     10      |  3   |    -107   |
	|----------|------------|-------------|------|-----------|
	|          | []         |     6       |  2   |    -109   |
	|----------|------------|-------------|------|-----------|
	|No Signal |            |     3       |  1   |    -111   |
	|----------|------------|-------------|------|-----------|
	|          |            |     0       |  0   |   <-113   |
	\--------------------------------------------------------/

	*/
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;

	int iVal = 0;

	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken);
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken);

#if defined(_LOG) && defined(_DEBUG) 
	CString sLog(pToken);
	CLog::Instance()->LogToFile(L"Info : 1 : RSSI Change : ");
	CLog::Instance()->LogToFile(sLog);
	CLog::Instance()->LogToFile(L"\n");
#endif
	if (pToken)
	{
		iVal = atoi(pToken);
		m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_RSSI, iVal);
	}
}

void CZTEModem::Process_Revision(char* szMsg)
{
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;

	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );

#if defined(_LOG) && defined(_DEBUG) 
	CString sLog(pToken);
	CLog::Instance()->LogToFile(L"Info : 1 : Firmware : ");
	CLog::Instance()->LogToFile(sLog);
	CLog::Instance()->LogToFile(L"\n");
#endif

	if (pToken)
	{
		m_pEventWnd->SendMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_FIRMWARE, (LPARAM)pToken);

	}
}

void CZTEModem::Process_IMEI(char* szMsg)
{
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;
	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );

#if defined(_LOG) && defined(_DEBUG) 
	CString sLog(pToken);
	CLog::Instance()->LogToFile(L"Info : 1 : IMEI : ");
	CLog::Instance()->LogToFile(sLog);
	CLog::Instance()->LogToFile(L"\n");
#endif

	if (pToken)
	{
		m_pEventWnd->SendMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_IMEI, (LPARAM)pToken);
	}
}
void CZTEModem::Process_CSQ_Msg(char* szMsg)
{
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;
	int iVal = 0;

	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );

#if defined(_LOG) && defined(_DEBUG) 
	CString sLog(pToken);
	CLog::Instance()->LogToFile(L"Info : 1 : CSQ : ");
	CLog::Instance()->LogToFile(sLog);
	CLog::Instance()->LogToFile(L"\n");
#endif

	if (pToken)
	{
		iVal = atoi(pToken);
		m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_RSSI, iVal);
	}
}
void CZTEModem::Process_CGDCONT_Msg(char* szMsg)
{
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;
	int iVal = 0;

	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );

	if (pToken && pToken[1] == '1') //We are only care about the first APN
	{
		pToken = strtok_s( NULL, szSepsMsg, &pNextToken  );
		pToken = strtok_s( NULL, szSepsMsg, &pNextToken  );
		m_pEventWnd->SendMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_APN,(LPARAM)pToken);
	}
}

void CZTEModem::Process_ZSNT_Msg(char* szMsg)
{
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;
	int iVal = 0;

	EModemMode eMode = eMODE_ANY;
	//	EModemBand eBand = EModemBand::eBAND_ANY;
	char szToken1[5];
	char szToken2[5];
	char szToken3[5];
	/*
	AT+ZSNT=0,0,0 (Auto) - Default
	AT+ZSNT=1,0,0 GPRS Only
	AT+ZSNT=2,0,0 3G Only
	AT+ZSNT=0,0,1 GPRS Preferred
	AT+ZSNT=0,0,2 3G Preferred
	*/
	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken );
	strncpy_s(szToken1, 5, strtok_s( NULL, szSepsMsg, &pNextToken ), _TRUNCATE);
	strncpy_s(szToken2, 5, strtok_s( NULL, szSepsMsg, &pNextToken ), _TRUNCATE);
	strncpy_s(szToken3, 5, strtok_s( NULL, szSepsMsg, &pNextToken ), _TRUNCATE);


	if (_stricmp(szToken1, "0") == 0
		&& _stricmp(szToken2, "0") == 0
		&& _stricmp(szToken3, "0") == 0)
	{
		eMode = CModem::eMODE_ANY;
	}

	if (_stricmp(szToken1, "1") == 0)
	{
		eMode = CModem::eMODE_2G_ONLY;
	}

	if (_stricmp(szToken1, "2") == 0)
	{
		eMode = CModem::eMODE_3G_ONLY;
	}

	if (_stricmp(szToken3, "1") == 0)
	{
		eMode = CModem::eMODE_2G_PREF;
	}

	if (_stricmp(szToken3, "2") == 0)
	{
		eMode = CModem::eMODE_3G_PREF;
	}

	//if (_stricmp(szToken1, "0") == 0
	//	&& _stricmp(szToken2, "0") == 0
	//	&& _stricmp(szToken3, "0") == 0)
	//{
	//	eBand = EModemBand::eBAND_ANY;
	//}

	//if (_stricmp(szBand, "80000") == 0)
	//{
	//	eBand = EModemBand::eBAND_GSM;
	//}

	//if (_stricmp(szBand, "100") == 0)
	//{
	//	eBand = EModemBand::eBAND_EGSM;
	//}

	//if (_stricmp(szBand, "200") == 0)
	//{
	//	eBand = EModemBand::eBAND_PGSM;
	//}

	//if (_stricmp(szBand, "80") == 0)
	//{
	//	eBand = EModemBand::eBAND_DCS;
	//}

	//if (_stricmp(szBand, "200000") == 0)
	//{
	//	eBand = EModemBand::eBAND_PCS;
	//}

	//if (_stricmp(szBand, "400000") == 0)
	//{
	//	eBand = EModemBand::eBAND_WCDMA;
	//}
	m_pEventWnd->SendMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_MODEM_MODE,(LPARAM) eMode);
	//m_pEventWnd->SendMessage(WM_MODEM_READ_EVNT, eMODEM_MODEM_BAND,(LPARAM) eBand);
}

void CZTEModem::Process_COPS_Msg(char* szMsg)
{
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;
	int iVal = 0;

	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );
	//Don't use post here or else the string will be gone

	if (pToken)
	{
		m_pEventWnd->SendMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_PROVIDER,(LPARAM)pToken);
	}
}
void CZTEModem::Process_COPN_Msg(char* szMsg)
{

	//pToken = strtok_s( NULL, szSeps, &pNextToken  );
	//int iID = atoi(pToken);
	//pToken = strtok_s( NULL, szSeps, &pNextToken  );
	//std::string sString;
	//sString = pToken;

	//ProviderList.insert(std::pair<int, std::string>(iID, sString));
	//return;
}
void CZTEModem::Process_DSFLOWRPT_Msg(char* szMsg)
{
	/*
	^DSFLOWRPT: N1, N2, N3, N4, N5, N6, N7 
	N1: Connection duration in seconds 
	N2: measured upload speed 
	N3: measured download speed 
	N4: number of sent data 
	N5: number of received data 
	N6: connection, supported by the maximum upload speed 
	N7: connection, supported by a maximum download speed 
	*/
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;
	int iVal = 0;

	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken  );

	//Get 1:Connection duration in seconds

	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );

	if( pToken != NULL )
	{
#if defined(_LOG) && defined(_DEBUG) 
		CString sLog(pToken);
		CLog::Instance()->LogToFile(L"Info : 1 :Connection duration in seconds : ");
		CLog::Instance()->LogToFile(sLog);
		CLog::Instance()->LogToFile(L"\n");
#endif

		if(pToken)
		{
			iVal = strtol (pToken, NULL, 16);
			m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_DURATION, iVal);
		}
	}
	//Get 2: measured upload speed
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken  );

	if( pToken != NULL )
	{
#if defined(_LOG) && defined(_DEBUG) 
		CString sLog(pToken);
		CLog::Instance()->LogToFile(L"Info : 2 : Measured upload speed : ");
		CLog::Instance()->LogToFile(sLog);
		CLog::Instance()->LogToFile(L"\n");
#endif
		if (pToken)
		{
			iVal = strtol (pToken, NULL, 16);
			m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_UPLOAD_SPEED, iVal);
		}
	}

	//Get 3: measured download speed
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );
	if( pToken != NULL )
	{
#if defined(_LOG) && defined(_DEBUG) 
		CString sLog(pToken);
		CLog::Instance()->LogToFile(L"Info : 3 : Measured download speed : ");
		CLog::Instance()->LogToFile(sLog);
		CLog::Instance()->LogToFile(L"\n");
#endif
		if (pToken)
		{
			iVal = strtol (pToken, NULL, 16);
			m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_DOWNLOAD_SPEED, iVal);
		}
	}

	//Get 4: number of sent data
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );
	if( pToken != NULL )
	{
#if defined(_LOG) && defined(_DEBUG) 
		CString sLog(pToken);
		CLog::Instance()->LogToFile(L"Info : 4 : Size of sent data ");
		CLog::Instance()->LogToFile(sLog);
		CLog::Instance()->LogToFile(L"\n");
#endif

		if (pToken)
		{
			iVal = strtol (pToken, NULL, 16);
			m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_UPLOADED_DATA, iVal);
		}
	}
	//Get 5: number of received data
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken  );

	if( pToken != NULL )
	{
#if defined(_LOG) && defined(_DEBUG) 
		CString sLog(pToken);
		CLog::Instance()->LogToFile(L"Info : 5 : Size of received data : ");
		CLog::Instance()->LogToFile(sLog);
		CLog::Instance()->LogToFile(L"\n");
#endif
		if (pToken)
		{
			iVal = strtol (pToken, NULL, 16);
			m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_DOWNLOADED_DATA, iVal);
		}
	}
	//Get 6: connection, supported by the maximum upload speed
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );
	if( pToken != NULL )
	{
#if defined(_LOG) && defined(_DEBUG) 
		CString sLog(pToken);
		CLog::Instance()->LogToFile(L"Info : 6 : connection, supported by the maximum upload speed : ");
		CLog::Instance()->LogToFile(sLog);
		CLog::Instance()->LogToFile(L"\n");
#endif
	}
	//Get 7: connection, supported by a maximum download speed
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );

	if( pToken != NULL )
	{
#if defined(_LOG) && defined(_DEBUG) 
		CString sLog(pToken);
		CLog::Instance()->LogToFile(L"Info : 7 : connection, supported by a maximum download speed : ");
		CLog::Instance()->LogToFile(sLog);
		CLog::Instance()->LogToFile(L"\n");
#endif
	}
	return;
}

void CZTEModem::Process_ZPAS_Msg(char* szMsg)
{
	/*
	+ZPAS:N1, N2
	N1:<network>: the type of current network
	No Service
	Limited Service
	GPRS
	GSM
	UMTS
	EDGE
	HSDPA

	N2:<srv_domain>: service domain
	CS_ONLY: CS domain service available.
	PS_ONLY: PS domain service available.
	CS_PS: CS&PS domain service available.
	CAMPED: camped in a cell.

	*/
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;
	int iVal = 0;

	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken  );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken  );

#if defined(_LOG) && defined(_DEBUG) 
	CString sLog1(pToken);
	CLog::Instance()->LogToFile(L"Info : 1 : Mode Change : ");
	CLog::Instance()->LogToFile(sLog1);
	CLog::Instance()->LogToFile(L"\n");
#endif

	CString sToken(pToken);
	sToken.MakeUpper();

	iVal = CModem::eMODEM_MODE_NA;

	if (sToken.Find(L"NO SERVICE") != -1)
	{
		iVal = CModem::eMODEM_MODE_NA;
	}
	else if (sToken.Find(L"LIMITED SERVICE") != -1)
	{
		iVal = CModem::eMODEM_MODE_NA;
	}
	else if (sToken.Find(L"GPRS") != -1)
	{
		iVal = CModem::eMODEM_MODE_GPRS;
	}
	else if (sToken.Find(L"GSM") != -1)
	{
		iVal = CModem::eMODEM_MODE_GSM;
	}
	else if (sToken.Find(L"UMTS") != -1)
	{
		iVal = CModem::eMODEM_MODE_WCDMA;
	}
	else if (sToken.Find(L"EDGE") != -1)
	{
		iVal = CModem::eMODEM_MODE_EDEG;
	}
	else if (sToken.Find(L"HSDPA") != -1)
	{
		iVal = CModem::eMODEM_MODE_HSDPA;
	}

	m_pEventWnd->PostMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_SYS_MODE, iVal);

}

void CZTEModem::Process_Manufacturer(char* szMsg)
{
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;
	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken  );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken  );

#if defined(_LOG) && defined(_DEBUG) 
	CString sLog(pToken);
	CLog::Instance()->LogToFile(L"Info : 1 : Manufacturer : ");
	CLog::Instance()->LogToFile(sLog);
	CLog::Instance()->LogToFile(L"\n");
#endif
	if (pToken)
	{
		m_pEventWnd->SendMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_MANUFACTURER, (LPARAM)pToken);
	}
}

void CZTEModem::Process_Model(char* szMsg)
{
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;
	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken  );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken  );

#if defined(_LOG) && defined(_DEBUG) 
	CString sLog(pToken);
	CLog::Instance()->LogToFile(L"Info : 1 : Model : ");
	CLog::Instance()->LogToFile(sLog);
	CLog::Instance()->LogToFile(L"\n");
#endif
	if (pToken)
	{
		m_pEventWnd->SendMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_MODEL, (LPARAM)pToken);
	}
}

void CZTEModem::Process_Hardware(char* szMsg)
{
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;

	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );

#if defined(_LOG) && defined(_DEBUG) 
	CString sLog(pToken);
	CLog::Instance()->LogToFile(L"Info : 1 : Hardware : ");
	CLog::Instance()->LogToFile(sLog);
	CLog::Instance()->LogToFile(L"\n");
#endif

	if (pToken)
	{
		m_pEventWnd->SendMessage(WM_MODEM_READ_EVNT, eMODEM_EVENT_HARDWARE, (LPARAM)pToken);
	}
}

void CZTEModem::OnStatisticsTime(void)
{
	//This function will be called every 2 seconds 
	AddToWriteQueue("AT+ZPAS?\r\n");
}