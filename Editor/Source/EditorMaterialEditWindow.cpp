
#include "EditorMaterialEditWindow.h"
#include "EditorWorldOutlinerView.h"
#include "EditorAssetBrowserView.h"
#include "EditorApp.h"

#include <Graphics/GraphicsSubsystem.h>
#include <Scene/SceneManager.h>
#include <Asset/AssetManager.h>
#include <Entity/Archetype.h>
#include <Graphics/StaticMeshRenderable.h>
#include <Entity/Components/StaticMeshComponent.h>
#include <Entity/Components/SkeletalMeshComponent.h>
#include <Serialize/ObjectArchiver.h>
#include <IO/InputManager.h>
#include <SubsystemCatalog.h>
#include <Utils/FileUtils.h> 
#include <Engine.h>

namespace Enjon
{
	/**
	* @brief Must be overriden
	*/
	void EditorViewport::UpdateView( )
	{ 
		GraphicsSubsystem* gfx = EngineSubsystem( GraphicsSubsystem ); 

		// Grab graphics context from world and then get framebuffer rendertarget texture
		World* world = mWindow->GetWorld( );

		// Cannot operate without a world!
		if (!world) {
			return;
		}

		GraphicsSubsystemContext* gfxCtx = world->GetContext< GraphicsSubsystemContext >( );
		u32 currentTextureId = gfxCtx->GetFrameBuffer( )->GetTexture( );

		// Rotate camera over time
		Camera* cam = gfxCtx->GetGraphicsScene( )->GetActiveCamera( );

		// Render game in window
		ImVec2 cursorPos = ImGui::GetCursorScreenPos( );

		// Cache off cursor position for scene view
		Vec2 padding( 20.0f, 8.0f );
		f32 width = ImGui::GetWindowWidth( ) - padding.x;
		f32 height = ImGui::GetWindowSize( ).y - ImGui::GetCursorPosY( ) - padding.y;
		mSceneViewWindowPosition = Vec2( cursorPos.x, cursorPos.y );
		mSceneViewWindowSize = Vec2( width, height );

		ImTextureID img = ( ImTextureID )Int2VoidP(currentTextureId);
		ImGui::Image( img, ImVec2( width, height ), ImVec2( 0, 1 ), ImVec2( 1, 0 ), ImColor( 255, 255, 255, 255 ), ImColor( 255, 255, 255, 0 ) );

		// Update camera aspect ratio
		gfxCtx->GetGraphicsScene( )->GetActiveCamera( )->SetAspectRatio( ImGui::GetWindowWidth( ) / ImGui::GetWindowHeight( ) );

		// Draw border around image
		ImDrawList* dl = ImGui::GetWindowDrawList( );
		ImVec2 a( mSceneViewWindowPosition.x, mSceneViewWindowPosition.y );
		ImVec2 b( mSceneViewWindowPosition.x + mSceneViewWindowSize.x, mSceneViewWindowPosition.y + mSceneViewWindowSize.y ); 
		dl->AddRect( a, b, ImColor( 0.0f, 0.64f, 1.0f, 0.48f ), 1.0f, 15, 1.5f ); 

		WindowSubsystem* ws = EngineSubsystem( WindowSubsystem );

		EditorAssetBrowserView* abv = mApp->GetEditorAssetBrowserView( );
		if ( abv->GetGrabbedAsset( ) && mWindow->IsMouseInWindow( ) && ws->NumberOfHoveredWindows() == 1 )
		{
			mWindow->SetFocus( );
		} 

		if ( abv->GetGrabbedAsset( ) )
		{ 
			Vec2 mp = GetSceneViewProjectedCursorPosition( );

			if ( mWindow->IsMouseInWindow( ) && mWindow->IsFocused( ) )
			{ 
				String label = Utils::format( "Asset: %s", abv->GetGrabbedAsset()->GetName().c_str() ).c_str( );
				ImVec2 txtSize = ImGui::CalcTextSize( label.c_str( ) );
				ImGui::SetNextWindowPos( ImVec2( ImGui::GetMousePos( ).x + 15.0f, ImGui::GetMousePos().y + 5.0f ) );
				ImGui::SetNextWindowSize( ImVec2( txtSize.x + 20.0f, txtSize.y ) );
				ImGui::Begin( "##grabbed_asset_window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar );
				{
					ImGui::Text( "%s", label.c_str( ) );
				}
				ImGui::End( ); 

				if ( ImGui::IsMouseReleased( 0 ) && IsHovered( ) )
				{
					HandleAssetDrop( );
				} 
			} 
		}

		if ( HasViewportCallback( ViewportCallbackType::CustomRenderOverlay ) )
		{
			mViewportCallbacks[ (u32)ViewportCallbackType::CustomRenderOverlay ]( nullptr );
		}

		if ( IsFocused( ) )
		{
			UpdateCamera( );
		}
		else
		{
			if ( mFocusSet )
			{
				// Reset to showing mouse cursor ( push and pop this, maybe? )
				mWindow->ShowMouseCursor( true ); 
			}

			mStartedFocusing = false;
			mFocusSet = false;
		}
	} 

	//===============================================================================================

	void EditorViewport::SetViewportCallback( ViewportCallbackType type, const ViewportCallback& callback )
	{
		mViewportCallbacks[ (u32)type ] = callback;
	}

	//===============================================================================================

	Vec2 EditorViewport::GetSceneViewProjectedCursorPosition( )
	{
		// Need to get percentage of screen and things and stuff from mouse position
		GraphicsSubsystem* gfx = EngineSubsystem( GraphicsSubsystem ); 
		Input* input = EngineSubsystem( Input );
		iVec2 viewport = gfx->GetViewport( );
		Vec2 mp = input->GetMouseCoords( );
		
		// X screen percentage
		f32 pX = f32( mp.x - mSceneViewWindowPosition.x ) / f32( mSceneViewWindowSize.x );
		f32 pY = f32( mp.y - mSceneViewWindowPosition.y ) / f32( mSceneViewWindowSize.y ); 

		return Vec2( (s32)( pX * viewport.x ), (s32)( pY * viewport.y ) );
	}

	bool EditorViewport::HasViewportCallback( const ViewportCallbackType& type )
	{
		return ( mViewportCallbacks.find( (u32)type ) != mViewportCallbacks.end( ) );
	}

	void EditorViewport::HandleAssetDrop( )
	{
		// Get projected mouse position
		Vec2 mp = GetSceneViewProjectedCursorPosition( );

		const Asset* grabbedAsset = mApp->GetEditorAssetBrowserView( )->GetGrabbedAsset( );

		// If material, then set material of overlapped object with this asset
		if ( grabbedAsset->Class( )->InstanceOf< Material >( ) )
		{
			// Check against graphics system for object
			GraphicsSubsystem* gfx = EngineSubsystem( GraphicsSubsystem ); 

			PickResult pickRes = gfx->GetPickedObjectResult( mp, mWindow->GetWorld( )->GetContext< GraphicsSubsystemContext >( ) );

			u32 subMeshIdx = pickRes.mSubmeshIndex;

			Entity* ent = pickRes.mEntity.Get( );

			if ( ent )
			{
				// If static mesh component
				if ( ent->HasComponent< StaticMeshComponent >( ) )
				{
					StaticMeshComponent* smc = ent->GetComponent< StaticMeshComponent >( );
					smc->SetMaterial( grabbedAsset );
				}
				else if ( ent->HasComponent< SkeletalMeshComponent >( ) )
				{
					SkeletalMeshComponent* smc = ent->GetComponent< SkeletalMeshComponent >( );
					smc->SetMaterial( grabbedAsset );
				}
			}
		} 
		else if ( grabbedAsset->Class( )->InstanceOf< Archetype >( ) )
		{
			if ( HasViewportCallback( ViewportCallbackType::AssetDropArchetype ) )
			{
				mViewportCallbacks[ (u32)ViewportCallbackType::AssetDropArchetype ]( (void*)grabbedAsset );
			} 
		}
		else if ( grabbedAsset->Class( )->InstanceOf< SkeletalMesh >( ) )
		{
			// Do things...  
			// Construct new entity in front of camera
			SkeletalMesh* mesh = grabbedAsset->ConstCast< SkeletalMesh >( );
			if ( mesh )
			{
				// Instantiate the archetype right in front of the camera for now
				GraphicsSubsystemContext* gfxCtx = GetWindow( )->GetWorld( )->GetContext< GraphicsSubsystemContext >( );
				Camera* cam = gfxCtx->GetGraphicsScene( )->GetActiveCamera( );
				Vec3 position = cam->GetPosition() + cam->Forward( ) * 5.0f; 
				EntityHandle handle = EngineSubsystem( EntityManager )->Allocate( mWindow->GetWorld( ) );
				if ( handle )
				{
					Entity* newEnt = handle.Get( );
					SkeletalMeshComponent* smc = newEnt->AddComponent< SkeletalMeshComponent >( );
					smc->SetMesh( mesh );
					newEnt->SetLocalPosition( position );
					newEnt->SetName( mesh->GetName( ) );
				}
			}
		}
		else if ( grabbedAsset->Class( )->InstanceOf< Mesh >( ) )
		{
			// Construct new entity in front of camera
			Mesh* mesh = grabbedAsset->ConstCast< Mesh >( );
			if ( mesh )
			{
				// Instantiate the archetype right in front of the camera for now
				GraphicsSubsystemContext* gfxCtx = GetWindow( )->GetWorld( )->GetContext< GraphicsSubsystemContext >( );
				Camera* cam = gfxCtx->GetGraphicsScene( )->GetActiveCamera( );
				Vec3 position = cam->GetPosition() + cam->Forward( ) * 5.0f; 
				EntityHandle handle = EngineSubsystem( EntityManager )->Allocate( mWindow->GetWorld( ) );
				if ( handle )
				{
					Entity* newEnt = handle.Get( );
					StaticMeshComponent* smc = newEnt->AddComponent< StaticMeshComponent >( );
					smc->SetMesh( mesh );
					newEnt->SetLocalPosition( position );
					newEnt->SetName( mesh->GetName( ) );
				}
			}
		}
		
	}

	//===============================================================================================

	void EditorViewport::CaptureState( )
	{
		Input* input = EngineSubsystem( Input );

		// Capture hovered state
		mIsHovered = ImGui::IsWindowHovered( ); 

		// Capture focused state
		mIsFocused = ( mIsHovered && ( input->IsKeyDown( KeyCode::RightMouseButton ) ) );
	}

	void EditorViewport::UpdateCamera( )
	{
		if ( IsFocused( ) && !mFocusSet )
		{
			if ( !mStartedFocusing )
			{
				mStartedFocusing = true;
				mFocusSet = true;
			}
		}

		Input* input = EngineSubsystem( Input ); 
 
		Enjon::Vec3 velDir( 0, 0, 0 ); 

		Window* window = this->GetWindow( );

		// Can't operate without a window
		if ( window == nullptr ) {
			return;
		}

		World* world = window->GetWorld( );

		// Can't operate without a world
		if ( world == nullptr ) {
			return;
		}

		GraphicsSubsystemContext* gsc = world->GetContext< GraphicsSubsystemContext >( );

		// Can't operate without a graphics subsystem context
		if ( gsc == nullptr ) {
			return;
		}

		// Get viewport of window
		iVec2 viewPort = window->GetViewport( );

		if ( mStartedFocusing )
		{
			Vec2 mc = input->GetMouseCoords( );
			Vec2 center = GetCenterOfViewport( );
			mMouseCoordsDelta = Vec2( (f32)(viewPort.x) / 2.0f - mc.x, (f32)(viewPort.y) / 2.0f - mc.y ); 
			mStartedFocusing = false;
		}

		Camera* camera = gsc->GetGraphicsScene( )->GetActiveCamera( );

		// Set camera speed 
		Vec2 mw = input->GetMouseWheel( ).y;
		f32 mult = mw.y == 1.0f ? 1.5f : mw.y == -1.0f ? 0.75f : 1.0f;
		mCameraSpeed = Math::Clamp(mCameraSpeed * mult, 0.25f, 128.0f);

		if ( input->IsKeyDown( Enjon::KeyCode::W ) )
		{
			Enjon::Vec3 F = camera->Forward( );
			velDir += F;
		}
		if ( input->IsKeyDown( Enjon::KeyCode::S ) )
		{
			Enjon::Vec3 B = camera->Backward( );
			velDir += B;
		}
		if ( input->IsKeyDown( Enjon::KeyCode::A ) )
		{
			velDir += camera->Left( );
		}
		if ( input->IsKeyDown( Enjon::KeyCode::D ) )
		{
			velDir += camera->Right( );
		}

		// Normalize velocity
		velDir = Enjon::Vec3::Normalize( velDir );

		f32 avgDT = Engine::GetInstance( )->GetWorldTime( ).GetDeltaTime( );

		// Set camera position
		camera->SetPosition( camera->GetPosition() + ( mCameraSpeed * avgDT * velDir ) );

		// Set camera rotation
		// Get mouse input and change orientation of camera
		Enjon::Vec2 mouseCoords = input->GetMouseCoords( ); 

		// Set cursor to not visible
		window->ShowMouseCursor( false );

		// Reset the mouse coords after having gotten the mouse coordinates
		Vec2 center = GetCenterOfViewport( );
		//SDL_WarpMouseInWindow( window->GetWindowContext( ), (s32)center.x, (s32)center.y );
		SDL_WarpMouseInWindow( window->GetWindowContext( ), ( f32 )viewPort.x / 2.0f - mMouseCoordsDelta.x, ( f32 )viewPort.y / 2.0f - mMouseCoordsDelta.y );

		// Offset camera orientation
		f32 xOffset = Enjon::Math::ToRadians( ( f32 )viewPort.x / 2.0f - mouseCoords.x - mMouseCoordsDelta.x ) * mMouseSensitivity / 100.0f;
		f32 yOffset = Enjon::Math::ToRadians( ( f32 )viewPort.y / 2.0f - mouseCoords.y - mMouseCoordsDelta.y ) * mMouseSensitivity / 100.0f;
		camera->OffsetOrientation( xOffset, yOffset ); 
	}

	Vec2 EditorViewport::GetCenterOfViewport( )
	{
		return Vec2( mSceneViewWindowPosition.x + mSceneViewWindowSize.x / 2.0f, mSceneViewWindowPosition.y + mSceneViewWindowSize.y / 2.0f );
	}

	EditorMaterialEditWindow::EditorMaterialEditWindow( const AssetHandle< Material >& mat )
		: mMaterial( mat ), mInitialized( false )
	{ 
	} 

	void EditorMaterialEditWindow::Init( const WindowParams& params )
	{ 
		// Construct scene in world
		if ( !mInitialized )
		{ 
			// Initialize new world 
			mWorld = new World( ); 
			// Register contexts with world
			mWorld->RegisterContext< GraphicsSubsystemContext >( ); 
			// Set material from data
			mMaterial = ( Material* )( params.mData );

			ConstructScene( );

			mInitialized = true;
		} 
	} 
 
	void EditorMaterialEditWindow::ConstructScene( )
	{ 
		GUIContext* guiContext = GetGUIContext( );

		// Add main menu options
		guiContext->RegisterMainMenu( "File" );
		guiContext->RegisterMainMenu( "View" );

		// Create viewport
		mViewport = new EditorViewport( Engine::GetInstance( )->GetApplication( )->ConstCast< EditorApp >( ), this, "Viewport" );

		//guiContext->RegisterDockingLayout( GUIDockingLayout( mViewport->GetViewName().c_str(), nullptr, GUIDockSlotType::Slot_Tab, 1.0f ) ); 

		// NOTE(): This should be done automatically for the user in the backend
		// Add window to graphics subsystem ( totally stupid way to do this )
		GraphicsSubsystem* gfx = EngineSubsystem( GraphicsSubsystem );
		gfx->AddWindow( this );

		World* world = GetWorld( );
		GraphicsScene* scene = world->GetContext< GraphicsSubsystemContext >( )->GetGraphicsScene( );

		// Need to create an external scene camera held in the viewport that can manipulate the scene view
		Camera* cam = scene->GetActiveCamera( );
		cam->SetNearFar( 0.1f, 1000.0f );
		cam->SetProjection( ProjectionType::Perspective );
		cam->SetPosition( Vec3( 0.0f, 0.0f, -3.0f ) );

		mRenderable.SetMesh( EngineSubsystem( AssetManager )->GetAsset< Mesh >( "models.unit_sphere" ) );
		mRenderable.SetPosition( cam->GetPosition() + cam->Forward() * 5.0f );
		mRenderable.SetScale( 2.0f );
		mRenderable.SetMaterial( mMaterial );
		scene->AddStaticMeshRenderable( &mRenderable ); 

		guiContext->RegisterWindow( "Properties", [ & ]
		{
			if ( ImGui::BeginDock( "Properties" ) )
			{
				if ( mMaterial )
				{
					ImGui::Text( "%s", Utils::format( "Material: %s", mMaterial.Get()->GetName().c_str() ).c_str( ) ); 
					ImGuiManager* igm = EngineSubsystem( ImGuiManager );
					igm->InspectObject( mMaterial.Get() ); 
				}

				ImGui::EndDock( );
			}
		} );

		auto saveMaterialOption = [ & ] ( )
		{ 
			if ( ImGui::MenuItem( "Save##save_mat_options", NULL ) )
			{
				if ( mMaterial )
				{
					mMaterial->Save( );
				} 
			}
		};

		// Register menu options
		guiContext->RegisterMenuOption("File", "Save##save_material_options", saveMaterialOption); 

		guiContext->RegisterDockingLayout( GUIDockingLayout( "Viewport", nullptr, GUIDockSlotType::Slot_Tab, 1.0f ) );
		guiContext->RegisterDockingLayout( GUIDockingLayout( "Properties", "Viewport", GUIDockSlotType::Slot_Left, 0.45f ) );
		guiContext->Finalize( );
	} 

	void EditorTextureEditWindow::Init( const WindowParams& params )
	{ 
		// Construct scene in world
		if ( !mTexture )
		{
			// Initialize new world 
			mWorld = new World( );
			// Register contexts with world
			mWorld->RegisterContext< GraphicsSubsystemContext >( );
			// Set material from data
			mTexture = ( Texture* )( params.mData );

			ConstructScene( );
		} 
	} 

	void EditorTextureEditWindow::ConstructScene( )
	{
		GUIContext* guiContext = GetGUIContext( );

		// Add main menu options
		guiContext->RegisterMainMenu( "File" );
		guiContext->RegisterMainMenu( "View" );

		// Create viewport
		//mViewport = new EditorViewport( Engine::GetInstance( )->GetApplication( )->ConstCast< EditorApp >( ), this );

		//guiContext->RegisterDockingLayout( GUIDockingLayout( mViewport->GetViewName( ).c_str( ), nullptr, GUIDockSlotType::Slot_Tab, 1.0f ) );

		// NOTE(): This should be done automatically for the user in the backend
		// Add window to graphics subsystem ( totally stupid way to do this )
		//GraphicsSubsystem* gfx = EngineSubsystem( GraphicsSubsystem );
		//gfx->AddWindow( this );

		//World* world = GetWorld( );
		//GraphicsScene* scene = world->GetContext< GraphicsSubsystemContext >( )->GetGraphicsScene( );

		// Need to create an external scene camera held in the viewport that can manipulate the scene view
		//Camera* cam = scene->GetActiveCamera( );
		//cam->SetNearFar( 0.1f, 1000.0f );
		//cam->SetProjection( ProjectionType::Perspective );
		//cam->SetRotation( Vec3( 180.f, 0.f, 180.f ) );
		//cam->SetPosition( Vec3( 1.0f, 1.0f, -2.5f ) );

		//mMaterial = new Material( ); 
		//mMaterial->SetShaderGraph( EngineSubsystem( AssetManager )->GetAsset< ShaderGraph >( "shaders.shadergraphs.defaultstaticgeom" ) );
		//mMaterial->SetUniform( "albedoMap", mTexture );
		//mMaterial->SetUniform( "metallicMap", EngineSubsystem( AssetManager )->GetAsset< Texture >( "textures.black" ) );
		//mMaterial->SetUniform( "roughMap", EngineSubsystem( AssetManager )->GetAsset< Texture >( "textures.white" ) );
		//mMaterial->SetUniform( "emissiveMap", EngineSubsystem( AssetManager )->GetAsset< Texture >( "textures.black" ) );
		//mMaterial->SetUniform( "aoMap", EngineSubsystem( AssetManager )->GetAsset< Texture >( "textures.white" ) );
		//mMaterial->SetUniform( "normalMap", EngineSubsystem( AssetManager )->GetAsset< Texture >( "textures.blue" ) );

		// Set as quad
		// Need to set material of it
		//mRenderable.SetMesh( EngineSubsystem( AssetManager )->GetDefaultAsset< Mesh >( ) );
		////mRenderable.SetPosition( cam->GetPosition( ) + cam->Forward( ) * 5.0f );
		//mRenderable.SetScale( 2.0f );
		//mRenderable.SetMaterial( mMaterial );
		//scene->AddStaticMeshRenderable( &mRenderable );

		guiContext->RegisterWindow( "Texture", [ & ] ( )
		{
			if ( ImGui::BeginDock( "Texture" ) )
			{
				if ( mTexture )
				{
					ImTextureID texId = Int2VoidP( mTexture->GetTextureId( ) );
					ImVec2 a = ImVec2( ImGui::GetWindowPos() + ImGui::GetWindowSize() / 2.f );
					ImVec2 b = a + Vec2( mTexture->GetWidth(), mTexture->GetHeight() );
					ImDrawList* dl = ImGui::GetWindowDrawList( );
					dl->AddImage( texId, a, b );
				}
			}
			ImGui::EndDock( );
		});

		guiContext->RegisterWindow( "Properties", [ & ]
		{
			if ( ImGui::BeginDock( "Properties" ) )
			{
				if ( mTexture )
				{

					//World* world = GetWorld( );
					//GraphicsScene* scene = world->GetContext< GraphicsSubsystemContext >( )->GetGraphicsScene( );
					ImGui::Text( "%s", Utils::format( "Texture: %s", mTexture.Get( )->GetName().c_str() ).c_str( ) );
					ImGuiManager* igm = EngineSubsystem( ImGuiManager );
					igm->InspectObject( mTexture.Get() ); 
					//igm->InspectObject( mMaterial );
					//igm->InspectObject( &mRenderable );
					//igm->InspectObject( scene->GetActiveCamera() ); 
				} 
			} 
			ImGui::EndDock( );
		} );

		auto saveTextureOption = [ & ] ( )
		{
			if ( ImGui::MenuItem( "Save##save_tex_options", NULL ) )
			{
				if ( mTexture )
				{
					//mTexture->Save( );
				}
			}
		};

		// Register menu options
		guiContext->RegisterMenuOption( "File", "Save##save_tex_options", saveTextureOption );

		guiContext->RegisterDockingLayout( GUIDockingLayout( "Texture", nullptr, GUIDockSlotType::Slot_Tab, 1.0f ) );
		guiContext->RegisterDockingLayout( GUIDockingLayout( "Properties", "Texture", GUIDockSlotType::Slot_Left, 0.45f ) );
		guiContext->SetActiveDock( "Texture" );
		guiContext->Finalize( );
	}

	//=================================================================================================

	void EditorGenericAssetEditWindow::Init( const WindowParams& params )
	{
		// Construct scene in world
		if ( !mAsset )
		{
			// Initialize new world 
			mWorld = new World( );
			// Register contexts with world
			mWorld->RegisterContext< GraphicsSubsystemContext >( );
			// Set asset from data 
			mAsset = ( Asset* )( params.mData );
			// Construct Scene 
			ConstructScene( );
		} 
	}

	//================================================================================================= 

	void EditorGenericAssetEditWindow::ConstructScene()
	{
		GUIContext* guiContext = GetGUIContext( );

		// Add main menu options
		guiContext->RegisterMainMenu( "File" ); 

		World* world = GetWorld( ); 

		guiContext->RegisterWindow( "Properties", [ & ]
		{
			if ( ImGui::BeginDock( "Properties" ) )
			{
				ImGui::ListBoxHeader( "##asset_props", ImVec2( ImGui::GetWindowWidth() - 20.f, ImGui::GetWindowHeight() - 15.f ) );
				{
					if ( mAsset )
					{ 
						World* world = GetWorld( );
						ImGui::Text( "     Asset:" ); ImGui::SameLine(); ImGui::SetCursorPosX( ImGui::GetWindowWidth() * 0.4f );
						ImGui::Text( "%s", Utils::format( "%s", mAsset.Get( )->GetName().c_str() ).c_str( ) );
						ImGuiManager* igm = EngineSubsystem( ImGuiManager );
						igm->InspectObject( mAsset.Get() ); 
					} 
				}
				ImGui::ListBoxFooter();
				ImGui::EndDock( );
			}
		} );

		auto saveAssetOption = [ & ] ( )
		{
			if ( ImGui::MenuItem( "Save##save_asset_option", NULL ) )
			{
				if ( mAsset )
				{
					mAsset->Save();
				}
			}
		};

		// Register menu options
		guiContext->RegisterMenuOption( "File", "Save##save_asset_option", saveAssetOption ); 
		guiContext->RegisterDockingLayout( GUIDockingLayout( "Properties", nullptr, GUIDockSlotType::Slot_Tab, 0.45f ) );
		guiContext->SetActiveDock( "Properties" );
		guiContext->Finalize( ); 
	}

	//=================================================================================================
}
