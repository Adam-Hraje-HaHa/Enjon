// @file Shader.h
// Copyright 2016-2017 John Jackson. All Rights Reserved.

#pragma once
#ifndef ENJON_SHADER_H
#define ENJON_SHADER_H 

#include "Asset/Asset.h"
#include "System/Types.h"
#include "Math/Vec4.h"
#include "Math/Mat4.h"
#include "Defines.h"
#include "Graphics/ShaderGraph.h"
#include "Graphics/Texture.h"
#include "Graphics/Color.h"

#include <vector>

namespace Enjon
{ 
	class Shader
	{
		public:
			/**
			* @brief Constructor
			*/
			Shader( );

			/**
			* @brief Constructor
			*/
			Shader( const AssetHandle< ShaderGraph >& graph, ShaderPassType passType, const String& vertexShaderCode, const String& fragmentShaderCode );
			
			/**
			* @brief Constructor
			*/
			Shader( const AssetHandle<ShaderGraph>& graph, ShaderPassType passType );

			/**
			* @brief Destructor
			*/
			~Shader( ); 

			/*
			* @brief
			*/
			Enjon::Result Compile( bool passedCode = false, const String& vertexShaderCode = "", const String& fragmentShaderCode = "" ); 
			
			/*
			* @brief
			*/
			Enjon::Result Recompile( ); 

			/*
			* @brief
			*/
			const Enjon::ShaderGraph* GetGraph( );

			/*
			* @brief
			*/
			void Use( );
			
			/*
			* @brief
			*/
			void Unuse( );

			/*
			* @brief
			*/
			u32 GetProgramID( ) const;

			/*
			* @brief
			*/
			s32 GetUniformLocation( const Enjon::String& uniformName );
			void SetUniform(const std::string& name, const s32& val);
			void SetUniform(const std::string& name, f32* val, s32 count);
			void SetUniform(const std::string& name, s32* val, s32 count);
			void SetUniform(const std::string& name, const f64& val);
			void SetUniform(const std::string& name, const f32& val);
			void SetUniform(const std::string& name, const Vec2& vector);
			void SetUniform(const std::string& name, const Vec3& vector);
			void SetUniform(const std::string& name, const Vec4& vector);
			void SetUniform(const std::string& name, const Mat4x4& matrix); 
			void SetUniform( const std::string& name, const ColorRGBA32& color );
			void SetUniformArrayElement( const std::string& name, const u32& index, const Mat4x4& mat );

			void BindTexture(const std::string& name, const u32& TextureID, const u32 Index);

		private:

			/*
			* @brief
			*/
			Enjon::Result CompileShader( const Enjon::String& shaderCode, u32 shaderID );
			
			/*
			* @brief
			*/
			Enjon::Result LinkProgram( );
			
			/*
			* @brief
			*/
			Enjon::Result DestroyProgram( );

		private: 
			u32 mProgramID			= 0; 
			u32 mVertexShaderID		= 0;
			u32 mFragmentShaderID	= 0;
			std::unordered_map< Enjon::String, u32 > mUniformMap;
			ShaderPassType mPassType;
			AssetHandle<ShaderGraph> mGraph;
	}; 
	
	ENJON_CLASS( Abstract )
	class ShaderUniform : public Enjon::Object
	{ 
		ENJON_CLASS_BODY( ShaderUniform )

		public: 

			/*
			* @brief
			*/
			virtual void Bind( const Shader* shader ) const = 0;
			
			template < typename T >
			T* Cast( )
			{
				static_assert( std::is_base_of<ShaderUniform, T>::value, "ShaderUniform::Cast -  T must inherit from ShaderUniform." );	
				return static_cast< T* >( this );
			}

			/*
			* @brief
			*/
			const Enjon::String& GetName( ) const { return mName; }

			/*
			* @brief
			*/
			u32 GetLocation( ) const { return mLocation; }
			
			/*
			* @brief
			*/
			UniformType GetType( ) const { return mType; }

		protected: 
			ENJON_PROPERTY( HideInEditor )
			UniformType mType;

			ENJON_PROPERTY( HideInEditor )
			u32 mLocation = 0;

			ENJON_PROPERTY( HideInEditor )
			Enjon::String mName;
	}; 

	ENJON_CLASS( ) 
	class UniformTexture : public ShaderUniform
	{
		ENJON_CLASS_BODY( UniformTexture )

		public:	

