// Copyright 2016-2017 John Jackson. All Rights Reserved.
// File: EntityArchiver.cpp

#include "Asset/AssetManager.h"
#include "Serialize/EntityArchiver.h"
#include "SubsystemCatalog.h"
#include "Base/World.h" 
#include "Engine.h"
#include "SubsystemCatalog.h"

namespace Enjon
{
	//=========================================================================================

	Result EntityArchiver::Serialize( const EntityHandle& entity )
	{
		// Make call to static function and return result
		return Serialize( entity, &mBuffer );
	} 

	//==========================================================================

	Result EntityArchiver::Serialize( const EntityHandle& entity, ByteBuffer* buffer )
	{ 
		if ( !buffer )
		{
			return Result::FAILURE;
		}

		// Write that entity was null for deserializing later on
		if ( !entity.Get( ) )
		{
			buffer->Write< u32 >( 0 );
			return Result::FAILURE;
		}

		// Write that entity was alive and serialized
		buffer->Write< u32 >( 1 );

		//==========================================================================
		// Local Transform
		//========================================================================== 

		Transform local = entity.Get( )->GetLocalTransform( );

		// Write out position
		buffer->Write< f32 >( local.GetPosition().x );
		buffer->Write< f32 >( local.GetPosition().y );
		buffer->Write< f32 >( local.GetPosition().z );

		// Write out rotation
		buffer->Write< f32 >( local.GetRotation().x );
		buffer->Write< f32 >( local.GetRotation().y );
		buffer->Write< f32 >( local.GetRotation().z );
		buffer->Write< f32 >( local.GetRotation().w );

		// Write out scale
		buffer->Write< f32 >( local.GetScale().x );
		buffer->Write< f32 >( local.GetScale().y );
		buffer->Write< f32 >( local.GetScale().z );

		// Write out entity UUID
		buffer->Write< UUID >( entity.Get( )->GetUUID( ) );

		// Write out entity name
		buffer->Write< String >( entity.Get( )->GetName( ) ); 

		// Write out entity archetype
		buffer->Write< UUID >( entity.Get( )->GetArchetype( ) ? entity.Get( )->GetArchetype( ).GetUUID( ) : UUID::Invalid( ) );

		// Serialize the prototype entity UUID 
		buffer->Write< UUID >( entity.Get( )->HasPrototypeEntity( ) ? entity.Get( )->GetPrototypeEntity( ).Get( )->GetUUID( ) : UUID::Invalid( ) );

		// Serialize instanced entity size
		buffer->Write< u32 >( (u32)entity.Get( )->GetInstancedEntities( ).size( ) );

		// Serialize all entity instance uuids
		for ( auto& e : entity.Get( )->GetInstancedEntities( ) )
		{
			buffer->Write< UUID >( e.Get( )->GetUUID( ) );
		}

		//==========================================================================
		// Components
		//========================================================================== 

		const Vector< Component* >& comps = entity.Get( )->GetComponents( );

		// Write out number of comps
		buffer->Write< u32 >( ( u32 )comps.size( ) );

		// Write out component data
		for ( auto& c : comps ) 
		{ 
			// Get component's meta class
			const MetaClass* compCls = c->Class( ); 

			// Write out component class
			buffer->Write< String >( compCls->GetName( ) );

			// Need to write out specific data regarding the component, namely how much size there is so that I can 
			// skip the data in the buffer
			ByteBuffer temp; 
			if ( c->SerializeData( &temp ) == Result::INCOMPLETE )
			{
				SerializeObjectDataDefault( c, compCls, &temp );
			}
			buffer->Write< u32 >( temp.GetSize( ) );

			// Serialize component data
			Result res = c->SerializeData( buffer );
			if ( res == Result::INCOMPLETE )
			{
				SerializeObjectDataDefault( c, compCls, buffer );
			}
		}

		//================================================================================
		// Entity Children
		//================================================================================

		const Vector< EntityHandle >& children = entity.Get( )->GetChildren( );

		// Write out number of children 
		buffer->Write< u32 >( ( u32 )children.size( ) );

		// Serialize all children into buffer
		for ( auto& c : children )
		{
			Serialize( c, buffer );
		}

		// Serialize entity default ( remaining unserialized properties )
		SerializeObjectDataDefault( entity.Get( ), entity.Get( )->Class( ), buffer );

		return Result::SUCCESS;
	}

