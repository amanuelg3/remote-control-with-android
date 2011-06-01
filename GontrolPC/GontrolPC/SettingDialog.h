#pragma once

#include "InputFilter.h"
#include "afxwin.h"
// CSettingDialog dialog
#include "LangRes.h"

class CSettingDialog : public CDialog, public CInputFilter
{
	DECLARE_DYNAMIC(CSettingDialog)

public:
	CSettingDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingDialog();
	virtual void LoadConfig();
	virtual void LoadConfig(IConfigMgr * cfmgr);
// Dialog Data
	enum { IDD = IDD_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdrawSlidermovespeed(NMHDR *pNMHDR, LRESULT *pResult);
public:
	virtual BOOL OnInitDialog();
	virtual BOOL IsShown();
protected:
	CLangRes * pLangRes;
	BOOL m_fShown;
	int m_langid;
	CComboBox m_comboLanguage;
	virtual void OnOK();
public:
	afx_msg void OnDestroy();
private:
	CString m_szLanguage;
};
