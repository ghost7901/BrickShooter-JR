#include "brick.h"
#include "cell.h"
#include "gstAnim.h"
#include "explode.h"
#include "common.h"
#include <gst.h>

extern GST *g_pGST;
extern float g_fMx;
extern float g_fMy;
extern HEFFECT g_sndShoot;
extern HEFFECT g_sndExpl[5];
extern HEFFECT g_sndLightning;
extern HEFFECT g_sndColChange;
extern HEFFECT g_sndDirChange;
extern gstAnimation *g_aniBrick[13];
extern bool g_bSound;
extern bool g_bPrompt;
extern bool g_bCanMove;
extern int g_nScore;
extern int g_nGState;

const int DIRECTION[8][2]={1,0,0,-1,-1,0,0,1,-1,-1,1,-1,1,1,-1,1};
bool g_bMoved=false;

/**********************************
*  CBrickBase elemention
**********************************/
CBrickBase::CBrickBase()
{
	m_ds=0;
	m_bMoving=false;
	m_bMoved=false;
	m_bKilled=false;
}

CBrickBase::~CBrickBase()
{
	if(m_pAni) { delete m_pAni; m_pAni=NULL; }
}

void CBrickBase::Update(float dt)
{	
	if(g_nGState==GS_PLAY && g_pGST->Input_KeyDown(GSTK_LBUTTON) && !m_bMoving && !g_bMoved )//&& g_pGST->Input_GetKeyState(GSTK_LBUTTON)) 
	{
		int nResult=CanMove((g_fMx-CELLX)/CELLSIZE+1,(g_fMy-CELLY)/CELLSIZE+1);
		int nMoveDir=LOBYTE(nResult);
		int nMoveTo=HIBYTE(nResult);

		if(nResult)
		{
			CBrickBase *brick;
			switch(nMoveDir)
			{
			case DS_DOWN:
				Undo(true);

				MoveTo(m_cx,nMoveTo);
				BrickGet(m_cx,2)->MoveTo(m_cx,3);
				BrickGet(m_cx,1)->MoveTo(m_cx,2);
				brick=BrickAdd(m_cx,0);
				brick->MoveTo(m_cx,1);
				break;
			case DS_UP:
				Undo(true);

				MoveTo(m_cx,nMoveTo);
				BrickGet(m_cx,13)->MoveTo(m_cx,12);
				BrickGet(m_cx,14)->MoveTo(m_cx,13);
				brick=BrickAdd(m_cx,15);
				brick->MoveTo(m_cx,14);
				break;
			case DS_RIGHT:
				Undo(true);

				MoveTo(nMoveTo,m_cy);
				BrickGet(2,m_cy)->MoveTo(3,m_cy);
				BrickGet(1,m_cy)->MoveTo(2,m_cy);
				brick=BrickAdd(0,m_cy);
				brick->MoveTo(1,m_cy);
				break;
			case DS_LEFT:
				Undo(true);

				MoveTo(nMoveTo,m_cy);
				BrickGet(13,m_cy)->MoveTo(12,m_cy);
				BrickGet(14,m_cy)->MoveTo(13,m_cy);
				brick=BrickAdd(15,m_cy);
				brick->MoveTo(14,m_cy);
				break;
			}

			m_pAni->SetFrame(CELLDIR(m_cx,m_cy));
			g_bMoved=true;
			if(g_bSound) g_pGST->Effect_Play(g_sndShoot);
		}
	}


	if(m_bMoving)
	{
		Moving(dt);
		m_bMoved=true;
	}
	if(ISINFIELD(m_cx,m_cy) && g_bMoved==false && m_bMoving==false && m_bMoved!=m_bMoving)
	{
		int nKilled=SearchKill();
		if(nKilled)
		{
			g_nScore+=nKilled>3 ? nKilled*3-6 : nKilled;
			if(g_bSound)
			{
				int nSndIndex;
				if(nKilled<4) nSndIndex=0; else nSndIndex=nKilled-3;
				g_pGST->Effect_Play(g_sndExpl[nSndIndex]);
			}
		}
	}
}

