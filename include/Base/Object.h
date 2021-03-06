// @file Object.h
// Copyright 2016-2017 John Jackson. All Rights Reserved.
#pragma once
#ifndef ENJON_OBJECT_H
#define ENJON_OBJECT_H 

// #include "MetaClass.h"
#include "System/Types.h"
#include "Base/ObjectDefines.h"
#include "Engine.h"					// I think including this is causing duplicate symbols
#include "Defines.h"

#include <limits>
#include <assert.h>
#include <functional>
#include <iterator>

// Forward Declarations
namespace Enjon
{ 
	class EntityArchiver;
	class AssetArchiver;
	class ObjectArchiver;
	class ByteBuffer;
	class Engine;
}

// TODO(): Clean up this file!  

namespace Enjon
{
	enum class MetaPropertyType
	{
		Object,
		Bool,
		ColorRGBA32,
		F32,
		F64,
		U8,
		U16,
		U32,
		U64,
		S8,
		S16,
		S32,
		S64,
		String,
		Array,
		HashMap,
		Vec2,
		Vec3,
		Vec4,
		Mat4x4,
		Quat,
		Enum,
		UUID,
		Transform,
		AssetHandle,
		EntityHandle,
		iVec2,
		iVec3,
		iVec4
	};

#define _META_PROP_TO_STR( str )\
	{ return str; } break;

	static inline const char* MetaPropertyTypeToStr( const MetaPropertyType& type )
	{
		switch ( type )
		{
			case MetaPropertyType::Object:			{ return "Object"; } break;
			case MetaPropertyType::Bool:			{ return "Bool"; } break;
			case MetaPropertyType::ColorRGBA32:		{ return "ColorRGBA32"; } break;
			case MetaPropertyType::F32:				{ return "f32"; } break;
			case MetaPropertyType::F64:				{ return "f64;"; } break;
			case MetaPropertyType::U8:				{ return "u8"; } break; 
			case MetaPropertyType::U16:				{ return "u16"; } break;
			case MetaPropertyType::U32:				{ return "u32"; } break;
			case MetaPropertyType::U64:				{ return "u64"; } break;
			case MetaPropertyType::S8:				{ return "s8"; } break;
			case MetaPropertyType::S16:				{ return "s16"; } break;
			case MetaPropertyType::S32:				{ return "s32"; } break;
			case MetaPropertyType::S64:				{ return "s64"; } break;
			case MetaPropertyType::String:			{ return "String"; } break;
			case MetaPropertyType::Array:			{ return "Array"; } break;
			case MetaPropertyType::HashMap:			{ return "HashMap"; } break;
			case MetaPropertyType::Vec2:			{ return "Vec2"; } break;
			case MetaPropertyType::Vec3:			{ return "Vec3"; } break;
			case MetaPropertyType::Vec4:			{ return "Vec4"; } break;
			case MetaPropertyType::Mat4x4:			{ return "Mat4x4"; } break;
			case MetaPropertyType::Quat:			{ return "Quaternion"; } break;
			case MetaPropertyType::Enum:			{ return "Enum"; } break;
			case MetaPropertyType::UUID:			{ return "UUID"; } break;
			case MetaPropertyType::Transform:		{ return "Transform"; } break;
			case MetaPropertyType::AssetHandle:		{ return "AssetHandle"; } break;
			case MetaPropertyType::EntityHandle:	{ return "EntityHandle"; } break;
			case MetaPropertyType::iVec2:			{ return "iVec2"; } break;
			case MetaPropertyType::iVec3:			{ return "iVec3"; } break;
			case MetaPropertyType::iVec4:			{ return "iVec4"; } break;
			default:								{ return "invalid"; } break;
		}
	}

	enum MetaPropertyFlags : u32
	{
		Default			 = 0x00,
		IsPointer		 = 0x01,
		NonSerializeable = 0x02,
		ReadOnly		 = 0x04,
		HideInEditor	 = 0x08
	};

	enum class MergeType
	{
		AcceptSource,
		AcceptDestination,
		AcceptMerge
	};

	using VoidCallback = std::function< void( void ) >;

	inline MetaPropertyFlags operator|( MetaPropertyFlags a, MetaPropertyFlags b )
	{
		return static_cast< MetaPropertyFlags >( static_cast< u32 >( a ) | static_cast< u32 >( b ) );
	}

	inline MetaPropertyFlags operator&( MetaPropertyFlags a, MetaPropertyFlags b )
	{
		return static_cast< MetaPropertyFlags >( static_cast< u32 >( a ) & static_cast< u32 >( b ) );
	}

	inline MetaPropertyFlags operator^( MetaPropertyFlags a, MetaPropertyFlags b )
	{
		return static_cast< MetaPropertyFlags >( static_cast< u32 >( a ) ^ static_cast< u32 >( b ) );
	}

	inline void operator^=( MetaPropertyFlags& a, MetaPropertyFlags b )
	{
		a = a ^ b;
	}

	inline void operator|=( MetaPropertyFlags& a, MetaPropertyFlags b )
	{
		a = a | b;
	}

	inline void operator&=( MetaPropertyFlags& a, MetaPropertyFlags b )
	{
		a = a & b;
	} 

	// Forward Declarations
	class MetaFunction;
	class MetaProperty;
	class MetaClass;
	class Object;

	union MetaClassPropertyTraitFlags
	{
		u32 IsEditable : 1;
		u32 IsVisible : 1; 
		u32 IsPointer : 1;
		u32 IsSerializable : 1;
	};
	
	// Don't really like this, but, ya know... wha ya gon' do?
	struct MetaPropertyTraits
	{
		MetaPropertyTraits( u32 isEditable = false, f32 uiMin = 0.0f, f32 uiMax = 1.0f, u32 isPointer = false, bool isVisible = true, MetaPropertyFlags flags = MetaPropertyFlags::Default )
			: mIsEditable( isEditable ), mUIMin( uiMin ), mUIMax( uiMax ), mIsPointer( isPointer ), mIsVisible( isVisible ), mFlags( flags )
		{ 
		}

