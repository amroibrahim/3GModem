#pragma once

#include <iostream>
#include <ras.h>
#include <raserror.h>

class CMyRAS
{
public:
	CMyRAS(void);
	~CMyRAS(void);

	int CheckIfAnyExistingConnection(void);
	int GetEntriesCount(void) {return m_iEntriesCount;};

	BOOL HangUp(void);	
	BOOL isDialing(void);
	BOOL isConnected(void);
	BOOL Initialize(CWnd* pEventWnd);
	BOOL Dial(CString sEntry, CString sUser, CString sPassword);
	BOOL GetDialErrorMsg(DWORD dwError, CString &sMsg);
	BOOL GetConnectionStatistics(DWORD &dwBytesXmited, DWORD &dwBytesRcved, DWORD &dwConnectDuration);
	RASCONNSTATE GetConnectionStatus(void);

	CString GetEntry(int iIndex) const;

	static VOID WINAPI RasDialEventNotifier (UINT unMsg, RASCONNSTATE rasconnstate, DWORD dwError);


private:
	int m_iEntriesCount;

	CString *m_psEntriesNames;

	static HRASCONN m_hRasConn;
	static CWnd* m_pEventWnd;
	static BOOL m_bConnected;
	static BOOL m_bDialing;
};
