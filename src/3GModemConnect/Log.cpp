#include "StdAfx.h"
#include "Log.h"

CLog* CLog::m_pInstance = 0;
CCriticalSection CLog::m_csinit;

CLog::CLog(void)
{
	m_LogFile.open("Log.txt", std::ios::out | std::ios::app);
	m_LogFile << "---------------------------------- Application Starting ----------------------------------";
	m_LogFile << std::endl;
}

CLog* CLog::Instance() 
{
	if (m_pInstance == 0) 
	{
		CLog::m_csinit.Lock();
		if (m_pInstance == 0) 
		{
			m_pInstance = new CLog;
		}
		CLog::m_csinit.Unlock();
	}
	return m_pInstance;
}

void CLog::LogToFile(CString sMsg)
{
	ASSERT(sMsg);
	TRACE("TRACE : %S\n", sMsg.GetString());
	m_cs.Lock();
	char szLine[2048]; //Max Read Buffer for COM port
	sprintf_s(szLine, 2048, "%S", sMsg.GetString());
	m_LogFile << szLine;
	//m_LogFile << std::endl;
	m_cs.Unlock();
}

void CLog::ShutDown()
{
	CLog::m_csinit.Lock();
	CLog::Instance()->m_LogFile.close();
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
	CLog::m_csinit.Unlock();
}