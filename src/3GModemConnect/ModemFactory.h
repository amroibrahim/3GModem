#pragma once

#include "Modem.h"

class CModemFactory
{
public:

	CModemFactory(void);
	~CModemFactory(void);
	CModem *SearchForModem();

private:
	int SearchForCOMPorts(void);
	BOOL SearchFor3GModem(CString &sManufacturer, CString &sModel); 
	CStringArray m_saPorts;
	CStringArray m_saName;
	int m_iPortIndex;
};
