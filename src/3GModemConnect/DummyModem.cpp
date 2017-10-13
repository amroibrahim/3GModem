#include "StdAfx.h"
#include "DummyModem.h"
#include "Log.h"

CDummyModem::CDummyModem(CString sPort)
:CModem(sPort)
{
}

CDummyModem::~CDummyModem(void)
{
}

BOOL CDummyModem::Initialize(CWnd* pEventWnd)
{
#ifdef _LOG
	CLog::Instance()->LogToFile(L"Info : Dummy Modem Initialize modem\n");
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

	return TRUE;
}

BOOL CDummyModem::GetModemInfo()
{
	if(!m_COMSerial.Initialize(this))
	{
#ifdef _LOG
		CLog::Instance()->LogToFile(L"Error : Failed to Initialize COM Port\n");
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

	m_COMSerial.AddToWriteQueue("ATZ\r\n");
	m_COMSerial.AddToWriteQueue("ATI\r\n"); //Get Modem Indo
	//Give the modem some time to answer
	Sleep(2000);

	//m_COMSerial.ShutDown();
	return TRUE; 

}

CString CDummyModem::GetModel()
{
	CString sModel;
	m_cs.Lock();
	sModel = m_sModel;
	m_cs.Unlock();

	return sModel;
}

CString CDummyModem::GetManufacturer()
{
	CString sManufacturer;
	m_cs.Lock();
	sManufacturer = m_sManufacturer;
	m_cs.Unlock();

	return sManufacturer;
}

void CDummyModem::ProcessLine(const char *sLine)
{
	if (!sLine)
	{
		return;
	}
	char szMessage[1025];
	strncpy_s(szMessage, 1025, sLine, _TRUNCATE);

	if(strstr(szMessage,"OK")!=0)
	{
		//to do need to set some flag here
		m_COMSerial.ExecuteNextCommand();
		return;
	}

	if(strstr(szMessage,"Manufacturer:")!=0)
	{
		Process_Manufacturer(szMessage);
		return;
	}

	if(strstr(szMessage,"Model:")!=0)
	{
		Process_Model(szMessage);
		return;
	}


}

void CDummyModem::Process_Manufacturer(char* szMsg)
{
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;
	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );
	//Adding support for EC169 "Manufacturer: +GMI: HUAWEI TECHNOLOGIES CO., LTD"
	if(pToken && strstr(pToken, "+GMI")!=0)
	{
		pToken = strtok_s( NULL, szSepsMsg, &pNextToken );
	}

#if defined(_LOG) && defined(_DEBUG) 
	CString sLog(pToken);
	CLog::Instance()->LogToFile(L"Info : 1 : Manufacturer : ");
	CLog::Instance()->LogToFile(sLog);
	CLog::Instance()->LogToFile(L"\n");
#endif

	m_cs.Lock();
	if(pToken)
	{
		m_sManufacturer = pToken;
	}
	m_cs.Unlock();
}

void CDummyModem::Process_Model(char* szMsg)
{
	char szSepsMsg[]   = ":,"; //to split a single message
	char *pToken = NULL;
	char *pNextToken = NULL;

	pToken = strtok_s( szMsg, szSepsMsg, &pNextToken );
	pToken = strtok_s( NULL, szSepsMsg, &pNextToken );

#if defined(_LOG) && defined(_DEBUG) 
	CString sLog(pToken);
	CLog::Instance()->LogToFile(L"Info : 1: Model : ");
	CLog::Instance()->LogToFile(sLog);
	CLog::Instance()->LogToFile(L"\n");
#endif

	m_cs.Lock();
	if (pToken)
	{
		m_sModel = pToken;
	}
	m_cs.Unlock();

}
