#ifndef LANGRES_H_
#define LANGRES_H_

class CLangRes
{
public:
	static CLangRes * GetInstance();
	LANGID LangNameToID(const std::wstring & szLangName);
	const wchar_t * LangIDToName(LANGID id);
	void SetLang(LANGID id);
	void SetLang(const std::wstring & szLangName);
	HMODULE GetResourceDll();
	LANGID GetLangID();
private:
	int LangIDToIndex(LANGID id);
	static const LANGID LANGID_EN = 0x0c09;
	static const LANGID LANGID_ZH = 0x0804;
	static const LANGID LANGID_ES = 0x0c0a;
	static const LANGID LANGID_FR = 0x040c;
	static const LANGID LANGID_DE = 0x0407;
	static const LANGID LANGID_RU = 0x0419;
	static const LANGID LANGID_PT = 0x0816;
	static const LANGID LANGID_JA = 0x0411;

	static const LANGID DEFAULT_LANGID = LANGID_EN;
	static const TCHAR * m_szDllDir;
	static const TCHAR * m_szDllPrefix;
	CLangRes();
	CLangRes(const CLangRes &);
	const CLangRes & operator=(const CLangRes &);
	std::map<std::wstring,LANGID> m_mapLangNameToID;

	struct LangSymbol
	{
		LANGID langid;
		TCHAR * tszSymbol[4];
		wchar_t wszLangName[32];
	};
	std::vector<LangSymbol> s_vSymbols;
	HMODULE m_hResourceDll;
	LANGID m_langid;
};


#endif