#ifndef FRAMECNVT_H_
#define FRAMECNVT_H_

class CFrameCnvt : public ISAXContentHandler
{
public:
	CFrameCnvt();
	~CFrameCnvt();
	int Convert(const char * frame, int length);
	void Reset();
	const std::vector<INPUT> & GetResult() const;
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

	virtual ULONG STDMETHODCALLTYPE AddRef( void);
	virtual ULONG STDMETHODCALLTYPE Release( void);
	//virtual HRESULT __stdcall putDocumentLocator (
	///*[in]*/ struct ISAXLocator * pLocator );
	//virtual HRESULT __stdcall startDocument ( );
	//virtual HRESULT __stdcall endDocument ( );
	//virtual HRESULT __stdcall startPrefixMapping (
	//	/*[in]*/ unsigned short * pwchPrefix,
	//	/*[in]*/ int cchPrefix,
	//	/*[in]*/ unsigned short * pwchUri,
	//	/*[in]*/ int cchUri );
	//virtual HRESULT __stdcall endPrefixMapping (
	//	/*[in]*/ unsigned short * pwchPrefix,
	//	/*[in]*/ int cchPrefix );
	//virtual HRESULT __stdcall startElement (
	//	/*[in]*/ unsigned short * pwchNamespaceUri,
	//	/*[in]*/ int cchNamespaceUri,
	//	/*[in]*/ unsigned short * pwchLocalName,
	//	/*[in]*/ int cchLocalName,
	//	/*[in]*/ unsigned short * pwchQName,
	//	/*[in]*/ int cchQName,
	///*[in]*/ struct ISAXAttributes * pAttributes );
	//virtual HRESULT __stdcall endElement (
	//	/*[in]*/ unsigned short * pwchNamespaceUri,
	//	/*[in]*/ int cchNamespaceUri,
	//	/*[in]*/ unsigned short * pwchLocalName,
	//	/*[in]*/ int cchLocalName,
	//	/*[in]*/ unsigned short * pwchQName,
	//	/*[in]*/ int cchQName );
	//virtual HRESULT __stdcall characters (
	//	/*[in]*/ unsigned short * pwchChars,
	//	/*[in]*/ int cchChars );
	//virtual HRESULT __stdcall ignorableWhitespace (
	//	/*[in]*/ unsigned short * pwchChars,
	//	/*[in]*/ int cchChars );
	//virtual HRESULT __stdcall processingInstruction (
	//	/*[in]*/ unsigned short * pwchTarget,
	//	/*[in]*/ int cchTarget,
	//	/*[in]*/ unsigned short * pwchData,
	//	/*[in]*/ int cchData );
	//virtual HRESULT __stdcall skippedEntity (
	//	/*[in]*/ unsigned short * pwchName,
	//	/*[in]*/ int cchName );
	virtual HRESULT STDMETHODCALLTYPE putDocumentLocator( 
		/* [in] */ ISAXLocator *pLocator);

	virtual HRESULT STDMETHODCALLTYPE startDocument( void);

	virtual HRESULT STDMETHODCALLTYPE endDocument( void);

	virtual HRESULT STDMETHODCALLTYPE startPrefixMapping( 
		/* [in] */ const wchar_t *pwchPrefix,
		/* [in] */ int cchPrefix,
		/* [in] */ const wchar_t *pwchUri,
		/* [in] */ int cchUri);

	virtual HRESULT STDMETHODCALLTYPE endPrefixMapping( 
		/* [in] */ const wchar_t *pwchPrefix,
		/* [in] */ int cchPrefix);

	virtual HRESULT STDMETHODCALLTYPE startElement( 
		/* [in] */ const wchar_t *pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ const wchar_t *pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ const wchar_t *pwchQName,
		/* [in] */ int cchQName,
		/* [in] */ ISAXAttributes *pAttributes);

	virtual HRESULT STDMETHODCALLTYPE endElement( 
		/* [in] */ const wchar_t *pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ const wchar_t *pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ const wchar_t *pwchQName,
		/* [in] */ int cchQName);

	virtual HRESULT STDMETHODCALLTYPE characters( 
		/* [in] */ const wchar_t *pwchChars,
		/* [in] */ int cchChars);

	virtual HRESULT STDMETHODCALLTYPE ignorableWhitespace( 
		/* [in] */ const wchar_t *pwchChars,
		/* [in] */ int cchChars);

	virtual HRESULT STDMETHODCALLTYPE processingInstruction( 
		/* [in] */ const wchar_t *pwchTarget,
		/* [in] */ int cchTarget,
		/* [in] */ const wchar_t *pwchData,
		/* [in] */ int cchData);

	virtual HRESULT STDMETHODCALLTYPE skippedEntity( 
		/* [in] */ const wchar_t *pwchName,
		/* [in] */ int cchName);

private:
	ISAXXMLReader * m_pRdr;
	ULONG m_ulRefCount;
	std::vector<INPUT> m_vOutInputs;
	static const wchar_t s_Uri[];
};


#endif