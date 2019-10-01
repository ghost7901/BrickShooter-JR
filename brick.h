#pragma once

#include "brickobj.h"

void BrickRelease();
void BrickReset();
void BrickUpdate(float dt);
void BrickRender();
bool BrickRotate1();
bool BrickRotate2();
CBrickBase *BrickAdd(int cx,int cy);
CBrickBase *BrickGet(int cx,int cy);
bool GameLoad(char *szFileName);
bool GameSave(char *szFileName);
void Undo(bool bSave=false);