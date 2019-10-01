#include "brick.h"
#include "gui.h"
#include "common.h"
#include <list>
#include <gst.h>

extern GST *g_pGST;
extern gstAnimation *g_aniBrick[13];
extern gstGUI *g_pPlayGUI;
extern bool g_bBomb;
extern bool g_bArrowChanger;
extern bool g_bUniversal;
extern bool g_bCanUndo;
extern int g_nLevel;
extern int g_nScore;
extern int g_nRotateCount;
extern float fRotdt;
std::list<CBrickBase *> g_lstBrick;

void BrickRelease()
{
	for(std::list<CBrickBase *>::iterator it=g_lstBrick.begin();it!=g_lstBrick.end();it++) delete (*it);
	g_lstBrick.clear();
}

void BrickReset()
{
	BrickRelease();
	
	CBrickBase *brick;
	for(int y=1;y<15;y++)
		for(int x=1;x<15;x++)
		{
			switch(CELLTYPE(x,y))
			{
			case CO_REDBRICK:
			case CO_GREENBRICK:
			case CO_BLUEBRICK:
			case CO_SKYYELLOWBRICK:
			case CO_YELLOWBRICK:
			case CO_GRAYBRICK:
			case CO_CYANBRICK:
			case CO_PINKBRICK:
				brick=new CSimpleBrick();
				break;
			case CO_UNIVERSALBRICK:
				brick=new CUniversalBrick();
				break;
			case CO_BOMBBRICK:
				brick=new CBombBrick();
				break;
			case CO_LIGHTNINGBRICK:
				if(ISINLEFTRIGHT(x,y)) brick=new CLightningBrickH(); else brick=new CLightningBrickV();
				break;
			case CO_COLCHANGEBRICK:
				brick=new CColChangeBrick();
				break;
			case CO_ARROWCHANGERBRICK:
				brick=new CArrowChangeBrick();
				break;
			default:
				continue;
			}
			
			if(brick)
			{
				brick->SetPos(x,y);
				brick->SetAni(g_aniBrick[CELLTYPE(x,y)]);
				g_lstBrick.push_back(brick);
			}
		}
}

void BrickUpdate(float dt)
{
	for(std::list<CBrickBase *>::iterator it=g_lstBrick.begin();it!=g_lstBrick.end();)
	{
		if((*it)->IsKill())
		{
			delete (*it);
			it=g_lstBrick.erase(it);
		}
		else
		{
			(*it)->Update(dt);
			++it;
		}		
	}
}

void BrickRender()
{
	g_pGST->Gfx_SetClipping(CELLX,CELLY,CELLSIZE*14,CELLSIZE*14);	
	for(std::list<CBrickBase *>::iterator it=g_lstBrick.begin();it!=g_lstBrick.end();it++)
	{
		(*it)->Render();
	}
	g_pGST->Gfx_SetClipping();	
}

CBrickBase *BrickGet(int cx,int cy)
{
	for(std::list<CBrickBase *>::iterator it=g_lstBrick.begin();it!=g_lstBrick.end();it++)
	{
		if((*it)->GetPosCX()==cx && (*it)->GetPosCY()==cy) return (*it);
	}

	return NULL;
}

CBrickBase *BrickAdd(int cx,int cy)
{
	CellRandomSeed();

	CELLTYPE(cx,cy)=CellRandom(CO_REDBRICK,CO_ARROWCHANGERBRICK);

	CBrickBase *brick;
	if(CELLTYPE(cx,cy)>=CO_REDBRICK && CELLTYPE(cx,cy)<=CO_PINKBRICK)
		brick=new CSimpleBrick();
	else if(CELLTYPE(cx,cy)==CO_BOMBBRICK)
		brick=new CBombBrick();
	else if(CELLTYPE(cx,cy)==CO_UNIVERSALBRICK)
		brick=new CUniversalBrick();
	else if(CELLTYPE(cx,cy)==CO_LIGHTNINGBRICK)
	{	if(ISINLEFTRIGHT(cx,cy)) brick=new CLightningBrickH(); else brick=new CLightningBrickV(); }
	else if(CELLTYPE(cx,cy)==CO_COLCHANGEBRICK)
		brick=new CColChangeBrick();
	else if(CELLTYPE(cx,cy)==CO_ARROWCHANGERBRICK)
		brick=new CArrowChangeBrick();

	brick->SetPos(cx,cy);
	brick->SetAni(g_aniBrick[CELLTYPE(cx,cy)]);

	g_lstBrick.push_back(brick);
	return brick;
}

