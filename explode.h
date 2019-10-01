#pragma once

#include <gstparticle.h>

void ExplodeAdd(float x,float y,int parIndex,bool bRotate=false);
void ExplodeAdd(float x,float y,gstParticleSystem *par);
void ExplodeUpdate(float dt);
void ExplodeRender();
bool ExplodeFinished();