		~MetaPropertyTraits( )
		{ 
		}

		/*
		* @brief
		*/
		f32 GetUIMax( ) const;
		
		/*
		* @brief
		*/
		f32 GetUIMin( ) const; 

		/*
		* @brief
		*/
		bool UseSlider( ) const;

		/*
		* @brief
		*/
		bool IsPointer( ) const;

		/*
		* @brief
		*/
		bool IsVisible( ) const;

		/*
		* @brief
		*/
		bool IsSerializeable( ) const;

		/*
		* @brief
		*/
		bool HasFlags( const MetaPropertyFlags& flags ) const;

		bool mIsEditable = false;
		bool mIsPointer = false;
		bool mIsVisible = true;
		f32 mUIMin = 0.0f;
		f32 mUIMax = 1.0f;
		MetaPropertyFlags mFlags = MetaPropertyFlags::Default;
	};

	class MetaProperty
	{
		friend MetaClass;
		friend Object;

		public:
			/*
			* @brief
			*/
			MetaProperty( ) = default; 

			/*
			* @brief
			*/
			MetaProperty( MetaPropertyType type, const std::string& name, u32 offset, u32 propIndex, MetaPropertyTraits traits, const Vector<MetaFunction*>& accessors = Vector<MetaFunction*>(), const Vector<MetaFunction*>& mutators = Vector<MetaFunction*>() )
				: mType( type ), mName( name ), mOffset( offset ), mIndex( propIndex ), mTraits( traits ), mAccessorCallbacks( accessors ), mMutatorCallbacks( mutators )
			{
			}

			/*
			* @brief
			*/
			virtual ~MetaProperty( ) = default;
		
			/*
			* @brief
			*/
			bool HasFlags( const MetaPropertyFlags& flags ) const;
			
			/*
			* @brief
			*/
			bool IsEditable( ) const;

			/*
			* @brief
			*/
			bool IsSerializeable( ) const;

			/*
			* @brief
			*/
			std::string GetName( ) const;

			/*
			* @brief
			*/
			MetaPropertyType GetType( ) const;

			/*
			* @brief
			*/
			const char* GetTypeStr( ) const;

			/*
			* @brief
			*/
			MetaPropertyTraits GetTraits( ) const; 
 
			/*
			* @brief
			*/
			template <typename T>
			const T* Cast( ) const
			{
				static_assert( std::is_base_of< MetaProperty, T >::value, "Must inherit from MetaProperty." );
				return static_cast< const T* >( this );
			}

			/*
			* @brief
			*/
			u32 GetOffset( ) const
			{
				return mOffset;
			}

			/*
			* @brief
			*/
			u32 GetIndex( ) const
			{
				return mIndex;
			}

			/*
			* @brief
			*/
			bool HasOverride( const Object* obj ) const;

			/*
			* @brief
			*/
			const Object* GetSourceObject( const Object* key );

			/*
			* @brief
			*/
			void AddOverride( const Object* obj, const Object* source );

			/*
			* @brief
			*/
			void RemoveOverride( const Object* obj );

			/*
			* @brief
			*/
			void AddOnValueChangedCallback( const VoidCallback& cb )
			{
				mOnValueChangedCallbacks.push_back( cb );
			}

		protected:
			MetaPropertyType mType;
			String mName;
			u32 mOffset;
			u32 mIndex;
			MetaPropertyTraits mTraits;
			Vector<MetaFunction*> mAccessorCallbacks;
			Vector<MetaFunction*> mMutatorCallbacks;
			Vector<VoidCallback> mOnValueChangedCallbacks;
			HashSet<const Object*> mPropertyOverrides;
			HashMap<const Object*, const Object*> mPropertyOverrideSourceMap;
	};

	class MetaPropertyPointerBase : public MetaProperty 
	{
		friend MetaClass; 
		friend Object;

		public:
			MetaPropertyPointerBase( ) = default;
			~MetaPropertyPointerBase( ) = default; 

			virtual const Object* GetValueAsObject( const Object* obj ) const = 0;
	};

	template <typename BaseType, typename ValueType>
	class MetaPropertyPointer : public MetaPropertyPointerBase
	{
		public:
			MetaPropertyPointer() = default;
			MetaPropertyPointer( MetaPropertyType type, const std::string& name, u32 offset, u32 propIndex, MetaPropertyTraits traits, ValueType* BaseType::*memberPointer )
			{
				mType = type;
				mName = name;
				mOffset = offset;
				mIndex = propIndex;
				mTraits = traits;
				mMemberPointer = memberPointer;
			}
			~MetaPropertyPointer() = default;

			// Need to be able to access the type like this: 
			const ValueType* GetValue( const Object* obj )
			{
				return reinterpret_cast< const BaseType* >( obj )->*mMemberPointer;
			}

			virtual const Object* GetValueAsObject( const Object* obj ) const override
			{
				return reinterpret_cast< const BaseType* >( obj )->*mMemberPointer;
			}

		private:
			ValueType* BaseType::*mMemberPointer;
	};

	class MetaPropertyEnum;
	class MetaPropertyEnumElement
	{
		friend MetaClass;
		friend MetaPropertyEnum;
		friend Object;
		public:
			MetaPropertyEnumElement( ) = default;
			~MetaPropertyEnumElement( ) = default;

			MetaPropertyEnumElement( const String& identifier, s32 value )
				: mIdentifier( identifier ), mValue( value )
			{ 
			}

			/*
			* @brief
			*/
			String Identifier( ) const
			{
				return mIdentifier;
			}

			/*
			* @brief
			*/
			s32 Value( ) const
			{
				return mValue;
			}
 
		protected:
			String mIdentifier;
			s32 mValue;
	};

