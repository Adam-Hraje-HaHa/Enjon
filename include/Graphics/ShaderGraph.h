// @file ShaderGraph.h
// Copyright 2016-2017 John Jackson. All Rights Reserved.

#pragma once
#ifndef ENJON_SHADER_GRAPH_H
#define ENJON_SHADER_GRAPH_H 

#include "Graphics/Shader.h" 

#include <unordered_map>
#include <set>
#include <vector>

namespace Enjon
{
	// These do not necessary need to evaluate to executable code, 
	// they just need to evaluate to glsl

	enum class ShaderGraphNodeVariableType
	{
		LocalVariable,
		UniformVariable
	};

	enum class ShaderPrimitiveType
	{
		Float,
		Vec3,
		Vec2,
		Vec4,
		Texture2D,
		Mat4,
		Integer
	};

	enum class ShaderOutputType
	{
		Float,
		Vec3,
		Vec2,
		Vec4,
		Mat4,
		Integer
	};

	enum class ShaderNodeState
	{
		Ready,
		Evaulated
	};

	class ShaderGraphNode
	{
		public:
			class Connection
			{
				public:
					Connection( ) {}

					Connection( const ShaderGraphNode* owner, u32 inputPortID = 0, u32 outputPortID = 0 ) 
						: mOwner( owner ), mInputPortID( inputPortID ), mOutputPortID( outputPortID )
					{
					}
					~Connection( ) {}

					u32 mInputPortID = 0;
					u32 mOutputPortID = 0;
					const ShaderGraphNode* mOwner = nullptr;

				private:
			};

		public:
		/**
		* @brief Constructor
		*/
		ShaderGraphNode( const Enjon::String& id )
			: mID( id )
		{
		}

		/**
		* @brief Destructor
		*/
		~ShaderGraphNode( ) {}

		/**
		* @brief Destructor
		*/
		void Execute( )
		{
		}

		template <typename T>
		T* Cast( )
		{
			return static_cast<T*>( this );
		}


		/**
		* @brief
		*/
		Enjon::String Evaluate( )
		{
			if ( mState == ShaderNodeState::Ready )
			{
				mState = ShaderNodeState::Evaulated;
				return EvaluateToGLSL( );
			}

			return "";
		}

		virtual ShaderOutputType EvaluateOutputType( u32 portID = 0 )
		{ 
			return mOutputType;
		}

		// Must be evaluated first
		virtual Enjon::String EvaluateAtPort( u32 portID )
		{
			return mID;
		}

		/**
		* @brief
		*/
		virtual Enjon::String EvaluateToGLSL( )
		{
			return "";
		}

		/**
		* @brief
		*/
		virtual Enjon::String GetDeclaration( )
		{
			return "";
		}

		/**
		* @brief
		*/
		Enjon::String GetID( ) const { return mID; }

		/**
		* @brief
		*/
		virtual Enjon::String GetQualifiedID( ) { return mID; }

		/**
		* @brief
		*/
		const std::vector< Connection >* GetInputs( ) const { return &mInputs; }

		/**
		* @brief
		*/
		ShaderPrimitiveType GetPrimitiveType( ) const { return mPrimitiveType; }

		/**
		* @brief
		*/
		ShaderNodeState GetState( ) const { return mState; }

		/**
		* @brief
		*/
		virtual Enjon::String GetDefinition( )
		{
			return "";
		}

		/**
		* @brief
		*/
		void AddInput( Connection connection )
		{ 
			mInputs.push_back( connection );
		}

		/**
		* @brief
		*/
		void RemoveInput( ShaderGraphNode* node );

	protected:
		ShaderGraphNodeVariableType mVariableType = ShaderGraphNodeVariableType::LocalVariable;
		ShaderPrimitiveType mPrimitiveType;
		ShaderOutputType mOutputType;
		ShaderNodeState mState = ShaderNodeState::Ready;
		u32 mMaxNumberInputs = 1;
		u32 mMaxNumberOutputs = 1;
		std::vector< Connection > mInputs;
		Enjon::String mID;
	};