void CBrickBase::Render()
{
	if(!m_bMoving)
		m_pAni->Render(m_x,m_y);
	else
	{
		switch(CELLDIR(m_cx,m_cy))
		{
		case DS_UP:
			m_pAni->Render(m_x,m_y+m_ds);
			break;
		case DS_DOWN:
			m_pAni->Render(m_x,m_y-m_ds);
			break;
		case DS_LEFT:
			m_pAni->Render(m_x+m_ds,m_y);
			break;
		case DS_RIGHT:
			m_pAni->Render(m_x-m_ds,m_y);
			break;
		}
	}
}

void CBrickBase::SetAni(gstAnimation *pAni)
{
	//if(m_pAni) delete m_pAni;
	m_pAni=new gstAnimation(*pAni);
	m_pAni->Play();
}

void CBrickBase::SetPos(int cx,int cy)
{	
	m_cx=cx;
	m_cy=cy;
	m_x=CELLX+CELLSIZE*(m_cx-1);
	m_y=CELLY+CELLSIZE*(m_cy-1);
}

WORD CBrickBase::CanMove(int cx,int cy)
{
	if((cx!=m_cx || cy!=m_cy) || (cy!=3 && cy!=12 && cx!=3 && cx!=12)) return 0;

	if(m_cy==3 && CELLTYPE(m_cx,4)==CO_CELL)
	{
		for(int y=5;y<12;y++)
			if(CELLTYPE(m_cx,y)!=CO_CELL)
			{
				return MAKEWORD(DS_DOWN,y-1);
			}
	}
	else if(m_cy==12 && CELLTYPE(m_cx,11)==CO_CELL)
	{
		for(int y=10;y>3;y--)
			if(CELLTYPE(m_cx,y)!=CO_CELL)
			{
				return MAKEWORD(DS_UP,y+1);
			}
	}
	else if(m_cx==3 && CELLTYPE(4,m_cy)==CO_CELL)
	{
		for(int x=5;x<12;x++)
			if(CELLTYPE(x,m_cy)!=CO_CELL)
			{
				return MAKEWORD(DS_RIGHT,x-1);
			}
	}
	else if(m_cx==12 && CELLTYPE(11,m_cy)==CO_CELL)
	{
		for(int x=10;x>3;x--)
			if(CELLTYPE(x,m_cy)!=CO_CELL)
			{
				return MAKEWORD(DS_LEFT,x+1);
			}
	}

	return 0;
}

bool CBrickBase::Moving(float dt)
{
	static int dSpeed=0;
	m_ds-=dt*10*56+0.1*dSpeed++;

	if(m_ds<0)
	{
		m_bMoving=false;
		m_bMoved=false;
		if(ISINFIELD(m_cx,m_cy)) g_bMoved=false;
		dSpeed=0;
		if(m_cx==0 || m_cx==15 || m_cy==0 || m_cy==15) CBrickBase::Kill();
		return false;
	}

	return true;
}

void CBrickBase::MoveTo(int cx,int cy)
{
	m_bMoving=true;

	CellMove(m_cx,m_cy,cx,cy);
	m_ds=CELLSIZE*abs((cx==m_cx) ? (cy-m_cy) : (cx-m_cx));
	SetPos(cx,cy);
}

/**********************************
*  CSimpleBrick elemention
**********************************/
std::list<POINT> g_lstNode;
void CSimpleBrick::Kill()
{
	ExplodeAdd(m_x-1+CELLSIZE/2,m_y-1+CELLSIZE/2,CELLTYPE(m_cx,m_cy));
	CBrickBase::Kill();
}