	class MetaPropertyEnum : public MetaProperty 
	{
		public:

			/*
			* @brief
			*/
			MetaPropertyEnum( ) = default;

			/*
			* @brief
			*/
			MetaPropertyEnum( MetaPropertyType type, const String& name, u32 offset, u32 propIndex, MetaPropertyTraits traits, const Vector< MetaPropertyEnumElement >& elements, const String& enumName )
				: mElements( elements ), mEnumTypeName( enumName )
			{
				mType = type;
				mName = name;
				mOffset = offset;
				mIndex = propIndex;
				mTraits = traits; 
			} 

			/*
			* @brief
			*/
			~MetaPropertyEnum( ) = default;

			/*
			* @brief
			*/
			const Vector< MetaPropertyEnumElement >& GetElements( ) const
			{
				return mElements;
			}

			String GetEnumName( ) const
			{
				return mEnumTypeName;
			}

		private:
			Vector< MetaPropertyEnumElement > mElements; 
			String mEnumTypeName;
	};

	class MetaPropertyTemplateBase : public MetaProperty
	{
		public:
			virtual const MetaClass* GetClassOfTemplatedArgument( ) const = 0;
	}; 

	class MetaPropertyArrayBase;
	struct MetaArrayPropertyProxy
	{
		MetaArrayPropertyProxy( ) = default;
		MetaArrayPropertyProxy( const MetaPropertyArrayBase* base, const MetaProperty* prop )
			: mBase( base ), mArrayPropertyTypeBase( prop )
		{ 
		} 

		const MetaPropertyArrayBase* mBase = nullptr;
		const MetaProperty* mArrayPropertyTypeBase = nullptr;
	};

	enum class ArraySizeType
	{
		Fixed,
		Dynamic
	};

	class MetaPropertyArrayBase : public MetaProperty
	{
		public: 
			virtual usize GetSize( const Object* object ) const = 0;
			virtual usize GetCapacity( const Object* object ) const = 0;
			virtual ArraySizeType GetArraySizeType( ) const = 0;
			virtual MetaPropertyType GetArrayType( ) const = 0; 
			virtual MetaArrayPropertyProxy GetProxy( ) const = 0;
			virtual void Resize( const Object* object, const usize& arraySize ) const = 0;
			virtual usize GetSizeInBytes( const Object* object ) const = 0;

			/*
			* @brief
			*/
			virtual ~MetaPropertyArrayBase( )
			{
				delete mArrayProperty;
				mArrayProperty = nullptr;
			}

		protected:
			MetaProperty* mArrayProperty = nullptr;
	}; 

	template <typename T>
	class MetaPropertyArray : public MetaPropertyArrayBase
	{
		public:

			/*
			* @brief
			*/
			MetaPropertyArray( MetaPropertyType type, const std::string& name, u32 offset, u32 propIndex, MetaPropertyTraits traits, ArraySizeType arraySizeType, MetaPropertyType arrayType, MetaProperty* arrayProp, usize arraySize = 0 )
				: mSize( arraySize ), mArraySizeType( arraySizeType ), mArrayType( arrayType )
			{ 
				// Default meta property member variables
				mType = type;
				mName = name;
				mOffset = offset;
				mIndex = propIndex;
				mTraits = traits; 
				mArrayProperty = arrayProp;
			}

			/*
			* @brief
			*/
			virtual ~MetaPropertyArray( )
			{
				delete mArrayProperty;
				mArrayProperty = nullptr;
			}

			/*
			* @brief
			*/
			virtual usize GetSize( const Object* object ) const override
			{
				switch ( mArraySizeType )
				{
					case ArraySizeType::Fixed:
					{
						return mSize; 
					} break;

					case ArraySizeType::Dynamic:
					{ 
						return ( ( Vector<T>* )( usize( object ) + mOffset ) )->size( ); 
					} break;
				}

				// Shouldn't get here
				return 0;
			}

			/*
			* @brief
			*/
			virtual usize GetSizeInBytes( const Object* object ) const override
			{
				// Get number of elements in array * size of templated argument type
				return GetSize( object ) * sizeof( T );
			}

			/*
			* @brief 
			*/
			virtual usize GetCapacity( const Object* object ) const override
			{
				switch ( mArraySizeType )
				{
					case ArraySizeType::Fixed:
					{
						return mSize;
					} break;

					case ArraySizeType::Dynamic:
					{
						return ( ( Vector< T >* )( usize( object ) + mOffset ) )->capacity( );
					} break;
				}

				// Shouldn't get here
				return 0;
			}

			/*
			* @brief 
			* @note Can ONLY reserve/resize space for dynamic arrays
			*/
			void Resize( const Object* object, const usize& arraySize ) const override 
			{
				if ( mArraySizeType == ArraySizeType::Dynamic )
				{
					( ( Vector<T>* )( usize( object ) + mOffset ) )->resize( (usize)arraySize ); 
				}
			} 

			/*
			* @brief
			*/
			void GetValueAt( const Object* object, usize index, T* out ) const
			{
				assert( index < GetCapacity( object ) );

				T* rawArr = GetRaw( object );
				*out = rawArr[index];
			} 

			T GetValueAs( const Object* object, usize index ) const
			{
				assert( index < GetCapacity( object ) ); 

				//T* rawArr = GetRaw( object );
				//return rawArr[ index ]; 

				switch (mArraySizeType)
				{
					case ArraySizeType::Dynamic:
					{
						// return ( T* )( &( ( ( Vector< T >* )( (usize)object + mOffset ) )->front() ) );
						// return ( T* )( ( ( ( Vector< T >* )( (usize)object + mOffset ) ) ) );
						return ((Vector< T >*)((usize)object + mOffset))->at( index );
					} break;

					default:
					case ArraySizeType::Fixed:
					{
						// return ( reinterpret_cast< T* >( (usize)object + mOffset ) );
						return (reinterpret_cast<T*>((usize)object + mOffset))[index];
					} break;
				}
			}