bool BrickRotate1()
{
	static float fRotdt=0;
	static int nRState=0;
	static float fScale=1;

	switch(nRState)
	{
	case 0:		
		for(std::list<CBrickBase *>::iterator it=g_lstBrick.begin();it!=g_lstBrick.end();it++)
		{
			if(ISINFIELD((*it)->GetPosCX(),(*it)->GetPosCY())) g_pGST->Gfx_SetTransform(CELLX+CELLSIZE*7,CELLY+CELLSIZE*7,0,0,fRotdt,fScale,fScale);
			(*it)->Render();
			if(ISINFIELD((*it)->GetPosCX(),(*it)->GetPosCY())) g_pGST->Gfx_SetTransform();
		}
		fRotdt+=-M_PI_2*0.007;
		
		if(fRotdt>-M_PI_4) fScale-=0.0073933983f; else fScale+=0.0073933983f;
		
		if(fRotdt<-M_PI_2)
		{ 
			for(std::list<CBrickBase *>::iterator it=g_lstBrick.begin();it!=g_lstBrick.end();it++)
			{
				if(ISINFIELD((*it)->GetPosCX(),(*it)->GetPosCY())) (*it)->SetPos(15-(*it)->GetPosCY(),(*it)->GetPosCX());
			}
			for(int y=4;y<=7;y++)
				for(int x=4;x<=7;x++)
				{					
					CELLINFO temp=CELL(x,y);
					CELL(x,y)=CELL(y,15-x);
					CELL(y,15-x)=CELL(15-x,15-y);
					CELL(15-x,15-y)=CELL(15-y,x);
					CELL(15-y,x)=temp;

					CELL(x,y).DirRotate(true);
					CELL(y,15-x).DirRotate(true);
					CELL(15-x,15-y).DirRotate(true);
					CELL(15-y,x).DirRotate(true);
				}
			nRState=1;
		}
		break;
	case 1:		
		for(std::list<CBrickBase *>::iterator it=g_lstBrick.begin();it!=g_lstBrick.end();it++)
		{
			if(ISINFIELD((*it)->GetPosCX(),(*it)->GetPosCY()))
			{
				g_pGST->Gfx_SetClipping(CELLX+CELLSIZE*3,CELLY+CELLSIZE*3,CELLSIZE*8,CELLSIZE*8);
				g_pGST->Gfx_SetTransform((*it)->GetPosX()+CELLSIZE/2,(*it)->GetPosY()+CELLSIZE/2,0,0,fRotdt,1,1);
			}
			(*it)->Render();
			if(ISINFIELD((*it)->GetPosCX(),(*it)->GetPosCY()))
			{
				g_pGST->Gfx_SetClipping();
				g_pGST->Gfx_SetTransform();
			}
		}
		fRotdt+=M_PI_2*0.01;
		if(fRotdt>0)
		{
			fRotdt=0;
			nRState=0;			
			fScale=1;
			return false;
		}
		break;
	}	
	
	return true;
}

