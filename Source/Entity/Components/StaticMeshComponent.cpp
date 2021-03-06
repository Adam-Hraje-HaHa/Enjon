#include "Entity/Components/StaticMeshComponent.h"
#include "Entity/EntityManager.h" 
#include "Graphics/GraphicsScene.h"
#include "Serialize/AssetArchiver.h"
#include "Asset/AssetManager.h"
#include "Graphics/GraphicsSubsystem.h"
#include "SubsystemCatalog.h"
#include "ImGui/ImGuiManager.h"
#include "Entity/EntityManager.h"
#include "Base/World.h"
#include "Engine.h"

namespace Enjon
{
	//====================================================================

	void StaticMeshComponent::ExplicitConstructor()
	{ 
		// Set explicit tick state
		mTickState = ComponentTickState::TickAlways;
	} 

	//====================================================================

	void StaticMeshComponent::ExplicitDestructor()
	{
		// Remove renderable from scene
		if (mRenderable.GetGraphicsScene() != nullptr)
		{
			mRenderable.GetGraphicsScene()->RemoveStaticMeshRenderable(&mRenderable);
		}
	}

	//==================================================================== 

	void StaticMeshComponent::PostConstruction( )
	{
		// Add default mesh and material for renderable
		AssetManager* am = EngineSubsystem( AssetManager );
		mRenderable.SetMesh( am->GetDefaultAsset< Mesh >( ) );

		// Set default materials for all material elements
		for ( u32 i = 0; i < mRenderable.GetMesh( )->GetSubMeshCount( ); ++i ) 
		{
			mRenderable.SetMaterial( am->GetDefaultAsset< Material >( ), i ); 
		} 

		// Get graphics scene from world graphics context
		World* world = mEntity->GetWorld( )->ConstCast< World >( );
		GraphicsScene* gs = world->GetContext< GraphicsSubsystemContext >( )->GetGraphicsScene( ); 

		// Add renderable to scene
		gs->AddStaticMeshRenderable( &mRenderable );

		// Set id of renderable to entity id
		mRenderable.SetRenderableID( mEntity->GetID( ) ); 
	}

	//==================================================================== 

	void StaticMeshComponent::AddToWorld( World* world )
	{
		RemoveFromWorld( );

		// Get graphics scene from world graphics context
		GraphicsScene* gs = world->GetContext< GraphicsSubsystemContext >( )->GetGraphicsScene( ); 

		// Add to graphics scene
		if ( gs )
		{
			gs->AddStaticMeshRenderable( &mRenderable );
		}
	}

	//==================================================================== 

	void StaticMeshComponent::RemoveFromWorld( )
	{
		if ( mRenderable.GetGraphicsScene( ) != nullptr )
		{
			mRenderable.GetGraphicsScene( )->RemoveStaticMeshRenderable( &mRenderable );
		} 
	}

	void StaticMeshComponent::Update( )
	{
		mRenderable.SetTransform(mEntity->GetWorldTransform());
	}
	
	//====================================================================
		
	Vec3 StaticMeshComponent::GetPosition() const
	{ 
		return mRenderable.GetPosition(); 
	}
	
	//====================================================================

	Vec3 StaticMeshComponent::GetScale() const
	{ 
		return mRenderable.GetScale(); 
	}

	//====================================================================

	Quaternion StaticMeshComponent::GetRotation() const 
	{ 
		return mRenderable.GetRotation(); 
	}

	//====================================================================

	AssetHandle< Material > StaticMeshComponent::GetMaterial( const u32& idx ) const
	{ 
		return mRenderable.GetMaterial( idx ); 
	}

	//====================================================================

	AssetHandle<Mesh> StaticMeshComponent::GetMesh() const
	{ 
		return mRenderable.GetMesh(); 
	}

	//====================================================================

	GraphicsScene* StaticMeshComponent::GetGraphicsScene() const
	{ 
		return mRenderable.GetGraphicsScene(); 
	}

	//====================================================================

	Transform StaticMeshComponent::GetTransform() const 
	{ 
		return mRenderable.GetTransform(); 
	} 

	//====================================================================

	StaticMeshRenderable* StaticMeshComponent::GetRenderable()
	{ 
		return &mRenderable; 
	}
	
	/* Sets world transform */
	void StaticMeshComponent::SetTransform( const Transform& transform )
	{
		mRenderable.SetTransform( transform );
	}

	//====================================================================

	void StaticMeshComponent::SetPosition(const Vec3& position)
	{
		mRenderable.SetPosition(position);
	}

	//====================================================================

	void StaticMeshComponent::SetScale(const Vec3& scale)
	{
		mRenderable.SetScale(scale);
	}

	//====================================================================

	void StaticMeshComponent::SetScale(const f32& scale)
	{
		mRenderable.SetScale(scale);
	}

	//====================================================================

	void StaticMeshComponent::SetRotation(const Quaternion& rotation)
	{
		mRenderable.SetRotation(rotation);
	}
 
	//====================================================================

	void StaticMeshComponent::SetMaterial( const AssetHandle< Material >& material, const u32& idx ) 
	{
		mRenderable.SetMaterial( material, idx );
	}

	//====================================================================

	void StaticMeshComponent::SetMesh(const AssetHandle<Mesh>& mesh)
	{
		mRenderable.SetMesh( mesh );
	}

	//====================================================================

	void StaticMeshComponent::SetGraphicsScene(GraphicsScene* scene)
	{
		mRenderable.SetGraphicsScene(scene);
	} 

	//====================================================================

	Result StaticMeshComponent::SerializeData( ByteBuffer* buffer ) const
	{
		// Write uuid of mesh
		buffer->Write< UUID >( mRenderable.GetMesh( )->GetUUID( ) );

		// Write out renderable material size
		buffer->Write< u32 >( mRenderable.GetMaterialsCount( ) );

		// Write uuid of materials in renderable
		for ( auto& mat : mRenderable.GetMaterials( ) )
		{
			buffer->Write< UUID >( mat.Get()->GetUUID( ) );
		}

		return Result::SUCCESS;
	}

	//====================================================================

	Result StaticMeshComponent::DeserializeData( ByteBuffer* buffer )
	{
		// Get asset manager
		AssetManager* am = EngineSubsystem( AssetManager );

		// Set mesh
		mRenderable.SetMesh( am->GetAsset< Mesh >( buffer->Read< UUID >( ) ) );

		// Get count of materials
		u32 matCount = buffer->Read< u32 >( );

		// Deserialize materials
		for ( u32 i = 0; i < matCount; ++i )
		{
			// Grab the material
			AssetHandle< Material > mat = am->GetAsset< Material >( buffer->Read< UUID >( ) );

			// Set material in renderable at index
			mRenderable.SetMaterial( mat, i );
		} 

		return Result::SUCCESS;
	}

	//==================================================================== 

	Result StaticMeshComponent::OnEditorUI( )
	{
		ImGuiManager* igm = EngineSubsystem( ImGuiManager );

		// Debug dump renderable
		igm->InspectObject( &mRenderable );

		// Reset renderable mesh
		mRenderable.SetMesh( mRenderable.GetMesh( ) );

		return Result::SUCCESS;
	}

	//==================================================================== 
}
