#pragma once 
#ifndef ENJON_UI_EDIT_WINDOW_H
#define ENJON_UI_EDIT_WINDOW_H

#include "EditorView.h"
#include "EditorTransformWidget.h"

#include <Base/World.h> 
#include <Graphics/Window.h>
#include <Asset/Asset.h>
#include <GUI/UIAsset.h>
#include <Graphics/Camera.h>
#include <Graphics/RenderPass.h>

#include <Utils/Property.h>

// Quick test for this yoga shit
#include <yoga/Yoga.h>

namespace Enjon
{
	class FrameBuffer;

	ENJON_CLASS( Construct )
	class UIRenderPass : public RenderPass
	{
		ENJON_CLASS_BODY( UIRenderPass )

		public:
			UIRenderPass( const AssetHandle< UI >& ui, const Window* window );

			u32 GetFramebufferTextureId() const;
 
		protected:

			virtual void Render() override;

		private: 
			AssetHandle< UI > mUI; 
			FrameBuffer* mFrameBuffer = nullptr;
			const Window* mWindow = nullptr;
			GUIContext* mGUIContext = nullptr;
	};

	class EditorViewport;

	ENJON_CLASS( Construct )
	class EditorUICanvasWindow : public Window
	{
		ENJON_CLASS_BODY( EditorUICanvasWindow )

		public:

			virtual void Init( const WindowParams& params ) override; 
			virtual void Update() override;

			void SetUI( const AssetHandle< UI >& ui ) 
			{
				mUI = ui;
			}

		protected:

			void ConstructScene();

		private:
			b32 mInitialized = false;
			AssetHandle< UI > mUI;
 
			YGNodeRef mRootNode = nullptr;
	};

	ENJON_CLASS( Construct )
	class EditorUIEditWindow : public Window
	{
		ENJON_CLASS_BODY( EditorUIEditWindow )

		public:

			virtual void ExplicitDestructor() override;

			virtual void Init( const WindowParams& params ) override;

			virtual void Update() override;

		protected:

			void ConstructScene();

		protected:

			void Canvas();
			void DrawUI();

			void SelectElement( UIElement* element );
			void DeselectElement();
			ImVec2 HandleMousePan();
			bool DrawRectHandle( const ImVec2& center, const u32& id, ImVec2* delta ); 
			void DrawUIElementsRecursive( UIElement* element );
			void AddDashedLine( const ImVec2& a, const ImVec2& b, const ImColor& color );
			void AddDashedLineRect( const ImVec2& a, const ImVec2& b, const ImColor& color );
			bool CanSelectThisElement( UIElement* element, Camera* camera ); 
			void ElementOutliner( UIElement* element );

		private:

			s32 mCanvasWindowID = -1;
			Camera mCamera;

			UIElement* mSelectedElement = nullptr;

			AssetHandle< UI > mUI;
			b32 mInitialized = false;
			b32 mCanvasInitialized = false;
 
			Vec2 mViewportSize = Vec2( 16.f * 70.f, 9.f * 70.f ); 
	};

	class UIStyleRulePopupEdit
	{ 
		public: 

			UIStyleRulePopupEdit( ) = default;

			UIStyleRulePopupEdit( const String& ruleName, const UIStyleState& state, const Vec2& position, const AssetHandle< UIStyleSheet >& styleSheet )
				: mRuleName( ruleName ), mState( state ), mPosition( position ), mStyleSheet( styleSheet )
			{ 
				mActive = true;
			}

			void Update( );

			String mRuleName;
			UIStyleState mState;
			Vec2 mPosition;
			bool mActive = false;
			AssetHandle< UIStyleSheet > mStyleSheet;
	};

	class RandomPopupWindow
	{
		public: 
			RandomPopupWindow( ) = default;
			RandomPopupWindow( const ImVec2& position, const std::function< bool( ) >& func )
				: mUpdateFunc( func ), mPosition( position )
			{ 
				mActive = true;
			}

			void Update( ); 
			
			ImVec2 mPosition;
			std::function< bool( ) > mUpdateFunc;
			bool mActive = false;
	};

	ENJON_CLASS( Construct )
	class EditorUIStyleSheetEditWindow : public Window
	{
		ENJON_CLASS_BODY( EditorUIStyleSheetEditWindow )

		public:

			virtual void Init( const WindowParams& params ) override;

			virtual void Update() override;

			static const char* GetLabelFromStylePropertyType( const UIStylePropertyType& type );

		protected:

			void ConstructScene(); 

			void DisplayRuleGroup( const String& ruleName, UIStyleRuleGroup* ruleGroup );
			void DisplayRule( const String& ruleName, UIStyleState state, UIStyleRule* rule );
			void EditStyleRulePopup( const String& ruleName, UIStyleState state, UIStyleRule* rule );

			bool RightClickCanvasPopupFunction( RandomPopupWindow* popupWindow );
			
			bool mCreateTagPopupWindow = false;

			AssetHandle< UIStyleSheet > mStyleSheet;

			UIStyleRulePopupEdit mPopupEdit;
			RandomPopupWindow mPopupWindow;

			String mCode = "";
	};
}

#endif