			/*
			* @brief
			*/
			void SetValueAt( const Object* object, usize index, const T& value ) const
			{
				assert( index < GetCapacity( object ) );

				// Grab raw array
				// T* rawArr = GetRaw( object );
				// rawArr[index] = value;

				switch ( mArraySizeType )
				{
					case ArraySizeType::Dynamic:
					{ 
						// return ( T* )( &( ( ( Vector< T >* )( (usize)object + mOffset ) )->front() ) );
						// return ( T* )( ( ( ( Vector< T >* )( (usize)object + mOffset ) ) ) );
						( (Vector< T >*)( (usize)object + mOffset ) )->at(index) = value;
					} break;

					default:
					case ArraySizeType::Fixed:
					{
						// return ( reinterpret_cast< T* >( (usize)object + mOffset ) );
						(reinterpret_cast< T* >( (usize)object + mOffset) )[index] = value;
					} break;
				}
			}
 
			/*
			* @brief
			*/
			virtual ArraySizeType GetArraySizeType( ) const override
			{
				return mArraySizeType;
			} 

			/*
			* @brief
			*/
			virtual MetaPropertyType GetArrayType( ) const override
			{
				return mArrayType;
			}

			/*
			* @brief
			*/
			virtual MetaArrayPropertyProxy GetProxy( ) const override
			{
				return MetaArrayPropertyProxy( this, mArrayProperty ); 
			} 

		private: 

			/*
			* @brief
			*/
			T* GetRaw( const Object* object ) const
			{ 
				switch ( mArraySizeType )
				{
					case ArraySizeType::Dynamic:
					{ 
						// return ( T* )( &( ( ( Vector< T >* )( (usize)object + mOffset ) )->front() ) );
						return ( T* )( ( ( ( Vector< T >* )( (usize)object + mOffset ) ) ) );
					} break;

					default:
					case ArraySizeType::Fixed:
					{
						return ( reinterpret_cast< T* >( (usize)object + mOffset ) );
					} break;
				}
			}

		private:
			T mClass; 
			usize mSize;
			ArraySizeType mArraySizeType;
			MetaPropertyType mArrayType;
	};

	class MetaPropertyHashMapBase : public MetaProperty
	{
		public: 
			virtual usize GetSize( const Object* object ) const = 0; 

			MetaPropertyType GetKeyType( ) const
			{
				return mKeyType;
			}

			MetaPropertyType GetValueType( ) const 
			{
				return mValueType;
			} 

			virtual usize GetSizeInBytes( const Object* object ) const = 0;

			const MetaClass* GetValueMetaClass() const;

			~MetaPropertyHashMapBase( )
			{ 
				delete mKeyProperty;
				delete mValueProperty;
				mKeyProperty = nullptr;
				mValueProperty = nullptr;
			} 

		protected:
			MetaProperty* mKeyProperty = nullptr;
			MetaProperty* mValueProperty = nullptr;
			MetaPropertyType mKeyType;
			MetaPropertyType mValueType;
			String mValueMetaClassName = "Invalid";
	}; 

	template <typename K, typename V>
	class MetaPropertyHashMap : public MetaPropertyHashMapBase
	{
		public:

			/*
			* @brief
			*/
			MetaPropertyHashMap( MetaPropertyType type, const std::string& name, u32 offset, u32 propIndex, MetaPropertyTraits traits, MetaPropertyType keyType, MetaPropertyType valType, MetaProperty* keyProp, MetaProperty* valProp, const std::string& valueMetaClsName )
			{ 
				// Default meta property member variables
				mType = type;
				mName = name;
				mOffset = offset;
				mIndex = propIndex;
				mTraits = traits; 
				mKeyProperty = keyProp;
				mValueProperty = valProp;
				mKeyType = keyType;
				mValueType = valType;
				mValueMetaClassName = valueMetaClsName;
			}

			/*
			* @brief
			*/
			~MetaPropertyHashMap( ) = default; 

			/*
			* @brief
			*/
			usize GetSize( const Object* object ) const override
			{
				return ( ( HashMap<K, V>* )( usize( object ) + mOffset ) )->size( ); 
			}

			/**
			* @brief
			*/
			usize GetSizeInBytes( const Object* object ) const override 
			{
				return ( usize )( GetSize( object ) * ( sizeof( K ) + sizeof( V ) ) );
			}

			/*
				Have to be able to iterate over the map - have to be able to list the keys as well as the values that are being iterated over 
			*/

			/*
			* @brief
			*/
			void GetValueAt( const Object* object, const K& key, V* out ) const
			{ 
				HashMap<K, V>* rawMap = GetRaw( object );
				*out = rawMap[key];
			} 

			/*
			* @brief
			*/
			V GetValueAs( const Object* object, const K& key ) const
			{ 
				HashMap<K, V>* rawMap = GetRaw( object );
				return rawMap[key];
			} 

			/*
			* @brief
			*/
			V GetValueAs( const Object* object, const typename HashMap< K, V >::iterator& iter ) const
			{ 
				return iter->second;
			} 

			/*
			* @brief
			*/
			typename HashMap< K, V >::iterator Begin( const Object* object ) const
			{
				HashMap< K, V >* rawMap = GetRaw( object );
				return rawMap->begin( );
			}

			bool KeyExists( const Object* object, const K& key ) const
			{ 
				HashMap< K, V >* rawMap = GetRaw( object );
				return ( rawMap->find( key ) != rawMap->end() );
			}

			/*
			* @brief
			*/
			typename HashMap< K, V >::iterator End( const Object* object ) const
			{
				HashMap< K, V >* rawMap = GetRaw( object );
				return rawMap->end( );
			} 

			/*
			* @brief
			*/
			void SetValueAt( const Object* object, const typename HashMap< K, V >::iterator& iter, const V& value ) const
			{ 
				iter->second = value;
			}

