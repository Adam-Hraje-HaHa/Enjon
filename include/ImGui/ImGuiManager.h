#pragma once
#ifndef ENJON_IM_GUI_MANAGER_H
#define ENJON_IM_GUI_MANAGER_H

#include "ImGui/imgui_impl_sdl_gl3.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_dock.h"

#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Graphics/Color.h"
#include "System/Types.h"
#include "Asset/UIStyleConfigAssetLoader.h"
#include "Graphics/Font.h"
#include "Subsystem.h"
#include "Defines.h"

#include <SDL2/SDL.h>

#include <functional>
#include <unordered_map>
#include <vector> 

struct ImFont;
struct ImGuiContext;

namespace Enjon
{ 
	class ImGuiManager;
	class Window;
	class WindowSubsystem;
	class Vec4;
	class Object;
	class MetaClass;
	class MetaPropertyArrayBase;
	class MetaPropertyHashMapBase;
	class MetaProperty;
	class Engine; 

	//==================================================================

	// Helper ImGui math functions
	static inline f32 ImVec2Length( const ImVec2& v )
	{
		return std::sqrt( v.x * v.x + v.y * v.y );
	}

	static inline ImVec2 operator+( const ImVec2& lhs, const ImVec2& rhs )
	{
		return ImVec2( lhs.x + rhs.x, lhs.y + rhs.y );
	}

	static inline ImVec2 operator+( const ImVec2& lhs, const Vec2& rhs )
	{
		return ImVec2( lhs.x + rhs.x, lhs.y + rhs.y );
	}

	static inline ImVec2 operator+=( const ImVec2& lhs, const ImVec2& rhs )
	{
		return ( lhs + rhs );
	} 

	static inline ImVec2 operator+=( const ImVec2& lhs, const Vec2& rhs )
	{
		return ( lhs + rhs );
	} 

	static inline ImVec2 operator-( const ImVec2& lhs, const ImVec2& rhs )
	{
		return ImVec2( lhs.x - rhs.x, lhs.y - rhs.y );
	}

	static inline ImVec2 operator-( const ImVec2& lhs, const Vec2& rhs )
	{
		return ImVec2( lhs.x - rhs.x, lhs.y - rhs.y ); 
	}

	static inline ImVec2 operator*( const ImVec2& lhs, const f32& scalar )
	{
		return ImVec2( lhs.x * scalar, lhs.y * scalar );
	}

	static inline ImVec2 operator*( const ImVec2& lhs, const ImVec2& rhs )
	{
		return ImVec2( lhs.x * rhs.x, lhs.y * rhs.y );
	}

	static inline ImVec2 operator/( const ImVec2& lhs, const f32& scalar )
	{
		return ImVec2( lhs.x / scalar, lhs.y / scalar );
	}

	static inline ImVec2 ImRotate( const ImVec2& v, const f32& cosA, const f32& sinA )
	{
		return ImVec2( v.x * cosA - v.y * sinA, v.x * sinA + v.y * cosA );
	}

	static inline ImColor ColorToImColor( const ColorRGBA32& color )
	{
		return ImColor( color.r, color.g, color.b, color.a );
	}

	static inline ImVec2 Vec2ToImVec2( const Vec2& v )
	{
		return ImVec2( v.x, v.y );
	}

	static inline ImVec2 Vec3ToImVec2( const Vec3& v )
	{
		return ImVec2( v.x, v.y );
	}

	using GUICallbackFunc = std::function<void( )>;

	enum GUIDockSlotType
	{
		Slot_Left,
		Slot_Right,
		Slot_Top,
		Slot_Bottom,
		Slot_Tab, 
		Slot_Float,
		Slot_None
	};

	enum class GUIFlags
	{
		AutoCalculateSize,
		None
	};

	class GUIWidget
	{ 
		public:
			/**
			* @brief
			*/
			GUIWidget( ) = default;

			/**
			* @brief
			*/
			GUIWidget( const String& label );

			/**
			* @brief
			*/
			GUIWidget( const String& label, const Vec2& position, const Vec2& size );

			/**
			* @brief
			*/
			virtual ~GUIWidget( ) = default;

			/**
			* @brief
			*/
			virtual void DoWidget( )
			{ 
				// Nothing by default
			}

			/**
			* @brief
			*/
			void SetSize( const Vec2& size );

			/**
			* @brief
			*/
			Vec2 GetSize( );

			/**
			* @brief
			*/
			Vec2 GetPosition( ); 

		protected: 
			Vec2 mPosition;
			Vec2 mSize = Vec2( 200.0f, 400.0f );
			String mLabel;
			bool mEnabled = false; 
			bool mHovered = false;
			bool mAutoCalculateSize = true;
	};

	class DockingWindow : public GUIWidget
	{
		public:
			/**
			* @brief
			*/
			DockingWindow( ) = default;

			/**
			* @brief
			*/
			DockingWindow( const String& label, const Vec2& position, const Vec2& size, u32 mViewFlags );

