#pragma once
#ifndef ENJON_GRAPHICS_SCENE_H
#define ENJON_GRAPHICS_SCENE_H

#include "Defines.h"
#include "System/Types.h"
#include "Graphics/Color.h"
#include "Graphics/Camera.h"
#include "Base/Object.h"

#include <set>
#include <vector>

namespace Enjon 
{ 
	class Renderable;
	class StaticMeshRenderable;
	class SkeletalMeshRenderable;
	class DirectionalLight;
	class PointLight;
	class SpotLight;
	class QuadBatch;

	enum class RenderableSortType
	{
		MATERIAL,
		DEPTH,
		NONE
	};

	struct AmbientSettings
	{
		AmbientSettings()
		{
			mColor = RGBA32_White();
			mIntensity = 1.0f;
		}

		AmbientSettings(ColorRGBA32& color, float intensity)
			: mColor(color), mIntensity(intensity)
		{
		}

		ColorRGBA32 mColor;	
		float mIntensity;
	};

	using RenderableID = u32;

	ENJON_CLASS( )
	class GraphicsScene : public Enjon::Object
	{
		ENJON_CLASS_BODY( GraphicsScene )

		public:

			/*
			* @brief
			*/
			virtual void ExplicitDestructor( ) override;

			/*
			* @brief
			*/
			void AddSkeletalMeshRenderable( SkeletalMeshRenderable* renderable );

			/*
			* @brief
			*/
			void RemoveSkeletalMeshRenderable( SkeletalMeshRenderable* renderable );

			/*
			* @brief
			*/
			void AddStaticMeshRenderable( StaticMeshRenderable* renderable );

			/*
			* @brief
			*/
			void AddNonDepthTestedStaticMeshRenderable( StaticMeshRenderable* renderable );

			/*
			* @brief
			*/
			void AddCustomRenderable( Renderable* renderable );

			/*
			* @brief
			*/
			void RemoveStaticMeshRenderable( StaticMeshRenderable* renderable );

			/*
			* @brief
			*/
			void RemoveNonDepthTestedStaticMeshRenderable( StaticMeshRenderable* renderable );

			/*
			* @brief
			*/
			void RemoveCustomRenderable( Renderable* renderable );

			/*
			* @brief
			*/
			void AddQuadBatch( QuadBatch* batch );

			/*
			* @brief
			*/
			void RemoveQuadBatch( QuadBatch* batch );

			/*
			* @brief
			*/
			void AddDirectionalLight( DirectionalLight* light );
			
			/*
			* @brief
			*/
			void RemoveDirectionalLight( DirectionalLight* light );

			/*
			* @brief
			*/
			void AddPointLight( PointLight* light );

			/*
			* @brief
			*/
			void RemovePointLight( PointLight* light );

			/*
			* @brief
			*/
			void AddSpotLight( SpotLight* light );

			/*
			* @brief
			*/
			void RemoveSpotLight( SpotLight* light );

			/*
			* @brief
			*/
			void AddCamera( Camera* camera );

			/*
			* @brief
			*/
			void RemoveCamera( Camera* camera );

			/*
			* @brief
			*/
			AmbientSettings* GetAmbientSettings() { return &mAmbientSettings; }

			/*
			* @brief
			*/
			void SetAmbientSettings( AmbientSettings& settings );

			/*
			* @brief
			*/
			void SetAmbientColor( ColorRGBA32& color );

			/*
			* @brief
			*/
			const Vector<StaticMeshRenderable*>& GetStaticMeshRenderables( ) const;

			/*
			* @brief
			*/
			const Vector<StaticMeshRenderable*>& GetNonDepthTestedStaticMeshRenderables();

			/*
			* @brief
			*/
			const Vector<SkeletalMeshRenderable*>& GetSkeletalMeshRenderables( ) const;

			/*
			* @brief
			*/
			const Vector<Renderable*>& GetCustomRenderables() const;

