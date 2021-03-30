//==========================================================================
//
//==========================================================================

#include <assert.h>
#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "com_model.h"
#include "studio.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "dlight.h"
#include "triangleapi.h"

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "studio_util.h"
#include "r_studioint.h"

#include "StudioModelRenderer.h"
#include "PhyStudioModelRenderer.h"
#include<StudioModelRenderer.h>
#include "Exports.h"

#include "phy_corpse.h"
#include"physics.h"

//
// Override the StudioModelRender virtual member functions here to implement custom bone
// setup, blending, etc.
//

// Global engine <-> studio model rendering code interface
extern engine_studio_api_t IEngineStudio;

// The renderer object, created on the stack.
PhyStudioModelRenderer g_StudioRenderer;
/*
====================
PhyStudioModelRenderer

====================
*/
PhyStudioModelRenderer::PhyStudioModelRenderer(void)
{
}
void PhyStudioModelRenderer::Init(void)
{
	CStudioModelRenderer::Init();
	InitPhysicsInterface(NULL);
	gPhysics.InitSystem("valve", &IEngineStudio);
}
int PhyStudioModelRenderer::StudioDrawRagdoll(int flags)
{
	alight_t lighting;
	vec3_t dir;

	m_pCurrentEntity = IEngineStudio.GetCurrentEntity();
	IEngineStudio.GetTimes(&m_nFrameCount, &m_clTime, &m_clOldTime);
	IEngineStudio.GetViewInfo(m_vRenderOrigin, m_vUp, m_vRight, m_vNormal);
	IEngineStudio.GetAliasScale(&m_fSoftwareXScale, &m_fSoftwareYScale);

	m_pRenderModel = m_pCurrentEntity->model;
	m_pStudioHeader = (studiohdr_t*)IEngineStudio.Mod_Extradata(m_pRenderModel);
	IEngineStudio.StudioSetHeader(m_pStudioHeader);
	IEngineStudio.SetRenderModel(m_pRenderModel);

	StudioSetUpTransform(0);

	if (flags & STUDIO_RENDER)
	{
		// see if the bounding box lets us trivially reject, also sets
		if (!IEngineStudio.StudioCheckBBox())
			return 0;

		(*m_pModelsDrawn)++;
		(*m_pStudioModelCount)++; // render data cache cookie

		if (m_pStudioHeader->numbodyparts == 0)
			return 1;
	}

	if (m_pCurrentEntity->curstate.movetype == MOVETYPE_FOLLOW)
	{
		StudioMergeBones(m_pRenderModel);
	}
	else
	{
		gPhysics.SetupBonesPhysically(m_pCurrentEntity->index);
		m_pCurrentEntity->origin.x = (*m_pbonetransform)[1][0][3];
		m_pCurrentEntity->origin.y = (*m_pbonetransform)[1][1][3];
		m_pCurrentEntity->origin.z = (*m_pbonetransform)[1][2][3];
	}
	StudioSaveBones();

	if (flags & STUDIO_EVENTS)
	{
		StudioCalcAttachments();
		IEngineStudio.StudioClientEvents();
		// copy attachments into global entity array
		if (m_pCurrentEntity->index > 0)
		{
			cl_entity_t* ent = gEngfuncs.GetEntityByIndex(m_pCurrentEntity->index);

			memcpy(ent->attachment, m_pCurrentEntity->attachment, sizeof(vec3_t) * 4);
		}
	}

	if (flags & STUDIO_RENDER)
	{
		lighting.plightvec = dir;
		IEngineStudio.StudioDynamicLight(m_pCurrentEntity, &lighting);

		IEngineStudio.StudioEntityLight(&lighting);

		// model and frame independant
		IEngineStudio.StudioSetupLighting(&lighting);

		// get remap colors

		m_nTopColor = m_pCurrentEntity->curstate.colormap & 0xFF;
		m_nBottomColor = (m_pCurrentEntity->curstate.colormap & 0xFF00) >> 8;

		IEngineStudio.StudioSetRemapColors(m_nTopColor, m_nBottomColor);

		StudioRenderModel();
	}

	return 1;
}

int PhyStudioModelRenderer::StudioDrawModel(int flags)
{
	m_pCurrentEntity = IEngineStudio.GetCurrentEntity();

	if (pgCorpseMgr->IsRagdollCorpse(m_pCurrentEntity))
		return StudioDrawRagdoll(flags);

	if (m_pCurrentEntity->index)// pass worldspawn or tempentity

		// hard coded here, we need a better way.
		if ((31 <= m_pCurrentEntity->curstate.sequence &&
			m_pCurrentEntity->curstate.sequence <= 43 &&
			strstr(m_pCurrentEntity->model->name, "scientist")) ||
			(15 <= m_pCurrentEntity->curstate.sequence &&
				m_pCurrentEntity->curstate.sequence <= 19 &&
				strstr(m_pCurrentEntity->model->name, "zombie")))
		{
			// If the entity (ie players, NPCs) is alive.
			if (!pgCorpseMgr->IsEntityDead(m_pCurrentEntity))
			{
				// let studio setup bone matrix, so we can use it as ragdoll init pose.
				CStudioModelRenderer::StudioDrawModel(0);

				TEMPENTITY* tempent = pgCorpseMgr->CreateRagdollCorpse(m_pCurrentEntity);
				// Set init ragdoll pose
				gPhysics.SetPose(tempent->entity.index, (float*)m_pbonetransform);
				pgCorpseMgr->EntityDie(m_pCurrentEntity);
			}
			else
			{
				// Simply return. Do not render the original entity.
				// Because it has already dead, a ragdoll replace it.
				return 0;
			}
		}
		else
		{
			pgCorpseMgr->EntityRespawn(m_pCurrentEntity);
		}

	// normal render
	return CStudioModelRenderer::StudioDrawModel(flags);
}