			/*
			* @brief
			*/
			void SetValueAt( const Object* object, const K& key, const V& value ) const
			{ 
				HashMap< K, V >* rawMap = GetRaw( object );
				rawMap->insert(std::make_pair(key,value));
			} 

			/*
			* @brief
			*/
			MetaPropertyType GetKeyType( ) const
			{
				return mKeyType;
			}

			/*
			* @brief
			*/
			MetaPropertyType GetValueType( ) const
			{
				return mValueType;
			} 

		private: 

			/*
			* @brief
			*/
			HashMap<K, V>* GetRaw( const Object* object ) const
			{ 
				return ( HashMap<K, V>* )( usize( object ) + mOffset );
			} 
	};

	typedef Vector< MetaProperty* > PropertyTable;
	typedef HashMap< Enjon::String, MetaFunction* > FunctionTable;
	typedef std::function< Object*( void ) > ConstructFunction;

	enum class MetaClassType
	{
		Object,
		Application,
		Component
	};

	class MetaClass
	{
		friend Object;

		public:

			/*
			* @brief
			*/
			MetaClass( ) = default;

			/*
			* @brief
			*/
			~MetaClass()
			{}
			// {
			// 	// Delete all functions
			// 	for ( auto& f : mFunctions )
			// 	{
			// 		delete f.second;
			// 		f.second = nullptr;
			// 	}


			// 	for ( auto& p : mProperties )
			// 	{
			// 		delete p;
			// 		p = nullptr;
			// 	}

			// 	// Clear properties and functions
			// 	mProperties.clear( );
			// 	mFunctions.clear( );

			// 	// Any further destruction that needs to occur
			// 	this->Destroy( ); 
			// } 

			MetaClassType GetMetaClassType( ) const 
			{
				return mMetaClassType;
			}

			u32 PropertyCount( ) const
			{
				return ( u32 )mProperties.size( );
			}

			u32 FunctionCount( ) const
			{
				return ( u32 )mFunctions.size( );
			} 

			bool PropertyExists( const u32& index ) const
			{
				return index < mPropertyCount;
			}

			bool FunctionExists( const Enjon::String& name ) const
			{
				return ( mFunctions.find( name ) != mFunctions.end( ) ); 
			}

			const MetaFunction* GetFunction( const Enjon::String& name ) const
			{
				if ( FunctionExists( name ) )
				{
					return const_cast<MetaClass*>(this)->mFunctions[ name ];
				}

				return nullptr;
			}

			usize GetSerializablePropertyCount( ) const
			{
				usize count = 0;
				for ( auto& p : mProperties )
				{
					if ( !p->HasFlags( MetaPropertyFlags::NonSerializeable ) )
					{
						count++;
					}
				}

				return count;
			}

			usize GetPropertyCount( ) const
			{
				return mPropertyCount;
			} 

			s32 FindPropertyIndexByName( const String& propertyName ) const
			{
				for ( usize i = 0; i < mPropertyCount; ++i )
				{
					if ( mProperties.at( i )->mName.compare( propertyName ) == 0 )
					{
						return i;
					}
				}

				return -1;
			}

			const MetaProperty* GetPropertyByName( const String& propertyName ) const
			{
				s32 index = FindPropertyIndexByName( propertyName );
				if ( index >= 0 && index < (s32)mPropertyCount )
				{
					return mProperties[ index ];
				}

				return nullptr;
			}

			const MetaProperty* GetProperty( const u32& index ) const
			{
				if ( PropertyExists( index ) )
				{
					return mProperties.at( index );
				}

				return nullptr;
			} 

			bool HasProperty( const MetaProperty* prop ) const
			{
				return ( ( prop->mIndex < mPropertyCount ) && ( mProperties.at( prop->mIndex ) == prop ) ); 
			}

			template < typename T >
			void GetValue( const Object* object, const MetaProperty* prop, T* out ) const
			{
				if ( HasProperty( prop ) )
				{
					T* val = reinterpret_cast< T* >( usize( object ) + prop->mOffset );
					*out = *val; 
				}
			} 

			template < typename T > 
			const T* GetValueAs( const Object* object, const MetaProperty* prop ) const
			{
				if ( HasProperty( prop ) )
				{
					if ( prop->GetTraits( ).IsPointer( ) )
					{
						return nullptr;
					}
					else
					{
						const T* val = reinterpret_cast< const T* >( usize( object ) +  prop->mOffset );
						return val; 
					}
				}

				return nullptr;
			} 

			template < typename T >
			void SetValue( const Object* object, const MetaProperty* prop, const T& value ) const;
			// {
			// // 	// if ( HasProperty( prop ) )
			// // 	// {
			// // 	// 	T* dest = reinterpret_cast< T* >( usize( object ) + prop->mOffset );
			// // 	// 	*dest = value;
					
			// // 	// 	// Call mutator callbacks
			// // 	// 	for ( auto& mutator : prop->mMutatorCallbacks )
			// // 	// 	{
			// // 	// 		if ( mutator )
			// // 	// 		{
			// // 	// 			mutator->Invoke<void>( object, value ); 
			// // 	// 		}
			// // 	// 	} 

			// // 	// 	for ( auto& m : prop->mOnValueChangedCallbacks )
			// // 	// 	{
			// // 	// 		m( );
			// // 	// 	}
			// // 	// }
			// } 

			template < typename T >
			void SetValue( Object* object, const MetaProperty* prop, const T& value );
			// {
			// // 	// if ( HasProperty( prop ) )
			// // 	// {
			// // 	// 	T* dest = reinterpret_cast< T* >( usize( object ) + prop->mOffset );
			// // 	// 	*dest = value;

			// // 	// 	// Call mutator callbacks
			// // 	// 	for ( auto& mutator : prop->mMutatorCallbacks )
			// // 	// 	{
			// // 	// 		if ( mutator )
			// // 	// 		{
			// // 	// 			mutator->Invoke<void>( object, value ); 
			// // 	// 		}
			// // 	// 	} 

