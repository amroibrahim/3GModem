#include "StdAfx.h"
#include "History.h"

CHistory* CHistory::m_pInstance = 0;

CHistory* CHistory::Instance() 
{
	if (m_pInstance == 0) 
	{
		m_pInstance = new CHistory;
	}
	return m_pInstance;
}

void CHistory::ShutDown(void)
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}
CHistory::CHistory(void)
{
	Load();
}


void CHistory::Load()
{
	std::fstream fHistoryFile;
	fHistoryFile.open("History", std::ios::in);
	while(fHistoryFile.is_open() && !fHistoryFile.eof())
	{
		char szLine[256];

		fHistoryFile >> szLine;

		if (strlen(szLine) < 2)
		{
			continue;
		}

		FILETIME ftStart, ftEnd;
		//SYSTEMTIME stStart, stEnd;

		int iDownloaded = 0;
		int iUploaded = 0;

		sscanf_s(szLine, "%d;%d;%d;%d;%d;%d",
			&ftStart.dwHighDateTime,
			&ftStart.dwLowDateTime,
			&ftEnd.dwHighDateTime,
			&ftEnd.dwLowDateTime,
			&iDownloaded, 
			&iUploaded);

		CHistoryItem historyitem(ftStart, ftEnd, iDownloaded, iUploaded);
		m_List.push_back(historyitem);
	}

	fHistoryFile.close();
}

void CHistory::Add(SYSTEMTIME stStart, SYSTEMTIME stEnd, int iDownloaded, int iUploaded)
{
	FILETIME ftStart, ftEnd;
	std::fstream fHistoryFile;

	SystemTimeToFileTime(&stStart, &ftStart);
	SystemTimeToFileTime(&stEnd, &ftEnd);

	CHistoryItem historyitem(ftStart, ftEnd, iDownloaded, iUploaded);
	m_List.push_back(historyitem);

	fHistoryFile.open("History", std::ios::out | std::ios::app);

	if (fHistoryFile.is_open())
	{
		fHistoryFile << ftStart.dwHighDateTime << ";" 
			<< ftStart.dwLowDateTime << ";"
			<< ftEnd.dwHighDateTime << ";"
			<< ftEnd.dwLowDateTime << ";"
			<< iDownloaded << ";"
			<< iUploaded << std::endl;
	}
	fHistoryFile.close();
}

void CHistory::GetIndexForTimeRange(SYSTEMTIME stStart, SYSTEMTIME stEnd, int &iStartIndex, int &iEndIndex)
{
	iStartIndex = -1;
	iEndIndex = - 1;

	stStart.wHour = 0;
	stStart.wMinute = 0;
	stStart.wSecond = 0;
	stStart.wMilliseconds = 0;

	stEnd.wHour = 23;
	stEnd.wMinute = 59;
	stEnd.wSecond = 59;
	stEnd.wMilliseconds = 999;

	FILETIME ftStart, ftEnd;
	SystemTimeToFileTime(&stStart, &ftStart);
	SystemTimeToFileTime(&stEnd, &ftEnd);

	for( std::vector<CHistoryItem>::size_type iItr = 0; iItr < m_List.size(); iItr++ ) 
	{
		//CompareFileTime Note:
		//ftStart < m_List[iItr].m_ftStart = -1
		//ftStart == m_List[iItr].m_ftStart = 0
		//ftStart > m_List[iItr].m_ftStart = 1

		int iCompareStart = CompareFileTime(&ftStart, &m_List[iItr].m_ftStart);
		int iCompareEnd = CompareFileTime(&ftEnd, &m_List[iItr].m_ftStart);

		if (iCompareStart <= 0 && iCompareEnd >= 0)
		{
			if (iStartIndex < 0)
			{
				iStartIndex = iItr;
			}
			iEndIndex = iItr;
		}


		//if (iCompareEnd >= 0)
		//{
		//	if (iStartIndex < 0)
		//	{
		//		iStartIndex = iItr;
		//	}
		//	iEndIndex = iItr;
		//}
	}

	return;
}

BOOL CHistory::GetItemAt(int iIndex, SYSTEMTIME &stStart, SYSTEMTIME &stEnd, int &iDownload, int &iUploaded)
{
	FileTimeToSystemTime(&m_List[iIndex].m_ftStart, &stStart);
	FileTimeToSystemTime(&m_List[iIndex].m_ftEnd, &stEnd);
	iDownload = m_List[iIndex].m_iDownloaded;
	iUploaded = m_List[iIndex].m_iUploaded;
	return TRUE;
}