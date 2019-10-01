#include <gstresource.h>
#include <stdio.h>
#include "common.h"
#include "gui.h"
#include "gamemain.h"
#include "play.h"
#include "brickobj.h"
#include "brick.h"

GST *g_pGST=NULL;
gstResourceManager *g_pResManager;
gstSprite *g_sprSplash;
gstSprite *g_sprLoading1;
gstSprite *g_sprLoading2;
gstSprite *g_sprAnyKey;
gstSprite *g_sprBgd;
gstSprite *g_sprMouse;
gstSprite *g_sprNewGameOn;
gstSprite *g_sprNewGameUp;
gstSprite *g_sprTipsDlg;
gstSprite *g_sprOptionDlg;
gstSprite *g_sprOkSign;
gstSprite *g_sprHelpDlg0;
gstSprite *g_sprHelpDlg1;
gstSprite *g_sprHighScoreDlg;
gstSprite *g_sprGameOver;
gstSprite *g_sprInputboxDlg;
gstSprite *g_sprSaveDlg;
gstSprite *g_sprLoadDlg;
gstAnimation *g_aniBrick[13];
gstAnimation *g_aniSplashBomb;
gstParticleSystem *g_parSplashBomb;
gstParticleSystem *g_parSplashExpl;
gstParticleSystem *g_parBomb;
gstParticleSystem *g_parExp;
gstParticleSystem *g_parLightning;
gstParticleSystem *g_parColChange;
gstParticleSystem *g_parLevelUp;
gstParticleSystem *g_parDirChange;
gstParticleManager *g_ParManager;
gstStringTable *g_pStrTab;
gstFont *g_fntChar;
gstFont *g_fntDigi;
HEFFECT g_sndBomb;
HEFFECT g_sndSplashExpl;
HEFFECT g_sndBgd;
HEFFECT g_sndShoot;
HEFFECT g_sndExpl[5];
HEFFECT g_sndLightning;
HEFFECT g_sndColChange;
HEFFECT g_sndDirChange;
HEFFECT g_sndLevelUp;
HEFFECT g_sndGameOver;
HEFFECT g_sndRotate;
HCHANNEL g_chaBomb;
HCHANNEL g_chaBgd;
gstGUI *g_pPlayGUI;
gstGUI *g_pOptionGUI;
gstGUI *g_pHelpGUI1;
gstGUI *g_pHelpGUI2;
gstGUI *g_pHighScoreGUI;
gstGUI *g_pGameOverGUI;
gstGUI *g_pEnterNameGUI;
gstGUI *g_pSaveGUI;
gstGUI *g_pLoadGUI;
//bool g_bFullScreen;
bool g_bWindowStateChanged=false;
bool g_bMusic;
bool g_bSound;
bool g_bBomb;
bool g_bArrowChanger;
bool g_bUniversal;
bool g_bLightning;
bool g_bColChange;
bool g_bPrompt;
bool g_bCanMove;
bool g_bCanUndo=false;
int g_nHelpTips=0;
int g_nScore=0;
int g_nLevel=1;
int g_nRotateCount=0;

float g_fdt=0;
float g_fMx,g_fMy;
float g_fadedt=1;

enum SYS_STATE
{
	SS_LOADING,
	SS_SPLASH,
	SS_BOMBING,
	SS_PLAY,
};

int g_nSysState=SS_LOADING;

int g_nGState=GS_PLAYSHOW;

bool FocusLost()
{
	if(g_bMusic && g_chaBgd) g_pGST->Channel_Pause(g_chaBgd);
	if(g_bSound && g_pGST->Channel_IsPlaying(g_chaBomb)) g_pGST->Channel_Pause(g_chaBomb);
	return true;
}

bool FocusGain()
{
	if(g_bMusic && g_chaBgd) g_pGST->Channel_Resume(g_chaBgd);
	if(g_bSound && !g_pGST->Channel_IsPlaying(g_chaBomb)) g_pGST->Channel_Resume(g_chaBomb);
	return true;
}

