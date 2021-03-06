#pragma once
#ifndef ENJON_MATERIAL_H
#define ENJON_MATERIAL_H 

#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Graphics/ShaderGraph.h"
#include "System/Types.h"
#include "Asset/Asset.h"

#include <unordered_map>

namespace Enjon { 

	class GLSLProgram;
	class MaterialAssetLoader;

	enum class TextureSlotType
	{
		Albedo,
		Normal,
		Emissive,
		Metallic,
		Roughness,
		AO,
		Count
	};

	ENJON_CLASS( )
	class Material : public Asset
	{
		ENJON_CLASS_BODY( Material )

		friend Shader;
		friend ShaderGraph;
		friend MaterialAssetLoader;

		public: 
			
			/*
			* @brief
			*/
			Material( const ShaderGraph* shaderGraph );
			
			/*
			* @brief
			*/
			Material( const AssetHandle< ShaderGraph >& shaderGraph ); 

			/*
			* @brief
			*/
			virtual void ExplicitDestructor( ) override;

			/*
			* @brief
			*/
			bool TwoSided( ) const { return mTwoSided; }

			/*
			* @brief
			*/
			void TwoSided( bool enable ) { mTwoSided = enable; }

			/*
			* @brief
			*/
			bool HasOverride( const String& uniformName ) const;

			/*
			* @brief
			*/
			const ShaderUniform* GetOverride( const String& uniformName ) const;

			/*
			* @brief
			*/
			void Bind( const Shader* shader ) const;

			/*
			* @brief
			*/
			void SetUniform( const String& name, const AssetHandle< Texture >& value ); 

			/*
			* @brief
			*/
			void SetUniform( const String& name, const Vec2& value ); 

			/*
			* @brief
			*/
			void SetUniform( const String& name, const Vec3& value ); 

			/*
			* @brief
			*/
			void SetUniform( const String& name, const Vec4& value );

			/*
			* @brief
			*/
			void SetUniform( const String& name, const Mat4x4& value );

			/*
			* @brief
			*/
			void SetUniform( const String& name, const f32& value ); 

			/*
			* @brief
			*/
			void SetShaderGraph( const AssetHandle< ShaderGraph >& graph ) const; 

			/*
			* @brief
			*/
			AssetHandle< ShaderGraph > GetShaderGraph( ) const;

			/*
			* @brief
			*/
			virtual Result OnEditorUI( ) override;

		protected:

			/*
			* @brief
			*/
			void AddOverride( ShaderUniform* uniform );

			/*
			* @brief
			*/
			void ClearAllOverrides( );

		protected: 
			ENJON_PROPERTY( Editable, HideInEditor )
			AssetHandle< ShaderGraph > mShaderGraph; 
			
			ENJON_PROPERTY( HideInEditor )
			HashMap< String, ShaderUniform* > mUniformOverrides;

			ENJON_PROPERTY( Editable ) 
			bool mTwoSided = false; 
	}; 
}


#endif