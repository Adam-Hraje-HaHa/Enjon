#ifndef ENJON_COMPONENT_SYSTEMS_H
#define ENJON_COMPONENT_SYSTEMS_H

#include <SDL2/SDL.h>

#include <Graphics/ParticleEngine2D.h>
#include <Graphics/Camera2D.h>
#include <AI/SpatialHash.h>

#include "ECS/Components.h"
#include "ECS/Entity.h"

#include "Masks.h"
#include "AnimationManager.h"
#include "Level.h"

#include <vector>
#include <unordered_map>

#define SCREEN_VERTICAL_CENTER			1440 / 2
#define SCREEN_HORIZONTAL_CENTER		900 / 2


struct TestSystem;
struct LabelSystem;
struct AttributeSystem;
struct PlayerControllerSystem;
struct Transform3DSystem;
struct CollisionSystem;
struct Animation2DSystem;
struct InventorySystem;
struct Renderer2DSystem;
struct AIControllerSystem;
struct EffectSystem;

// EntityManagerDeprecateds hold pointers to their system, which hold the arrays of data
// ComponentSystems don't need to know of mangers...or do they? Yes, they'll hold pointers to the managers of which they belong
// namespace ECS { namespace Systems {

// 	const float TILE_SIZE = 32.0f;
	
// 	struct EntityManagerDeprecated
// 	{
// 		eid32 NextAvailableID;
// 		eid32 MinID;
// 		eid32 MaxAvailableID;
// 		eid32 Length;

// 		struct Transform3DSystem* TransformSystem;
// 		struct AIControllerSystem* AIControllerSystem;
// 		struct PlayerControllerSystem* PlayerControllerSystem;
// 		struct Animation2DSystem* Animation2DSystem;
// 		struct LabelSystem* LabelSystem;
// 		struct CollisionSystem* CollisionSystem;
// 		struct AttributeSystem* AttributeSystem;
// 		struct Renderer2DSystem* Renderer2DSystem;
// 		struct InventorySystem* InventorySystem;
// 		struct EffectSystem* EffectSystem;
// 		EG::Particle2D::ParticleEngine2D* ParticleEngine;

// 		bitmask32 Masks[MAX_ENTITIES];
// 		Component::EntityType Types[MAX_ENTITIES];

// 		eid32 Player;

// 		int Width; 
// 		int Height;

// 		SpatialHash::Grid* Grid;
// 		Enjon::Graphics::Camera2D* Camera;

// 		Level* Lvl;

// 		std::vector<eid32> Entities;

// 		Enjon::f32 Time;
// 	};

// 	////////////////////
// 	// Entity Manager //
// 	////////////////////
	
// 	namespace EntitySystem 
// 	{ 
// 		// Creates and returns instance of an Entity Manager
// 		// TODO(John): Write custom allocators instead of using malloc/delete
// 		struct EntityManagerDeprecated* NewEntityManagerDeprecated(int Width, int Height, Enjon::Graphics::Camera2D* Camera, Level* Lvl);

// 		// Get global world object
// 		struct EntityManagerDeprecated* World();

// 		// Creates a blank entity, returns the eid and places in manager
// 		eid32 CreateEntity(struct EntityManagerDeprecated* Manager, bitmask32 Components);

// 		// Removes entity from manager by setting its bitfield to COMPONENT_NONE
// 		void RemoveEntity(struct EntityManagerDeprecated* Manager, eid32 Entity); 

// 		// Returns whether or not entity's bitfield is set to COMPONENT_NONE or not
// 		bool IsAlive(struct EntityManagerDeprecated* Manager, eid32 Entity);

		/* Turns off component from entity by bitwise ^= */
		// void RemoveComponents(struct EntityManagerDeprecated* Manager, eid32 Entity, bitmask32 Components);

// 		/* Adds components to entity by bitwise | */
// 		void AddComponents(bitmask32 Components);

// 		/* Returns world time in ticks */
// 		Enjon::f32 WorldTime();

// 		void Update();
// 	}
// }}



#endif