int g_nProcess;
bool GameLoop()
{
	#if _DEBUG
	if(g_pGST->Input_GetKeyState(GSTK_ESCAPE)) return true;
	#endif	

	g_pGST->Gfx_BeginScene();

	switch(g_nSysState)
	{
	case SS_LOADING:
		g_nProcess=g_pResManager->CacheProcess();
		g_sprLoading1->Render(0,0);
		g_sprLoading2->SetTextureRect(0,0,g_nProcess*800.0f/100.0f,73);
		g_sprLoading2->Render(4,135);

		if(g_nProcess==100)
		{
			ResourceInit();
			if(g_bSound) g_chaBomb=g_pGST->Effect_PlayEx(g_sndBomb,100,0,1.0f,true);
			g_nSysState=SS_SPLASH;
		}
		break;
	case SS_SPLASH:
		g_fdt+=g_pGST->Timer_GetDelta();
		if(g_pGST->Input_GetKey())
		{
			g_pGST->Channel_Stop(g_chaBomb);
			g_nSysState=SS_BOMBING;
			g_fdt=0;
			if(g_bSound) g_pGST->Effect_Play(g_sndSplashExpl);
		}
				
		g_sprSplash->Render(0,0);
		if(int(g_fdt)%2==0) g_sprAnyKey->Render(280,520);
				
		g_aniSplashBomb->Update(g_pGST->Timer_GetDelta());
		g_aniSplashBomb->Render(227,100);
				
		g_parSplashBomb->Update(g_pGST->Timer_GetDelta());
		g_parSplashBomb->Render();

		g_fntChar->SetColor(ARGB(255,255,108,0));
		g_fntChar->SetScale(1);
		g_fntChar->Render(402,257,GSTTEXT_CENTER,g_pStrTab->GetString("ProgramName"));
		g_fntChar->Render(402,342,GSTTEXT_CENTER,g_pStrTab->GetString("Author"));
		g_fntChar->SetScale(0.6f);
		g_fntChar->printf(402,317,GSTTEXT_CENTER,g_pStrTab->GetString("Info"));

		g_fntChar->SetColor(ARGB(255,255,255,255));
		g_fntChar->SetScale(1);
		g_fntChar->Render(400,255,GSTTEXT_CENTER,g_pStrTab->GetString("ProgramName"));
		g_fntChar->Render(400,340,GSTTEXT_CENTER,g_pStrTab->GetString("Author"));
		g_fntChar->SetScale(0.6f);
		g_fntChar->printf(400,315,GSTTEXT_CENTER,g_pStrTab->GetString("Info"));
		break;
	case SS_BOMBING:		
		g_parSplashExpl->Update(g_pGST->Timer_GetDelta());
		g_parSplashExpl->Render();
		
		g_fdt+=g_pGST->Timer_GetDelta();
		if(g_fdt>.5)
		{			
			if(g_fadedt<0)
			{
				g_pGST->Gfx_RenderRectangle(0,0,800,600,ARGB(255,255,255,255));
				if(g_bMusic) g_chaBgd=g_pGST->Effect_PlayEx(g_sndBgd,100,0,1.0f,true);				
				if(!PlayInit()) return true;
				g_nSysState=SS_PLAY;
				g_fadedt=1;
			}
			else
			{
				g_pGST->Gfx_RenderRectangle(0,0,800,600,ARGB((1-g_fadedt)*255,255,255,255));
			}
			g_fadedt-=1.3*g_pGST->Timer_GetDelta();
		}
		break;
	case SS_PLAY:		
		PlayLoop();
		if(g_nGState==GS_PLAYSHOW)
		{
			if(g_fadedt<0)
			{
				g_nGState=GS_PLAY;
				g_fadedt=1;				
			}
			else
			{
				g_pGST->Gfx_RenderRectangle(0,0,800,600,ARGB(g_fadedt*255,255,255,255));
			}
			g_fadedt-=0.3*g_pGST->Timer_GetDelta();
		}
		else if(g_nGState==GS_PLAY)
		{
			if(DoCommands(g_pPlayGUI->Update(g_pGST->Timer_GetDelta()))) return true;
			g_pPlayGUI->Render();			
		}
		else
		{
			switch(g_nGState)
			{
			case GS_HIGHSCORES:
				DoCommands(g_pHighScoreGUI->Update(g_pGST->Timer_GetDelta()));
				g_pHighScoreGUI->Render();
				g_fntChar->SetScale(1.2f);
				g_fntChar->Render(275,80,GSTTEXT_CENTER,"HIGH SCORES");
				g_fntChar->SetScale(1.0f);
				for(int i=1;i!=6;i++)
				{
					char temp[256];
					sprintf(temp,"HIGHSCORE%d",i);
					g_fntChar->printf(275,125+(i-1)*80,GSTTEXT_CENTER,"%d.%s",i,g_pGST->Ini_GetString(temp,"name","UnKnown"));
					g_fntChar->printf(275,160+(i-1)*80,GSTTEXT_CENTER,"%d(%d)",g_pGST->Ini_GetInt(temp,"score",0),g_pGST->Ini_GetInt(temp,"level",0));
				}
				break;
			case GS_OPTIONS:
				DoCommands(g_pOptionGUI->Update(g_pGST->Timer_GetDelta()));
				g_pOptionGUI->Render();	
				break;
			case GS_HELP1:				
				DoCommands(g_pHelpGUI1->Update(g_pGST->Timer_GetDelta()));
				g_pHelpGUI1->Render();
				break;
			case GS_HELP2:
				DoCommands(g_pHelpGUI2->Update(g_pGST->Timer_GetDelta()));
			//	g_sprHelpDlg0->SetColor(ARGB((1-ALPHA)*255,255,255,255));
				g_sprHelpDlg0->Render(100,100);
				g_pHelpGUI2->Render();
				break;
			case GS_LOAD:
				DoCommands(g_pLoadGUI->Update(g_pGST->Timer_GetDelta()));
			//	g_sprTipsDlg->Render(150,195);
			//	g_sprLoadDlg->Render(175,220);
				g_pLoadGUI->Render();
				break;
			case GS_SAVE:
				DoCommands(g_pSaveGUI->Update(g_pGST->Timer_GetDelta()));
				g_pSaveGUI->Render();
				break;
			case GS_UNDO:
				break;
			case GS_ROTATE1:
				if(!BrickRotate1()) g_nGState=GS_PLAY;
				break;
			case GS_ROTATE2:
				if(!BrickRotate2()) g_nGState=GS_PLAY;
				break;
			case GS_ENTERNAME:
				g_sprInputboxDlg->Render(3,200);
				DoCommands(g_pEnterNameGUI->Update(g_pGST->Timer_GetDelta()));
				g_pEnterNameGUI->Render();
				break;
			case GS_GAMEOVER:
				g_sprGameOver->Render(160,207);
				DoCommands(g_pGameOverGUI->Update(g_pGST->Timer_GetDelta()));
				break;
			}
		}		

		g_pGST->Input_GetMousePos(&g_fMx,&g_fMy);
		if(g_pGST->Input_IsMouseOver()) g_sprMouse->Render(g_fMx,g_fMy);
		break;
	}

	#ifdef _DEBUG
	if(g_fntDigi)
	{
	//	g_timerfont->SetColor(0xFFFFFFFF);
		g_fntDigi->printf(470,1,GSTTEXT_LEFT,"%d",g_pGST->Timer_GetFPS());
		g_fntDigi->printf(470,21,GSTTEXT_LEFT,"%f",g_pGST->Timer_GetDelta());
	}
	#endif
	g_pGST->Gfx_EndScene();

	if(g_bWindowStateChanged)
	{
		g_pGST->System_SetState(GST_WINDOWED, !g_pGST->System_GetState(GST_WINDOWED));
		g_bWindowStateChanged=false;
	}

	return false;
}

