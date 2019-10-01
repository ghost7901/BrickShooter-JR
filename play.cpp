#include "common.h"
#include "play.h"
#include "brick.h"
#include "cell.h"
#include "explode.h"
#include "gui.h"
#include <gst.h>
#include <gstgui.h>
#include <gstfont.h>
#include <gstparticle.h>

extern GST *g_pGST;
extern int g_nLevel;
extern int g_nScore;
extern int g_nGState;
extern int g_nRotateCount;
extern gstSprite *g_sprTipsDlg;
extern gstGUI *g_pTipsGUI;
extern gstGUI *g_pPlayGUI;
extern gstGUI *g_pHighScoreGUI;
extern gstFont *g_fntChar;
extern HEFFECT g_sndLevelUp;
extern HEFFECT g_sndGameOver;
extern gstSprite *g_sprBgd;
extern gstSprite *g_sprGameOver;
extern gstFont *g_fntDigi;
extern bool g_bSound;
extern bool g_bMoved;
extern gstParticleSystem *g_parLevelUp;

bool PlayLoop()
{	
	g_sprBgd->Render(0,0);
	g_fntDigi->printf(65,37,GSTTEXT_CENTER,"%d",g_nScore);
	g_fntDigi->printf(65,97,GSTTEXT_CENTER,"%d",g_nLevel);

	BrickUpdate(g_pGST->Timer_GetDelta());
	ExplodeUpdate(g_pGST->Timer_GetDelta());

	if(g_nGState!=GS_ROTATE1 && g_nGState!=GS_ROTATE2) BrickRender();
	ExplodeRender();	
	
	if(CellIsClear())
	{
		if(ExplodeFinished())
		{
			if(g_bSound) g_pGST->Effect_Play(g_sndLevelUp);
			g_nScore+=50*g_nLevel++;
			
			PlayInit();
			ExplodeAdd(300,300,g_parLevelUp);
		}
	}
	else if(!g_bMoved && g_nGState==GS_PLAY && PlayIsOver())
	{
		if(g_bSound) g_pGST->Effect_Play(g_sndGameOver);
		g_nGState=GS_GAMEOVER;			
	}

	return false;
}

bool PlayInit()
{	
	CellInit();
	BrickReset();
	
	g_nRotateCount+=ROTATECOUNT;
	if(g_nRotateCount>0)
	{
		g_pPlayGUI->EnableCtrl(BTN_ROTATE1,true);
		g_pPlayGUI->EnableCtrl(BTN_ROTATE2,true);
	}

	return true;
}

bool PlayRelease()
{
	BrickRelease();
	DeleteFile("./Save/Undo.bks");
	return false;
}

bool PlayIsOver()
{
	for(int cx=4;cx<12;cx++)
	{
		if(CELLTYPE(cx,4)==CO_CELL)
		{
			for(int cy=5;cy<12;cy++)
			{
				if(CELLTYPE(cx,cy)!=CO_CELL) return false;
			}
		}
	}

	for(int cx=4;cx<12;cx++)
	{
		if(CELLTYPE(cx,11)==CO_CELL)
		{
			for(int cy=10;cy>3;cy--)
			{
				if(CELLTYPE(cx,cy)!=CO_CELL) return false;
			}
		}
	}
	
	for(int cy=4;cy<12;cy++)
	{
		if(CELLTYPE(4,cy)==CO_CELL)
		{
			for(int cx=5;cx<12;cx++)
			{
				if(CELLTYPE(cx,cy)!=CO_CELL) return false;
			}
		}
	}

	for(int cy=4;cy<12;cy++)
	{
		if(CELLTYPE(11,cy)==CO_CELL)
		{
			for(int cx=10;cx>3;cx--)
			{
				if(CELLTYPE(cx,cy)!=CO_CELL) return false;
			}
		}
	}

	return true;
}