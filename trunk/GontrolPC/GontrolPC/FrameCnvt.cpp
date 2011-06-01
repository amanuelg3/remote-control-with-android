#include "stdafx.h"
#include "FrameCnvt.h"

const wchar_t CFrameCnvt::s_Uri[] = {L"http://android.sse.tongji.edu.cn/ControlData"};

void CFrameCnvt::Reset()
{
	m_vOutInputs.clear();
}

int CFrameCnvt::Convert(const char * frame, int length)
{
	VARIANT varFrame;
	ZeroMemory(&varFrame,sizeof(varFrame));
	varFrame.vt = VT_ARRAY|VT_UI1;
	if (SafeArrayAllocDescriptor(1,&varFrame.parray))
	{
#ifdef _DEBUG
		::OutputDebugString(_T("CFrameCnvt: Safe Array Alloc Error"));
#endif
		return -1;
	}
	varFrame.parray->rgsabound[0].lLbound = 0;
	varFrame.parray->rgsabound[0].cElements = length-1;
	varFrame.parray->fFeatures = FADF_AUTO|FADF_FIXEDSIZE;
	varFrame.parray->cbElements = 1;
	varFrame.parray->pvData = (PVOID)frame;
	HRESULT hr = m_pRdr->parse(varFrame);
	if (hr)
	{
#ifdef _DEBUG
		::OutputDebugString(_T("CFrameCnvt: SAX2 Parse Error"));
#endif
		return -1;
	}
	SafeArrayDestroyDescriptor(varFrame.parray);
	return 0;
}

const std::vector<INPUT> & CFrameCnvt::GetResult() const
{
	return m_vOutInputs;
}

CFrameCnvt::CFrameCnvt()
{
	m_ulRefCount = 1;
	::CoInitialize(0);
	::CoCreateInstance(__uuidof(SAXXMLReader),NULL,CLSCTX_ALL,__uuidof(ISAXXMLReader),(void**)&m_pRdr);
	m_pRdr->putContentHandler(this);
}

CFrameCnvt::~CFrameCnvt()
{
	m_pRdr->Release();
	::CoUninitialize();
}


HRESULT STDMETHODCALLTYPE CFrameCnvt::QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if (riid == __uuidof(ISAXContentHandler))
	{
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}

ULONG STDMETHODCALLTYPE CFrameCnvt::AddRef( void)
{
	m_ulRefCount++;
	return m_ulRefCount;
}

ULONG STDMETHODCALLTYPE CFrameCnvt::Release( void)
{
	m_ulRefCount--;
	if (!m_ulRefCount)
	{
		delete this;
		return 0;
	}
	return m_ulRefCount;
}