			// // 	// 	for ( auto& m : prop->mOnValueChangedCallbacks )
			// // 	// 	{
			// // 	// 		m( );
			// // 	// 	}
			// // 	// }
			// } 

			void SetValue( Object* objA, const MetaProperty* propA, const Object* objB, const MetaProperty* propB );

			/** 
			* @brief
			*/
			bool InstanceOf( const MetaClass* cls ) const;

			/** 
			* @brief
			*/
			template < typename T >
			bool InstanceOf( ) const;
			// {
			// 	return false;
			// 	// MetaClassRegistry* mr = const_cast< MetaClassRegistry* >( Engine::GetInstance( )->GetMetaClassRegistry( ) );
			// 	// const MetaClass* cls = mr->Get< T >( );
			// 	// if ( !cls )
			// 	// {
			// 	// 	cls = mr->RegisterMetaClass< T >( );
			// 	// } 

			// 	// return ( cls && cls == this ); 
			// } 

			// Method for getting type id from MetaClass instead of Object
			virtual u32 GetTypeId( ) const 
			{
				return mTypeId;
			}

			String GetName( ) const
			{
				return mName;
			}

			const PropertyTable& GetProperties( ) const { return mProperties; }

			const FunctionTable& GetFunctions( ) const { return mFunctions; }

			/*
			* @brief
			*/
			Object* Construct( ) const
			{
				if ( mConstructor )
				{
					return mConstructor( );
				}
				return nullptr;
			}

		protected:

			virtual void Destroy( )
			{ 
			}

		protected:
			PropertyTable mProperties;
			FunctionTable mFunctions;
			u32 mPropertyCount;
			u32 mFunctionCount;
			u32 mTypeId;
			MetaClassType mMetaClassType = MetaClassType::Object;
			String mName;

			// Not sure if this is the best way to do this, but whatever...
			ConstructFunction mConstructor = nullptr;
	};

	class MetaClassComponent : public MetaClass
	{
		friend Object;

		public: 

			/*
			* @brief
			*/
			MetaClassComponent( ) = default;

			/*
			* @brief
			*/
			const Vector< String >& GetRequiredComponentList( ) const
			{
				return mRequiredComponentList;
			}


		protected:

			virtual void Destroy( ) override
			{
				mRequiredComponentList.clear( );
			}

		protected: 

			Vector< String > mRequiredComponentList;
	};

	class MetaClassRegistry
	{
		public:
			MetaClassRegistry( )
			{
			}

			/*
			* @brief
			*/
			~MetaClassRegistry( )
			{
				// Delete all meta classes from registry
				for ( auto& c : mRegistry )
				{
					delete c.second;
				}

				mRegistry.clear( );
			}

			template <typename T>
			MetaClass* RegisterMetaClass( );
			// {
			// 	// Must inherit from object to be able to registered
			// 	static_assert( std::is_base_of<Object, T>::value, "MetaClass::RegisterMetaClass() - T must inherit from Object." );

			// 	// Get id of object
			// 	u32 id = GetTypeId< T >( );

			// 	// If available, then return
			// 	if ( HasMetaClass< T >( ) )
			// 	{
			// 		return mRegistry[ id ];
			// 	}

			// 	// Otherwise construct it and return
			// 	MetaClass* cls = Object::ConstructMetaClass< T >( ); 

			// 	mRegistry[ id ] = cls;
			// 	mRegistryByClassName[cls->GetName()] = cls;

			// 	// Further registration of metaclass
			// 	RegisterMetaClassLate( cls );

			// 	return cls;
			// 	// return nullptr;
			// }
 
			/**
			* @brief
			*/
			void RegisterMetaClassLate( const MetaClass* cls );

			/**
			* @brief
			*/
			template <typename T>
			void UnregisterMetaClass( );
			// {
			// 	// Must inherit from object to be able to registered
			// 	// static_assert( std::is_base_of<Object, T>::value, "MetaClass::RegisterMetaClass() - T must inherit from Object." ); 
			// 	// UnregisterMetaClass( Object::GetClass< T >( ) ); 
			// }

			/**
			* @brief
			*/
			void UnregisterMetaClass( const MetaClass* cls );

			/**
			* @brief
			*/
			template <typename T>
			u32 GetTypeId( ) const; 

			/**
			* @brief
			*/
			template < typename T >
			bool HasMetaClass( )
			{
				return ( mRegistry.find( GetTypeId< T >( ) ) != mRegistry.end( ) );
			} 

			/**
			* @brief
			*/
			bool HasMetaClass( const String& className )
			{
				return ( mRegistryByClassName.find( className ) != mRegistryByClassName.end( ) );
			}

			/**
			* @brief
			*/
			bool HasMetaClass( const u32& typeId )
			{
				return ( mRegistry.find( typeId ) != mRegistry.end( ) );
			}

			/**
			* @brief
			*/
			template < typename T >
			const MetaClass* Get( )
			{
				//return HasMetaClass< T >( ) ? mRegistry[ Object::GetTypeId< T >( ) ] : nullptr;
				return HasMetaClass< T >( ) ? mRegistry[ GetTypeId< T >( ) ] : nullptr;
			}

			/**
			* @brief
			*/
			const MetaClass* GetClassByName( const String& className )
			{
				if ( HasMetaClass( className ) )
				{
					return mRegistryByClassName[className];
				}

				return nullptr;
			}

			const HashMap< u32, MetaClass* >& GetRegistry( ) const
			{
				return mRegistry;
			}

			/**
			* @brief
			*/
			const MetaClass* GetClassById( const u32& typeId );

		private:
			HashMap< u32, MetaClass* > mRegistry; 
			HashMap< String, MetaClass* > mRegistryByClassName;
	};