bool ResourceInit()
{
	g_sprSplash=g_pResManager->GetSprite("spSplash");
	g_sprBgd=g_pResManager->GetSprite("Background");
	g_sprMouse=g_pResManager->GetSprite("Mouse");
	g_sprTipsDlg=g_pResManager->GetSprite("tipsDlg");
	g_sprOptionDlg=g_pResManager->GetSprite("optionDlg");
	g_sprOkSign=g_pResManager->GetSprite("okSign");
	g_sprHelpDlg0=g_pResManager->GetSprite("helpDlg0");
	g_sprHelpDlg1=g_pResManager->GetSprite("helpDlg1");
	g_sprHighScoreDlg=g_pResManager->GetSprite("highScoreDlg");
	g_sprGameOver=g_pResManager->GetSprite("gameoverDlg");
	g_sprInputboxDlg=g_pResManager->GetSprite("inputboxDlg");
	g_sprSaveDlg=g_pResManager->GetSprite("saveDlg");
	g_sprLoadDlg=g_pResManager->GetSprite("loadDlg");

	g_sndBomb=g_pResManager->GetEffect("splashSndBomb");
	g_sndSplashExpl=g_pResManager->GetEffect("splashSndExpl");
	g_sndBgd=g_pResManager->GetEffect("Background");
	g_sndShoot=g_pResManager->GetEffect("Shoot");
	g_sndExpl[0]=g_pResManager->GetEffect("Expl3");
	g_sndExpl[1]=g_pResManager->GetEffect("Expl4");
	g_sndExpl[2]=g_pResManager->GetEffect("Expl5");
	g_sndExpl[3]=g_pResManager->GetEffect("Expl6");
	g_sndExpl[4]=g_pResManager->GetEffect("Expl7");
	g_sndLightning=g_pResManager->GetEffect("Lightning");
	g_sndColChange=g_pResManager->GetEffect("Colchange");
	g_sndDirChange=g_pResManager->GetEffect("DirChange");
	g_sndLevelUp=g_pResManager->GetEffect("Levelup");
	g_sndGameOver=g_pResManager->GetEffect("Gameover");
	g_sndRotate=g_pResManager->GetEffect("Rotate");

	g_aniSplashBomb=g_pResManager->GetAnimation("splashBombAnim");
	g_aniBrick[0]=g_pResManager->GetAnimation("redBrickAnim");
	g_aniBrick[1]=g_pResManager->GetAnimation("greenBrickAnim");
	g_aniBrick[2]=g_pResManager->GetAnimation("blueBrickAnim");
	g_aniBrick[3]=g_pResManager->GetAnimation("skyyellowBrickAnim");
	g_aniBrick[4]=g_pResManager->GetAnimation("yellowBrickAnim");
	g_aniBrick[5]=g_pResManager->GetAnimation("grayBrickAnim");
	g_aniBrick[6]=g_pResManager->GetAnimation("cyanBrickAnim");
	g_aniBrick[7]=g_pResManager->GetAnimation("pinkBrickAnim");
	g_aniBrick[8]=g_pResManager->GetAnimation("universalBrickAnim");
	g_aniBrick[9]=g_pResManager->GetAnimation("bombBrickAnim");		
	g_aniBrick[10]=g_pResManager->GetAnimation("lightningBrickAnim");
	g_aniBrick[11]=g_pResManager->GetAnimation("colChangeBrickAnim");
	g_aniBrick[12]=g_pResManager->GetAnimation("arrowchangerBrickAnim");
	
	g_parSplashBomb=g_pResManager->GetParticleSystem("splash_bomb");
	g_parSplashExpl=g_pResManager->GetParticleSystem("splash_expl");
	g_parBomb=g_pResManager->GetParticleSystem("parBomb");
	g_parExp=g_pResManager->GetParticleSystem("parExp");
	g_parLightning=g_pResManager->GetParticleSystem("parLightning");
	g_parColChange=g_pResManager->GetParticleSystem("parColChange");
	g_parLevelUp=g_pResManager->GetParticleSystem("parLevelup");
	g_parDirChange=g_pResManager->GetParticleSystem("parDirChange");

	g_fntChar=g_pResManager->GetFont("charFont");
	g_fntDigi=g_pResManager->GetFont("digiFont");

	g_pPlayGUI=new gstGUI();
	g_pPlayGUI->AddCtrl(new CGUIButton(BTN_NEWPLAY,g_pResManager->GetSprite("newGameMouseOff"),g_pResManager->GetSprite("newGameMouseOn")));
	g_pPlayGUI->AddCtrl(new CGUIButton(BTN_HIGHSCORES,g_pResManager->GetSprite("highScoreMouseOff"),g_pResManager->GetSprite("highScoreMouseOn")));
	g_pPlayGUI->AddCtrl(new CGUIButton(BTN_OPTIONS,g_pResManager->GetSprite("optionMouseOff"),g_pResManager->GetSprite("optionMouseOn")));
	g_pPlayGUI->AddCtrl(new CGUIButton(BTN_HELP1,g_pResManager->GetSprite("helpMouseOff"),g_pResManager->GetSprite("helpMouseOn")));
	g_pPlayGUI->AddCtrl(new CGUIButton(BTN_LOAD,g_pResManager->GetSprite("readGameMouseOff"),g_pResManager->GetSprite("readGameMouseOn")));
	g_pPlayGUI->AddCtrl(new CGUIButton(BTN_SAVE,g_pResManager->GetSprite("saveGameMouseOff"),g_pResManager->GetSprite("saveGameMouseOn")));
	CGUIButton *button=new CGUIButton(BTN_UNDO,g_pResManager->GetSprite("undoMouseOff"),g_pResManager->GetSprite("undoMouseOn"));
	button->bEnabled=false;
	g_pPlayGUI->AddCtrl(button);
	g_pPlayGUI->AddCtrl(new CGUIButton(BTN_CLOSE,g_pResManager->GetSprite("exitGameMouseOff"),g_pResManager->GetSprite("exitGameMouseOn")));
	g_pPlayGUI->AddCtrl(new CGUIButton(BTN_ROTATE1,g_pResManager->GetSprite("rotate1MouseOff"),g_pResManager->GetSprite("rotate1MouseOn"),g_pResManager->GetSprite("rotate1Disable")));
	g_pPlayGUI->AddCtrl(new CGUIButton(BTN_ROTATE2,g_pResManager->GetSprite("rotate2MouseOff"),g_pResManager->GetSprite("rotate2MouseOn"),g_pResManager->GetSprite("rotate2Disable")));

	g_pOptionGUI=new gstGUI();
	g_pOptionGUI->AddCtrl(new CGUISprite(100,100,g_sprOptionDlg));
	g_pOptionGUI->AddCtrl(new CGUICheckButton(CHECK_MUSIC,148,178,24,24,g_bMusic));
	g_pOptionGUI->AddCtrl(new CGUICheckButton(CHECK_SOUND,148,214,24,24,g_bSound));
	g_pOptionGUI->AddCtrl(new CGUICheckButton(CHECK_FULLSCREEN,148,250,24,24,!g_pGST->System_GetState(GST_WINDOWED)));
	g_pOptionGUI->AddCtrl(new CGUICheckButton(CHECK_BOMB,148,334,24,24,g_bBomb));
	g_pOptionGUI->AddCtrl(new CGUICheckButton(CHECK_ARROWCHANGER,285,334,24,24,g_bArrowChanger));
	g_pOptionGUI->AddCtrl(new CGUICheckButton(CHECK_LIGHTNING,148,382,24,24,g_bLightning));
	g_pOptionGUI->AddCtrl(new CGUICheckButton(CHECK_COLCHANGE,285,382,24,24,g_bColChange));
	g_pOptionGUI->AddCtrl(new CGUICheckButton(CHECK_UNIVERSAL,148,430,24,24,g_bUniversal));
	g_pOptionGUI->AddCtrl(new CGUICheckButton(CHECK_PROMPT,411,178,24,24,g_bPrompt));
	g_pOptionGUI->AddCtrl(new CGUICheckButton(CHECK_CANMOVE,411,214,24,24,g_bCanMove));
	g_pOptionGUI->AddCtrl(new CGUIButton(BTN_OK,0,0,0,440,422,139,65));

	g_pHelpGUI1=new gstGUI();
	g_pHelpGUI1->AddCtrl(new CGUISprite(100,100,g_sprHelpDlg0));
	g_pHelpGUI1->AddCtrl(new CGUIButton(BTN_HELP2,0,0,0,441,422,138,65));

	g_pHelpGUI2=new gstGUI();
	g_pHelpGUI2->AddCtrl(new CGUISprite(100,100,g_sprHelpDlg1));
	g_pHelpGUI2->AddCtrl(new CGUIButton(BTN_OK,0,0,0,441,422,138,65));

	g_pHighScoreGUI=new gstGUI();
	g_pHighScoreGUI->AddCtrl(new CGUISprite(50,50,g_sprHighScoreDlg));
	g_pHighScoreGUI->AddCtrl(new CGUIButton(BTN_HIGHSCORE,0,0,0,50,50,500,500));

	g_pGameOverGUI=new gstGUI();
	g_pGameOverGUI->AddCtrl(new CGUIButton(BTN_GAMEOVEROK,0,0,0,370,361,80,37));

	g_pEnterNameGUI=new gstGUI();
	CGUIEdit *text=new CGUIEdit(TEXT_ENTERNAME,40,298,530,20,g_fntChar,ARGB(255,255,0,0),ARGB(0,255,0,0));
	text->SetAlign(GSTTEXT_CENTER);
	g_pEnterNameGUI->AddCtrl(text);
	g_pEnterNameGUI->AddCtrl(new CGUIButton(BTN_ENTERNAMEOK,0,0,0,516,355,83,38));

	g_pSaveGUI=new gstGUI();
	g_pSaveGUI->AddCtrl(new CGUISprite(150,195,g_sprTipsDlg));
	g_pSaveGUI->AddCtrl(new CGUISprite(175,220,g_sprSaveDlg));
	g_pSaveGUI->AddCtrl(new CGUIButton(BTN_SLOTA,0,0,0,212,250,151,22));
	g_pSaveGUI->AddCtrl(new CGUIButton(BTN_SLOTB,0,0,0,212,281,151,22));
	g_pSaveGUI->AddCtrl(new CGUIButton(BTN_SLOTC,0,0,0,212,312,151,22));
	g_pSaveGUI->AddCtrl(new CGUIButton(BTN_OK,0,0,0,362,350,82,39));

	g_pLoadGUI=new gstGUI();
	g_pLoadGUI->AddCtrl(new CGUISprite(150,195,g_sprTipsDlg));
	g_pLoadGUI->AddCtrl(new CGUISprite(175,220,g_sprLoadDlg));
	g_pLoadGUI->AddCtrl(new CGUIButton(BTN_SLOTA,0,0,0,212,250,151,22));
	g_pLoadGUI->AddCtrl(new CGUIButton(BTN_SLOTB,0,0,0,212,281,151,22));
	g_pLoadGUI->AddCtrl(new CGUIButton(BTN_SLOTC,0,0,0,212,312,151,22));
	g_pLoadGUI->AddCtrl(new CGUIButton(BTN_OK,0,0,0,362,350,82,39));

	g_aniSplashBomb->Play();
	g_parSplashBomb->FireAt(320,115);
	g_parSplashExpl->FireAt(320,115);
	g_parLevelUp->info.sprite->SetBlendMode(BLEND_ALPHAADD);
	
	return true;
}