	enum class ShaderGraphMainNodeInputType
	{
		Albedo,
		Normal,
		Metallic,
		Roughness,
		Emissive,
		AmbientOcculsion,
		VertexPositionOffset,
		Count
	};

	// Main entry point for shader graph
	class ShaderGraphMainNode : public ShaderGraphNode
	{
		public:

			/**
			* @brief
			*/
			ShaderGraphMainNode( )
				: ShaderGraphNode( "MainNode" )
			{
				// Max number of inputs allowed for main node 
				mMaxNumberInputs = u32( ShaderGraphMainNodeInputType::Count );

				// Resize inputs to fit max
				//mInputs.resize( mMaxNumberInputs );
			}

			/**
			* @brief
			*/
			~ShaderGraphMainNode( )
			{
			} 

			/*
			* @brief
			*/
			Enjon::String EvaluateToGLSL( ) override
			{ 
				// Evaluate basic for now
				Enjon::String albedo = EvaluateAtPort( (u32)ShaderGraphMainNodeInputType::Albedo ) + "\n";

				return albedo;
			}

		private:
			/**
			* @brief
			*/
			Enjon::String EvaluateAtPort( u32 idx )
			{
				switch ( ShaderGraphMainNodeInputType( idx ) )
				{
					case ShaderGraphMainNodeInputType::Albedo: 
					{
						// Evaluate connection at this port
						for ( auto& c : mInputs )
						{
							// Find input to this
							if ( c.mInputPortID == (u32) ShaderGraphMainNodeInputType::Albedo )
							{
								// String to return
								Enjon::String albedoOutput = "";

								// Get input node and evaluate
								ShaderGraphNode* input = const_cast< ShaderGraphNode* >( c.mOwner ); 
								albedoOutput += input->Evaluate( ) + "\n";

								// Final output string
								Enjon::String finalOutput = "AlbedoColorOut = ";
								Enjon::String qid = input->GetQualifiedID( );

								// Based on output type, need to format final output
								switch ( input->EvaluateOutputType( c.mOutputPortID ) )
								{
									case ShaderOutputType::Float:
									{
										finalOutput += "vec4( " + qid + ", " + qid + ", " + qid + ", 1.0 );\n";
									} break;

									case ShaderOutputType::Vec2:
									{
										finalOutput += "vec4( " + qid + ", " + "1.0, " + "1.0 );\n";
									} break;
									
									case ShaderOutputType::Vec3:
									{
										finalOutput += "vec4( " + qid + + ", 1.0 );\n";
									} break;
									
									case ShaderOutputType::Vec4:
									{
										finalOutput += qid + ";\n";
									} break; 
								}

								// Final output
								albedoOutput += finalOutput;

								// return
								return albedoOutput;
							}
						}

					} break;

					default:
					{
					} break;
				}

				return "Nothing valid found!"; 
			}
			

	};

	class ShaderGraph
	{
	public:
		/**
		* @brief Constructor
		*/
		ShaderGraph( );

		/**
		* @brief Destructor
		*/
		~ShaderGraph( );

		/**
		* @brief
		*/
		ShaderGraphNode* AddNode( ShaderGraphNode* node );

		/**
		* @brief Removes node from graph and frees memory of node
		*/
		void RemoveNode( ShaderGraphNode* node );

		/**
		* @brief
		*/
		void Reset( );

		/**
		* @brief
		*/
		void Compile( );

		/**
		* @brief
		*/
		bool VariableExists( const Enjon::String& var );

		/**
		* @brief
		*/
		void RegisterVariable( const Enjon::String& var );
		
		/**
		* @brief
		*/
		void RegisterDeclaration( const Enjon::String& var );

		/**
		* @brief
		*/
		void RegisterRequiredDefinitions( const ShaderGraphNode* node );

		/**
		* @brief
		*/
		void UnregisterVariable( const Enjon::String& var );
		
		/**
		* @briefConnectionType
		*/
		Enjon::String GetShaderOutput( ) const 
		{
			return mShaderCodeOutput;
		} 
		
		/**
		* @brief
		*/
		void Connect( const ShaderGraphNode::Connection& connection );

