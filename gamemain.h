#ifndef _GAMEMAIN_H
#define _GAMEMAIN_H

#pragma once

#include <gstsprite.h>
#include <gstgui.h>

bool GameLoop();
bool GameInitiate();
void GameRelease();
bool ResourceInit();
bool DoCommands(int id);
void IniLoad();
void IniSave();
bool FocusLost();
bool FocusGain();
void Show(gstSprite *spr,float x,float y,int speed,gstGUI *gui=0);
bool HighScore(bool bSave=false);

#endif