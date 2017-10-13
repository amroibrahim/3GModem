#pragma once
#include <vector>
#include <fstream>


class CHistoryItem
{
public:
	CHistoryItem()
	{
		m_iDownloaded = 0;
		m_iUploaded = 0;
	}

	CHistoryItem(FILETIME ftStart, FILETIME ftEnd, int iDownloaded, int iUploaded)
	{
		m_ftStart = ftStart;
		m_ftEnd = ftEnd;
		m_iDownloaded = iDownloaded;
		m_iUploaded = iUploaded;

	};

	FILETIME m_ftStart;
	FILETIME m_ftEnd;
	int m_iDownloaded;
	int m_iUploaded;
};

class CHistory
{
public:
	static CHistory* Instance();
	static void ShutDown();
	void Add(SYSTEMTIME stStart, SYSTEMTIME stEnd, int iDownloaded, int iUploaded);
	void GetIndexForTimeRange(SYSTEMTIME stStart, SYSTEMTIME stEnd, int &iStartIndex, int &iEndIndex);
	BOOL GetItemAt(int iIndex, SYSTEMTIME &stStart, SYSTEMTIME &stEnd, int &iDownload, int &iUploaded);


private:
	CHistory(void);
	CHistory(CHistory const&);
	CHistory& operator=(CHistory const&);  

	static CHistory *m_pInstance;


	std::vector<CHistoryItem> m_List;

	void Save();
	void Load();
};
