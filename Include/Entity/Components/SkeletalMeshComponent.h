#pragma once
#ifndef ENJON_SKELETAL_MESH_COMPONENT_H
#define ENJON_SKELETAL_MESH_COMPONENT_H

#include "Entity/Component.h"
#include "Graphics/Renderable.h"
#include "Graphics/Material.h"
#include "Graphics/SkeletalMeshRenderable.h"
#include "System/Types.h"

namespace Enjon
{
	class AnimationSubsystem;
	class SkeletalAnimationComponent;

	ENJON_CLASS( )
	class SkeletalMeshComponent : public Component
	{
		friend AnimationSubsystem;
		friend SkeletalAnimationComponent;

		ENJON_COMPONENT( SkeletalMeshComponent )

		public:

			/* 
			* @brief
			*/
			virtual void ExplicitConstructor() override; 

			/*
			* @brief
			*/
			virtual void ExplicitDestructor() override;

			/*
			* @brief
			*/
			virtual void PostConstruction( ) override;

			/*
			* @brief
			*/
			virtual void Update( ) override; 

			/*
			* @brief
			*/
			virtual Result OnEditorUI( ) override;
 
			/* 
			* @brief Get position of transform 
			*/
			Vec3 GetPosition() const;

			/* 
			* @brief Get scale of transform 
			*/
			Vec3 GetScale() const;

			/* 
			* @brief Get orientation of transform 
			*/
			Quaternion GetRotation() const;

			/* 
			* @brief Get material of renderable 
			*/
			AssetHandle< Material > GetMaterial( const u32& idx = 0 ) const;

			/* 
			* @brief Get mesh of renderable 
			*/
			AssetHandle< Mesh > GetMesh() const;

			/* 
			* @brief Get scene of renderable 
			*/
			GraphicsScene* GetGraphicsScene() const;

			/* 
			* @brief Get world transform 
			*/
			Transform GetTransform() const;
			
			/* 
			* @brief Get renderable 
			*/
			SkeletalMeshRenderable* GetRenderable();

			/* 
			* @brief Sets world transform 
			*/
			void SetTransform(const Transform& transform); 

			/* 
			* @brief Set position of transform 
			*/
			void SetPosition(const Vec3& position);

			/* 
			* @brief Set scale of transform 
			*/
			void SetScale(const Vec3& scale);

			/* 
			* @brief Set scale of transform 
			*/
			void SetScale(const f32& scale);

			/* 
			* @brief Set orientation of transform 
			*/
			void SetRotation(const Quaternion& rotation); 

			/* 
			* @brief Set material of renderable 
			*/
			void SetMaterial( const AssetHandle< Material >& material, const u32& idx = 0 );

			/* 
			* @brief Set mesh of renderable 
			*/
			void SetMesh(const AssetHandle<SkeletalMesh>& mesh);

			/* 
			* @brief Set scene of renderable 
			*/
			void SetGraphicsScene(GraphicsScene* scene); 

			/*
			* @brief
			*/
			virtual Result SerializeData( ByteBuffer* buffer ) const override;

			/*
			* @brief
			*/
			virtual Result DeserializeData( ByteBuffer* buffer ) override; 

		protected:

			/**
			* @brief
			*/
			void UpdateAndCalculateTransforms( ); 

		protected: 
			
			ENJON_PROPERTY( )
			SkeletalMeshRenderable mRenderable;
	};
}

#endif