	// Base model for all Enjon classes that participate in reflection
	class Object
	{
		friend ObjectArchiver;
		friend MetaClassRegistry;
		friend AssetArchiver;
		friend Engine;

		public:

			/**
			*@brief
			*/
			Object( ) 
			{
			}

			/**
			*@brief
			*/
			virtual ~Object( )
			{ 
			}

			/**
			*@brief
			*/
			virtual const MetaClass* Class( ) const
			{
				return nullptr;
			}

			/**
			*@brief
			*/
			template <typename T>
			inline static void AssertIsObject( )
			{
				static_assert( std::is_base_of<Object, T>::value, "T must inherit from Object." ); 
			} 

			/**
			*@brief
			*/
			template <typename T>
			const T* Cast( ) const
			{
				Object::AssertIsObject< T >( ); 
				return static_cast< T* >( const_cast< Object* > ( this ) );
			}

			/**
			*@brief
			*/
			template <typename T>
			T* ConstCast( ) const
			{
				Object::AssertIsObject< T >( );
				return static_cast< T* >( const_cast< Object* >( this ) );
			} 

			/**
			*@brief
			*/
			template <typename T>
			static void BindMetaClass( )
			{
				MetaClassRegistry* registry = const_cast< MetaClassRegistry* >( Engine::GetInstance( )->GetMetaClassRegistry( ) );
				registry->RegisterMetaClass< T >( );
			}

			/**
			*@brief
			*/
			template <typename T>
			static void UnbindMetaClass( )
			{
				MetaClassRegistry* registry = const_cast< MetaClassRegistry* >( Engine::GetInstance( )->GetMetaClassRegistry( ) ); 
				registry->UnregisterMetaClass( Object::GetClass< T >( ) );
			}

			/**
			*@brief
			*/
			static void UnbindMetaClass( const MetaClass* cls )
			{
				MetaClassRegistry* registry = const_cast< MetaClassRegistry* >( Engine::GetInstance( )->GetMetaClassRegistry( ) ); 
				registry->UnregisterMetaClass( cls ); 
			}

			/**
			*@brief
			*/
			u32 GetTypeId( )
			{
				if ( Class( ) )
				{
					return Class( )->GetTypeId( );
				}

				return std::numeric_limits<u32>::max( );
			}

			/**
			*@brief
			*/
			bool TypeValid( ) const; 

			template <typename T>
			static const MetaClass* GetClass( )
			{ 
				MetaClassRegistry* mr = const_cast< MetaClassRegistry* >( Engine::GetInstance()->GetMetaClassRegistry( ) );
				const MetaClass* cls = mr->Get< T >( );
				if ( !cls )
				{
					cls = mr->RegisterMetaClass< T >( );
				}
				return cls;
				// return nullptr;
			}


			template < typename T >
			static void AssertIsType( const Object* object )
			{
				assert( Object::GetClass< T >( ) == object->Class() );
			} 

			/**
			*@brief Could return null!
			*/
			static const MetaClass* GetClass( const u32& typeId );

			/**
			*@brief Could return null!
			*/
			static const MetaClass* GetClass( const String& className )
			{
				MetaClassRegistry* mr = const_cast<MetaClassRegistry*> ( Engine::GetInstance( )->GetMetaClassRegistry( ) );
				return mr->GetClassByName( className );
				// return nullptr;
			} 

		protected:

			// Default to u32 max; If id set to this, then is not set by engine and invalid
			u32 mTypeId = std::numeric_limits<u32>::max();

		protected:
			template <typename T>
			static MetaClass* ConstructMetaClass( );

			static void BindMetaClasses( ); 

			template <typename T>
			static void RegisterMetaClass( )
			{
				MetaClassRegistry* mr = const_cast< MetaClassRegistry* >( Engine::GetInstance()->GetMetaClassRegistry( ) );
				mr->RegisterMetaClass< T >( );
			}

		public:
			/*
			* @brief
			*/
			virtual Result SerializeData( ByteBuffer* buffer ) const;

			/*
			* @brief
			*/
			virtual Result DeserializeData( ByteBuffer* buffer ); 

			/*
			* @brief
			*/
			virtual Result DeserializeLateInit( );
 
			/*
			* @brief
			*/
			virtual Result OnEditorUI( );

			/*
			* @brief
			*/
			virtual Result OnViewportDebugDraw();
 
			/*
			* @brief
			*/
			virtual Result MergeWith( Object* other, MergeType mergeType ); 

			/*
			* @brief
			*/
			virtual Result RecordPropertyOverrides( Object* source ); 

			/*
			* @brief
			*/
			virtual Result ClearAllPropertyOverrides( );

			/*
			* @brief
			*/
			virtual Result HasPropertyOverrides( bool& result ) const;

		private:

			/**
			*@brief
			*/
			static u32 GetUniqueTypeId( ) noexcept
			{
				static u32 lastId { 1 };
				return lastId++;
			} 

		protected:
			virtual const Enjon::MetaClass* GetClassInternal( ) const
			{
				return nullptr;
			}

			virtual void ExplicitConstructor( )
			{ 
				// Do nothing by default
			}

			virtual void ExplicitDestructor( )
			{ 
				// Do nothing by default
			} 
	};

	template <typename T>
	class MetaPropertyAssetHandle : public MetaPropertyTemplateBase
	{ 
		public: 
			MetaPropertyAssetHandle( MetaPropertyType type, const std::string& name, u32 offset, u32 propIndex, MetaPropertyTraits traits )
			{
				Object::AssertIsObject<T>( );

				mType = type;
				mName = name;
				mOffset = offset;
				mIndex = propIndex;
				mTraits = traits;
			}

			~MetaPropertyAssetHandle( ) = default;

			virtual const MetaClass* GetClassOfTemplatedArgument() const override
			{
				return Object::GetClass<T>();	
			}

			T mClass;
	};

#define META_FUNCTION_IMPL( )\
	friend MetaClass;\
	friend Object;\
	virtual void Base( ) override\
	{\
	} 

