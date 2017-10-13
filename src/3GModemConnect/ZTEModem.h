#pragma once
#include "modem.h"

class CZTEModem :
	public CModem
{
public:
	CZTEModem(CString sPort);
	virtual ~CZTEModem(void);

private:
	virtual BOOL Initialize(CWnd* pEventWnd);

	virtual void OnStatisticsTime(void);

	virtual void ProcessLine(const char *szLine);
	virtual void Process_DSFLOWRPT_Msg(char* szMsg);
	virtual void Process_CGDCONT_Msg(char* szMsg);
	virtual void Process_ZSNT_Msg(char* szMsg);
	virtual void Process_CSQ_Msg(char* szMsg);
	virtual void Process_COPS_Msg(char* szMsg);
	virtual void Process_COPN_Msg(char* szMsg);
	virtual void Process_ZPAS_Msg(char* szMsg);
	virtual void Process_RSSI_Msg(char* szMsg);


	virtual void Process_Revision(char* szMsg);
	virtual void Process_IMEI(char* szMsg);
	virtual void Process_Manufacturer(char* szMsg);
	virtual void Process_Model(char* szMsg);
	virtual void Process_Hardware(char* szMsg);
};
