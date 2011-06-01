#ifndef CONFIGMGR_H_
#define CONFIGMGR_H_

#ifndef CONFIGMGR_EXPORTS
#ifdef VSENVIRON
#ifdef _DEBUG
#pragma comment(lib,"..\\DEBUG\\ConfigMgr.lib")
#else
#pragma comment(lib,"..\\RELEASE\\ConfigMgr.lib")
#endif
#endif
#define CONFIGMGR_API extern "C" __declspec(dllimport)
#else
#define CONFIGMGR_API extern "C" __declspec(dllexport)
#endif


class I
{
public:
	virtual ULONG AddRef() = 0;
	virtual ULONG Release() = 0;
};

#define DECLARE_REF \
	public: \
	virtual ULONG AddRef(); \
	virtual ULONG Release(); \
	private: \
	ULONG _m_ulCount;

#define DEFINE_REF(CLASSNAME) \
	ULONG CLASSNAME::AddRef() \
	{ \
		return ++_m_ulCount; \
	} \
	ULONG CLASSNAME::Release() \
	{ \
		ULONG rt = --_m_ulCount; \
		if (!rt) \
		{	\
			delete this; \
		}	\
		return rt; \
	}

#define INIT_REF \
	_m_ulCount = 1



class IConfigMgr : public I
{
public:
	virtual void SetTag(const wchar_t * wszTag) = 0;
	virtual const wchar_t * GetTag() const = 0;
	virtual int Open(bool fCreate) = 0;
	virtual int Close() = 0;
	virtual int SetKeyValueString(const wchar_t * wszKey, 
		const wchar_t * wszValue) = 0;
	virtual int SetKeyValueInt(const wchar_t * wszKey, 
		int Value) = 0;
	virtual int GetKeyValueString(const wchar_t * wszKey,
		wchar_t * wszValue, int * pLength) const = 0;
	virtual int GetKeyValueInt(const wchar_t * wszKey,
		int * pValue) const = 0;
};

enum ConfigStoreType{INISTORE = 0, REGSTORE};
CONFIGMGR_API
IConfigMgr * CreateConfigMgr(int type);

#endif