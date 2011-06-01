#include "stdafx.h"
#include "ConfigMgr.h"
#include "IniConfigMgr.h"
#ifdef _DEBUG
#include <stdio.h>
#endif
using namespace std;

CIniConfigMgr::CIniConfigMgr()
{
	ZeroMemory(m_wszTag,sizeof(wchar_t)*MAX_PATH);
	m_fModified = false; 
	INIT_REF;
}

CIniConfigMgr::~CIniConfigMgr()
{
	Close();
}

void CIniConfigMgr::SetTag(const wchar_t * wszTag)
{
	wcscpy_s(m_wszTag,MAX_PATH - 1, wszTag);
}

const wchar_t * CIniConfigMgr::GetTag() const
{
	return m_wszTag;
}

int CIniConfigMgr::Open(bool fCreate)
{
	Close();
	wchar_t wszFileName[MAX_PATH];
	wcscpy_s(wszFileName,MAX_PATH,m_wszTag);
	if (!wcschr(wszFileName,L'.'))
	{
		wcscat_s(wszFileName,MAX_PATH,L".ini");
	}
	FILE * fpconfig = NULL;
	const int BUFSIZE = 4096;
	wchar_t wszBuffer[BUFSIZE];
	wstring key;
	wstring value;
	int state = 0;
	m_mapKeyValue.clear();
	m_fModified = false; 
	bool fbreak = false;
	if (!_wfopen_s(&fpconfig,wszFileName,L"r")) 
	{
		int i = 0;
		while (!fbreak) 
		{
			if (WEOF == (wszBuffer[i] = fgetwc(fpconfig)))
			{
				wszBuffer[i] = L'\0';
				fbreak = true;
			}
			if (state == 1 && (wszBuffer[i] == L'\0' || wszBuffer[i] == L';' || wszBuffer[i] == L'\n'))
			{
				wszBuffer[i] = 0;
				while (wszBuffer[--i] == L' ')
				{
					wszBuffer[i] = 0;
				}
				value = wszBuffer;
				pair<map<wstring,wstring>::iterator,bool> rs = 
					m_mapKeyValue.insert(make_pair<wstring,wstring>(key,value));
				if (!rs.second)
				{
					rs.first->second = value;
				}
#ifdef _DEBUG
				wprintf(L"%s=%s\n",key.c_str(),value.c_str());
#endif

				if (wszBuffer[i] == L';')
				{
					state = 3;
				}
				else
				{
					state = 0;
				}
				i = 0;
				continue;
			}
			if (wszBuffer[i] >= L' ' && wszBuffer[i] < 0x7F)
			{
				if (!i && wszBuffer[i] == L' ')
				{
					continue;
				}
				if (wszBuffer[i] == L'=' && !state)
				{
					wszBuffer[i] = 0;
					while (wszBuffer[--i] == L' ')
					{
						wszBuffer[i] = 0;
					}
					key = wszBuffer;
					i = 0;
					state = 1;
				}	
				else
				{
					i++;
					if (i >= BUFSIZE)
					{
						i--;
					}
				}
			}
			else if (wszBuffer[i] == L'\n')
			{
				i = 0;
				state = 0;
			}
		}
		fclose(fpconfig);
		return 0;
	}
	if (fCreate)
	{
		if (_wfopen_s(&fpconfig,wszFileName,L"w"))
		{
			wchar_t * namesplitter = wcsrchr(wszFileName,L'\\');
			*namesplitter = NULL;
			_wmkdir(wszFileName);
			*namesplitter = L'\\';
			if (_wfopen_s(&fpconfig,wszFileName,L"w"))
			{
				return ERROR_ACCESS_DENIED;
			}
		}
		fclose(fpconfig);
		return 0;
	}
	return ERROR_FILE_NOT_FOUND;
}

int CIniConfigMgr::Close()
{
	if (m_fModified)
	{
		wchar_t wszFileName[MAX_PATH];
		wcscpy_s(wszFileName,MAX_PATH,m_wszTag);
		wcscat_s(wszFileName,MAX_PATH,L".ini");
		FILE * fpconfig = NULL;
		wstring wszTemp;
		const wchar_t * wszTempBuffer;
		int length;
		int i;
		if (!_wfopen_s(&fpconfig,wszFileName,L"w")) 
		{
			map<wstring,wstring>::iterator iter = m_mapKeyValue.begin();
			map<wstring,wstring>::iterator iterend = m_mapKeyValue.end();
			while (iter != iterend)
			{
				wszTemp = iter->first;
				wszTempBuffer = wszTemp.c_str();
				length = wszTemp.length();
				i = 0;
				while (i < length)
				{
					fputwc(wszTempBuffer[i],fpconfig);
					i++;
				}
				fputwc(L'=',fpconfig);

				wszTemp = iter->second;
				wszTempBuffer = wszTemp.c_str();
				length = wszTemp.length();
				i = 0;
				while (i < length)
				{
					fputwc(wszTempBuffer[i],fpconfig);
					i++;
				}
				fputwc(L'\n',fpconfig);
				iter++;
			}
			fclose(fpconfig);
			return 0;
		}
		return ERROR_ACCESS_DENIED;
	}
	return 0;
}

int CIniConfigMgr::SetKeyValueString(const wchar_t * wszKey, 
		const wchar_t * wszValue)
{
	wstring key = wszKey;
	wstring value = wszValue;
	map<wstring,wstring>::iterator iter = m_mapKeyValue.find(key);
	if (iter == m_mapKeyValue.end())
	{
		m_mapKeyValue.insert(make_pair<wstring,wstring>(key,value));
	}
	else
	{
		iter->second = value;
	}
	m_fModified = true;
	return 0;
}

int CIniConfigMgr::SetKeyValueInt(const wchar_t * wszKey, 
		int Value)
{
	wchar_t wszTemp[MAX_PATH];
	wstring key = wszKey;
	wstring value;
	_itow_s(Value,wszTemp,16);
	value = wszTemp;
	map<wstring,wstring>::iterator iter = m_mapKeyValue.find(key);
	if (iter == m_mapKeyValue.end())
	{
		m_mapKeyValue.insert(make_pair<wstring,wstring>(key,value));
	}
	else
	{
		iter->second = value;
	}
	m_fModified = true;
	return 0;
}

int CIniConfigMgr::GetKeyValueString(const wchar_t * wszKey,
		wchar_t * wszValue, int * pLength) const
{
	wstring key = wszKey;
	map<wstring,wstring>::const_iterator iter = m_mapKeyValue.find(key);
	if (iter == m_mapKeyValue.end())
	{
		return ERROR_FILE_NOT_FOUND;
	}
	else
	{
		if (*pLength)
		{
			if (ERANGE == wcscpy_s(wszValue,*pLength,iter->second.c_str()))
			{
				wszValue[*pLength - 1] = 0;
				*pLength = iter->second.length() + 1;
			}
		}
		else
		{
			*pLength = iter->second.length() + 1;
		}
		return 0;
	}
}

int CIniConfigMgr::GetKeyValueInt(const wchar_t * wszKey,
		int * pValue) const
{
	wstring key = wszKey;
	map<wstring,wstring>::const_iterator iter = m_mapKeyValue.find(key);
	if (iter == m_mapKeyValue.end())
	{
		return ERROR_FILE_NOT_FOUND;
	}
	else
	{
		swscanf_s(iter->second.c_str(),L"%x",pValue);
		return 0;
	}
}

DEFINE_REF(CIniConfigMgr);