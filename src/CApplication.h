#ifndef SRC_CAPPLICATION_H_
#define SRC_CAPPLICATION_H_

#include "CAccepter.h"
#include "CSingleton.h"

class CApplication : public util::SSingleton<CApplication>
{
public:
	CApplication();
	~CApplication();
	bool onInit(int argc,char *argv[]);
	bool onRun();
	bool onStop();
	bool onInstallationPauseSign();
	static void	onHandleSign(int nSign);

public:
	static  bool	g_staic_run;

private:
	int 	_uiAccepterNum;
	std::vector<CAccepter*>		_pAccepters;
};

#define gApplication (CApplication::Instance())
#endif /* SRC_CAPPLICATION_H_ */