	private:
		/**
		* @brief
		*/
		void RecurseThroughChildrenAndBuildVariables( const ShaderGraphNode* node ); 

		/*
		* @Brief
		*/
		void BeginFragmentMain( );

		/*
		* @Brief
		*/
		void EndFragmentMain( );

	private:
		ShaderGraphMainNode mMainNode;
		std::set< const ShaderGraphNode* > mNodes;
		std::set< Enjon::String > mRegisteredVariables;
		std::set< Enjon::String > mDeclarations;
		std::set< const ShaderGraphNode* > mDefinesOnStart;
		Enjon::String mShaderCodeOutput;
	};

	class ShaderGraphFunctionNode : public ShaderGraphNode
	{
	public:
		/**
		* @brief Constructor
		*/
		ShaderGraphFunctionNode( const Enjon::String& id )
			: ShaderGraphNode( id )
		{
			mPrimitiveType = ShaderPrimitiveType::Float;
			mOutputType = ShaderOutputType::Float;
		}

		/**
		* @brief Destructor
		*/
		~ShaderGraphFunctionNode( ) {}

	protected:
	};

	class BinaryFunctionNode : public ShaderGraphFunctionNode
	{
	public:

		BinaryFunctionNode( const Enjon::String& id )
			: ShaderGraphFunctionNode( id )
		{
			mMaxNumberInputs = 2;
			mMaxNumberOutputs = 1;

			//mInputs.resize( mMaxNumberInputs );
		}

		~BinaryFunctionNode( )
		{

		}

	protected:
	};

	class UnaryFunctionNode : public ShaderGraphFunctionNode
	{
	public:

		UnaryFunctionNode( const Enjon::String& id )
			: ShaderGraphFunctionNode( id )
		{
			mMaxNumberInputs = 1;
		}

		~UnaryFunctionNode( )
		{

		}

	protected:
	};

	template < typename T >
	class ShaderPrimitiveNode : public ShaderGraphNode
	{
	public:
		ShaderPrimitiveNode( const Enjon::String& id )
			: ShaderGraphNode( id )
		{
			mVariableType = ShaderGraphNodeVariableType::LocalVariable;
		}

		~ShaderPrimitiveNode( )
		{

		}

	protected:
		T mData;
	};

	class ShaderFloatNode : public ShaderPrimitiveNode< f32 >
	{
	public:
		ShaderFloatNode( const Enjon::String& id )
			: ShaderPrimitiveNode( id )
		{
			mPrimitiveType = ShaderPrimitiveType::Float;
			mOutputType = ShaderOutputType::Float;
			mMaxNumberInputs = 1;
		}

		ShaderFloatNode( const Enjon::String& id, f32 value )
			: ShaderPrimitiveNode( id )
		{
			mData = value;
			mMaxNumberInputs = 1;
		}

		~ShaderFloatNode( ) { }

		Enjon::String EvaluateToGLSL( ) override
		{
			switch ( mVariableType )
			{
			case ShaderGraphNodeVariableType::LocalVariable:
			{
				return GetQualifiedID( ) + " = " + std::to_string( mData ) + ";";
			}
			break;

			case ShaderGraphNodeVariableType::UniformVariable:
			{
				return "";
			}
			break;

			default:
			{
				return "";
			}
			break;
			}
		}

		virtual Enjon::String EvaluateAtPort( u32 portID ) override
		{
			// Only one port, so return qualified id
			return GetQualifiedID( );
		}

		virtual Enjon::String GetDeclaration( ) override
		{
			switch ( mVariableType )
			{ 
				case ShaderGraphNodeVariableType::LocalVariable:
				{
					return "float " + mID + ";";
				}
				break;

				case ShaderGraphNodeVariableType::UniformVariable:
				{
					return "uniform float " + mID + ";";
				}
				break;

				default:
				{
					return "float " + mID + ";";
				}
				break;
			}
		}

	protected:
	};