	//=========================================================================================

	Result EntityArchiver::Deserialize( const String& filePath, Vector< EntityHandle >& out )
	{
		return Result::FAILURE;
	}

	//=========================================================================================

	EntityHandle EntityArchiver::Deserialize( const String& filePath )
	{
		Reset( );

		// Fill buffer to read
		mBuffer.ReadFromFile( filePath );

		// If valid, read from buffer and fill out entity handle
		if ( mBuffer.GetStatus( ) == BufferStatus::ReadyToRead )
		{
			return Deserialize( &mBuffer );
		}

		// Return invalid entity handle
		return EntityHandle( );
	}

	EntityHandle EntityArchiver::Deserialize( ByteBuffer* buffer )
	{
		// Read status of entity from buffer
		u32 entityStatus = buffer->Read< u32 >( );

		// If entity wasn't alive, then it wasn't serialized
		if ( !entityStatus )
		{
			// Return empty entity handle
			return EntityHandle();
		}

		// Get entity manager from engine
		EntityManager* entities = Engine::GetInstance( )->GetSubsystemCatalog( )->Get< EntityManager >( )->ConstCast< EntityManager >( );

		// Handle to fill out
		// TODO(): This will fail if entity cannot be allocated! Need to check for that and then exit gracefully.
		EntityHandle handle = entities->Allocate( );

		// Deseralize internally with default world
		return EntityArchiver::DeserializeInternal( handle, buffer, Engine::GetInstance( )->GetWorld( ) );
	} 

	//========================================================================================= 

	EntityHandle EntityArchiver::Deserialize( ByteBuffer* buffer, World* world, bool isInstanced )
	{
		// Read status of entity from buffer
		u32 entityStatus = buffer->Read< u32 >( );

		// If entity wasn't alive, then it wasn't serialized
		if ( !entityStatus )
		{
			// Return empty entity handle
			return EntityHandle();
		}

		// Get entity manager from engine
		EntityManager* entities = Engine::GetInstance( )->GetSubsystemCatalog( )->Get< EntityManager >( )->ConstCast< EntityManager >( );

		// Handle to fill out
		// TODO(): This will fail if entity cannot be allocated! Need to check for that and then exit gracefully.
		EntityHandle handle = entities->Allocate( world );

		// Continue to deserialize data into entity
		return EntityArchiver::DeserializeInternal( handle, buffer, world, isInstanced ); 
	}

	//========================================================================================= 