			/*
			* @brief
			*/
			const HashSet<QuadBatch*>& GetQuadBatches() const { return mQuadBatches; }

			/*
			* @brief
			*/
			const HashSet<DirectionalLight*>& GetDirectionalLights() const { return mDirectionalLights; }

			/*
			* @brief
			*/
			const HashSet<PointLight*>& GetPointLights() const { return mPointLights; }

			/*
			* @brief
			*/
			const HashSet<SpotLight*>& GetSpotLights() const { return mSpotLights; }

			/*
			* @brief
			*/
			Camera* GetActiveCamera( );

			/*
			* @brief
			*/
			void SetActiveCamera( Camera* camera );

			const HashSet< Camera* >& GetCameras( ) const 
			{
				return mCameras;
			}

		private: 

			/*
			* @brief
			*/
			void SortStaticMeshRenderables( RenderableSortType type = RenderableSortType::MATERIAL );

		private:

			/*
			* @brief
			*/
			static bool CompareMaterial(Renderable* a, Renderable* b);

			/*
			* @brief
			*/
			static bool CompareDepth(Renderable* a, Renderable* b);

		private:

			ENJON_PROPERTY( )
			Vector< StaticMeshRenderable* > mSortedStaticMeshRenderables; 

			ENJON_PROPERTY( )
			Vector< SkeletalMeshRenderable* > mSortedSkeletalMeshRenderables; 
 
			ENJON_PROPERTY( )
			Vector< StaticMeshRenderable* > mNonDepthTestedStaticMeshRenderables;

			// Default list of any assortment of custom user renderable types? Not sure about this...
			ENJON_PROPERTY()
			Vector< Renderable* > mSortedCustomRenderables;

			HashSet<Camera*> mCameras;
			HashSet<Renderable*> mCustomRenderables;
			HashSet<StaticMeshRenderable*> mStaticMeshRenderables;
			HashSet<SkeletalMeshRenderable*> mSkeletalMeshRenderables;
			HashSet<QuadBatch*> mQuadBatches;
			HashSet<DirectionalLight*> mDirectionalLights;
			HashSet<PointLight*> mPointLights;
			HashSet<SpotLight*> mSpotLights; 
			AmbientSettings mAmbientSettings; 

			// Not sure that I like this "solution"
			Camera* mActiveCamera = nullptr;
			Camera mDefaultCamera;

			// Scene should own all the data for renderables, lights, etc. and should hand those out on request from components
			// Or could have a graphics subsystem which holds all of these objects?
			/*
				template <typename T, typename... Args>
				const T* GraphicsFactory::Construct( Args&&... args )
				{ 
				}

				// Should scene own its own camera? How will this work, exactly?

				// Should scene actually be an asset in and of itself? So the graphics subsystem will actually hold an asset handle to a scene as opposed to a scene itself? Not sure that I really like that approach though, since it
					seems fairly abstracted away...

				Result Scene::SerializeData( ByteBuffer* buffer )
				{ 
					// Camera data
					ObjectArchiver::Serialize( &mCamera, buffer );

					// Get the entity manager
					const EntityManager* em = Engine::GetInstance()->GetSubsystemCatalog()->Get< EntityManager >();	

					buffer->Write< u32 >( em->GetActiveEntities().size() );

					// Write all entity data to buffer
					for ( auto& e : em->GetActiveEntities() )
					{
						// Only write out top-level entities
						if ( !e.HasParent() )
						{
							EntityArchiver::Serialize( e, buffer );
						} 
					}
				}

				Result DeserializeData( ByteBuffer* buffer )
				{ 
					// Deserialize camera data
					ObjectArchiver::Deserialize( buffer, &mCam );

					// Deserialize entity data 
					u32 entityCount = buffer->Read< u32 >();

					for ( u32 i = 0; i < entityCount; ++i )
					{
						EntityArchiver::Deserialize( buffer );
					} 
				}
			*/ 
	};
}

#endif