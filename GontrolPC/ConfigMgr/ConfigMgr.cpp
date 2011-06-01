// ConfigMgr.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


#include "stdafx.h"
#include "ConfigMgr.h"
#include "IniConfigMgr.h"

IConfigMgr * CreateConfigMgr(int type)
{
	IConfigMgr * pConfigMgr = NULL;  
	switch (type)
	{
	case INISTORE:
		pConfigMgr = new CIniConfigMgr();
		break;
	case REGSTORE:
		pConfigMgr = NULL;
		break;
	}
	return pConfigMgr;
}