#pragma once

#include<APIProxy.h>
#include<cl_entity.h>
#include<r_efx.h>

// magic num
#define PhyCorpseFlag1 (753951)
#define PhyCorpseFlag2 (152358)

#define MAX_ENTITIES 512


class CorpseManager
{
public:
	CorpseManager(void);

	// check if the entity is already dead
	bool IsEntityDead(cl_entity_t* ent);

	// tells the mgr that the entity died just now.
	void EntityDie(cl_entity_t* ent);

	// if entity plays any sequences other than death sequences,
	// we tells the mgr this entity is alive.
	void EntityRespawn(cl_entity_t* ent);

	// create ragdoll corpse for specified entity
	TEMPENTITY* CreateRagdollCorpse(cl_entity_t* ent);

	// check if the entity is a ragdoll corpse (temp entity)
	bool IsRagdollCorpse(cl_entity_t* ent);

private:
	// max server side entity count elements
	bool _entityDead[MAX_ENTITIES];
	int _corpseIndex = MAX_ENTITIES;
};

extern CorpseManager* pgCorpseMgr;