bool GameInitiate()
{
	g_pGST->Resource_AttachPack("data.dat","WuJiKai");
	g_pResManager=new gstResourceManager("data/resource.txt");
	g_ParManager=new gstParticleManager(50);
	g_pStrTab=new gstStringTable("data/String.txt");

	if(g_pResManager && g_ParManager && g_pStrTab)
	{
		g_sprLoading1=g_pResManager->GetSprite("spSplash1");
		g_sprLoading2=g_pResManager->GetSprite("spSplash2");
		g_sprAnyKey=g_pResManager->GetSprite("spAnyKey");
		g_pResManager->CacheBegin();
		return true;
	}
	
	return false;
}

void GameRelease()
{
	IniSave();
	SAFE_DELETE(g_pResManager);
	SAFE_DELETE(g_ParManager);
	SAFE_DELETE(g_pStrTab);
	SAFE_DELETE(g_pPlayGUI);
	SAFE_DELETE(g_pOptionGUI);
	SAFE_DELETE(g_pHelpGUI1);
	SAFE_DELETE(g_pHelpGUI2);
	SAFE_DELETE(g_pHighScoreGUI);
	SAFE_DELETE(g_pGameOverGUI);
	SAFE_DELETE(g_pEnterNameGUI);
	SAFE_DELETE(g_pSaveGUI);
	SAFE_DELETE(g_pLoadGUI);
	PlayRelease();
}