	class ShaderVec4Node : public ShaderPrimitiveNode< Vec4 >
	{
	public:
		ShaderVec4Node( const Enjon::String& id )
			: ShaderPrimitiveNode( id )
		{
			mPrimitiveType = ShaderPrimitiveType::Vec4;
			mOutputType = ShaderOutputType::Vec4;
			mMaxNumberInputs = 4;
		}

		ShaderVec4Node( const Enjon::String& id, const Vec4& vec )
			: ShaderPrimitiveNode( id )
		{
			mData = vec;
			mPrimitiveType = ShaderPrimitiveType::Vec4;
			mOutputType = ShaderOutputType::Vec4;
			mMaxNumberInputs = 4;
		}

		~ShaderVec4Node( ) { }

		Enjon::String EvaluateToGLSL( ) override
		{
			Enjon::String x = std::to_string( mData.x );
			Enjon::String y = std::to_string( mData.y );
			Enjon::String z = std::to_string( mData.z );
			Enjon::String w = std::to_string( mData.w );

			Enjon::String def = "vec4 " + GetQualifiedID( ) + " = vec4(" + x + ", " + y + ", " + z + ", " + w + ");";

			switch ( mVariableType )
			{
			case ShaderGraphNodeVariableType::LocalVariable:
			{
				return def;
			}
			break;

			case ShaderGraphNodeVariableType::UniformVariable:
			{
				return "";
			}
			break;

			default:
			{
				return "";
			}
			break;
			}
		} 

		virtual Enjon::String GetDeclaration( ) override
		{
			return ( "vec4 " + mID + ";" );
		}
	};

	class ShaderMultiplyNode : public BinaryFunctionNode
	{
	public:
		ShaderMultiplyNode( const Enjon::String& id )
			: BinaryFunctionNode( id )
		{
		}

		~ShaderMultiplyNode( )
		{
		}

		virtual ShaderOutputType EvaluateOutputType( u32 portID = 0 ) override
		{
			if ( mInputs.size( ) < 2 )
			{
				return ShaderOutputType::Float;
			}

			Connection a_conn = mInputs.at( 0 );
			Connection b_conn = mInputs.at( 1 );

			// Look at inputs to determine output type of this node
			ShaderGraphNode* a = const_cast<ShaderGraphNode*>( mInputs.at( 0 ).mOwner );
			ShaderGraphNode* b = const_cast<ShaderGraphNode*>( mInputs.at( 1 ).mOwner );

			// Evaluate output type
			ShaderOutputType aType = a->EvaluateOutputType( a_conn.mOutputPortID );
			ShaderOutputType bType = b->EvaluateOutputType( b_conn.mOutputPortID ); 

			switch ( aType )
			{
				case ShaderOutputType::Float:
				{
					switch ( bType )
					{
						case ShaderOutputType::Float: mOutputType = ShaderOutputType::Float; break;
						case ShaderOutputType::Vec2: mOutputType = ShaderOutputType::Vec2; break;
						case ShaderOutputType::Vec3: mOutputType = ShaderOutputType::Vec3; break;
						case ShaderOutputType::Vec4: mOutputType = ShaderOutputType::Vec4; break;
					}

				} break;

				case ShaderOutputType::Vec2:
				{
					if ( bType == ShaderOutputType::Vec2 || bType == ShaderOutputType::Float )
					{
						mOutputType = ShaderOutputType::Vec2;
					}
					else
					{
						// Throw error here!
					}

				} break;

				case ShaderOutputType::Vec3:
				{
					if ( bType == ShaderOutputType::Vec3 || bType == ShaderOutputType::Float )
					{
						mOutputType = ShaderOutputType::Vec3;
					}
					else
					{
						// Throw error
					}
				} break;

				case ShaderOutputType::Vec4:
				{
					if ( bType == ShaderOutputType::Vec4 || bType == ShaderOutputType::Float )
					{
						mOutputType = ShaderOutputType::Vec4; 
					}
					else
					{
						// Throw error
					}
				} break; 
			}

			return mOutputType;
		}

