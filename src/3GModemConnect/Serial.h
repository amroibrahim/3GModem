#pragma once
#include <queue>

class CModem;

class CSerial
{
public:
	CSerial();
	virtual ~CSerial(void);

	void Close(void);
	void ShutDown(void);
	void AddToWriteQueue(std::string sBuffer);
	void ExecuteNextCommand(void);

	BOOL Open(CString szPort);
	BOOL StartThreads(void);
	BOOL Initialize(CModem* pmodem);
	BOOL SetPortSettings(void);

private:

	static UINT ReaderThread(LPVOID pParam);
	static UINT WriterThread(LPVOID pParam);

	struct CommandItem 
	{
		std::string sATCommand; 
		int iNumberOfRetry;
	};

	BOOL m_bIsOpen;

	CModem *m_pModem;

	HANDLE  m_hCOMPort;
	HANDLE m_pReaderThread;
	HANDLE m_pWriteThread;

	CCriticalSection m_cs;

	DCB originaldcb;

	OVERLAPPED m_oReaderThreadExit; 
	OVERLAPPED m_oWriterThreadExit;
	OVERLAPPED m_oWriteNextCommandInQueue;

	COMMTIMEOUTS originalcommtimeouts;

	std::queue<CommandItem> m_qCommandsQueue;

};
