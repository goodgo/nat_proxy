#include "CApplication.h"

int main(int argc ,char *argv[])
{
	if (gApplication->onInit(argc, argv))
	{
		gApplication->onRun();
		while (CApplication::g_staic_run)
			sleep(1);

		gApplication->onStop();
	}

	DEBUGINFO("Server exit.");
	return 0;
}
