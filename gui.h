#ifndef _GUI_H
#define _GUI_H

#include <gstguictrls.h>

#define ALPHA 0.1f
#define SHOWSPEED (1-ALPHA)/0.3

class CGUIObject:public gstGUIObject
{
public:
	CGUIObject():m_fdt(0) { }
	virtual void Enter() { m_fdt=1.0f; }
	virtual void Update(float dt) { if(m_fdt>ALPHA) m_fdt-=SHOWSPEED*dt; if(m_fdt<ALPHA) m_fdt=ALPHA; }
protected:
	float m_fdt;
};

class CGUISprite:public CGUIObject
{
public:
	CGUISprite(float x,float y,gstSprite *spr):m_x(x),m_y(y),m_spr(spr) { id=-1; bVisible=true; bEnabled=false; bStatic=true; rect.Set(0,0,0,0); }
	virtual void Render() { m_spr->SetColor(ARGB((1-m_fdt)*255,255,255,255)); m_spr->Render(m_x,m_y); }
protected:
	float m_x,m_y;
	gstSprite *m_spr;
};

class CGUIButton:public gstGUIObject
{
public:
	CGUIButton(int _id,gstSprite *sprUp,gstSprite *sprOn,gstSprite *sprDisable=0,float _x=-1,float _y=-1,float _w=-1,float _h=-1);
	virtual ~CGUIButton();

	virtual void Render();
	virtual void MouseOver(bool bOver) { m_bOver=bOver; } 
	virtual bool MouseLButton(bool bDown) { m_bClick=bDown; return !bDown; }
	
protected:
	gstSprite *m_sprUp,*m_sprOn,*m_sprDisable;
	bool m_bClick,m_bOver;
};

class CGUICheckButton:public CGUIObject
{
public:
	CGUICheckButton(int _id,float x,float y,float w,float h,bool bChecked);
	virtual ~CGUICheckButton();

	virtual void Render();
	virtual bool MouseLButton(bool bDown);

	void SetState(bool bCheck) { m_bChecked=bCheck; }
	bool GetState() { return m_bChecked; }

protected:
	bool m_bChecked;
};

#define MAX_STRING 16
#include <vector>
using namespace std;

struct st_Char
{
	bool bIME;
	char szChar[3];
};

class CGUIEdit:public gstGUIObject
{
public:
	CGUIEdit(int _id,float x,float y,float w,float h,gstFont *fnt,DWORD dwTextColor,DWORD dwFrameColor);
	~CGUIEdit(void);

	void SetAlign(int nAlign);
	char *GetText() { return m_szText; }

	virtual void Render();
	virtual void Update(float dt);
	virtual void Focus(bool bFocused);
	virtual bool KeyClick(int key,int chr);

protected:
	bool m_bFocus;
	char m_szText[MAX_STRING];
	DWORD m_dwTextColor;
	DWORD m_dwFrameColor;
	float m_fTime;
	bool m_bBlink;
	int m_nCurPos;
	int m_nAlign;
	bool m_bComposition;
	gstFont *m_pFont;

	vector<st_Char *> m_pCharArray;

	bool AddCharEx(const char *szNewChar,int nPos=-1);
	bool AddChar(const char szNewChar,int nPos=-1);
	bool DelChar(int nPos);
	void ClearCharArray();
	void RedrawCursor();
	void UpdataString();

public:
	void StartComposition();
	void EndComposition();
	bool IsComposition() { return m_bComposition;}
	void OnImeCharMsg(const char *szNewChar);
};

#endif