bool DoCommands(int id)
{
	switch(id)
	{
	case BTN_NEWPLAY:
		g_nLevel=1;
		g_nScore=0;
		PlayInit();
		g_nRotateCount=ROTATECOUNT;
		break;
	case BTN_HIGHSCORES:
		g_nGState=GS_HIGHSCORES;
		g_pHighScoreGUI->Enter();
		break;
	case BTN_OPTIONS:
		((CGUICheckButton *)g_pOptionGUI->GetCtrl(CHECK_FULLSCREEN))->SetState(!g_pGST->System_GetState(GST_WINDOWED));
		g_nGState=GS_OPTIONS;
		g_pOptionGUI->Enter();
		break;
	case BTN_HELP1:
		g_nGState=GS_HELP1;
		g_pHelpGUI1->Enter();
		break;
	case BTN_HELP2:
		g_nGState=GS_HELP2;
		g_pHelpGUI2->Enter();
		break;
	case BTN_LOAD:
		g_nGState=GS_LOAD;
		g_pLoadGUI->Enter();
		break;
	case BTN_SAVE:
		g_nGState=GS_SAVE;
		g_pSaveGUI->Enter();
		break;
	case BTN_CLOSE:
		return true;
	case BTN_UNDO:
		g_nGState=GS_UNDO;
		if(g_bCanUndo)
		{
			Undo();
			BrickReset();
		}
		g_nGState=GS_PLAY;		
		break;
	case BTN_ROTATE1:
		Undo(true);
		g_nRotateCount--;
		if(g_nRotateCount==0)
		{
			g_pPlayGUI->EnableCtrl(BTN_ROTATE1,false);
			g_pPlayGUI->EnableCtrl(BTN_ROTATE2,false);
		}
		if(g_bSound) g_pGST->Effect_Play(g_sndRotate);		
		g_nGState=GS_ROTATE1;
		break;
	case BTN_ROTATE2:
		Undo(true);
		g_nRotateCount--;
		if(g_nRotateCount==0)
		{
			g_pPlayGUI->EnableCtrl(BTN_ROTATE1,false);
			g_pPlayGUI->EnableCtrl(BTN_ROTATE2,false);
		}
		if(g_bSound) g_pGST->Effect_Play(g_sndRotate);		
		g_nGState=GS_ROTATE2;
		break;

	case BTN_OK:
		g_nGState=GS_PLAY;
		break;
	case BTN_ENTERNAMEOK:
		if(strcmp(((CGUIEdit *)g_pEnterNameGUI->GetCtrl(TEXT_ENTERNAME))->GetText(),"")==0) return false;
		HighScore(true);
		g_nLevel=1;
		g_nScore=0;
		PlayInit();
		g_nGState=GS_HIGHSCORES;
		break;
	case BTN_GAMEOVEROK:
		if(HighScore())
			g_nGState=GS_ENTERNAME;
		else
		{
			g_nLevel=1;
			g_nScore=0;
			PlayInit();
			g_nGState=GS_PLAY;
		}
		break;

	case BTN_SLOTA:
		if(g_nGState==GS_LOAD)
		{
			if(!GameLoad("./Save/A.bks")) return false;
			BrickReset();
		}
		else
			GameSave("./Save/A.bks");
		g_nGState=GS_PLAY;
		break;
	case BTN_SLOTB:
		if(g_nGState==GS_LOAD)
		{
			if(!GameLoad("./Save/B.bks")) return false;
			BrickReset();
		}
		else
			GameSave("./Save/B.bks");
		g_nGState=GS_PLAY;
		break;
	case BTN_SLOTC:
		if(g_nGState==GS_LOAD)
		{
			if(!GameLoad("./Save/C.bks")) return false;
			BrickReset();
		}
		else
			GameSave("./Save/C.bks");
		g_nGState=GS_PLAY;
		break;

	case BTN_HIGHSCORE:
		g_nGState=GS_PLAY;
		break;

	case CHECK_MUSIC:
		g_bMusic=((CGUICheckButton *)g_pOptionGUI->GetCtrl(CHECK_MUSIC))->GetState();
		if(g_bMusic) g_chaBgd=g_pGST->Effect_PlayEx(g_sndBgd,100,0,1.0f,true); else g_pGST->Channel_StopAll();
		break;
	case CHECK_SOUND:
		g_bSound=((CGUICheckButton *)g_pOptionGUI->GetCtrl(CHECK_SOUND))->GetState();
		break;
	case CHECK_FULLSCREEN:
		//g_pGST->System_SetState(GST_WINDOWED, !g_pGST->System_GetState(GST_WINDOWED));
		//g_bFullScreen=((CGUICheckButton *)g_pOptionGUI->GetCtrl(CHECK_FULLSCREEN))->GetState();
		g_bWindowStateChanged=true;
		break;
	case CHECK_BOMB:
		g_bBomb=((CGUICheckButton *)g_pOptionGUI->GetCtrl(CHECK_BOMB))->GetState();
		break;
	case CHECK_ARROWCHANGER:
		g_bArrowChanger=((CGUICheckButton *)g_pOptionGUI->GetCtrl(CHECK_ARROWCHANGER))->GetState();
		break;
	case CHECK_LIGHTNING:
		g_bLightning=((CGUICheckButton *)g_pOptionGUI->GetCtrl(CHECK_LIGHTNING))->GetState();
		break;
	case CHECK_COLCHANGE:
		g_bColChange=((CGUICheckButton *)g_pOptionGUI->GetCtrl(CHECK_COLCHANGE))->GetState();
		break;
	case CHECK_UNIVERSAL:
		g_bUniversal=((CGUICheckButton *)g_pOptionGUI->GetCtrl(CHECK_UNIVERSAL))->GetState();
		break;
	case CHECK_PROMPT:
		g_bPrompt=((CGUICheckButton *)g_pOptionGUI->GetCtrl(CHECK_PROMPT))->GetState();
		break;
	case CHECK_CANMOVE:
		g_bCanMove=((CGUICheckButton *)g_pOptionGUI->GetCtrl(CHECK_CANMOVE))->GetState();
		break;
	}


	return false;
}

