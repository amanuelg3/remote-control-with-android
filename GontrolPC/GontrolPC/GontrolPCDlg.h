
// GontrolPCDlg.h : header file
//

#pragma once

#include "EmInput.h"
#include "SettingDialog.h"
#include "LangRes.h"
#include "SafeQueue.h"
#include "MyThread.h"

#define WM_NOTIFYICON (WM_USER+1500)

// CGontrolPCDlg dialog
class CGontrolPCDlg : public CDialog , public IMyThread
{
// Construction
public:
	CGontrolPCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_GONTROLPC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	CEmInput * m_pEmInput;
	static const int DEFAULT_BEACONPORT = 55555;
	static const int DEFAULT_RECVPORT = 44444;
	int m_BeaconPort;
	int m_RecvPort;
	bool m_fStart;
	bool m_fHide;
	CSettingDialog m_SettingDlg;
	NOTIFYICONDATA m_nticond;
	WINDOWPLACEMENT m_wndpl;
	IConfigMgr * m_cfmgr;
	CLangRes * m_pLangRes;
	static unsigned int __stdcall sNotifyStatusThread(void * p);
	virtual int Start();
	CSafeQueue<ConnMsg> m_qConnMsg;
// Implementation
protected:
	HICON m_hIcon;
	//bool OSVersion6();
	//BOOL TryElvtPriv(const TCHAR * szMessage);
	void ShowNotifyBalloon(const TCHAR * szInfo, UINT timeout = 10000);
	void SetNotifyIcon(HICON hIcon);
	void SetNotifyTip(const TCHAR * szTip);
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnNotifyIcon(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnNcPaint();
	afx_msg void OnNotifyiconExit();
	afx_msg void OnNotifyiconStart();
	afx_msg void OnUpdateNotifyiconStart(CCmdUI *pCmdUI);
	afx_msg void OnNotifyiconStop();
	afx_msg void OnUpdateNotifyiconStop(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNotifyiconSetting();
};
