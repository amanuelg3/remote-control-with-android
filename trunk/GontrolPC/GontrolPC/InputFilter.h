#ifndef INPUTFILTER_H_
#define INPUTFILTER_H_

class CInputFilter
{
public:
	CInputFilter();
	virtual ~CInputFilter();
	virtual void SetConfig(IConfigMgr * cfmgr);
	virtual void LoadConfig();
	virtual void LoadConfig(IConfigMgr * cfmgr);
	virtual int SaveConfig(); 
	virtual void ProcessData(std::vector<INPUT> & v);
	virtual void RecalcDepend();

protected:
	int m_iMoveSpeed;
	float m_fMoveFactor;
	float m_fAccMoveX;
	float m_fAccMoveY;
	IConfigMgr * m_cfmgr;

	int m_iLoadMoveSpeed;
private:
	static const int DEFAULT_MOVESPEED = 75; 
	int AdjustMove(int delta, float * pfAcc);
//	TCHAR m_szTempFileName[MAX_PATH];
//	IConfigMgr * m_tempcfmgr;
};


#endif