bool HighScore(bool bSave)
{
	static int nSavePos=-1;
	if(bSave && nSavePos!=-1)
	{
		char szSavePos[30];
		char szLoadPos[30];
		for(int i=5;i>nSavePos;i--)
		{
			sprintf(szSavePos,"HIGHSCORE%d",i);
			sprintf(szLoadPos,"HIGHSCORE%d",i-1);
			g_pGST->Ini_SetString(szSavePos,"name",g_pGST->Ini_GetString(szLoadPos,"name",""));
			g_pGST->Ini_SetInt(szSavePos,"score",g_pGST->Ini_GetInt(szLoadPos,"score",0));
			g_pGST->Ini_SetInt(szSavePos,"level",g_pGST->Ini_GetInt(szLoadPos,"level",0));
		}

		sprintf(szSavePos,"HIGHSCORE%d",nSavePos);
		
		g_pGST->Ini_SetString(szSavePos,"name",((CGUIEdit *)g_pEnterNameGUI->GetCtrl(TEXT_ENTERNAME))->GetText());
		g_pGST->Ini_SetInt(szSavePos,"score",g_nScore);
		g_pGST->Ini_SetInt(szSavePos,"level",g_nLevel);

		nSavePos=-1;
		return true;
	}
	else
	{
		char szLoadPos[30];
		
		for(int i=1;i<=5;i++)
		{
			sprintf(szLoadPos,"HIGHSCORE%d",i);
			if(g_pGST->Ini_GetInt(szLoadPos,"score",0)<g_nScore)
			{
				nSavePos=i;
				return true;
			}
		}

		return false;
	}
}

