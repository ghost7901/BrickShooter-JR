#ifndef _BRICKOBJ_H
#define _BRICKOBJ_H

#pragma once

#include "cell.h"
#include <gstanim.h>
#include <list>

class CBrickBase
{
public:
	CBrickBase();
	virtual ~CBrickBase();

	WORD CanMove(int cx,int cy);
	void SetPos(int cx,int cy);
	void SetFrame(int nFrame) { m_pAni->SetFrame(nFrame); }

	int GetPosX() { return m_x; }
	int GetPosY() { return m_y; }
	int GetPosCX() { return m_cx; }
	int GetPosCY() { return m_cy; }
	bool IsKill() { return m_bKilled; }
	bool IsMove() { return m_bMoving; }

	virtual void Update(float dt);
	virtual void Render();
	virtual void SetAni(gstAnimation *pAni);
	virtual bool Moving(float dt);
	virtual void Kill() { m_bKilled=true; CellClear(m_cx,m_cy); }
	void MoveTo(int cx,int cy);

protected:
	bool m_bKilled;
	bool m_bMoving;
	bool m_bMoved;
	int m_cx;
	int m_cy;
	int m_x;
	int m_y;
	float m_ds;
	gstAnimation *m_pAni;

public:
	virtual int SearchKill() { return 0; }
};

class CSimpleBrick:public CBrickBase
{
public:
	virtual void Update(float dt);
	virtual void Render();
	virtual void Kill();
protected:	
	int SearchKill();
	void SearchNode(int _x,int _y,int nBrickType);
	void SearchMove();
};

class CBombBrick:public CBrickBase
{
public:
	virtual void Update(float dt);
	virtual void Kill();
protected:
	int SearchKill();	
};

class CUniversalBrick:public CSimpleBrick
{
protected:
	int SearchKill();
};

class CLightningBrickV:public CBrickBase
{
public:
	virtual void Update(float dt);
	virtual void Kill();
protected:
	int SearchKill();	
};

class CLightningBrickH:public CBrickBase
{
public:
	virtual void Update(float dt);
	virtual void Render();
	virtual void Kill();
	virtual void SetAni(gstAnimation *pAni);
protected:
	int SearchKill();
};

class CColChangeBrick:public CSimpleBrick
{
public:
	virtual void Update(float dt);
	virtual void Kill();
protected:
	int SearchKill();	
};

class CArrowChangeBrick:public CBrickBase
{
public:
	virtual void Update(float dt);
	virtual void Kill();
protected:
	int SearchKill();
};

#endif