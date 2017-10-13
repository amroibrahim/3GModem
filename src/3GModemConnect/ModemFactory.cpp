#include "StdAfx.h"

#include <WinIoCtl.h>
#include <SetupAPI.h>
#include <atlbase.h>

#include "ModemFactory.h"
#include "DummyModem.h"
#include "HuaweiModem.h"
#include "HuaweiCDMAModem.h"
#include "ZTEModem.h"
#include "Log.h"


DEFINE_GUID(GUID_CLASS_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);

CModemFactory::CModemFactory(void)
{
	m_iPortIndex = -1;
}

CModemFactory::~CModemFactory(void)
{
}


BOOL CModemFactory::SearchFor3GModem(CString &sManufacturer, CString &sModel)
{
	BOOL bFound = FALSE;
	CDummyModem *modem;
	int iCount = m_saName.GetCount();
#ifdef _LOG
	CLog::Instance()->LogToFile(L"Info : Ports List :\n");
#endif
	for (int iItr = 0; iItr < iCount ; iItr++)
	{
#ifdef _LOG
		CLog::Instance()->LogToFile(L"\t");
		CLog::Instance()->LogToFile(m_saName[iItr]);
		CLog::Instance()->LogToFile(L"\n");
#endif
		if(m_saName[iItr].CompareNoCase(L"HUAWEI Mobile Connect - 3G PC UI Interface") == 0
			||m_saName[iItr].CompareNoCase(L"Vodafone Mobile Broadband Secondary Port Modem (Huawei)") == 0)
		{
#ifdef _LOG
			CLog::Instance()->LogToFile(L"Info : Modem Found\n");
#endif
			modem = new CDummyModem(m_saPorts[iItr]);
			modem->GetModemInfo();
			sManufacturer = modem->GetManufacturer();
			sModel = modem->GetModel();
			modem->ShutDown();
			m_iPortIndex = iItr;
			delete modem;
			bFound = TRUE;
		}
		else if(m_saName[iItr].CompareNoCase(L"ZTE NMEA Device") == 0)
		{
#ifdef _LOG
			CLog::Instance()->LogToFile(L"Info : Modem Found\n");
#endif
			modem = new CDummyModem(m_saPorts[iItr]);
			modem->GetModemInfo();
			sManufacturer = modem->GetManufacturer();
			sModel = modem->GetModel();
			modem->ShutDown();
			m_iPortIndex = iItr;
			delete modem;
			bFound = TRUE;
		}
	}
	return bFound;
}

CModem *CModemFactory::SearchForModem()
{
#ifdef _LOG
	CLog::Instance()->LogToFile(L"Info : Searching for modem\n");
#endif
	CString sManufacturer, sModel;
	SearchForCOMPorts();
	if(!SearchFor3GModem(sManufacturer, sModel))
	{
#ifdef _LOG
		CLog::Instance()->LogToFile(L"Error: No Modem Found\n");
#endif
		return NULL;
	}

	if (sManufacturer.IsEmpty())
	{
#ifdef _LOG
		CLog::Instance()->LogToFile(L"Error : Modem didn't return a Manufacturer\n");
		return NULL;
#endif
	}

	sManufacturer.Trim();
	sManufacturer.MakeLower();
	if(sManufacturer.Find(L"huawei") != -1)
	{
#ifdef _LOG
		CLog::Instance()->LogToFile(L"Info : Creating a Huawei Modem\n");
#endif
		if(sModel.Find(L"EC169") != -1)
		{
			//Huawei CDMA Modem
			return new CHuaweiCDMAModem(m_saPorts[m_iPortIndex]);
		}
		else
		{
			//Normal Huawei Modem
			return new CHuaweiModem(m_saPorts[m_iPortIndex]);
		}
	}
	else if (sManufacturer.Find(L"zte") != -1)
	{
#ifdef _LOG
		CLog::Instance()->LogToFile(L"Info : Creating a ZTE Modem\n");
#endif
		return new CZTEModem(m_saPorts[m_iPortIndex]);
	}
#ifdef _LOG
	CLog::Instance()->LogToFile(L"Error: Modem Unknown\n");
#endif
	return NULL;

}

int CModemFactory::SearchForCOMPorts(void)
{
	m_saPorts.RemoveAll();
	m_saName.RemoveAll();

	GUID guid = GUID_CLASS_COMPORT;

	HDEVINFO hdevinfo = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hdevinfo == INVALID_HANDLE_VALUE)
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

	SP_DEVINFO_DATA devinfodata;
	devinfodata.cbSize = sizeof(SP_DEVINFO_DATA);
	int iIndex = 0;

	while (SetupDiEnumDeviceInfo(hdevinfo, iIndex, &devinfodata))
	{
		HKEY hkey = SetupDiOpenDevRegKey(hdevinfo, &devinfodata, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);

		if (hkey)
		{
			TCHAR szPort[256];
			DWORD dwSize = sizeof(szPort);
			DWORD dwType = 0;
			if ((RegQueryValueEx(hkey, L"PortName", NULL, &dwType, (LPBYTE)szPort, &dwSize) == ERROR_SUCCESS))
			{
				if(dwType == REG_SZ)
				{
					TCHAR szPortName[256];
					DWORD dwSizeName = sizeof(szPortName);
					m_saPorts.Add(szPort);
					if (SetupDiGetDeviceRegistryProperty(hdevinfo, &devinfodata, SPDRP_DEVICEDESC, NULL, (PBYTE)szPortName, dwSizeName, &dwSizeName) && (dwType == REG_SZ))
					{
						m_saName.Add(szPortName);
					}
					else
					{
						m_saName.Add(L""); //No Name
					}
				}
			}
			RegCloseKey(hkey);
		}
		iIndex++;
	}

	//Cleaning
	SetupDiDestroyDeviceInfoList(hdevinfo);

	return 0;
}
