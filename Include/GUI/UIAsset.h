#pragma once
#ifndef ENJON_UI_ASSET_H
#define ENJON_UI_ASSET_H 

#include "System/Types.h"
#include "Math/Maths.h"
#include "Asset/Asset.h"
#include "Graphics/Texture.h"
#include "Graphics/Font.h"
#include "Asset/UIStyleConfigAssetLoader.h" 

namespace Enjon
{ 
#define UIBindFunction( elem_ptr, elem_type, var, func )\
	do {\
		elem_type* elem = ( elem_type* )( elem_ptr );\
		if ( elem ) {\
			elem->var = func;\
		}\
	} while ( 0 )

	// Forward Decls.
	class UI;

/*
	// A UI should be able to be associated with a given 'style sheet'
	// This style sheet should have a collection of classes with styles
	// Style options for any given class/item
	// When deserializing a given element, should set the style for its ygnode via the class it's associated with
	// What about hover states? Should I look for an associated hover/active/selected as well? 

	StyleElementFloat 
	StyleElementVec2
	StyleElementVec4

	// Should these be assets? I don't want a litter of assets around for any given UI...want these all contained, if possible.
	class UIStyleElement
	{
	};

	ENJON_ENUM()
	enum class UIMouseState
	{
		Hovered,
		Active,
		Selected
	};

	// Is there a way to do this without having to heap allocate everything?... 

	// Would like to be able to push/pop on styles ( that way only the styles that are set are affected? )
	class UIStyleSettings
	{
		// All the possible styles that can exist? 
		// Base stylings to inherit from? 
		PositionType		- Are these part of the stylings? Or are they individual element information?... 
		Anchors				--^
		Width 
		Height
		
		Justification
		Alignment
		Direction
		Flex: 
			-Grow, Shrink
		Colors: 
			BGColor
			BorderColor
		Border
	};

	class UIStyleSheet
	{
		// What does this need to hold? Map of all selector classes to styles to be set?  

		HashMap< String, UIStyleSettings > mStyles;   
	};

	class UISelectorClass
	{
		String mClassID;	
		
		// Hover state
		// Active state
		// Selected state
	}; 

	{
		.element_class: {
			width: auto;
			height: 100px; 
			flex-grow: 1;
		} 

		.container {
			flex-wrap: wrap;
		}
	}
*/ 
 
	ENJON_CLASS( Abstract )
	class UIElement : public Object
	{
		ENJON_CLASS_BODY( UIElement )

		public: 

			virtual void ExplicitConstructor() override;
			virtual void ExplicitDestructor() override;

			/*
			* @brief
			*/
			virtual void OnUI() = 0;

			/*
			* @brief
			*/
			static inline void DefaultFunc( UIElement* elem ) {
				// Do nothing...
			} 

			const UIStyleConfiguration& GetCurrentStyleConfiguration( ) const;

			virtual Result SerializeData( ByteBuffer* buffer ) const override; 
			virtual Result DeserializeData( ByteBuffer* buffer ) override;

			void SetStyleWidth( const f32& width ) {
				//mStyleConfiguration.mSize.x = width;
				mStyleOverrides.AddStyle( UIStylePropertyType::Width, width );
			}

			f32 GetStyleWidth( ) const {
				switch ( mState )
				{
					default:
					case UIStyleState::Default: 
					{ 
						return mStyleConfiguration.mSize.x;
					} break;
					case UIStyleState::Hovered: 
					{ 
						return mHoverStyleConfiguration.mSize.x;
					} break;
					case UIStyleState::Active: 
					{ 
						return mActiveStyleConfiguration.mSize.x;
					} break;
				}
			}

			void SetStyleHeight( const f32& height ) {
				//mStyleConfiguration.mSize.y = height;
				mStyleOverrides.AddStyle( UIStylePropertyType::Height, height );
			}

			f32 GetStyleHeight( ) const {
				switch ( mState )
				{
					default:
					case UIStyleState::Default: 
					{ 
						return mStyleConfiguration.mSize.y;
					} break;
					case UIStyleState::Hovered: 
					{ 
						return mHoverStyleConfiguration.mSize.y;
					} break;
					case UIStyleState::Active: 
					{ 
						return mActiveStyleConfiguration.mSize.y;
					} break;
				}
			}

			void SetStylePadding( const Vec4& padding ) {
				mStyleConfiguration.mPadding = padding;
			}

			Vec4 GetStylePadding( ) const {
				return mStyleConfiguration.mPadding;
			}

			void SetStyleMargin( const Vec4& margin ) {
				mStyleConfiguration.mMargin = margin;
			}

			Vec4 GetStyleMargin( ) const {
				return mStyleConfiguration.mMargin;
			}

			void SetStyleFlexGrow( const f32& grow ) {
				mStyleConfiguration.mFlexGrow = grow;
			}

			void SetStyleFlexShrink( const f32& shrink ) {
				mStyleConfiguration.mFlexShrink = shrink;
			}

			void SetStylePositionType( const UIElementPositionType& type ) {
				mStyleConfiguration.mPositionType = type;
			}

			void SetStyleAlignContent( const UIElementAlignment& alignment ) {
				mStyleConfiguration.mAlignContent = alignment;
			}

			UIElementAlignment GetStyleAlignContent( ) const {
				return mStyleConfiguration.mAlignContent;
			}

			void SetStyleAlignSelf( const UIElementAlignment& alignment ) {
				mStyleConfiguration.mAlignSelf = alignment;
			}

			void SetStyleFlexDirection( const UIElementFlexDirection& direction ) {
				mStyleConfiguration.mFlexDirection = direction;
			}

			UIElementFlexDirection GetStyleFlexDirection( ) const {
				return mStyleConfiguration.mFlexDirection;
			}

