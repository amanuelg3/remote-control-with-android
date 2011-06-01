
// GontrolPCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GontrolPC.h"
#include "GontrolPCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMERID_HIDEWINDOW 1

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGontrolPCDlg dialog


CGontrolPCDlg::CGontrolPCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGontrolPCDlg::IDD, pParent)
{

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_BeaconPort = DEFAULT_BEACONPORT;
	m_RecvPort = DEFAULT_RECVPORT;
	m_pLangRes = CLangRes::GetInstance();
	m_fStart = false;
	m_fHide = true;
	m_ThreadAddr = sNotifyStatusThread;

}

void CGontrolPCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGontrolPCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
//	ON_WM_NCPAINT()
	ON_MESSAGE(WM_NOTIFYICON,&CGontrolPCDlg::OnNotifyIcon)
	ON_COMMAND(ID_NOTIFYICON_EXIT, &CGontrolPCDlg::OnNotifyiconExit)
	ON_COMMAND(ID_NOTIFYICON_START, &CGontrolPCDlg::OnNotifyiconStart)
	ON_UPDATE_COMMAND_UI(ID_NOTIFYICON_START, &CGontrolPCDlg::OnUpdateNotifyiconStart)
	ON_COMMAND(ID_NOTIFYICON_STOP, &CGontrolPCDlg::OnNotifyiconStop)
	ON_UPDATE_COMMAND_UI(ID_NOTIFYICON_STOP, &CGontrolPCDlg::OnUpdateNotifyiconStop)
	ON_WM_TIMER()
	ON_COMMAND(ID_NOTIFYICON_SETTING, &CGontrolPCDlg::OnNotifyiconSetting)
END_MESSAGE_MAP()


// CGontrolPCDlg message handlers

BOOL CGontrolPCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);	

	ZeroMemory(&m_nticond,sizeof(NOTIFYICONDATA));
	m_nticond.cbSize = sizeof(NOTIFYICONDATA);
	m_nticond.hWnd = this->GetSafeHwnd();
	m_nticond.uID = 1;
	m_nticond.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
	m_nticond.uCallbackMessage = WM_NOTIFYICON;
	m_nticond.hIcon = LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINSTOP));
	_tcscpy_s(m_nticond.szInfoTitle,_countof(m_nticond.szInfoTitle),_T("Gontrol"));
	m_nticond.szInfoTitle[_countof(m_nticond.szInfoTitle) - 1] = NULL;
	_tcscpy_s(m_nticond.szTip,_countof(m_nticond.szTip),_T("Gontrol"));
	m_nticond.szTip[_countof(m_nticond.szTip) - 1] = NULL;
	Shell_NotifyIcon(NIM_ADD,&m_nticond);

	m_cfmgr = CreateConfigMgr(INISTORE);
	CString szUserProfilePath;
	szUserProfilePath.GetEnvironmentVariable(_T("APPDATA"));
	m_cfmgr->SetTag(szUserProfilePath +_T("\\Gontrol\\Gontrol"));
	int result = m_cfmgr->Open(true);
	if (result)
	{
		CString szConfigFailMsg;
		szConfigFailMsg.LoadString(IDS_CONFIGFILEFAIL);
		ShowNotifyBalloon(szConfigFailMsg);
	}	// end of ' if(m_cfmgr->Open(true))
	else
	{
		m_SettingDlg.LoadConfig(m_cfmgr);
	}

	m_pEmInput = new CEmInput();
	m_pEmInput->SetBeaconPort(m_BeaconPort);
	m_pEmInput->SetRecvFramePort(m_RecvPort);
	m_pEmInput->SetInputFilter(&m_SettingDlg);
	m_pEmInput->SetMsgQueue(&m_qConnMsg);

	OnNotifyiconStart();

	this->GetWindowPlacement(&m_wndpl);
	this->MoveWindow(0,0,0,0);
	Start();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGontrolPCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGontrolPCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGontrolPCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGontrolPCDlg::OnDestroy()
{
	this->RaiseExit();
	delete m_pEmInput;
	Stop();
	Shell_NotifyIcon(NIM_DELETE,&m_nticond);
	m_cfmgr->Close();
	m_cfmgr->Release();
	CDialog::OnDestroy();
}