		virtual Enjon::String EvaluateToGLSL( ) override
		{
			Enjon::String finalEvaluation = "";

			// Evaluate inputs
			for ( auto& c : mInputs )
			{
				ShaderGraphNode* owner = const_cast<ShaderGraphNode*>( c.mOwner );
				finalEvaluation += owner->Evaluate( ) + "\n"; 
			}

			// Get connections
			Connection a_conn = mInputs.at( 0 );
			Connection b_conn = mInputs.at( 1 );

			// Get shader graph nodes
			ShaderGraphNode* a = const_cast<ShaderGraphNode*>( a_conn.mOwner );
			ShaderGraphNode* b = const_cast<ShaderGraphNode*>( b_conn.mOwner );

			// Evaluate final line of code
			finalEvaluation += GetQualifiedID( ) + " = " + a->EvaluateAtPort( a_conn.mOutputPortID ) + " * " + b->EvaluateAtPort( b_conn.mOutputPortID ) + ";";

			// Return
			return finalEvaluation;
		}

		virtual Enjon::String EvaluateAtPort( u32 portID ) override
		{
			// Multiply node only has one output, so just return its qualified id
			return GetQualifiedID( );
		}

		virtual Enjon::String GetDeclaration( ) override
		{
			switch ( EvaluateOutputType( ) )
			{
				case ShaderOutputType::Float: return "float " + mID + ";"; break;
				case ShaderOutputType::Vec2: return "vec2 " + mID + ";"; break;
				case ShaderOutputType::Vec3: return "vec3 " + mID + ";"; break;
				case ShaderOutputType::Vec4: return "vec4 " + mID + ";"; break;
				default: return "";
			}
		}

	protected:

	};

	class ShaderTexture2DNode : public ShaderGraphNode
	{
	public:
		enum class TexturePortType
		{
			RGB,
			R,
			G,
			B,
			A
		};
		/**
		* @brief Constructor
		*/
		ShaderTexture2DNode( const Enjon::String& id )
			: ShaderGraphNode( id )
		{
			mVariableType = ShaderGraphNodeVariableType::UniformVariable;
			mPrimitiveType = ShaderPrimitiveType::Texture2D;
			mMaxNumberInputs = 1;
			mMaxNumberOutputs = 5; 
		}

		/**
		* @brief Destructor
		*/
		~ShaderTexture2DNode( )
		{
		}

		/**
		* @brief
		*/
		Enjon::String EvaluateToGLSL( ) override
		{
			// Already defined previously
			return "";
		}

		virtual ShaderOutputType EvaluateOutputType( u32 portID )
		{
			TexturePortType type = TexturePortType( portID );

			switch ( type )
			{
				case TexturePortType::RGB: 
				{
					return ShaderOutputType::Vec3;
				} break;

				case TexturePortType::R:
				case TexturePortType::G:
				case TexturePortType::B:
				case TexturePortType::A:
				{
					return ShaderOutputType::Float; 
				} break;
				default:
				{
					return ShaderOutputType::Float; 
				} break;
			}
		}

		virtual Enjon::String EvaluateAtPort( u32 portID ) override
		{
			switch ( TexturePortType( portID ) )
			{
				case TexturePortType::RGB: 
				{
					return GetQualifiedID( ) + ".rgb";
				} break;

				case TexturePortType::R: 
				{
					return GetQualifiedID( ) + ".r";
				} break;
				
				case TexturePortType::G: 
				{
					return GetQualifiedID( ) + ".g"; 
				} break;
				
				case TexturePortType::B: 
				{ 
					return GetQualifiedID( ) + ".b"; 
				} break;
				
				case TexturePortType::A: 
				{
					return GetQualifiedID( ) + ".a"; 
				} break;

				default:
				{
					return GetQualifiedID( );
				} break;
			}
		}

		/**
		* @brief
		*/
		virtual Enjon::String GetDeclaration( ) override
		{
			return ( "uniform sampler2D " + mID + ";" );
		}

		/**
		* @brief
		*/
		virtual Enjon::String GetDefinition( ) override
		{
			return ( "vec4 " + mID + "_sample = texture(" + mID + ", texCoords);" );
		}

		/**
		* @brief
		*/
		virtual Enjon::String GetQualifiedID( )
		{
			return mID + "_sample";
		}

	private:
	};
}

#endif








