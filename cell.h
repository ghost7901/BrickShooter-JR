#ifndef _CELL_H
#define _CELL_H

#pragma once

#include <gstrect.h>

#define CELLX 20
#define CELLY 20
#define CELLSIZE 40
#define ISINFIELD(cx,cy) ((cx)>3 && (cx)<12 && (cy)>3 && (cy)<12)
#define ISINLAYER3(cx,cy) ((cx)==3 || (cx)==12 || (cy)==3 || (cy)==12)
#define ISINLEFTRIGHT(cx,cy) ((cy)>3 && (cy)<12 && ((cx)<4 || (cx)>11))
#define CELL(cx,cy) (g_CellInfo[(cy)][(cx)])
#define CELLTYPE(cx,cy) (g_CellInfo[(cy)][(cx)].nType)
#define CELLDIR(cx,cy) (g_CellInfo[(cy)][(cx)].nDirection)

const gstRect FIELDRECT(CELLX+CELLSIZE*3,CELLY+CELLSIZE*3,CELLX+CELLSIZE*11,CELLY+CELLSIZE*11);

enum CellObject
{	
	CO_REDBRICK,
	CO_GREENBRICK,
	CO_BLUEBRICK,
	CO_YELLOWBRICK,
	CO_SKYYELLOWBRICK,	
	CO_GRAYBRICK,
	CO_CYANBRICK,
	CO_PINKBRICK,
	CO_UNIVERSALBRICK,
	CO_BOMBBRICK,
	CO_LIGHTNINGBRICK,
	CO_COLCHANGEBRICK,
	CO_ARROWCHANGERBRICK,
	CO_CELL,
};

enum Direction_State
{
	DS_NONE,
	DS_RIGHT,
	DS_UP,
	DS_LEFT,
	DS_DOWN,
	DS_STATIC,
};

struct CELLINFO
{
	int nType;
	int nDirection;
	void operator =(CELLINFO p)
	{
		nType=p.nType;
		nDirection=p.nDirection;
	}
	void DirRotate(bool bCCW)
	{
		if(nDirection==DS_NONE || nDirection==DS_STATIC) return;
		if(bCCW)
		{
			if(nDirection==1)
				nDirection=4;
			else
				nDirection--;
		}
		else
		{
			if(nDirection==4)
				nDirection=1;
			else
				nDirection++;
		}
	}
};

extern CELLINFO CELL(16,16);

void CellInit();
int CellRandom(int nStart,int nEnd);
void CellClear(int cx,int cy);
bool CellIsClear();
void CellMove(int SourCX,int SourCY,int DestCX,int DestCY);
void CellRandomSeed();

#endif