void CGontrolPCDlg::OnNcPaint()
{
	if (m_fHide)
	{
		ShowWindow(SW_HIDE);
	}
	// TODO: Add your message handler code here
	// Do not call CDialog::OnNcPaint() for painting messages
}

LRESULT CGontrolPCDlg::OnNotifyIcon(WPARAM wParam,LPARAM lParam)
{
	CMenu NTIconMenu;
	switch(lParam)
	{
	case WM_LBUTTONDBLCLK:
		OnNotifyiconSetting();
		break;
	case WM_RBUTTONDOWN:
		this->SetForegroundWindow();
		
		NTIconMenu.Attach(LoadMenu(CLangRes::GetInstance()->GetResourceDll(),MAKEINTRESOURCE(IDR_NTICON_MENU)));
		//NTIconMenu.LoadMenu(IDR_NTICON_MENU);
		//NTIconMenu.DestroyMenu();
		//NTIconMenu.LoadMenu(IDR_NTICON_MENU);
		POINT MousePoint;
		GetCursorPos(&MousePoint);
		NTIconMenu.GetSubMenu(0)->SetDefaultItem(ID_NOTIFYICON_SETTING);
		if (m_fStart)
		{
			NTIconMenu.GetSubMenu(0)->EnableMenuItem(ID_NOTIFYICON_START,TRUE);
			NTIconMenu.GetSubMenu(0)->EnableMenuItem(ID_NOTIFYICON_STOP,FALSE);
		}
		else
		{
			NTIconMenu.GetSubMenu(0)->EnableMenuItem(ID_NOTIFYICON_START,FALSE);
			NTIconMenu.GetSubMenu(0)->EnableMenuItem(ID_NOTIFYICON_STOP,TRUE);
		}
		NTIconMenu.GetSubMenu(0)->TrackPopupMenu(TPM_RIGHTBUTTON ,MousePoint.x,MousePoint.y,this);
		this->PostMessage(WM_NULL,0,0);
		break;
	//case WM_LBUTTONDOWN:
	//	this->SetWindowPlacement(&m_wndpl);
	//	break;
	}
	return 0;
}

void CGontrolPCDlg::OnNotifyiconExit()
{
	OnCancel();
}

void CGontrolPCDlg::OnNotifyiconStart()
{
	if (m_pEmInput->Start())
	{
		CString szStartErrorMsg;
		szStartErrorMsg.LoadString(m_pLangRes->GetResourceDll(),IDS_STARTERRORMSG);
		ShowNotifyBalloon(szStartErrorMsg,20000);
	}
	else
	{
		CString szStartErrorMsg;
		szStartErrorMsg.LoadString(m_pLangRes->GetResourceDll(),IDS_WAITCONNECT);
		SetNotifyIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME)));
		ShowNotifyBalloon(szStartErrorMsg,20000);
		m_fStart = true;
	}
}

void CGontrolPCDlg::OnUpdateNotifyiconStart(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_fStart);
}

void CGontrolPCDlg::OnNotifyiconStop()
{
	m_pEmInput->Stop();
	SetNotifyIcon(LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINSTOP)));
	m_fStart = false;
}

void CGontrolPCDlg::OnUpdateNotifyiconStop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_fStart);
}

void CGontrolPCDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMERID_HIDEWINDOW)
	{
		ShowWindow(SW_HIDE);
		KillTimer(TIMERID_HIDEWINDOW);
	}
	CDialog::OnTimer(nIDEvent);
}

void CGontrolPCDlg::SetNotifyIcon(HICON hIcon)
{
	m_nticond.uFlags &= ~NIF_INFO;
	m_nticond.uTimeout = 0;
	m_nticond.szInfo[0] = NULL;
	m_nticond.hIcon = hIcon;
	Shell_NotifyIcon(NIM_MODIFY,&m_nticond);
}