	class MetaFunction
	{
		friend MetaClass;
		friend Object;

		public:
			MetaFunction( )
			{
			}

			virtual ~MetaFunction( )
			{ 
			}

			template < typename RetVal, typename T, typename... Args >
			RetVal Invoke( T* obj, Args&&... args );
			// {
			// 	RetVal val;
			// 	return val;
			// // 	Object::AssertIsObject<T>( );
			// // 	return static_cast< MetaFunctionImpl< T, RetVal, Args&&... >* >( this )->mFunc( obj, std::forward<Args>( args )... );
			// }

			const Enjon::String& GetName( ) const
			{
				return mName;
			}

		protected:
			virtual void Base( ) = 0;

		protected:
			Enjon::String mName = "";
	};

	template < typename T, typename RetVal, typename... Args >
	struct MetaFunctionImpl : public MetaFunction
	{ 
		META_FUNCTION_IMPL( )

		MetaFunctionImpl( std::function< RetVal( T*, Args&&... ) > function, const Enjon::String& name )
			: mFunc( function )
		{ 
			mName = name;
		}

		~MetaFunctionImpl( )
		{ 
		}

		std::function< RetVal( T*, Args&&... ) > mFunc;
	};

	template < typename T >
	struct MetaFunctionImpl< T, void, void > : public MetaFunction
	{ 
		META_FUNCTION_IMPL( )
		
		MetaFunctionImpl( std::function< void( T* ) > function, const Enjon::String& name )
			: mFunc( function )
		{ 
			mName = name;
		}

		~MetaFunctionImpl( )
		{ 
		}
		std::function< void( T* ) > mFunc;
	};

	template < typename T, typename RetVal >
	struct MetaFunctionImpl< T, RetVal, void > : public MetaFunction
	{
		META_FUNCTION_IMPL( )

		MetaFunctionImpl( std::function< RetVal( T* ) > function, const Enjon::String& name )
			: mFunc( function )
		{ 
			mName = name;
		}

		std::function< RetVal( T* ) > mFunc;
	};

	template < typename T >
	MetaClass* MetaClassRegistry::RegisterMetaClass( )
	{
		// Must inherit from object to be able to registered
		static_assert( std::is_base_of<Object, T>::value, "MetaClass::RegisterMetaClass() - T must inherit from Object." );

		// Get id of object
		u32 id = GetTypeId< T >( );

		// If available, then return
		if ( HasMetaClass< T >( ) )
		{
			return mRegistry[ id ];
		}

		// Otherwise construct it and return
		MetaClass* cls = Object::ConstructMetaClass< T >( ); 

		mRegistry[ id ] = cls;
		mRegistryByClassName[cls->GetName()] = cls;

		// Further registration of metaclass
		RegisterMetaClassLate( cls );

		return cls;
	}

	template < typename T >
	bool MetaClass::InstanceOf( ) const
	{
		MetaClassRegistry* mr = const_cast< MetaClassRegistry* >( Engine::GetInstance( )->GetMetaClassRegistry( ) );
		const MetaClass* cls = mr->Get< T >( );
		if ( !cls )
		{
			cls = mr->RegisterMetaClass< T >( );
		} 

		return ( cls && cls == this ); 
	} 

	template < typename RetVal, typename T, typename... Args >
	RetVal MetaFunction::Invoke( T* obj, Args&&... args )
	{
		// RetVal val;
		// return val;
		Object::AssertIsObject<T>( );
		return static_cast< MetaFunctionImpl< T, RetVal, Args&&... >* >( this )->mFunc( obj, std::forward<Args>( args )... );
	}

	// template < typename T >
	// static void MetaClass::AssertIsType( const Object* object )
	// {
	// 	assert( Object::GetClass< T >( ) == object->Class() );
	// } 

	template <typename T>
	void MetaClassRegistry::UnregisterMetaClass( )
	{
		// Must inherit from object to be able to registered
		static_assert( std::is_base_of<Object, T>::value, "MetaClass::RegisterMetaClass() - T must inherit from Object." ); 
		UnregisterMetaClass( Object::GetClass< T >( ) ); 
	}

	template < typename T >
	void MetaClass::SetValue( const Object* object, const MetaProperty* prop, const T& value ) const
	{
		if ( HasProperty( prop ) )
		{
			T* dest = reinterpret_cast< T* >( usize( object ) + prop->mOffset );
			*dest = value;
			
			// Call mutator callbacks
			for ( auto& mutator : prop->mMutatorCallbacks )
			{
				if ( mutator )
				{
					mutator->Invoke<void>( object, value ); 
				}
			} 

			for ( auto& m : prop->mOnValueChangedCallbacks )
			{
				m( );
			}
		}
	} 

	template < typename T >
	void MetaClass::SetValue( Object* object, const MetaProperty* prop, const T& value )
	{
		if ( HasProperty( prop ) )
		{
			T* dest = reinterpret_cast< T* >( usize( object ) + prop->mOffset );
			*dest = value;

			// Call mutator callbacks
			for ( auto& mutator : prop->mMutatorCallbacks )
			{
				if ( mutator )
				{
					mutator->Invoke<void>( object, value ); 
				}
			} 

			for ( auto& m : prop->mOnValueChangedCallbacks )
			{
				m( );
			}
		}
	}

	// MetaClass::~MetaClass( )
	// {
	// 	// Delete all functions
	// 	for ( auto& f : mFunctions )
	// 	{
	// 		delete f.second;
	// 		f.second = nullptr;
	// 	}


	// 	for ( auto& p : mProperties )
	// 	{
	// 		delete p;
	// 		p = nullptr;
	// 	}

	// 	// Clear properties and functions
	// 	mProperties.clear( );
	// 	mFunctions.clear( );

	// 	// Any further destruction that needs to occur
	// 	this->Destroy( ); 
	// } 

}

#endif

