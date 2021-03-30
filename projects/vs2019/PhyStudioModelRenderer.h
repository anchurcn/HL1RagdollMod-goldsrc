// Author: Anchur

#if !defined( PHYSTUDIOMODELRENDERER_H )
#define PHYSTUDIOMODELRENDERER_H
#if defined( _WIN32 )
#pragma once
#endif

#include<StudioModelRenderer.h>
/*
====================
PhyStudioModelRenderer

====================
*/
class PhyStudioModelRenderer : public CStudioModelRenderer
{
public:
	PhyStudioModelRenderer(void);
	// override public interfaces
	virtual int StudioDrawModel(int flags);
	virtual int StudioDrawPlayer(int flags, struct entity_state_s* pplayer);

	virtual void Init(void);
	int StudioDrawRagdoll(int flags);
};

#endif // PHYSTUDIOMODELRENDERER_H