bool BrickRotate2()
{
	static float fRotdt=0;
	static int nRState=0;
	static float fScale=1;

	switch(nRState)
	{
	case 0:		
		for(std::list<CBrickBase *>::iterator it=g_lstBrick.begin();it!=g_lstBrick.end();it++)
		{
			if(ISINFIELD((*it)->GetPosCX(),(*it)->GetPosCY())) g_pGST->Gfx_SetTransform(CELLX+CELLSIZE*7,CELLY+CELLSIZE*7,0,0,fRotdt,fScale,fScale);
			(*it)->Render();
			if(ISINFIELD((*it)->GetPosCX(),(*it)->GetPosCY())) g_pGST->Gfx_SetTransform();
		}
		fRotdt+=M_PI_2*0.007;
		
		if(fRotdt<M_PI_4) fScale-=0.0073933983f; else fScale+=0.0073933983f;
		
		if(fRotdt>M_PI_2)
		{ 
			for(std::list<CBrickBase *>::iterator it=g_lstBrick.begin();it!=g_lstBrick.end();it++)
			{
				if(ISINFIELD((*it)->GetPosCX(),(*it)->GetPosCY())) (*it)->SetPos((*it)->GetPosCY(),15-(*it)->GetPosCX());
			}
			for(int y=4;y<=7;y++)
				for(int x=4;x<=7;x++)
				{			
					CELLINFO temp=CELL(x,y);
					CELL(x,y)=CELL(15-y,x);
					CELL(15-y,x)=CELL(15-x,15-y);
					CELL(15-x,15-y)=CELL(y,15-x);
					CELL(y,15-x)=temp;

					CELL(x,y).DirRotate(false);
					CELL(15-y,x).DirRotate(false);
					CELL(15-x,15-y).DirRotate(false);
					CELL(y,15-x).DirRotate(false);
				}
			nRState=1;
		}
		break;
	case 1:		
		for(std::list<CBrickBase *>::iterator it=g_lstBrick.begin();it!=g_lstBrick.end();it++)
		{
			if(ISINFIELD((*it)->GetPosCX(),(*it)->GetPosCY()))
			{
				g_pGST->Gfx_SetClipping(CELLX+CELLSIZE*3,CELLY+CELLSIZE*3,CELLSIZE*8,CELLSIZE*8);
				g_pGST->Gfx_SetTransform((*it)->GetPosX()+CELLSIZE/2,(*it)->GetPosY()+CELLSIZE/2,0,0,fRotdt,1,1);
			}
			(*it)->Render();
			if(ISINFIELD((*it)->GetPosCX(),(*it)->GetPosCY()))
			{
				g_pGST->Gfx_SetClipping();
				g_pGST->Gfx_SetTransform();
			}
		}
		fRotdt-=M_PI_2*0.01;
		if(fRotdt<0)
		{
			fRotdt=0;
			nRState=0;			
			fScale=1;
			return false;
		}
		break;
	}	
	
	return true;
}

void Undo(bool bSave)
{
	if(bSave)
	{
		g_bCanUndo=true;

		GameSave("./Save/Undo.bks");
	}
	else
	{
		g_bCanUndo=false;

		GameLoad("./Save/Undo.bks");
		if(g_nRotateCount>0)
		{
			g_pPlayGUI->EnableCtrl(BTN_ROTATE1,true);
			g_pPlayGUI->EnableCtrl(BTN_ROTATE2,true);
		}
	}
	g_pPlayGUI->EnableCtrl(BTN_UNDO,g_bCanUndo);
}

bool GameLoad(char *szFileName)
{
	FILE *fp=fopen(szFileName,"rb");
	if(!fp) return false;	
	fread(g_CellInfo,sizeof(g_CellInfo),1,fp);
	fread(&g_nScore,sizeof(g_nScore),1,fp);
	fread(&g_nLevel,sizeof(g_nLevel),1,fp);
	fread(&g_nRotateCount,sizeof(g_nRotateCount),1,fp);
	fclose(fp);
	return true;
}

bool GameSave(char *szFileName)
{
	FILE *fp=fopen(szFileName,"wb");
	if(!fp) return false;
	fwrite(g_CellInfo,sizeof(g_CellInfo),1,fp);
	fwrite(&g_nScore,sizeof(g_nScore),1,fp);
	fwrite(&g_nLevel,sizeof(g_nLevel),1,fp);
	fwrite(&g_nRotateCount,sizeof(g_nRotateCount),1,fp);
	fclose(fp);
	return true;
}