void CSimpleBrick::Update(float dt)
{	
	if(ISINFIELD(m_cx,m_cy))
	{			
		int nCurFrame=m_pAni->GetFrame();
		if(CELLDIR(m_cx,m_cy)==DS_NONE)
		{			
			if(g_pGST->Random_Int(0,10000)>9990)
				nCurFrame = nCurFrame==0 ? 6 : 0;
			m_pAni->SetFrame(nCurFrame);
		}
		else
		{
			if(nCurFrame==6)
			{
				if(g_pGST->Random_Int(0,10000) > 9900) nCurFrame=CELLDIR(m_cx,m_cy);
			}
			else
			{
				if(g_pGST->Random_Int(0,10000) > 9990) nCurFrame=6;
			}

			m_pAni->SetFrame(nCurFrame);
		}

		if(g_bCanMove && g_nGState==GS_PLAY && !m_bMoving && CELLDIR(m_cx,m_cy)!=DS_NONE)
		{
			SearchMove();
		}
	}
	else if(g_bPrompt && ISINLAYER3(m_cx,m_cy))
	{
		if(((int)(g_fMx-CELLX)/CELLSIZE+1)==m_cx && ((int)(g_fMy-CELLY)/CELLSIZE+1)==m_cy)
		{
			int nResult=CanMove((g_fMx-CELLX)/CELLSIZE+1,(g_fMy-CELLY)/CELLSIZE+1);
			if(nResult)	SetFrame(LOBYTE(nResult));			
		}
		else
			SetFrame(DS_STATIC);
	}
	else
		m_pAni->SetFrame(DS_STATIC);

	CBrickBase::Update(dt);
}

void CSimpleBrick::Render()
{
	CBrickBase::Render();

	if(g_bCanMove && ISINFIELD(m_cx,m_cy) && CELLDIR(m_cx,m_cy)==DS_NONE)	 
	{
		g_pGST->Gfx_RenderRectangle(m_x,m_y,m_x+CELLSIZE,m_y+2,0xffffffff);
		g_pGST->Gfx_RenderRectangle(m_x+CELLSIZE-2,m_y,m_x+CELLSIZE,m_y+CELLSIZE,0xffffffff);
		g_pGST->Gfx_RenderRectangle(m_x,m_y+CELLSIZE-2,m_x+CELLSIZE,m_y+CELLSIZE,0xffffffff);
		g_pGST->Gfx_RenderRectangle(m_x,m_y,m_x+2,m_y+CELLSIZE,0xffffffff);
	}
}

int CSimpleBrick::SearchKill()
{
	int nKilled=0;

	SearchNode(m_cx,m_cy,CELLTYPE(m_cx,m_cy));
	if(g_lstNode.size()>2)
	{
		nKilled=g_lstNode.size();

		for(std::list<POINT>::iterator it=g_lstNode.begin();it!=g_lstNode.end();it++)
		{
			BrickGet((*it).x,(*it).y)->Kill();			
		}
	}

	g_lstNode.clear();

	return nKilled;
}

void CSimpleBrick::SearchNode(int _x,int _y,int nBrickType)
{
	POINT ptNode={ _x,_y };
	g_lstNode.push_back(ptNode);

	bool bHas;
	int x,y;
	for(int i=0;i<4;i++)
	{
		x=_x+DIRECTION[i][0];
		y=_y+DIRECTION[i][1];
		
		if(!ISINFIELD(x,y) || (CELLTYPE(x,y)!=nBrickType && CELLTYPE(x,y)!=CO_UNIVERSALBRICK)) continue;

		bHas=false;
		for(std::list<POINT>::iterator it=g_lstNode.begin();it!=g_lstNode.end();it++)
		{
			if(x==(*it).x && y==(*it).y)
			{
				bHas=true;
				break;
			}
		}
		if(bHas) continue;

		SearchNode(x,y,nBrickType);
	}
}