int PhyStudioModelRenderer::StudioDrawPlayer(int flags, entity_state_s* pplayer)
{
	return CStudioModelRenderer::StudioDrawPlayer(flags, pplayer);
}

////////////////////////////////////
// Hooks to class implementation
////////////////////////////////////

/*
====================
R_StudioDrawPlayer

====================
*/
int R_StudioDrawPlayer(int flags, entity_state_t* pplayer)
{
	return g_StudioRenderer.StudioDrawPlayer(flags, pplayer);
}

/*
====================
R_StudioDrawModel

====================
*/
int R_StudioDrawModel(int flags)
{
	return g_StudioRenderer.StudioDrawModel(flags);
}

/*
====================
R_StudioInit

====================
*/
void R_StudioInit(void)
{
	g_StudioRenderer.Init();
}

// The simple drawing interface we'll pass back to the engine
r_studio_interface_t studio =
{
	STUDIO_INTERFACE_VERSION,
	R_StudioDrawModel,
	R_StudioDrawPlayer,
};

/*
====================
HUD_GetStudioModelInterface

Export this function for the engine to use the studio renderer class to render objects.
====================
*/
int DLLEXPORT HUD_GetStudioModelInterface(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio)
{
	//	RecClStudioInterface(version, ppinterface, pstudio);

	if (version != STUDIO_INTERFACE_VERSION)
		return 0;

	// Point the engine to our callbacks
	*ppinterface = &studio;

	// Copy in engine helper functions
	memcpy(&IEngineStudio, pstudio, sizeof(IEngineStudio));

	// Initialize local variables, etc.
	R_StudioInit();

	// Success
	return 1;
}

#pragma region phy_corpse.cpp
bool gMapExistTempEnt = 0;
CorpseManager* pgCorpseMgr = nullptr;

CorpseManager::CorpseManager(void)
{
	memset(_entityDead, 0, MAX_ENTITIES);
}

bool CorpseManager::IsEntityDead(cl_entity_t* ent)
{
	return _entityDead[ent->index];
}

void CorpseManager::EntityDie(cl_entity_t* ent)
{
	_entityDead[ent->index] = true;
}

void CorpseManager::EntityRespawn(cl_entity_t* ent)
{
	_entityDead[ent->index] = false;
}
void OnRagdollKill(struct tempent_s* ent, float frametime, float currenttime)
{
	gPhysics.DisposeRagdollController(ent->entity.index);
	gEngfuncs.Con_DPrintf("free ragdoll [%d]\n", ent->entity.index);
}
TEMPENTITY* CorpseManager::CreateRagdollCorpse(cl_entity_t* ent)
{
	TEMPENTITY* tempent = gEngfuncs.pEfxAPI->CL_TempEntAlloc(ent->curstate.origin, ent->model);
	tempent->entity.curstate.iuser1 = ent->index;
	tempent->entity.curstate.iuser3 = PhyCorpseFlag1;
	tempent->entity.curstate.iuser4 = PhyCorpseFlag2;
	tempent->entity.curstate.body = ent->curstate.body;
	tempent->entity.curstate.skin = ent->curstate.skin;
	entity_state_t* entstate = &ent->curstate;
	entity_state_t* tempstate = &tempent->entity.curstate;
	tempent->entity.angles = ent->angles;
	tempent->entity.latched = ent->latched;
	tempstate->angles = entstate->angles;
	tempstate->animtime = entstate->animtime;
	tempstate->sequence = entstate->sequence;
	tempstate->aiment = entstate->aiment;
	tempstate->frame = entstate->frame;


	tempent->die = gEngfuncs.GetClientTime() + 30;
	tempent->flags = FTENT_KILLCALLBACK;
	tempent->callback = OnRagdollKill;
	tempent->entity.index = _corpseIndex++;
	gPhysics.CreateRagdollControllerModel(tempent->entity.index, (ent->model));
	gPhysics.StartRagdoll(tempent->entity.index);
	gPhysics.SetVelocity(tempent->entity.index, (Vector3*)&ent->curstate.velocity);

	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	Vector v = (ent->origin - local->origin).Normalize();
	v = v * 5;
	gPhysics.SetVelocity(tempent->entity.index, (Vector3*)&v);

	gEngfuncs.Con_DPrintf("corpse [%d]'s velocity is %f\n", tempent->entity.index, ent->curstate.velocity.Length());
	gEngfuncs.Con_DPrintf("create corpse [%d] for entity [%d]\n", tempent->entity.index, ent->index);
	return tempent;
}

bool CorpseManager::IsRagdollCorpse(cl_entity_t* ent)
{
	return (ent->curstate.iuser3 == PhyCorpseFlag1 &&
		ent->curstate.iuser4 == PhyCorpseFlag2);
}
#pragma endregion

