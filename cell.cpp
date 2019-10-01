#include "cell.h"
#include <gst.h>
#include <stdio.h>

CELLINFO CELL(16,16);

extern GST *g_pGST;
extern int g_nScore;
extern int g_nLevel;
extern bool g_bBomb;
extern bool g_bArrowChanger;
extern bool g_bUniversal;
extern bool g_bLightning;
extern bool g_bColChange;
extern char g_szPath[MAX_PATH];

void CellInit()
{
	g_pGST->Random_Seed();
	for(int y=0;y<15;y++) for(int x=0;x<15;x++) { CELLTYPE(x,y)=CO_CELL; CELLDIR(x,y)=DS_STATIC; }

	for(int y=1;y<4;y++)
		for(int x=4;x<12;x++)
		{
			CELLTYPE(x,y)=CellRandom(CO_REDBRICK,CO_ARROWCHANGERBRICK);
		}

	for(int y=4;y<12;y++)
		for(int x=12;x<15;x++)
		{
			CELLTYPE(x,y)=CellRandom(CO_REDBRICK,CO_ARROWCHANGERBRICK);
		}

	for(int y=12;y<15;y++)
		for(int x=4;x<12;x++)
		{
			CELLTYPE(x,y)=CellRandom(CO_REDBRICK,CO_ARROWCHANGERBRICK);
		}

	for(int y=4;y<12;y++)
		for(int x=1;x<4;x++)
		{
			CELLTYPE(x,y)=CellRandom(CO_REDBRICK,CO_ARROWCHANGERBRICK);
		}

	CELLTYPE(7,6)=CO_PINKBRICK;
	CELLTYPE(8,6)=CO_CYANBRICK;
	CELLTYPE(6,7)=CO_REDBRICK;
	CELLTYPE(9,7)=CO_GRAYBRICK;
	CELLTYPE(6,8)=CO_GREENBRICK;
	CELLTYPE(9,8)=CO_SKYYELLOWBRICK;
	CELLTYPE(7,9)=CO_BLUEBRICK;
	CELLTYPE(8,9)=CO_YELLOWBRICK;

	CELLDIR(7,6)=DS_NONE;
	CELLDIR(8,6)=DS_NONE;
	CELLDIR(6,7)=DS_NONE;
	CELLDIR(9,7)=DS_NONE;
	CELLDIR(6,8)=DS_NONE;
	CELLDIR(9,8)=DS_NONE;
	CELLDIR(7,9)=DS_NONE;
	CELLDIR(8,9)=DS_NONE;

	for(int i=0;i<g_nLevel-1;i++)
	{
		int cx,cy;
		do
		{
			cx=g_pGST->Random_Int(4,11);
			cy=g_pGST->Random_Int(4,11);
		}while(CELLTYPE(cx,cy)!=CO_CELL);

		CELLTYPE(cx,cy)=g_pGST->Random_Int(CO_REDBRICK,CO_PINKBRICK);
		CELLDIR(cx,cy)=DS_NONE;
	}
}

int CellRandom(int nStart,int nEnd)
{
	int nResult;
	do
	{
		nResult=g_pGST->Random_Int(nStart,nEnd);
	}while((!g_bBomb && nResult==CO_BOMBBRICK)
		|| (!g_bArrowChanger && nResult==CO_ARROWCHANGERBRICK)
		|| (!g_bUniversal && nResult==CO_UNIVERSALBRICK)
		|| (!g_bLightning && nResult==CO_LIGHTNINGBRICK)
		|| (!g_bColChange && nResult==CO_COLCHANGEBRICK)
		|| (g_pGST->Random_Int(0,100)<90 && (nResult>=CO_UNIVERSALBRICK && nResult<=CO_ARROWCHANGERBRICK)));

	return nResult;
}

void CellClear(int cx,int cy)
{
	CELLTYPE(cx,cy)=CO_CELL;
	CELLDIR(cx,cy)=DS_NONE;
}

bool CellIsClear()
{
	for(int y=4;y<12;y++)
		for(int x=4;x<12;x++)
			if(CELLTYPE(x,y)!=CO_CELL) return false;

	return true;
}

void CellMove(int SourCX,int SourCY,int DestCX,int DestCY)
{
	if(DestCX==SourCX)
	{
		CELLDIR(DestCX,DestCY) = (DestCY>SourCY) ? DS_DOWN : DS_UP;		
	}
	else if(DestCY==SourCY)
	{
		CELLDIR(DestCX,DestCY) = (DestCX>SourCX) ? DS_RIGHT : DS_LEFT;
	}

	CELLTYPE(DestCX,DestCY)=CELLTYPE(SourCX,SourCY);			
	CellClear(SourCX,SourCY);
}

void CellRandomSeed()
{
	int nSeed=0,nTotalDir=0;
	for(int y=4;y<12;y++) for(int x=4;x<12;x++) { nSeed+=CELLTYPE(x,y); nTotalDir+=CELLDIR(x,y); }
	nSeed*=nTotalDir;
	g_pGST->Random_Seed(nSeed);
}