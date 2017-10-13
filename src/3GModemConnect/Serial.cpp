#include "StdAfx.h"
#include "Serial.h"
#include "Modem.h"
#include "Log.h"

#define PORT_MAX_READ_BUFFER		2048
#define PORT_MAX_WRITE_BUFFER		1024
#define BUFFER_SIZE					1024
#define READ_TIMEOUT				500
#define WRITE_TIMEOUT				500
#define CHECK_WRITE_QUEUE			50
#define WAIT_AFTER_WRITE			50



CSerial::CSerial(void)
{
	m_pModem = NULL;
	m_pReaderThread = NULL;
	m_pWriteThread = NULL;

	m_oReaderThreadExit.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_oWriterThreadExit.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_oWriteNextCommandInQueue.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_oReaderThreadExit.hEvent)
	{
		ResetEvent(m_oReaderThreadExit.hEvent);
	}
	else
	{
		//TODO: ERROR
	}

	if (m_oWriterThreadExit.hEvent)
	{
		ResetEvent(m_oWriterThreadExit.hEvent);
	}
	else
	{
		//TODO: ERROR
	}

	
	if (m_oWriteNextCommandInQueue.hEvent)
	{
		ResetEvent(m_oWriteNextCommandInQueue.hEvent);
	}
	else
	{
		//TODO: ERROR
	}

}

CSerial::~CSerial(void)
{

}

void CSerial::ShutDown()
{
	SetEvent( m_oReaderThreadExit.hEvent);
	SetEvent( m_oWriterThreadExit.hEvent);

	// Wait for the thread to exit.
	if ( WAIT_TIMEOUT == WaitForSingleObject( m_pReaderThread, 3000 ) )
	{
		//force it closed with Terminate thread
		//Not a good choice tho
		TerminateThread( m_pReaderThread, -1000 );
	}

	// Wait for the thread to exit.
	if ( WAIT_TIMEOUT == WaitForSingleObject( m_pWriteThread, 3000 ) )
	{
		//force it closed with Terminate thread
		//Not a good choice tho
		TerminateThread( m_pWriteThread, -1000 );
	}

	CloseHandle(m_pReaderThread);
	CloseHandle(m_pWriteThread);
	CloseHandle(m_oReaderThreadExit.hEvent);
	CloseHandle(m_oWriterThreadExit.hEvent);
	CloseHandle(m_oWriteNextCommandInQueue.hEvent);
	CloseHandle(m_hCOMPort);

}

BOOL CSerial::Initialize(CModem* pmodem)
{
	m_pModem = pmodem;
	return TRUE;
}

BOOL CSerial::Open(CString sPort)
{

	//Need to check on sPort there is an issue with CreateFile with ports over 9
	//a work around exists rename port eg: COM10  -> \\.\COM10 -> "\\\\.\\COM10" 
	CString sCorrectPortName;

	if(sPort.GetLength() > 4 )
	{
		sCorrectPortName = "\\\\.\\";
		sCorrectPortName += sPort;
	}
	else
	{
		sCorrectPortName = sPort;
	}

	m_hCOMPort = ::CreateFile( sCorrectPortName, 
		GENERIC_READ | GENERIC_WRITE, 
		0, 
		0, 
		OPEN_EXISTING, 
		FILE_FLAG_OVERLAPPED, 
		0);

	if (m_hCOMPort == INVALID_HANDLE_VALUE) 
	{
#if defined(_LOG) && defined(_DEBUG)
		CString sLog;
		DWORD dwLastError = GetLastError();
		sLog.Format(L"%d", dwLastError);
		CLog::Instance()->LogToFile(L"Error : SetupDiGetClassDevs failed with error ");
		CLog::Instance()->LogToFile(sLog);
		CLog::Instance()->LogToFile(L"\n");
#endif
		return FALSE;
	}

	if (!GetCommTimeouts( m_hCOMPort, &originalcommtimeouts))
	{
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Error : GetCommTimeouts");
#endif
		return FALSE;
	}

	if (!GetCommState(m_hCOMPort , &originaldcb)) // Save Original current DCB settings
	{
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Error : GetCommState Error");
#endif
	}

	SetPortSettings();

	if(!StartThreads())
	{
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Error : StartThreads");
#endif
		return FALSE;
	}

	m_bIsOpen = TRUE;
	return TRUE;
}

