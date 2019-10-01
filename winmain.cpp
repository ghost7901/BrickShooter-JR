#include <gst.h>
#include "gamemain.h"
#include "resource.h"

extern GST *g_pGST;
//extern bool g_bFullScreen;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	g_pGST=gstCreate(GST_VERSION);
	
	g_pGST->System_SetState(GST_INIFILE,"./config.ini");
	IniLoad();
	g_pGST->System_SetState(GST_FRAMEFUNC,GameLoop);
	g_pGST->System_SetState(GST_FOCUSLOSTFUNC,FocusLost);
	g_pGST->System_SetState(GST_FOCUSGAINFUNC,FocusGain);
	g_pGST->System_SetState(GST_WINDOWED,g_pGST->Ini_GetInt("setup","FullScreen",1)==1 ? false : true);
	g_pGST->System_SetState(GST_SCREENWIDTH,800);
	g_pGST->System_SetState(GST_SCREENHEIGHT,600);
	g_pGST->System_SetState(GST_SCREENBPP,32);
	//g_pGST->System_SetState(GST_FPS,100);
	g_pGST->System_SetState(GST_TITLE,"BrickShooter Jr. Made by ghost79 (Îâ´º)");
	g_pGST->System_SetState(GST_LOGFILE,"GameLog.log");
	g_pGST->System_SetState(GST_ICON,MAKEINTRESOURCE(IDI_ICON1));
//	g_pGST->System_SetState(GST_SHOWSPLASH,false);

	if(g_pGST->System_Initiate() && GameInitiate())
	{
		g_pGST->System_Start();
	}

	GameRelease();
	g_pGST->System_Shutdown();
	g_pGST->Release();
	return 0;
}