			/**
			* @brief
			*/
			~DockingWindow( ); 

			/**
			* @brief
			*/
			virtual void DoWidget( )
			{ 
				// Nothing by default...
			}

		protected:

		private:
			u32 mViewFlags = 0;

	};

	class PopupWindow : public GUIWidget
	{ 
		public:

			/**
			* @brief
			*/
			PopupWindow( ) = default;

			/**
			* @brief
			*/
			PopupWindow( const String& label, const Vec2& position, const Vec2& size );

			/**
			* @brief
			*/
			~PopupWindow( ) = default; 

			/**
			* @brief
			*/
			void operator+=( const GUICallbackFunc& func );

			/**
			* @brief
			*/
			bool Enabled( ); 

			/**
			* @brief
			*/
			void RegisterCallback( const GUICallbackFunc& func ); 

			/**
			* @brief
			*/
			void SetFadeInSpeed( const f32& speed );

			/** 
			* @brief
			*/
			void SetFadeOutSpeed( const f32& speed );

			/**
			* @brief
			*/
			void Activate( const Vec2& position );

			/**
			* @brief
			*/
			void Deactivate( );

			/**
			* @brief
			*/
			bool Hovered( );

			/**
			* @brief
			*/
			virtual void DoWidget( ) override;

		protected:

			/**
			* @brief
			*/
			void Reset( );

		private:
			Vector< GUICallbackFunc > mCallbacks;
			f32 mFadeTimer = 0.0f;
			f32 mFadeInSpeed = 5.0f;
			f32 mFadeOutSpeed = 10.0f;
			f32 mFadeOutTimer = 1.0f;
			bool mBeginDisable = false;
	};

	struct GUIDockingLayout
	{
		GUIDockingLayout(const char* child, const char* parent, GUIDockSlotType st, float weight)
			: mChild(child), mParent(parent), mSlotType(st), mWeight(weight)
		{}
		~GUIDockingLayout(){}

		const char* mChild;
		const char* mParent;
		GUIDockSlotType mSlotType;
		float mWeight;
	}; 

	typedef struct GUIContextParams
	{
		b32 mUseRootDock = true;
		AssetHandle< UIStyleConfig > mUIStyle;
	} GUIContextParams;
 
	class GUIContext
	{
		friend Window;
		friend ImGuiManager;
		friend WindowSubsystem;
		friend GraphicsSubsystem;

		public:

			/**
			* @brief
			*/
			GUIContext( ) = default;

			/**
			* @brief
			*/
			~GUIContext( ) = default;

			/**
			* @brief
			*/
			GUIContext( Window* window )
				: mWindow( window )
			{ 
			}

			ImGuiContext* GetContext( )
			{
				return mContext;
			}

			void SetContext( ImGuiContext* context )
			{
				mContext = context;
			}

			/** 
			* @brief
			*/
			bool HasWindow( const String& windowName );

			/** 
			* @brief
			*/
			bool HasMainMenu( const String& menu );

			/** 
			* @brief
			*/
			bool HasMainMenuOption( const String& menu, const String& menuOptionName );

			/** 
			* @brief
			*/
			void Register( const GUICallbackFunc& func);

			/** 
			* @brief
			*/
			void RegisterMenuOption( const String& menuName, const String& optionName, const GUICallbackFunc& func );

			/** 
			* @brief
			*/
			void RegisterWindow(const String& windowName, const GUICallbackFunc& func);

			/** 
			* @brief
			*/
			void RegisterDockingWindow( const String& dockName, const GUICallbackFunc& func );

			/** 
			* @brief
			*/
			void RegisterDockingLayout(const GUIDockingLayout& layout);

			/** 
			* @brief
			*/
			void RegisterMainMenu( const String& menuName );

			/** 
			* @brief
			*/
			void SetActiveDock( const char* );

			/** 
			* @brief 
			* @note NOT TO BE CALLED WHILE EXECUTING ANY IMGUI CODE
			*/
			void SetGUIContextParams( const GUIContextParams& params );

			/** 
			* @brief 
			* @note NOT TO BE CALLED WHILE EXECUTING ANY IMGUI CODE
			*/
			void SetUIStyleConfig( const AssetHandle< UIStyleConfig >& style );

			/** 
			* @brief 
			* @note NOT TO BE CALLED WHILE EXECUTING ANY IMGUI CODE
			*/
			void ClearContext( );

			/** 
			* @brief 
			* @note NOT TO BE CALLED WHILE EXECUTING ANY IMGUI CODE
			*/
			void Finalize();
 
			/** 
			* @brief 
			* @note NOT TO BE CALLED OUTSIDE OF IMGUI CODE
			*/
			void RootDock( ); 

			/** 
			* @brief
			*/
			void Render( ); 

			/** 
			* @brief
			*/
			s32 MainMenu( ); 

		protected:

			void CreateMainMenu( const String& menuName );

			/** 
			* @brief
			*/
			void LateInit( );

