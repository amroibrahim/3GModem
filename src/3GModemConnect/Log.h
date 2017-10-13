#pragma once
#include <fstream>

class CLog
{
public:

	static CLog* Instance();
	static void ShutDown();
	void LogToFile(CString sMsg);


private:
	CLog(void);
	CLog(CLog const&);
	CLog& operator=(CLog const&);  

	static CLog *m_pInstance;
	static CCriticalSection m_csinit;

	std::ofstream m_LogFile;
	CCriticalSection m_cs;

};