BOOL CSerial::StartThreads(void)
{

	m_pReaderThread =  CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) ReaderThread, (LPVOID) this, 0, NULL); 
	//AfxBeginThread (ReaderThread, this);

	if(m_pReaderThread == NULL)
	{
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Error : Failed to start Reader & Status thread");
#endif
		return FALSE;
	}

	m_pWriteThread  =  CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) WriterThread, (LPVOID) this, 0, NULL); 
	//AfxBeginThread (WriterThread, this);
	if(m_pWriteThread == NULL)
	{
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Error : Failed to start Write thread");
#endif
		return FALSE;
	}
	return TRUE;
}

UINT CSerial::ReaderThread(LPVOID pParam)
{
	DWORD dwRead;
	BOOL bWaitingOnRead = FALSE;
	OVERLAPPED oReader = {0}; 
	BOOL bReaderThreadDone = FALSE;
	char lpBuf[BUFFER_SIZE];
	DWORD dwRes;
	HANDLE hThreads[2];

	CSerial *pSerial = (CSerial*) pParam;

	oReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (oReader.hEvent == NULL)
	{
		return -1;
		//TODO: ERROR
	}

	hThreads[0] = oReader.hEvent;
	hThreads[1] = pSerial->m_oReaderThreadExit.hEvent;

	while (!bReaderThreadDone)
	{
		if (!bWaitingOnRead) 
		{
			// Issue read operation.
			if (!ReadFile(pSerial->m_hCOMPort, lpBuf, BUFFER_SIZE, &dwRead, &oReader)) 
			{
				if (GetLastError() != ERROR_IO_PENDING)     // read not delayed?
				{
					// Error in communications; report it.
				}
				else
				{
					bWaitingOnRead = TRUE;
				}
			}
			else 
			{    
				//read completed 
				//Don't use post message here u need to wait for the GUI to take action on the string
				pSerial->m_pModem->ReceivedMessage(lpBuf, dwRead);
			}
		}

		if (bWaitingOnRead) 
		{

			//dwRes = WaitForMultipleObjects(oReader.hEvent, 500);
			dwRes = WaitForMultipleObjects(2, hThreads, FALSE, READ_TIMEOUT);

			switch(dwRes)
			{
				// Read completed.
			case WAIT_OBJECT_0:
				if (!GetOverlappedResult(pSerial->m_hCOMPort, &oReader, &dwRead, FALSE))
				{
					// Error in communications; report it.
				}
				else
				{
					// Read completed successfully
					//Don't use post message here u need to wait for the GUI to take action on the string
					pSerial->m_pModem->ReceivedMessage(lpBuf, dwRead);
				}
				bWaitingOnRead = FALSE;
				break;

			case WAIT_OBJECT_0 + 1:
				bReaderThreadDone = TRUE;
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
	}

		CloseHandle(oReader.hEvent);
	return 0;
}

UINT CSerial::WriterThread(LPVOID pParam)
{
	char lpBuf[2048];
	DWORD dwToWrite;

	OVERLAPPED oWriter = {0};
	DWORD dwWritten;
	DWORD dwRes;
	BOOL bWriterThreadDone = FALSE;
	BOOL bExeucteWriteCommand = FALSE;


	CSerial *pSerial = (CSerial*) pParam;

	HANDLE hThreadsWriteExit[2];
	HANDLE hThreadsQueueExit[2];

	// Create this write operation's OVERLAPPED structure's hEvent.
	oWriter.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (oWriter.hEvent == NULL)
	{
		//error creating overlapped event handle
		//return FALSE;
	}

	hThreadsWriteExit[0] = oWriter.hEvent;
	hThreadsWriteExit[1] = pSerial->m_oWriterThreadExit.hEvent;

	hThreadsQueueExit[0] = pSerial->m_oWriteNextCommandInQueue.hEvent;
	hThreadsQueueExit[1] = pSerial->m_oWriterThreadExit.hEvent;


	// Create this write operation's OVERLAPPED structure's hEvent.
	oWriter.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (oWriter.hEvent == NULL)
	{
		// error creating overlapped event handle
		return FALSE;
	}

	while (!bWriterThreadDone)
	{
		//Wait for command a command to execute or exit events
		dwRes = WaitForMultipleObjects(2, hThreadsQueueExit, FALSE, INFINITE);
		switch(dwRes)
		{
			// Read completed.
		case WAIT_OBJECT_0:
			pSerial->m_cs.Lock();
			ResetEvent(pSerial->m_oWriteNextCommandInQueue.hEvent);
			if(pSerial->m_qCommandsQueue.size())
			{
				bExeucteWriteCommand = TRUE;
			}
			pSerial->m_cs.Unlock();
			break;

		case WAIT_OBJECT_0 + 1:
			bWriterThreadDone = TRUE;
			break;


		default:
			// Error in the WaitForSingleObject; abort.
			// This indicates a problem with the OVERLAPPED structure's
			// event handle.
			break;
		}
		// Issue write.
		if(bExeucteWriteCommand)
		{
			bExeucteWriteCommand = FALSE;
			CommandItem commanditem; 
			pSerial->m_cs.Lock();
			commanditem = pSerial->m_qCommandsQueue.front();
			pSerial->m_cs.Unlock();
			strncpy_s(lpBuf, 2048, commanditem.sATCommand.c_str(), _TRUNCATE);
			dwToWrite = commanditem.sATCommand.length();


			//iWriteQueueSize--;
			if (!WriteFile(pSerial->m_hCOMPort, lpBuf, dwToWrite, &dwWritten, &oWriter)) 
			{
				if (GetLastError() != ERROR_IO_PENDING) 
				{ 
					// WriteFile failed, but isn't delayed. Report error and abort.
					//fRes = FALSE;
				}
				else
				{
					// Write is pending.
					dwRes = WaitForSingleObject(oWriter.hEvent, WRITE_TIMEOUT);
					//dwRes = WaitForMultipleObjects(2, hThreadsWriteExit, FALSE, INFINITE);
					//dwRes = WaitForMultipleObjects(2, hThreadsQueueExit, FALSE, INFINITE);

					switch(dwRes)
					{
						// OVERLAPPED structure's event has been signaled. 
					case WAIT_OBJECT_0:
						if (!GetOverlappedResult(pSerial->m_hCOMPort, &oWriter, &dwWritten, FALSE))
						{
							//fRes = FALSE;
						}
						else
						{
							// Write operation completed successfully.
							//fRes = TRUE;
							Sleep(WAIT_AFTER_WRITE);
						}
						break;

					case WAIT_OBJECT_0 + 1:
						bWriterThreadDone = TRUE;
						break;

					default:
						// An error has occurred in WaitForSingleObject.
						// This usually indicates a problem with the
						// OVERLAPPED structure's event handle.
						//fRes = FALSE;
						break;
					}
				}
			}
			else
			{
				// WriteFile completed immediately.
				//fRes = TRUE;
				Sleep(WAIT_AFTER_WRITE);
			}
		}
	}
	CloseHandle(oWriter.hEvent);
	return 0;
}



void CSerial::AddToWriteQueue(std::string sBuffer)
{
	CommandItem commanditem;
	commanditem.sATCommand = sBuffer;
	commanditem.iNumberOfRetry = 0;

	m_cs.Lock();
	m_qCommandsQueue.push(commanditem);
	if(m_qCommandsQueue.size() == 1) //Queue was empty u need to notify to execute next command
	{
		SetEvent(m_oWriteNextCommandInQueue.hEvent);
	}
	m_cs.Unlock();
}


void CSerial::ExecuteNextCommand()
{
	m_cs.Lock();
	m_qCommandsQueue.pop();
	if(m_qCommandsQueue.size() > 0) //make sure that queue is not empty or else nothing to be executed
	{
		SetEvent(m_oWriteNextCommandInQueue.hEvent);
	}
	m_cs.Unlock();
}


BOOL CSerial::SetPortSettings(void)
{

	DCB dcb = originaldcb;
	dcb.DCBlength = sizeof(dcb);

	//For Now I will use defaults 
	dcb.BaudRate		  = CBR_9600;
	dcb.ByteSize		  = 8;
	dcb.Parity			  = NOPARITY;
	dcb.StopBits		  = ONESTOPBIT;


	if (!SetCommState(m_hCOMPort, &dcb))
	{
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Error : SetCommState");
#endif
		return FALSE;
	}

	COMMTIMEOUTS commtimeouts = { 0x01, 0, 0, 0, 0 };
	// set new timeouts
	if (!SetCommTimeouts(m_hCOMPort, &commtimeouts))
	{
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Error : SetCommTimeouts");
#endif
		return FALSE;
	}

	if(!SetupComm(m_hCOMPort, PORT_MAX_READ_BUFFER, PORT_MAX_WRITE_BUFFER))
	{
#if defined(_LOG) && defined(_DEBUG) 
		CLog::Instance()->LogToFile(L"Error : SetupComm");
#endif
		return FALSE;
	}

	return TRUE;
}