		private: 
			Vector<GUICallbackFunc> mGuiFuncs;
			HashMap<String, GUICallbackFunc> mWindows;
			HashMap<String, HashMap<String, GUICallbackFunc>> mMainMenuOptions;
			Vector< String > mMainMenuLayout;
			Vector<GUIDockingLayout> mDockingLayouts; 
			HashMap< Enjon::String, ImFont* > mFonts;
			ImGuiContext* mContext = nullptr;
			Window* mWindow = nullptr;
			const char* mActiveDock = nullptr;
			GUIContextParams mParams;
	}; 

	ENJON_CLASS( )
	class ImGuiManager : public Subsystem
	{
		friend Engine;
		friend GUIWidget;
		friend WindowSubsystem;

		ENJON_CLASS_BODY( ImGuiManager )

		public:

			/**
			*@brief
			*/
			virtual Result Initialize() override;

			/**
			*@brief
			*/
			virtual void Update( const f32 dT ) override;

			/**
			*@brief
			*/
			virtual Result Shutdown() override;

			ImGuiContext* Init(Window* window);
			void LateInit(Window* window);
			bool HasWindow( const String& windowName );
			bool HasMenuOption( const String& menu, const String& menuOptionName );
			void Register(std::function<void()> func);
			void RegisterMenuOption( const String& menuName, const String& optionName, std::function<void( )> func );
			void RegisterWindow(const String& windowName, std::function<void()> func);
			void RegisterDockingLayout(const GUIDockingLayout& layout);
			void RenderGameUI(Enjon::Window* window, f32* view, f32* projection);
			void Render(Window* window);
			void InspectObject( const Object* object );
			void DebugDumpObject( const Enjon::Object* object );
			void DebugDumpProperty( const Enjon::Object* object, const Enjon::MetaProperty* prop );
			void DebugDumpArrayProperty( const Enjon::Object* object, const Enjon::MetaPropertyArrayBase* prop );
			void DebugDumpHashMapProperty( const Enjon::Object* object, const Enjon::MetaPropertyHashMapBase* prop );
			ImFont* GetFont( const Enjon::String& name );
			ImGuiContext* GetContext( ); 

			void SetContextByWindow( Window* window ); 

			void LoadStyle( const AssetHandle< UIStyleConfig >& config, GUIContext* ctx = nullptr );

			void AddFont( const String& filePath, const u32& size, GUIContext* ctx, const char* fontName );

			void AddFont( const AssetHandle< UIFont >& ui, const u32& pointSize, GUIContext* ctx = nullptr );

			ImGuiContext* GetContextByWindow( Window* window );

		public:

			/**
			* @brief
			*/
			void Text( const String& text );

			/**
			* @brief
			*/
			bool CheckBox( const String& text, bool* option = nullptr );

			/**
			* @brief
			*/
			void SameLine( );

			/**
			* @brief
			*/
			bool Button( const String& text );

			/**
			* @brief
			*/
			bool Selectable( const String& text );

			/**
			* @brief
			*/
			void Separator( );

			/**
			* @brief
			*/
			void PushFont( const String& fontName );

			/**
			* @brief
			*/
			void PushFont( const AssetHandle< UIFont >& font, u32 ptSize = 16 );

			/**
			* @brief
			*/
			void PopFont( );

			/**
			* @brief
			*/
			bool DragFloat2( const String& label, Vec2* vec, f32 speed = 1.0f, f32 min = 0.0f, f32 max = 0.0f );

			/**
			* @brief
			*/

			bool InputText( const char* label, char* buffer, usize buffSize );

		protected:
			void BindContext( ); 
			void ProcessEvent( SDL_Event* event );
			void RemoveWindowFromContextMap( SDL_Window* window ); 

		private:
			s32 MainMenu();
			void ImGuiStyles();
			void InitializeDefaults(); 

			void AddWindowToContextMap( SDL_Window* window, ImGuiContext* ctx );

		private:
			Vector<std::function<void()>> mGuiFuncs;
			HashMap<String, std::function<void()>> mWindows;
			HashMap<String, HashMap<String, std::function<void()>>> mMainMenuOptions;
			Vector<GUIDockingLayout> mDockingLayouts; 
			HashMap< Enjon::String, ImFont* > mFonts;
			ImGuiContext* mContext = nullptr;
			HashMap< SDL_Window*, ImGuiContext* > mImGuiContextMap;
 
		public:
			// SDF parameters
			f32 mGamma = 0.085f;
			f32 mBuffer = 0.454f;
	};

	ENJON_CLASS( )
	class EditorEvent : public Object
	{ 
		ENJON_CLASS_BODY( EditorEvent )

		public:

		protected:

	};

	ENJON_CLASS( )
	class EditorDraggedItemEvent : public EditorEvent
	{
		ENJON_CLASS_BODY( EditorDraggedItemEvent )

		public:
			EditorDraggedItemEvent( Object* object )
				: mObject( object )
			{
			}

		protected:
			Object* mObject = nullptr; 
	}; 
}

#endif











