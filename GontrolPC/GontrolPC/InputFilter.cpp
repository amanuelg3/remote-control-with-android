#include "stdafx.h"
#include "InputFilter.h"
#include "math.h"

CInputFilter::CInputFilter()
{
	m_iMoveSpeed = DEFAULT_MOVESPEED;
	m_iLoadMoveSpeed = -1;
	//m_tempcfmgr = CreateConfigMgr(INISTORE);
	//TCHAR szTempPath[MAX_PATH];
	//GetTempPath(MAX_PATH,szTempPath);
	//GetTempFileName(szTempPath,_T("Gontrol"),0,m_szTempFileName);
	//m_tempcfmgr->SetTag(m_szTempFileName);
	//m_tempcfmgr->Open(true);
	RecalcDepend();
}

CInputFilter::~CInputFilter()
{

}
void CInputFilter::SetConfig(IConfigMgr * cfmgr)
{
	m_cfmgr = cfmgr;
}

void CInputFilter::LoadConfig()
{
	int iTemp;
	if (!m_cfmgr->GetKeyValueInt(L"MoveSpeed",&iTemp))
	{
		m_iMoveSpeed = m_iLoadMoveSpeed = iTemp; 
	}
	RecalcDepend();
}

int CInputFilter::SaveConfig()
{
	int rtVal = ERROR_FILE_NOT_FOUND;
	if (m_cfmgr)
	{
		rtVal = 0;
		if (m_iMoveSpeed != m_iLoadMoveSpeed)
		{
			rtVal |= m_cfmgr->SetKeyValueInt(L"MoveSpeed",m_iMoveSpeed);
		}
	}
	RecalcDepend();
	//if (rtVal)
	//{
	//	m_tempcfmgr->SetKeyValueInt(L"MoveSpeed",m_iMoveSpeed);
	//}
	return rtVal;
}

void CInputFilter::LoadConfig(IConfigMgr * cfmgr)
{
	SetConfig(cfmgr);
	LoadConfig();
}

void CInputFilter::RecalcDepend()
{
	m_fMoveFactor = 1.0f / (4.0f - (4.0f-0.25f) * (float)m_iMoveSpeed/100.0f);//  logf(m_iMoveSpeed + 1.01f);
}


int CInputFilter::AdjustMove(int delta, float * pfAcc)
{
	float fd = delta * m_fMoveFactor;
	if (*pfAcc >= 0 && fd > 0
		|| *pfAcc <= 0 && fd < 0)
	{
		fd += *pfAcc;
		*pfAcc = 0;
	}
	else
	{
		if (abs(*pfAcc) < abs(fd))
		{
			fd -= *pfAcc;
			*pfAcc = 0;
		}
		else
		{
			fd = 0;
			*pfAcc -= fd;
		}
	}
	delta = (int)fd;
	*pfAcc += fd - (float)delta;
	return delta;
}

void CInputFilter::ProcessData(std::vector<INPUT> &v)
{
	for (unsigned i = 0; i < v.size(); i++)
	{
		if (v[i].type == INPUT_MOUSE && 
			(v[i].mi.dwFlags & MOUSEEVENTF_MOVE) &&
			!(v[i].mi.dwFlags & MOUSEEVENTF_ABSOLUTE))
		{
			v[i].mi.dx = AdjustMove(v[i].mi.dx,&m_fAccMoveX);
			v[i].mi.dy = AdjustMove(v[i].mi.dy,&m_fAccMoveX);

		}//end of mouse move data

	}
}