void CGontrolPCDlg::SetNotifyTip(const TCHAR * szTip)
{
	m_nticond.uFlags &= ~NIF_INFO;
	m_nticond.uTimeout = 0;
	m_nticond.szInfo[0] = NULL;
	_tcscpy_s(m_nticond.szTip,_countof(m_nticond.szTip),szTip);
	m_nticond.szTip[_countof(m_nticond.szTip) - 1] = NULL;
	Shell_NotifyIcon(NIM_MODIFY,&m_nticond);
}

void CGontrolPCDlg::ShowNotifyBalloon(const TCHAR * szInfo, UINT timeout)
{
	m_nticond.uFlags |= NIF_INFO;
	m_nticond.uTimeout = timeout;
	_tcscpy_s(m_nticond.szInfo,_countof(m_nticond.szInfo),szInfo);
	m_nticond.szInfo[_countof(m_nticond.szInfo) - 1] = NULL;
	Shell_NotifyIcon(NIM_MODIFY,&m_nticond);
}

//bool CGontrolPCDlg::OSVersion6()
//{
//	OSVERSIONINFOEX osvi;
//	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
//	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
//	GetVersionEx ((OSVERSIONINFO *) &osvi);
//	if (osvi.dwMajorVersion >= 6)
//	{
//		return true;
//	}
//	return false;
//}
//
//BOOL CGontrolPCDlg::TryElvtPriv(const TCHAR * szMessage)
//{
//	if (!OSVersion6())
//	{
//		return TRUE+2;
//	}
//	CString szElvtPrivAsk;
//	szElvtPrivAsk.LoadString(IDS_ELVTPRIVASK);
//	if (MessageBox(szElvtPrivAsk,szMessage,MB_YESNO|MB_ICONQUESTION) == IDYES)
//	{
//		int (*pfnNeedElevatePriv)();
//		int (*pfnCreateElevatePrivProcess)();
//		BOOL rtVal = FALSE;
//		HMODULE hModule = LoadLibrary(_T("ElvtPriv.dll"));
//		if (hModule)
//		{
//			pfnNeedElevatePriv = 
//				(int (*)())GetProcAddress(hModule,"NeedElevatePriv");
//			pfnCreateElevatePrivProcess = 
//				(int (*)())GetProcAddress(hModule,"CreateElevatePrivProcess");
//			if (pfnNeedElevatePriv && pfnNeedElevatePriv()
//				&& pfnCreateElevatePrivProcess )
//			{
//				int ElvtResult = pfnCreateElevatePrivProcess();
//				switch (ElvtResult)
//				{
//				case 0:
//					rtVal = TRUE;
//					break;
//				case -1:
//					rtVal = TRUE+1;
//					break;
//				default:
//					rtVal = FALSE;
//				}
//			}
//			FreeLibrary(hModule);
//		}	//end of 'if (hModule)
//		return rtVal;
//	}
//	else
//	{
//		return TRUE+1;
//	}
//}

void CGontrolPCDlg::OnNotifyiconSetting()
{
	if (!m_SettingDlg.IsShown())
	{
		m_SettingDlg.DoModal();
	}
	else
	{
		m_SettingDlg.SetFocus();
	}
}

int CGontrolPCDlg::Start()
{
	return IMyThread::Start();
}

unsigned int __stdcall CGontrolPCDlg::sNotifyStatusThread(void * p)
{
	CGontrolPCDlg * pThis = dynamic_cast<CGontrolPCDlg *>(reinterpret_cast<IMyThread *>(p));
	if (!pThis)
	{
#ifdef _DEBUG
		::OutputDebugString(_T("RecvFrame: Thread this Pointer Error"));
#endif
		return -1;
	}
	while (!pThis->m_fExit)
	{
		try
		{
			ConnMsg msg = pThis->m_qConnMsg.Take(THREADWAIT);
			CString NotifyMessage;
			NotifyMessage.LoadString(pThis->m_pLangRes->GetResourceDll(),msg.StringID);
			NotifyMessage += _T("\n");
			NotifyMessage += msg.szData;
			pThis->ShowNotifyBalloon(NotifyMessage);
		}
		catch (TimeoutException &)
		{}
	}
	return 0;
}