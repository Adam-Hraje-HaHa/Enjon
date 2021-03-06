// File: Archetype.h
// Copyright 2016-2018 John Jackson. All Rights Reserved.

#ifndef ENJON_ARCHETYPE_H
#define ENJON_ARCHETYPE_H
#pragma once

#include "Asset/Asset.h"
#include "Math/Transform.h"

namespace Enjon
{
	class ArchetypeAssetLoader;
	class EntityHandle;
	class Entity;
	class World;

	ENJON_CLASS( )
	class Archetype : public Asset
	{
		friend ArchetypeAssetLoader;

		ENJON_CLASS_BODY( Archetype )

		public:

			/**
			* @brief Copy Constructor
			*/
			Archetype( const Archetype& other );

			/**
			* @brief
			*/
			virtual void ExplicitConstructor( ) override; 
			
			/**
			* @brief
			*/
			virtual Result SerializeData( ByteBuffer* buffer ) const override;

			/**
			* @brief
			*/
			virtual Result DeserializeData( ByteBuffer* buffer ) override; 

			/**
			* @brief
			*/
			EntityHandle ConstructFromEntity( const EntityHandle& entity ); 

			/**
			* @brief
			*/
			EntityHandle Instantiate( const Transform& transform = Transform(), World* world = nullptr ); 

			/**
			* @brief
			*/
			EntityHandle GetRootEntity( ); 

			/**
			* @brief
			*/
			EntityHandle CopyRootEntity( Transform transform, World* world );
 
			/**
			* @brief
			*/
			virtual Result CopyFromOther( const Asset* other ) override;

			/**
			* @brief
			*/
			virtual Result Reload( ) override; 

			/**
			* @brief
			*/
			bool ExistsInHierachy( const AssetHandle< Archetype >& archetype );

			/**
			* @brief
			*/
			static void RecursivelyMergeEntities( const EntityHandle& source, const EntityHandle& dest, MergeType mergeType );

		private:

			/**
			* @brief
			*/
			void RecursivelySetArchetype( const EntityHandle& handle );

			/**
			* @brief
			*/
			void RecursivelySetToRoot( const EntityHandle& entity );

			/**
			* @brief
			*/
			void RecursivelyRemoveFromRoot( const EntityHandle& entity );

			/**
			* @brief
			*/
			void RecursivelyFixInstancedEntities( const EntityHandle& dest, const HashMap< String, String >& uuidMap );

			/**
			* @brief
			*/
			bool RecursivelySearchForArchetypeInstance( const AssetHandle< Archetype >& archetype, const EntityHandle& entity );

			/**
			* @brief
			*/
			void FillUUIDMap( const EntityHandle& entity, HashMap< String, String >* uuidMap ); 

			/**
			* @brief
			*/
			HashMap< String, String > ConstructUUIDMap( const EntityHandle& entity );

		protected: 
			Entity* mRoot = nullptr;
	};
}

/*
	// What does it look like to instantiate an archetype? 
	// What does it look like to create an archetype from an existing entity? 
	// Probably need a way to parent entities within the editor first

	// How do we build an archetype? Pass in an entity to construct a new one? Can we edit an existing archetype? Seems like that's something feasible. 

	// So you pass in an entity and then tell the archetype to clone its data with that entity. 
	// Constructing a new archetype asset should be as simple as giving you an empty entity with no components. 

	// What about serializing / deserializing data? Just grab an entity archiver and use that? Serialize exactly the same way as scene data would? Need to alter the entity archiver then. 

*/

#endif
