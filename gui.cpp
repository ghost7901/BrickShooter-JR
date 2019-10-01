#include "gui.h"

extern GST *g_pGST;
extern gstSprite *g_sprOkSign;
/***********************************************
*    CGUIButton
***********************************************/
CGUIButton::CGUIButton(int _id,gstSprite *sprUp,gstSprite *sprOn,gstSprite *sprDisable,float _x,float _y,float _w,float _h)
{
	id=_id;
	bStatic=false;
	bVisible=true;
	bEnabled=true;

	m_bOver=false;
	m_bClick=false;

	m_sprUp=sprUp;
	m_sprOn=sprOn;
	m_sprDisable=sprDisable;

	if(_x==-1 && _y==-1 && _w==-1 && _h==-1)
	{
		float x,y,w,h;
		sprUp->GetTextureRect(&x,&y,&w,&h);
		rect.Set(x,y,x+w,y+h);
	}
	else
	{
		rect.Set(_x,_y,_x+_w,_y+_h);
	}
}

CGUIButton::~CGUIButton()
{
//	if(m_sprUp) delete m_sprUp;
//	if(m_sprOn) delete m_sprOn;
//	if(m_sprDisable) delete m_sprDisable;
}

void CGUIButton::Render()
{
	if(bEnabled)
	{
		if(m_bOver)
			if(m_sprOn) m_sprOn->Render(rect.x1,rect.y1);
		else
			if(m_sprUp) m_sprUp->Render(rect.x1,rect.y1);
	}
	else if(m_sprDisable)
		m_sprDisable->Render(rect.x1,rect.y1);
}

/***********************************************
*    CGUICheckButton
***********************************************/
CGUICheckButton::CGUICheckButton(int _id,float x,float y,float w,float h,bool bChecked)
{
	id=_id;
	m_bChecked=bChecked;

	bStatic=false;
	bVisible=true;
	bEnabled=true;

	rect.Set(x,y,x+w,y+h);
}

CGUICheckButton::~CGUICheckButton()
{
}

bool CGUICheckButton::MouseLButton(bool bDown)
{
	 if(g_pGST->Input_KeyDown(GSTK_LBUTTON)) m_bChecked=!m_bChecked;
	 
	 return !bDown;
}

void CGUICheckButton::Render()
{
	g_sprOkSign->SetColor(ARGB((1-m_fdt)*255,255,255,255));
	if(m_bChecked) g_sprOkSign->Render(rect.x1-5,rect.y1-5);
}

/***********************************************
*    CGUIEdit
***********************************************/
CGUIEdit::CGUIEdit(int _id,float x,float y,float w,float h,gstFont *fnt,DWORD dwTextColor,DWORD dwFrameColor)
{
	id=_id;
	bStatic=false;
	bVisible=true;
	bEnabled=true;
	rect.Set(x,y,x+w,y+h);

	m_nCurPos=0;
	m_fTime=0;
	m_bFocus=false;
	m_bBlink=false;
	m_bComposition=false;
	m_dwTextColor=dwTextColor;
	m_dwFrameColor=dwFrameColor;
	m_pFont=fnt;

	m_nAlign=GSTTEXT_LEFT;
	strcpy(m_szText,"");
	ClearCharArray();
}

CGUIEdit::~CGUIEdit(void)
{
	ClearCharArray();
}

void CGUIEdit::ClearCharArray()
{
	size_t size=m_pCharArray.size();
	for(size_t i=0;i<size;i++)
	{
		if(m_pCharArray[i]) delete m_pCharArray[i];
	}
	m_pCharArray.clear();
}

void CGUIEdit::SetAlign(int nAlign)
{
	m_nAlign=nAlign;
}

void CGUIEdit::Render()
{
	gst->Gfx_RenderLine(rect.x1-1, rect.y1-1, rect.x2+1, rect.y1-1, m_dwFrameColor);
	gst->Gfx_RenderLine(rect.x2+1, rect.y1-1, rect.x2+1, rect.y2+1, m_dwFrameColor);
	gst->Gfx_RenderLine(rect.x2+1, rect.y2+1, rect.x1-1, rect.y2+1, m_dwFrameColor);
	gst->Gfx_RenderLine(rect.x1-1, rect.y2+1, rect.x1-1, rect.y1-1, m_dwFrameColor);

	DWORD dwLastColor=m_pFont->GetColor();
	m_pFont->SetColor(m_dwTextColor);
	float x,y;
	if(m_nAlign==GSTTEXT_LEFT)
	{
		x=rect.x1;
		y=rect.y1+2;
	}
	else if(m_nAlign=GSTTEXT_CENTER)
	{
		x=rect.x1+(rect.x2-rect.x1)/2;
		y=rect.y1+2;
	}
	else if(m_nAlign=GSTTEXT_RIGHT)
	{
		x=rect.x2;
		y=rect.y1+2;
	}
	m_pFont->Render(x,y,m_nAlign,m_szText);
	m_pFont->SetColor(dwLastColor);

	if(m_bFocus && m_bBlink) RedrawCursor();
}