			/*
			* @brief
			*/
			UniformTexture( const Enjon::String& name, const Enjon::AssetHandle< Enjon::Texture >& texture, u32 location ); 

			/*
			* @brief
			*/
			void CopyFields( const UniformTexture* other );
			
			/*
			* @brief
			*/
			virtual void Bind( const Shader* shader ) const override;

			/*
			* @brief
			*/
			void SetTexture( const Enjon::AssetHandle< Enjon::Texture >& texture )
			{
				mTexture = texture;
			}
			
			/*
			* @brief
			*/
			Enjon::AssetHandle< Enjon::Texture > GetTexture( ) const
			{
				return mTexture;
			}
 
		private:

			ENJON_PROPERTY( )
			Enjon::AssetHandle< Enjon::Texture > mTexture;
	};

	ENJON_CLASS( )
	class UniformVec2 : public ShaderUniform
	{
		ENJON_CLASS_BODY( UniformVec2 )

		public: 

			/*
			* @brief
			*/
			UniformVec2( const Enjon::String& name, const Vec2& value, u32 location = 0 )
			{
				mName = name;
				mLocation = location;
				mValue = value;
				mType = UniformType::Vec2; 
			} 

			/*
			* @brief
			*/
			virtual void Bind( const Shader* shader ) const override
			{
				const_cast< Enjon::Shader* >( shader )->SetUniform( mName, mValue );
			}

			void SetValue( const Vec2& value )
			{
				mValue = value;
			}

			const Vec2& GetValue( ) const 
			{ 
				return mValue; 
			}

			/*
			* @brief
			*/
			void CopyFields( const UniformVec2* other );

		private:

			ENJON_PROPERTY( )
			Vec2 mValue;
	};

	ENJON_CLASS( )
	class UniformVec3 : public ShaderUniform
	{
		ENJON_CLASS_BODY( UniformVec3 )

	public: 

		/*
		* @brief
		*/
		UniformVec3( const Enjon::String& name, const Vec3& value, u32 location = 0 )
		{
			mName = name;
			mLocation = location;
			mValue = value;
			mType = UniformType::Vec3;
		} 

		/*
		* @brief
		*/
		virtual void Bind( const Shader* shader ) const override
		{
			const_cast< Enjon::Shader* >( shader )->SetUniform( mName, mValue );
		}

		void SetValue( const Vec3& value )
		{
			mValue = value;
		}

		const Vec3& GetValue( ) const
		{
			return mValue;
		}

		/*
		* @brief
		*/
		void CopyFields( const UniformVec3* other );

	private:

		ENJON_PROPERTY( )
		Vec3 mValue;
	};

	ENJON_CLASS( )
	class UniformVec4 : public ShaderUniform
	{
		ENJON_CLASS_BODY( UniformVec4 )

	public:
 
		/*
		* @brief
		*/
		UniformVec4( const Enjon::String& name, const Vec4& value, u32 location = 0 )
		{
			mName = name;
			mLocation = location;
			mValue = value;
			mType = UniformType::Vec4;
		} 

		/*
		* @brief
		*/
		virtual void Bind( const Shader* shader ) const override
		{
			const_cast< Enjon::Shader* >( shader )->SetUniform( mName, mValue );
		}

		void SetValue( const Vec4& value )
		{
			mValue = value;
		}

		const Vec4& GetValue( ) const
		{
			return mValue;
		}

		/*
		* @brief
		*/
		void CopyFields( const UniformVec4* other );

	private:

		ENJON_PROPERTY( )
		Vec4 mValue;
	};

	ENJON_CLASS( )
	class UniformFloat : public ShaderUniform
	{
		ENJON_CLASS_BODY( UniformFloat )

		public: 

			/*
			* @brief
			*/
			UniformFloat( const Enjon::String& name, const f32& value, u32 location = 0 )
			{
				mName = name;
				mLocation = location;
				mValue = value;
				mType = UniformType::Float;
			} 

			/*
			* @brief
			*/
			virtual void Bind( const Shader* shader ) const override
			{
				const_cast< Enjon::Shader* >( shader )->SetUniform( mName, mValue );
			}

			void SetValue( const f32& value )
			{
				mValue = value;
			}

			const f32& GetValue( ) const
			{
				return mValue;
			}

			/*
			* @brief
			*/
			void CopyFields( const UniformFloat* other );

		private: 

			ENJON_PROPERTY( )
			f32 mValue;
	};
}

#endif










