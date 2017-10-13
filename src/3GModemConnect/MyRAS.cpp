#include "StdAfx.h"
#include "MyRAS.h"
#include "Log.h"



HRASCONN CMyRAS::m_hRasConn = NULL;
CWnd* CMyRAS::m_pEventWnd = NULL;
BOOL CMyRAS::m_bConnected = FALSE;
BOOL CMyRAS::m_bDialing = FALSE;

VOID WINAPI CMyRAS::RasDialEventNotifier (UINT unMsg, RASCONNSTATE rasconnstate, DWORD dwError)
{
	switch(rasconnstate)
	{
	case RASCS_Connected:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_Connected\n");
#endif 
		m_bConnected = TRUE;
		m_bDialing = FALSE;
		break;

	case RASCS_Disconnected:
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : RASCS_Disconnected\n");
#endif 
		m_bConnected = FALSE;
		m_bDialing = FALSE;
		m_hRasConn = NULL;
		break;
	}

	if (dwError && m_hRasConn)
	{
		RasHangUp(m_hRasConn);
		m_hRasConn = NULL;
		m_bDialing = FALSE;
		m_bConnected = FALSE;
	}

	//Forward the message to UI 
	m_pEventWnd->PostMessage(WM_DIAL_EVNT, rasconnstate, dwError);
}


CMyRAS::CMyRAS(void)
{
	m_iEntriesCount = 0;
	m_psEntriesNames = NULL;
}

CMyRAS::~CMyRAS(void)
{
	if(m_psEntriesNames)
	{
		delete [] m_psEntriesNames;
	}
}