void CGUIEdit::Update(float dt)
{
	if(m_bFocus)
	{
		m_fTime+=dt;
		if(m_fTime>0.3)
		{
			m_fTime=0;
			m_bBlink=!m_bBlink;
		}
	}
}

void CGUIEdit::Focus(bool bFocused)
{
	m_bFocus=bFocused;
}

bool CGUIEdit::KeyClick(int key,int chr)
{
	if(m_bFocus && !IsComposition())
	{
		int size=(int)m_pCharArray.size();		

		switch(key)
		{
		case GSTK_ENTER:
		case GSTK_TAB:
			break;
		case GSTK_HOME:
			m_nCurPos=0;
			break;
		case GSTK_END:
			m_nCurPos=size;
			break;
		case GSTK_LEFT:
			if(m_nCurPos>0) m_nCurPos--;
			break;
		case GSTK_RIGHT:
			if(m_nCurPos<size) m_nCurPos++;
			break;
		case GSTK_DELETE:
			DelChar(m_nCurPos);
			break;
		case GSTK_BACKSPACE:
			if(m_nCurPos==0) break;
			DelChar(--m_nCurPos);
			break;
		default:
			if(chr==0 || IsComposition() || size==MAX_STRING) break;
			AddChar(chr,m_nCurPos++);
			UpdataString();
			break;
		}

		m_bBlink=true;
		m_fTime=0;
	}
	return false;
}

void CGUIEdit::StartComposition()
{
	m_bComposition=true;
}

void CGUIEdit::EndComposition()
{
	m_bComposition=false;
	UpdataString();
}

void CGUIEdit::OnImeCharMsg(const char *szImeChar)
{
	if(m_bFocus) AddCharEx(szImeChar,m_nCurPos++);
}

bool CGUIEdit::AddCharEx(const char *szNewChar,int nPos)
{
	st_Char *_pChar=new st_Char;
	if(!_pChar) return false;

	if(strlen(szNewChar)>2 || strlen(szNewChar)==0) return false;

	_pChar->bIME=(strlen(szNewChar))==1 ? false : true;
	strcpy(_pChar->szChar,szNewChar);

	if(nPos<0)
	{
		m_pCharArray.push_back(_pChar);
	}
	else
	{
		size_t size=m_pCharArray.size();
		if(nPos>(int)size)
			AddCharEx(szNewChar);
		else
		{
			vector<st_Char *>::iterator it;
			it=m_pCharArray.begin()+nPos;//&m_pCharArray[nPos];//////
			m_pCharArray.insert(it,_pChar);
		}
	}

	return true;
}

bool CGUIEdit::AddChar(const char szNewChar,int nPos)
{
	char szBuf[3];

	szBuf[0]=szNewChar;
	szBuf[1]='\0';

	return AddCharEx(szBuf,nPos);
}

bool CGUIEdit::DelChar(int nPos)
{
	size_t size=m_pCharArray.size();
	if(nPos<(int)size && nPos>=0)
	{
		if(m_pCharArray[nPos])
		{
			vector<st_Char *>::iterator it;
			it=m_pCharArray.begin()+nPos;
			m_pCharArray.erase(it);
			UpdataString();
			return true;
		}
	}
	return false;
}

void CGUIEdit::RedrawCursor()
{
	if(IsComposition())	return;
	
	size_t size=m_pCharArray.size();
	if(size>=0)
	{
		char szChar[MAX_STRING+1];
		int i;
		for(i=0;i<m_nCurPos;i++) szChar[i]=m_szText[i];
		szChar[i]='\0';
		
		int nCurPos;
		if(m_nAlign==GSTTEXT_LEFT)
			nCurPos=m_pFont->GetStringWidth(szChar)+m_pFont->GetTracking()+m_pFont->GetSpacing();
		else if(m_nAlign=GSTTEXT_CENTER)
			nCurPos=(rect.x2-rect.x1)/2-m_pFont->GetStringWidth(m_szText)/2+m_pFont->GetStringWidth(szChar)+m_pFont->GetTracking()+m_pFont->GetSpacing();
		else if(m_nAlign==GSTTEXT_RIGHT)
			nCurPos=rect.x2-m_pFont->GetStringWidth(m_szText)+m_pFont->GetStringWidth(szChar)+m_pFont->GetTracking()+m_pFont->GetSpacing();

		gst->Gfx_RenderLine(rect.x1 + nCurPos, rect.y1 + 1, rect.x1 + nCurPos, rect.y2 - 1, m_dwTextColor);
	}
}

void CGUIEdit::UpdataString()
{
	size_t size=m_pCharArray.size();
	strcpy(m_szText,"");
	if(size>=0)
	{
		for(size_t i=0;i<size;i++)
		{
			st_Char *_pChar=m_pCharArray[i];
			if(_pChar) strcat(m_szText,_pChar->szChar);
		}
	}
}