			void SetStyleJustification( const UIElementJustification& justification ) {
				mStyleConfiguration.mJustification = justification;
			}

			UIElementJustification GetStyleJustification( ) const {
				return mStyleConfiguration.mJustification;
			}

			f32 GetStyleFlexGrow( ) const {
				return mStyleConfiguration.mFlexGrow;
			}

			Vec4 CalculateChildBounds( const AssetHandle< UI >& ui );
			void CalculateLayout( const AssetHandle< UI >& ui ); 
			void CalculateStyle( const AssetHandle< UI >& ui );

			/*
			* @brief
			*/
			UIElement* AddChild( UIElement* element ); 

			/*
			* @brief
			*/
			UIElement* RemoveChild( UIElement* element );

			/*
			* @brief
			*/
			void RemoveFromParent( );

			//ENJON_FUNCTION()
			void SetSize( const Vec2& size );

			//ENJON_FUNCTION()
			//void SetInlineStyles( const UIStyleSettings& styles );

			Vec4 GetCalculatedLayoutRect(); 
			Vec2 GetCalculatedLayoutPosition();
			Vec2 GetCalculatedLayoutSize();
		
			ENJON_PROPERTY( DisplayName = "ID" )
			String mID = ""; 

			ENJON_PROPERTY( HideInEditor, NonSerializeable )
			Vector< UIElement* > mChildren;

			ENJON_PROPERTY( HideInEditor, NonSerializeable )
			UIElement* mParent = nullptr; 

			ENJON_PROPERTY( HideInEditor, NonSerializeable )
			UIStyleConfiguration mStyleConfiguration;

			ENJON_PROPERTY( HideInEditor, NonSerializeable )
			UIStyleConfiguration mHoverStyleConfiguration;

			ENJON_PROPERTY( HideInEditor, NonSerializeable )
			UIStyleConfiguration mActiveStyleConfiguration;

			ENJON_PROPERTY( HideInEditor )
			Vector< String > mStyleSelectors;

			ENJON_PROPERTY( HideInEditor, NonSerializeable ) 
			UIStyleState mState = UIStyleState::Default; 

			ENJON_PROPERTY( HideInEditor, NonSerializeable )
			UIStyleRule mStyleOverrides;

			public:
			Vec2 mCalculatedLayoutPosition;
			Vec2 mCalculatedLayoutSize;
			Vec4 mChildBounds;
	}; 

	// Not sure if I like this here...need a way to be able to view this in the editor and set the function
	ENJON_CLASS( )
	class UICallback : public Object
	{
		ENJON_CLASS_BODY( UICallback )

		public:

			virtual void ExplicitConstructor() override
			{
				mCallback = UIElement::DefaultFunc; 
			}

			void operator()( UIElement* elem )
			{
				mCallback( elem );
			}

			void operator=( const std::function< void( UIElement* ) >& func )
			{
				mCallback = func;
			}

			std::function< void( UIElement* ) > mCallback;
	};

	ENJON_CLASS()
	class UIElementButton : public UIElement
	{
		ENJON_CLASS_BODY( UIElementButton )

		public: 

			/*
			* @brief
			*/
			virtual void OnUI() override; 

		public:

			ENJON_PROPERTY( )
			String mText = ""; 

			// Can I serialize this somehow? 
			UICallback mOnClick;
	}; 

	ENJON_CLASS( Construct )
	class UIElementText : public UIElement
	 {
		ENJON_CLASS_BODY( UIElementText )

		public:

			virtual void OnUI() override; 

		 public:

			ENJON_PROPERTY()
			String mText = ""; 

			UICallback mOnSetText;
	 };

	ENJON_CLASS( Construct )
	class UIElementImage : public UIElement
	{
		ENJON_CLASS_BODY( UIElementImage )

		public:

			virtual void OnUI() override;

		public:

			ENJON_PROPERTY()
			AssetHandle< Texture > mImage;

			ENJON_PROPERTY( )
			Vec2 mUV0 = Vec2( 0.0f, 0.f );

			ENJON_PROPERTY( )
			Vec2 mUV1 = Vec2( 1.0f, 1.f );
			
			// Want to make these events that can be registered
			UICallback mOnSetImage;
	};

	ENJON_CLASS( Construct )
	class UIElementCanvas : public UIElement
	{
		ENJON_CLASS_BODY( UIElementCanvas )

		public:

			/*
			* @brief
			*/
			virtual void OnUI() override; 
	}; 

	ENJON_CLASS( )
	class UI : public Asset
	{ 
		ENJON_CLASS_BODY( UI ) 

		public:

			virtual void ExplicitConstructor( ) override;

			/*
			* @brief
			*/
			virtual void ExplicitDestructor( ) override;

			/*
			* @brief
			*/
			virtual Result SerializeData( ByteBuffer* buffer ) const override;
			
			/*
			* @brief
			*/
			virtual Result DeserializeData( ByteBuffer* buffer ) override; 

			/*
			* @brief
			*/
			virtual Result OnEditorUI() override; 

			/*
			* @brief
			*/
			void OnUI() const;

			/*
			* @brief
			*/
			UIElement* FindElement( const char* label ) const;

			void CalculateLayout( const u32& width, const u32& height );

			AssetHandle< UIStyleSheet > GetStyleSheet( ) const;

		public: 

			// Will handle all serialization of ui manually
			ENJON_PROPERTY( NonSerializeable, HideInEditor ) 
			UIElementCanvas mRoot; 

			ENJON_PROPERTY( HideInEditor )
			AssetHandle< UIStyleConfig > mStyleConfig;

			ENJON_PROPERTY( HideInEditor )
			AssetHandle< UIStyleSheet > mStyleSheet;
	}; 
}

#endif