void CSimpleBrick::SearchMove()
{
	int x=m_cx+DIRECTION[CELLDIR(m_cx,m_cy)-1][0];
	int y=m_cy+DIRECTION[CELLDIR(m_cx,m_cy)-1][1];
	if(CELLTYPE(x,y)==CO_CELL)
	{
		MoveTo(x,y);
	}
	else
	{
		if(y==3)
		{
			BrickGet(m_cx,1)->MoveTo(m_cx,0);
			BrickGet(m_cx,2)->MoveTo(m_cx,1);
			BrickGet(m_cx,3)->MoveTo(m_cx,2);
			MoveTo(x,y);					
		}
		else if(y==12)
		{
			BrickGet(m_cx,14)->MoveTo(m_cx,15);
			BrickGet(m_cx,13)->MoveTo(m_cx,14);
			BrickGet(m_cx,12)->MoveTo(m_cx,13);
			MoveTo(x,y);
		}
		else if(x==3)
		{
			BrickGet(1,m_cy)->MoveTo(0,m_cy);
			BrickGet(2,m_cy)->MoveTo(1,m_cy);
			BrickGet(3,m_cy)->MoveTo(2,m_cy);
			MoveTo(x,y);
		}
		else if(x==12)
		{
			BrickGet(14,m_cy)->MoveTo(15,m_cy);
			BrickGet(13,m_cy)->MoveTo(14,m_cy);
			BrickGet(12,m_cy)->MoveTo(13,m_cy);
			MoveTo(x,y);
		}
	}
}

/**********************************
*  CUniversal elemention
**********************************/
int CUniversalBrick::SearchKill()
{
	std::list<POINT> lstKilled;
	for(int i=CO_REDBRICK;i<=CO_PINKBRICK;i++)
	{
		SearchNode(m_cx,m_cy,i);
		if(g_lstNode.size()>2)
		{
			for(std::list<POINT>::iterator it=g_lstNode.begin();it!=g_lstNode.end();it++)
			{
				POINT pt={(*it).x,(*it).y};
				lstKilled.push_back(pt);
			}			
		}
		
		g_lstNode.clear();
	}
	
	int nKilled=0;
	for(std::list<POINT>::iterator it=lstKilled.begin();it!=lstKilled.end();it++)
	{
		if(CELLTYPE((*it).x,(*it).y)!=CO_CELL)
		{
			BrickGet((*it).x,(*it).y)->Kill();
			nKilled++;
		}
	}

	return nKilled;
}

/**********************************
*  CBombBrick elemention
**********************************/
void CBombBrick::Update(float dt)
{
	m_pAni->Update(dt);
	CBrickBase::Update(dt);
}

int CBombBrick::SearchKill()
{
	Kill();
	int nKilled=0;

	int cx,cy;
	for(int i=0;i<8;i++)
	{
		cx=m_cx+DIRECTION[i][0];
		cy=m_cy+DIRECTION[i][1];
		if(ISINFIELD(cx,cy) && CELLTYPE(cx,cy)!=CO_CELL)
		{
			BrickGet(cx,cy)->CBrickBase::Kill();
			nKilled++;
		}
	}

	return nKilled;
}

void CBombBrick::Kill()
{	
	ExplodeAdd(m_x+CELLSIZE/2,m_y+CELLSIZE/2,CO_BOMBBRICK);
	CBrickBase::Kill();
}

/**********************************
*  CLightningBrickV elemention
**********************************/
void CLightningBrickV::Update(float dt)
{
	m_pAni->Update(dt);
	CBrickBase::Update(dt);
}

void CLightningBrickV::Kill()
{
	ExplodeAdd(m_x+CELLSIZE/2,m_y+CELLSIZE/2,CO_LIGHTNINGBRICK);

	CBrickBase::Kill();
}

int CLightningBrickV::SearchKill()
{
	Kill();
	int nKilled=0;

	for(int cy=4;cy<12;cy++)
	{
		if(CELLTYPE(m_cx,cy)!=CO_CELL)
		{
			BrickGet(m_cx,cy)->Kill();
			nKilled++;
		}
	}

	if(g_bSound) g_pGST->Effect_Play(g_sndLightning);
	
	return nKilled;
}

/**********************************
*  CLightningBrickH elemention
**********************************/
void CLightningBrickH::SetAni(gstAnimation *pAni)
{
	CBrickBase::SetAni(pAni);
	m_pAni->SetHotSpot(CELLSIZE/2,CELLSIZE/2);
}

void CLightningBrickH::Update(float dt)
{
	m_pAni->Update(dt);
	CBrickBase::Update(dt);
}