int CMyRAS::CheckIfAnyExistingConnection()
{
	DWORD dwCb = sizeof(RASCONN);
	DWORD dwErr = ERROR_SUCCESS;
	DWORD dwConnections = 0;
	RASCONN* lpRasConn = NULL;

	lpRasConn = (RASCONN*)HeapAlloc(GetProcessHeap(), 0, dwCb);

	if (lpRasConn == NULL)
	{
		dwErr = ERROR_NOT_ENOUGH_MEMORY;
		return -1;
	}

	lpRasConn->dwSize = sizeof(RASCONN);

	dwErr = RasEnumConnections(lpRasConn, &dwCb, &dwConnections);
	if (ERROR_BUFFER_TOO_SMALL != dwErr)
	{
		return -1;
	}

	if (ERROR_SUCCESS == dwErr)
	{
		DWORD i;
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Info : The following RAS connections are currently active\n");
#endif 
		for (i = 0; i < dwConnections; i++)
		{
#if defined(_LOG) && defined(_DEBUG) 
			CString sLog(lpRasConn[i].szEntryName);
			CLog::Instance()->LogToFile(L"Info : Entry : ");
			CLog::Instance()->LogToFile(sLog);
			CLog::Instance()->LogToFile(L"\n");
#endif
		}
	}
	else
	{
#if defined(_LOG) && defined(_DEBUG) 
		CString sLog;
		sLog.Format(L"%d", dwErr);
		CLog::Instance()->LogToFile(L"Error : RasEnumConnections failed with error code: ");
		CLog::Instance()->LogToFile(sLog);
		CLog::Instance()->LogToFile(L"\n");
#endif
	}

	if (NULL != lpRasConn)
	{
		HeapFree(GetProcessHeap(), 0, lpRasConn);
		lpRasConn = NULL;
	}

	return dwConnections;
}
BOOL CMyRAS::Initialize(CWnd* pEventWnd)
{
	m_pEventWnd = pEventWnd;
	// I Need to find what OS version is running
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	DWORD dwCb = 0;
	DWORD dwRet = ERROR_SUCCESS;
	DWORD dwEntries = 0;
	LPRASENTRYNAME lpRasEntryName = NULL;
	if(osvi.dwMajorVersion > 5) //check OS version 
	{
		//This code way under Windows Vist and Windows 7
		// Call RasEnumEntries with lpRasEntryName = NULL. dwCb is returned with the required buffer size and 
		// a return code of ERROR_BUFFER_TOO_SMALL
		dwRet = RasEnumEntries(NULL, NULL, lpRasEntryName, &dwCb, &dwEntries);
		//Save all Needed data
		m_iEntriesCount = dwEntries;
		m_psEntriesNames = new CString [m_iEntriesCount];

		if (dwRet == ERROR_BUFFER_TOO_SMALL)
		{
			// Allocate the memory needed for the array of RAS entry names.
			lpRasEntryName = (LPRASENTRYNAME) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwCb);
			if (lpRasEntryName == NULL)
			{
				//CLog::Instance()->Log("Error : HeapAlloc failed!");
				return FALSE;
			}
			// The first RASENTRYNAME structure in the array must contain the structure size
			lpRasEntryName[0].dwSize = sizeof(RASENTRYNAME);

			// Call RasEnumEntries to enumerate all RAS entry names
			dwRet = RasEnumEntries(NULL, NULL, lpRasEntryName, &dwCb, &dwEntries);

			// If successful, print the RAS entry names 
			if (ERROR_SUCCESS == dwRet)
			{
				//CLog::Instance()->Log("Info : Address book entries were found " );
				for (DWORD i = 0; i < dwEntries; i++)
				{
					m_psEntriesNames[i] = lpRasEntryName[i].szEntryName;
				}
			}
			//Deallocate memory for the connection buffer
			HeapFree(GetProcessHeap(), 0, lpRasEntryName);
			lpRasEntryName = NULL;
			return TRUE;
		}

		// There was either a problem with RAS or there are RAS entry names to enumerate    
		if(dwEntries >= 1)
		{
			//CLog::Instance()->Log("Error : The operation failed to acquire the buffer size.");
		}
		else
		{
#ifdef _LOG
			CLog::Instance()->LogToFile(L"Warning : There were no Address book entries found\n");
#endif
		}

		return FALSE;
	}
	else
	{
		//Windows XP 
		dwCb = sizeof(RASENTRYNAME);

		//lpRasEntryName should be allocated and intialized with some size
		lpRasEntryName = (LPRASENTRYNAME) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwCb); 
		lpRasEntryName[0].dwSize = sizeof(RASENTRYNAME);

		// Call RasEnumEntries with lpRasEntryName = NULL. dwCb is returned with the required buffer size and 
		// a return code of ERROR_BUFFER_TOO_SMALL
		dwRet = RasEnumEntries(NULL, NULL, lpRasEntryName, &dwCb, &dwEntries);

		//Save all Needed data
		m_iEntriesCount = dwEntries;
		m_psEntriesNames = new CString [m_iEntriesCount];

		if (dwRet == ERROR_BUFFER_TOO_SMALL || dwRet == 0)
		{
			//Now we know the right size free what is already allocated
			HeapFree(GetProcessHeap(), 0, lpRasEntryName);
			lpRasEntryName = NULL;

			//Now allocate the correct size
			lpRasEntryName = (LPRASENTRYNAME) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwCb);
			if (lpRasEntryName == NULL)
			{
				//CLog::Instance()->Log("Error : HeapAlloc failed!");
				return FALSE;
			}

			// The first RASENTRYNAME structure in the array must contain the structure size
			lpRasEntryName[0].dwSize = sizeof(RASENTRYNAME);

			// Call RasEnumEntries to enumerate all RAS entry names
			dwRet = RasEnumEntries(NULL, NULL, lpRasEntryName, &dwCb, &dwEntries);

			// If successful, print the RAS entry names 
			if (ERROR_SUCCESS == dwRet)
			{
				//CLog::Instance()->Log("Info : Address book entries were found " );
				for (unsigned int i = 0; i < dwEntries; i++)
				{
					m_psEntriesNames[i] = lpRasEntryName[i].szEntryName;
				}
			}
			//Deallocate memory for the connection buffer
			HeapFree(GetProcessHeap(), 0, lpRasEntryName);
			lpRasEntryName = NULL;
			return TRUE;
		}

		// There was either a problem with RAS or there are RAS entry names to enumerate    
		if(dwEntries >= 1)
		{
			//CLog::Instance()->Log("Error : The operation failed to acquire the buffer size.");
		}
		else
		{
			//CLog::Instance()->Log("Warning : There were no Address book entries found");
		}

		return FALSE;
	}
}

CString CMyRAS::GetEntry(int iIndex)  const
{
	if (iIndex < 0 || m_iEntriesCount <= iIndex )
	{
		return NULL;
	}

	return m_psEntriesNames[iIndex];
}

