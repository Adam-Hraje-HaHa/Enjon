#ifndef ENJON_INVENTORY_SYSTEM_H
#define ENJON_INVENTORY_SYSTEM_H

#include "ECS/Components.h"
#include "ECS/Entity.h"
#include "ECS/ComponentSystems.h"

struct InventorySystem
{
	ECS::Systems::EntityManagerDeprecated* Manager;
	ECS::Component::InventoryComponent Inventories[MAX_ENTITIES];
}; 

namespace ECS { namespace Systems { namespace Inventory {

	// Updates Transforms of EntityManagerDeprecated
	void Update(InventorySystem* System);

	// Creates new Transform3DSystem
	InventorySystem* NewInventorySystem(struct EntityManagerDeprecated* Manager);		
}}}


#endif