void IniLoad()
{	
	g_bMusic=g_pGST->Ini_GetInt("setup","Music",1)==1 ? true : false;
	g_bSound=g_pGST->Ini_GetInt("setup","Sound",1)==1 ? true : false;
	g_bBomb=g_pGST->Ini_GetInt("setup","Bomb",1)==1 ? true : false;
	g_bArrowChanger=g_pGST->Ini_GetInt("setup","ArrowChanger",1)==1 ? true : false;
	g_bUniversal=g_pGST->Ini_GetInt("setup","Universal",1)==1 ? true : false;
	g_bLightning=g_pGST->Ini_GetInt("setup","Lightning",1)==1 ? true : false;
	g_bColChange=g_pGST->Ini_GetInt("setup","ColChange",1)==1 ? true : false;
	g_bPrompt=g_pGST->Ini_GetInt("setup","Prompt",1)==1 ? true : false;
	g_bCanMove=g_pGST->Ini_GetInt("setup","CanMove",1)==1 ? true : false;
}

void IniSave()
{
	g_pGST->Ini_SetInt("setup","FullScreen",!g_pGST->System_GetState(GST_WINDOWED));
	g_pGST->Ini_SetInt("setup","Music",g_bMusic);
	g_pGST->Ini_SetInt("setup","Sound",g_bSound);
	g_pGST->Ini_SetInt("setup","Bomb",g_bBomb);
	g_pGST->Ini_SetInt("setup","ArrowChanger",g_bArrowChanger);
	g_pGST->Ini_SetInt("setup","Universal",g_bUniversal);
	g_pGST->Ini_SetInt("setup","Lightning",g_bLightning);
	g_pGST->Ini_SetInt("setup","ColChange",g_bColChange);
	g_pGST->Ini_SetInt("setup","Prompt",g_bPrompt);
	g_pGST->Ini_SetInt("setup","CanMove",g_bCanMove);
}