BOOL CMyRAS::Dial(CString sEntry, CString sUser, CString sPassword)
{
	m_bDialing = TRUE;
	DWORD dwRet = ERROR_SUCCESS;

	RASDIALPARAMS rasdialparams;
	ZeroMemory(&rasdialparams, sizeof(RASDIALPARAMS));
	rasdialparams.dwSize = sizeof(RASDIALPARAMS);

	rasdialparams.szPhoneNumber[0] = TEXT('\0');
	rasdialparams.szCallbackNumber[0] = TEXT('\0');
	wcscpy_s(rasdialparams.szEntryName, sEntry);
	wcscpy_s (rasdialparams.szUserName, sUser);
	wcscpy_s (rasdialparams.szPassword, sPassword);
	wcscpy_s (rasdialparams.szDomain, L"");

	//dwRet = RasDial (NULL, NULL, &rasdialparams, 0xFFFFFFFF, (LPVOID)m_pEventWnd->GetSafeHwnd(), &m_hRasConn);
	dwRet = RasDial (NULL, NULL, &rasdialparams, 0L, (LPVOID)CMyRAS::RasDialEventNotifier, &m_hRasConn);
	if (dwRet)
	{
		//TODO: error try using RasGetErrorString
		//TODO: Code needs cleaning
		int ERROR_VAL = dwRet;
		int BUFFER_SIZE = 256;

		DWORD dwRetVal = ERROR_SUCCESS;
		UINT  uErrorValue = ERROR_VAL;
		DWORD cBufSize = BUFFER_SIZE;
		WCHAR lpszErrorString[256];
		dwRetVal = RasGetErrorString(uErrorValue, lpszErrorString, cBufSize);

		if(dwRetVal == ERROR_SUCCESS)
		{
			MessageBox(NULL, lpszErrorString, L"Error", MB_OK);
		}
		else
		{
			//wprintf(L"RasGetErrorString failed, Return Value: %d", dwRetVal);
		}
		m_bDialing = FALSE;
		return FALSE;
	}
	return TRUE;
}

BOOL CMyRAS::HangUp(void)
{
	//if hangup and exit you neet to Sleep(3000) after hangup; MSDN
	DWORD dwRet =  RasHangUp(m_hRasConn);
	if(dwRet == ERROR_SUCCESS)
	{
		m_bConnected = FALSE;
		m_hRasConn = NULL;
		m_pEventWnd->PostMessage(WM_DIAL_EVNT, RASCS_Disconnected, 0);
	}
	return TRUE;
}

BOOL CMyRAS::GetDialErrorMsg(DWORD dwError, CString &sMsg)
{
	DWORD dwRetVal = ERROR_SUCCESS;
	if (dwError)
	{
		WCHAR lpszErrorString[256];
		RasGetErrorString(dwError, lpszErrorString, 256);
		sMsg = lpszErrorString;
	}
	if(dwRetVal == ERROR_SUCCESS)
	{
		return TRUE; 
	}
	else
	{
		return FALSE;
	}
}

RASCONNSTATE CMyRAS::GetConnectionStatus(void)
{
	RASCONNSTATUS rasconnstatus;
	DWORD dwRet;

	ZeroMemory(&rasconnstatus, sizeof(RASCONNSTATUS));
	rasconnstatus.dwSize = sizeof(RASCONNSTATUS);
	dwRet = RasGetConnectStatus(m_hRasConn, &rasconnstatus);
	RASCONNSTATE rasconnstate = rasconnstatus.rasconnstate;
	return rasconnstate;
}

BOOL CMyRAS::GetConnectionStatistics(DWORD &dwBytesXmited, DWORD &dwBytesRcved, DWORD &dwConnectDuration)
{
	RAS_STATS rasstats;
	DWORD dwRet;

	ZeroMemory(&rasstats, sizeof(RAS_STATS));
	rasstats.dwSize = sizeof(RAS_STATS);
	dwRet = RasGetConnectionStatistics(m_hRasConn, &rasstats);
	if (dwRet == ERROR_SUCCESS)
	{
		dwBytesXmited = rasstats.dwBytesXmited;
		dwBytesRcved = rasstats.dwBytesRcved;
		dwConnectDuration = rasstats.dwConnectDuration;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CMyRAS::isConnected(void)
{
	return m_bConnected;
}

BOOL CMyRAS::isDialing(void)
{
	return m_bDialing;
}
