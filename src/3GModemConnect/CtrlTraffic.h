#pragma once

#include <vector>
// CTraffic

class CCtrlTraffic : public CStatic
{
	DECLARE_DYNAMIC(CCtrlTraffic)

public:
	CCtrlTraffic();
	virtual ~CCtrlTraffic();

	int GetMaxSpeed(void);

	void SetUpload(float m_fUpload);
	void SetDownload(float m_fDownload);
	void AddDownload(int iDownload);
	void AddUpload(int iUpload);

	afx_msg void OnPaint();

protected:
	DECLARE_MESSAGE_MAP()

private:
	
	std::vector<int> m_vDownloads;
	std::vector<int> m_vUploads;
	
	float m_fDownloadSpeed;
	float m_fUploadSpeed;
};


