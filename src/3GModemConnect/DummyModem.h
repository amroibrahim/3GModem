#pragma once
#include "modem.h"

class CDummyModem :
	public CModem
{
public:
	CDummyModem(CString sPort);
	virtual ~CDummyModem(void);

	virtual BOOL Initialize(CWnd* pEventWnd);
	BOOL GetModemInfo(void);
	CString GetManufacturer(void);
	CString GetModel(void);

private:
	CCriticalSection m_cs;
	virtual void ProcessLine(const char *szLine);
	void Process_Manufacturer(char* szMsg);
	void Process_Model(char* szMsg);

	CString m_sManufacturer;
	CString m_sModel;
};