HRESULT STDMETHODCALLTYPE CFrameCnvt::putDocumentLocator( 
            /* [in] */ ISAXLocator *pLocator)
{
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE CFrameCnvt::startDocument( void)
{

	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE CFrameCnvt::endDocument( void)
{

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CFrameCnvt::startPrefixMapping( 
            /* [in] */ const wchar_t *pwchPrefix,
            /* [in] */ int cchPrefix,
            /* [in] */ const wchar_t *pwchUri,
            /* [in] */ int cchUri)
{

	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE CFrameCnvt::endPrefixMapping( 
            /* [in] */ const wchar_t *pwchPrefix,
            /* [in] */ int cchPrefix)
{
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE CFrameCnvt::startElement( 
            /* [in] */ const wchar_t *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ const wchar_t *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ const wchar_t *pwchQName,
            /* [in] */ int cchQName,
            /* [in] */ ISAXAttributes *pAttributes)
{
	int cchValue;
	const wchar_t * wszValue;
	const int TMPBUFSIZE = 512;
	wchar_t wszTempBuffer[TMPBUFSIZE];
	if (!wcscmp(pwchLocalName,L"KeyboardData"))
	{
		INPUT input;
		ZeroMemory(&input,sizeof(INPUT));
		input.type = INPUT_KEYBOARD;
		m_vOutInputs.push_back(input);
	}
	else if (!wcscmp(pwchLocalName,L"MouseData"))
	{
		INPUT input;
		ZeroMemory(&input,sizeof(INPUT));
		input.type = INPUT_MOUSE;
		m_vOutInputs.push_back(input);
	}
	else if (!wcscmp(pwchLocalName,L"Move"))
	{
		MOUSEINPUT & input = m_vOutInputs[m_vOutInputs.size()-1].mi;
		input.dwFlags |= MOUSEEVENTF_MOVE;
		if (!pAttributes->getValueFromName(s_Uri, _countof(s_Uri) - 1, L"MouseX", 6, &wszValue, &cchValue ))
		{
			wcsncpy_s(wszTempBuffer,TMPBUFSIZE,wszValue,cchValue);
			cchValue = TMPBUFSIZE > cchValue ? cchValue : TMPBUFSIZE;
			wszTempBuffer[cchValue] = 0;
			input.dx = _wtoi(wszTempBuffer);
		}
		if (!pAttributes->getValueFromName(s_Uri, _countof(s_Uri) - 1, L"MouseY", 6, &wszValue, &cchValue ))
		{
			wcsncpy_s(wszTempBuffer,TMPBUFSIZE,wszValue,cchValue);
			cchValue = TMPBUFSIZE > cchValue ? cchValue : TMPBUFSIZE;
			wszTempBuffer[cchValue] = 0;
			input.dy = _wtoi(wszTempBuffer);
		}
		if (!pAttributes->getValueFromName(s_Uri, _countof(s_Uri) - 1, L"Absolute", 8, &wszValue, &cchValue ))
		{
			wcsncpy_s(wszTempBuffer,TMPBUFSIZE,wszValue,cchValue);
			cchValue = TMPBUFSIZE > cchValue ? cchValue : TMPBUFSIZE;
			wszTempBuffer[cchValue] = 0;
			if (!wcscmp(wszTempBuffer,L"true"))
			{
				input.dwFlags |= MOUSEEVENTF_ABSOLUTE;
			}
		}
	}
	else if (!wcscmp(pwchLocalName,L"ButtonDown"))
	{
		MOUSEINPUT & input = m_vOutInputs[m_vOutInputs.size()-1].mi;
		int number = 1;
		if (!pAttributes->getValueFromName(s_Uri, _countof(s_Uri) - 1, L"ButtonNumber", 12, &wszValue, &cchValue ))
		{
			wcsncpy_s(wszTempBuffer,TMPBUFSIZE,wszValue,cchValue);
			cchValue = TMPBUFSIZE > cchValue ? cchValue : TMPBUFSIZE;
			wszTempBuffer[cchValue] = 0;	
			number = _wtoi(wszTempBuffer);
		}
		switch (number)
		{
		case 5:
			input.mouseData = XBUTTON2;
			input.dwFlags |= MOUSEEVENTF_XDOWN;
			break;
		case 4:
			input.mouseData = XBUTTON1;
			input.dwFlags |= MOUSEEVENTF_XDOWN;
			break;
		case 3:
			input.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
			break;
		case 2:
			input.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
			break;
		case 1:
		default:
			input.dwFlags |= MOUSEEVENTF_LEFTDOWN;
			break;
		}
	}
	else if (!wcscmp(pwchLocalName,L"ButtonUp"))
	{
		MOUSEINPUT & input = m_vOutInputs[m_vOutInputs.size()-1].mi;
		int number = 1;
		if (!pAttributes->getValueFromName(s_Uri, _countof(s_Uri) - 1, L"ButtonNumber", 12, &wszValue, &cchValue ))
		{
			wcsncpy_s(wszTempBuffer,TMPBUFSIZE,wszValue,cchValue);
			cchValue = TMPBUFSIZE > cchValue ? cchValue : TMPBUFSIZE;
			wszTempBuffer[cchValue] = 0;	
			number = _wtoi(wszTempBuffer);
		}
		switch (number)
		{
		case 5:
			input.mouseData = XBUTTON2;
			input.dwFlags |= MOUSEEVENTF_XUP;
			break;
		case 4:
			input.mouseData = XBUTTON1;
			input.dwFlags |= MOUSEEVENTF_XUP;
			break;
		case 3:
			input.dwFlags |= MOUSEEVENTF_RIGHTUP;
			break;
		case 2:
			input.dwFlags |= MOUSEEVENTF_MIDDLEUP;
			break;
		case 0:
			input.dwFlags |= MOUSEEVENTF_VIRTUALDESK|MOUSEEVENTF_ABSOLUTE;
			break;
		case 1:
		default:
			input.dwFlags |= MOUSEEVENTF_LEFTUP;
			break;
		}
	}
	else if (!wcscmp(pwchLocalName,L"Wheel"))
	{
		MOUSEINPUT & input = m_vOutInputs[m_vOutInputs.size()-1].mi;
		input.dwFlags |= MOUSEEVENTF_WHEEL;
		if (!pAttributes->getValueFromName(s_Uri, _countof(s_Uri) - 1, L"WheelDelta", 10, &wszValue, &cchValue ))
		{
			wcsncpy_s(wszTempBuffer,TMPBUFSIZE,wszValue,cchValue);
			cchValue = TMPBUFSIZE > cchValue ? cchValue : TMPBUFSIZE;
			wszTempBuffer[cchValue] = 0;	
			input.mouseData = _wtoi(wszTempBuffer);
		}
		if (!pAttributes->getValueFromName(s_Uri, _countof(s_Uri) - 1, L"WheelType", 9, &wszValue, &cchValue ))
		{
			wcsncpy_s(wszTempBuffer,TMPBUFSIZE,wszValue,cchValue);
			cchValue = TMPBUFSIZE > cchValue ? cchValue : TMPBUFSIZE;
			wszTempBuffer[cchValue] = 0;	
			if (!wcscmp(wszTempBuffer,L"Horizontal"))
			{
				input.dwFlags &= ~MOUSEEVENTF_WHEEL;
				input.dwFlags |= MOUSEEVENTF_HWHEEL;
				VK_ESCAPE;
			}
		}
	}
	else if (!wcscmp(pwchLocalName,L"KeyDown"))
	{
		KEYBDINPUT & input = m_vOutInputs[m_vOutInputs.size()-1].ki;
		if (!pAttributes->getValueFromName(s_Uri, _countof(s_Uri) - 1, L"VirtualKey", 10, &wszValue, &cchValue ))
		{
			wcsncpy_s(wszTempBuffer,TMPBUFSIZE,wszValue,cchValue);
			cchValue = TMPBUFSIZE > cchValue ? cchValue : TMPBUFSIZE;
			wszTempBuffer[cchValue] = 0;	
			input.wVk = _wtoi(wszTempBuffer);
			input.wScan = MapVirtualKey(input.wVk,MAPVK_VK_TO_VSC);
			if ((input.wScan >> 8)== 0xE0)
			{
				input.wScan &= 0xFF;
				input.dwFlags |= KEYEVENTF_EXTENDEDKEY;
			}
		}
	
	}
	else if (!wcscmp(pwchLocalName,L"KeyUp"))
	{
		KEYBDINPUT & input = m_vOutInputs[m_vOutInputs.size()-1].ki;
		input.dwFlags |= KEYEVENTF_KEYUP;
		if (!pAttributes->getValueFromName(s_Uri, _countof(s_Uri) - 1, L"VirtualKey", 10, &wszValue, &cchValue ))
		{
			wcsncpy_s(wszTempBuffer,TMPBUFSIZE,wszValue,cchValue);
			cchValue = TMPBUFSIZE > cchValue ? cchValue : TMPBUFSIZE;
			wszTempBuffer[cchValue] = 0;	
			input.wVk = _wtoi(wszTempBuffer);
			input.wScan = MapVirtualKey(input.wVk,MAPVK_VK_TO_VSC);
			if ((input.wScan >> 8)== 0xE0)
			{
				input.wScan &= 0xFF;
				input.dwFlags |= KEYEVENTF_EXTENDEDKEY;
			}
		}

	}

	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE CFrameCnvt::endElement( 
            /* [in] */ const wchar_t *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ const wchar_t *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ const wchar_t *pwchQName,
            /* [in] */ int cchQName)
{
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE CFrameCnvt::characters( 
            /* [in] */ const wchar_t *pwchChars,
            /* [in] */ int cchChars)
{

	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE CFrameCnvt::ignorableWhitespace( 
            /* [in] */ const wchar_t *pwchChars,
            /* [in] */ int cchChars)
{

	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE CFrameCnvt::processingInstruction( 
            /* [in] */ const wchar_t *pwchTarget,
            /* [in] */ int cchTarget,
            /* [in] */ const wchar_t *pwchData,
            /* [in] */ int cchData)
{

	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE CFrameCnvt::skippedEntity( 
            /* [in] */ const wchar_t *pwchName,
            /* [in] */ int cchName)
{

	return S_OK;
}