#include <list>
#include "explode.h"
#include "cell.h"

extern gstParticleSystem *g_parBomb;
extern gstParticleSystem *g_parExp;
extern gstParticleSystem *g_parLightning;
extern gstParticleSystem *g_parColChange;
extern gstParticleSystem *g_parDirChange;
extern gstParticleManager *g_ParManager;

void ExplodeAdd(float x,float y,int parIndex,bool bRotate)
{
	gstParticleSystem *parExpl;	
	if(parIndex>=CO_REDBRICK && parIndex<=CO_PINKBRICK || parIndex==CO_UNIVERSALBRICK)
		parExpl=g_parExp;
	else if(parIndex==CO_BOMBBRICK)
		parExpl=g_parBomb;
	else if(parIndex==CO_LIGHTNINGBRICK)
		parExpl=g_parLightning;
	else if(parIndex==CO_COLCHANGEBRICK)
		parExpl=g_parColChange;
	else if(parIndex==CO_ARROWCHANGERBRICK)
		parExpl=g_parDirChange;
		
	DWORD col1,col2;
	switch(parIndex)
	{
	case CO_REDBRICK:
		col1=col2=ARGB(255,255,0,0);
		break;
	case CO_GREENBRICK:
		col1=col2=ARGB(255,0,255,0);
		break;
	case CO_BLUEBRICK:
		col1=col2=ARGB(255,0,0,255);
		break;
	case CO_SKYYELLOWBRICK:
		col1=col2=ARGB(255,255,150,3);
		break;
	case CO_YELLOWBRICK:
		col1=col2=ARGB(255,255,255,0);
		break;
	case CO_GRAYBRICK:
		col1=col2=ARGB(255,192,192,192);
		break;
	case CO_CYANBRICK:
		col1=col2=ARGB(255,0,255,255);
		break;
	case CO_PINKBRICK:
		col1=col2=ARGB(255,255,0,156);
		break;
	case CO_BOMBBRICK:
		col1=ARGB(255,255,255,0);
		col2=ARGB(255,234,64,0);
		break;
	case CO_UNIVERSALBRICK:
		col1=ARGB(255,255,255,255);
		col2=ARGB(255,255,255,255);
		break;
	case CO_LIGHTNINGBRICK:
	case CO_COLCHANGEBRICK:
		col1=col2=ARGB(255,255,255,255);
		break;
	case CO_ARROWCHANGERBRICK:
		col1=col2=ARGB(255,255,255,255);
		break;
	default:
		col1=col2=ARGB(255,0,0,0);
	}
	parExpl->info.colColorStart=col1;
	parExpl->info.colColorEnd=col2;
	parExpl->info.fDirection = bRotate ? M_PI_2 : 0;

	gstParticleSystemInfo parInfo=parExpl->info;
	g_ParManager->SpawnPS(&parInfo,x,y);
}

void ExplodeAdd(float x,float y,gstParticleSystem *par)
{
	gstParticleSystemInfo parInfo=par->info;
	g_ParManager->SpawnPS(&parInfo,x,y);
}

void ExplodeUpdate(float dt)
{
	g_ParManager->Update(dt);
}

void ExplodeRender()
{	
	g_ParManager->Render();
}

bool ExplodeFinished()
{
	return g_ParManager->IsEmpty();
}