	EntityHandle EntityArchiver::DeserializeInternal( const EntityHandle& handle, ByteBuffer* buffer, World* world, bool isInstanced ) 
	{
		// Cache off entity manager
		EntityManager* em = EngineSubsystem( EntityManager );
		AssetManager* am = EngineSubsystem( AssetManager );

		//==========================================================================
		// Local Transform
		//========================================================================== 

		Transform local;

		// Read in position
		Vec3 position; 
		position.x = buffer->Read< f32 >( );
		position.y = buffer->Read< f32 >( );
		position.z = buffer->Read< f32 >( ); 
		local.SetPosition( position );

		// Read in rotation
		Quaternion rotation;
		rotation.x = buffer->Read< f32 >( );
		rotation.y = buffer->Read< f32 >( );
		rotation.z = buffer->Read< f32 >( );
		rotation.w = buffer->Read< f32 >( );
		local.SetRotation( rotation );

		// Read in scale
		Vec3 scale;
		scale.x = buffer->Read< f32 >( );
		scale.y = buffer->Read< f32 >( );
		scale.z = buffer->Read< f32 >( );
		local.SetScale( scale );

		// Get the entity
		Entity* ent = handle.Get( );

		 //Set the transform of the entity
		ent->SetLocalTransform( local );

		// Remove from uuid map before setting uuid
		em->RemoveFromUUIDMap( ent );

		// Read in uuid
		ent->SetUUID( buffer->Read< UUID >( ) ); 

		// Read in name
		ent->SetName( buffer->Read< String >( ) );

		// Read in archetype
		ent->SetArchetype( am->GetAsset< Archetype >( buffer->Read< UUID >( ) ) );

		// Store prototype id
		UUID prototypeID = buffer->Read< UUID >( );

		// Deserialize the prototype entity UUID 
		ent->SetPrototypeEntity( em->GetEntityByUUID( prototypeID ) );

		// If archetype is default, remove the archetype and then set the id to invalid
		if ( ent->GetArchetype( ) == am->GetDefaultAsset< Archetype >( ) || !ent->GetArchetype( ) )
		{
			ent->SetArchetype( nullptr );
			ent->SetPrototypeEntity( EntityHandle::Invalid( ) );
		}
		
		// Deserialize instanced entity size
		u32 entityInstanceSize = buffer->Read< u32 >( );

		// Attempt to add entity instance
		for ( u32 i = 0; i < entityInstanceSize; ++i )
		{
			// If entity is valid, then set its prototype entity to this entity
			EntityHandle h = em->GetEntityByUUID( buffer->Read< UUID >( ) );

			// Only set prototype entities IFF we're not instancing this entity from another source
			if ( h && !isInstanced )
			{
				h.Get( )->SetPrototypeEntity( ent );
			}
		} 

		//=================================================================
		// Components
		//=================================================================

		u32 numComps = buffer->Read< u32 >( );

		for ( u32 i = 0; i < numComps; ++i )
		{
			// Get component's meta class
			const MetaClass* cmpCls = Object::GetClass( buffer->Read< String >( ) );

			u32 compWriteSize = buffer->Read< u32 >( );

			if ( cmpCls )
			{
				// Attach new component to entity using MetaClass
				Component* cmp = ent->AddComponent( cmpCls );
				if ( cmp )
				{
					Result res = cmp->DeserializeData( buffer );
					if ( res == Result::INCOMPLETE )
					{
						res = DeserializeObjectDataDefault( cmp, cmpCls, buffer );
					}

					// Deserialize late init ( Not sure where to do this... )
					cmp->DeserializeLateInit( );
				}
			}
			else
			{
				buffer->AdvanceReadPosition( compWriteSize );
			}
		}

		//=================================================================
		// Entity Children
		//=================================================================

		u32 numChildren = buffer->Read< u32 >();

		// Deserialize all children and add to handle
		for ( u32 i = 0; i < numChildren; ++i )
		{
			EntityHandle child = Deserialize( buffer, world );
			Entity* childEnt = child.Get( );
			if ( childEnt )
			{
				// Grab local transform of child that was just deserialized
				Transform localTrans = childEnt->GetLocalTransform( );

				// After adding child, local transform will be incorrect
				ent->AddChild( child );

				// Restore local transform
				childEnt->SetLocalTransform( localTrans );
			}
		}

		// Deserialize object default
		DeserializeObjectDataDefault( ent, ent->Class( ), buffer );

		// If prototype entity, then record all property overrides and then attempt merge
		if ( ent->HasPrototypeEntity() && !isInstanced )
		{
			// Clear all property overrides
			ObjectArchiver::ClearAllPropertyOverrides( ent );

			// Record all property overrides
			ObjectArchiver::RecordAllPropertyOverrides( ent->mPrototypeEntity.Get( ), ent );

			// Merge objects 
			ObjectArchiver::MergeObjects( ent->mPrototypeEntity.Get( ), ent, MergeType::AcceptMerge );
		} 

		return ent;
	}

	//========================================================================================= 
}