void CLightningBrickH::Render()
{
	if(!m_bMoving)
		m_pAni->RenderEx(m_x+CELLSIZE/2,m_y+CELLSIZE/2,-M_PI_2);
	else
	{
		switch(CELLDIR(m_cx,m_cy))
		{
		case DS_UP:
			m_pAni->RenderEx(m_x+CELLSIZE/2,m_y+CELLSIZE/2+m_ds,-M_PI_2);
			break;
		case DS_DOWN:
			m_pAni->RenderEx(m_x+CELLSIZE/2,m_y+CELLSIZE/2-m_ds,-M_PI_2);
			break;
		case DS_LEFT:
			m_pAni->RenderEx(m_x+CELLSIZE/2+m_ds,m_y+CELLSIZE/2,-M_PI_2);
			break;
		case DS_RIGHT:
			m_pAni->RenderEx(m_x+CELLSIZE/2-m_ds,m_y+CELLSIZE/2,-M_PI_2);
			break;
		}
	}
}

void CLightningBrickH::Kill()
{
	ExplodeAdd(m_x+CELLSIZE/2,m_y+CELLSIZE/2,CO_LIGHTNINGBRICK,true);

	CBrickBase::Kill();
}

int CLightningBrickH::SearchKill()
{
	Kill();
	int nKilled=0;

	for(int cx=4;cx<12;cx++)
	{
		if(CELLTYPE(cx,m_cy)!=CO_CELL)
		{
			BrickGet(cx,m_cy)->Kill();
			nKilled++;
		}
	}

	if(g_bSound) g_pGST->Effect_Play(g_sndLightning);
	
	return nKilled;
}

/**********************************
*  CColChangeBrick elemention
**********************************/
void CColChangeBrick::Update(float dt)
{
	m_pAni->Update(dt);
	CBrickBase::Update(dt);
}

void CColChangeBrick::Kill()
{
	ExplodeAdd(m_x+CELLSIZE/2,m_y+CELLSIZE/2,CO_COLCHANGEBRICK);
	CBrickBase::Kill();
}

int CColChangeBrick::SearchKill()
{
	Kill();
	int nKilled=0;

	CellRandomSeed();
	int cx,cy;
	for(int i=0;i<8;i++)
	{
		cx=m_cx+DIRECTION[i][0];
		cy=m_cy+DIRECTION[i][1];
		if(ISINFIELD(cx,cy) && CELLTYPE(cx,cy)!=CO_CELL)
		{
			CELLTYPE(cx,cy)=CellRandom(CO_REDBRICK,CO_PINKBRICK);
			CBrickBase *brick=BrickGet(cx,cy);
			brick->SetAni(g_aniBrick[CELLTYPE(cx,cy)]);
			nKilled+=brick->SearchKill();
		}
	}
	
	if(g_bSound) g_pGST->Effect_Play(g_sndColChange);
	return nKilled;
}

/**********************************
*  CArrowChangeBrick elemention
**********************************/
void CArrowChangeBrick::Update(float dt)
{
	m_pAni->Update(dt);

	CBrickBase::Update(dt);
}

void CArrowChangeBrick::Kill()
{
	ExplodeAdd(m_x+CELLSIZE/2,m_y+CELLSIZE/2,CO_ARROWCHANGERBRICK);

	CBrickBase::Kill();
}

int CArrowChangeBrick::SearchKill()
{
	Kill();
	CellRandomSeed();
	int cx,cy;
	for(int i=0;i<8;i++)
	{
		cx=m_cx+DIRECTION[i][0];
		cy=m_cy+DIRECTION[i][1];
		if(ISINFIELD(cx,cy) && CELLTYPE(cx,cy)!=CO_CELL)
		{
			CELLDIR(cx,cy)=g_pGST->Random_Int(DS_NONE,DS_DOWN);
			BrickGet(cx,cy)->SetFrame(CELLDIR(cx,cy));
		}
	}
	if(g_bSound) g_pGST->Effect_Play(g_sndDirChange);
	return 0;
}