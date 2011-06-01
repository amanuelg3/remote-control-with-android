#ifndef INICONFIGMGR_H_
#define INICONFIGMGR_H_

class CIniConfigMgr : public IConfigMgr
{
public:
	CIniConfigMgr();
	virtual ~CIniConfigMgr();
	virtual void SetTag(const wchar_t * wszTag);
	virtual const wchar_t * GetTag() const;
	virtual int Open(bool fCreate);
	virtual int Close();
	virtual int SetKeyValueString(const wchar_t * wszKey, 
		const wchar_t * wszValue);
	virtual int SetKeyValueInt(const wchar_t * wszKey, 
		int Value);
	virtual int GetKeyValueString(const wchar_t * wszKey,
		wchar_t * wszValue, int * pLength) const;
	virtual int GetKeyValueInt(const wchar_t * wszKey,
		int * pValue) const;
	DECLARE_REF
private:
	wchar_t m_wszTag[MAX_PATH];
	std::map<std::wstring,std::wstring> m_mapKeyValue;
	bool m_fModified;
};

#endif