// File: EditorAssetBrowserView.cpp
// Copyright 2016-2018 John Jackson. All Rights Reserved.

#include "EditorAssetBrowserView.h"
#include "EditorInspectorView.h"
#include "EditorApp.h"
#include "EditorMaterialEditWindow.h"
#include "EditorArchetypeEditWindow.h" 
#include "EditorUIEditWindow.h"
#include "Project.h" 

#include <Engine.h>
#include <SubsystemCatalog.h>
#include <ImGui/ImGuiManager.h>
#include <IO/InputManager.h>
#include <Asset/AssetManager.h>
#include <GUI/UIAsset.h>
#include <Graphics/GraphicsSubsystem.h>
#include <Scene/SceneManager.h>
#include <Graphics/Window.h>
#include <Utils/FileUtils.h>
#include <Engine.h>
#include <Utils/FileUtils.h> 
#include <SubsystemCatalog.h>

#include <fs/filesystem.hpp>
 
namespace FS = ghc::filesystem; 

namespace Enjon
{
	//=========================================================================

	EditorAssetBrowserView::EditorAssetBrowserView( EditorApp* app, Window* window )
		: EditorView( app, window, "Asset Browser", ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse )
	{ 
		Initialize( );
	}

	//=========================================================================

	void EditorAssetBrowserView::InitializeCurrentDirectory( Project* project )
	{ 
		// If not empty, then return
		if ( mCurrentDirectory.compare( "" ) != 0 )
		{
			return;
		}

		// If application is valid, set project path
		Application* app = project->GetApplication( );
		if ( app )
		{
			// Set current directory to project's asset directory
			mCurrentDirectory = FS::path(project->GetProjectPath( ) + "Assets").string();
			// Set root directory to be the root
			mRootDirectory = mCurrentDirectory;
		} 
	}

	//=========================================================================

	void EditorAssetBrowserView::AttemptDirectoryBackTraversal( )
	{
		// Can go backwards iff not the root directory
		FS::path current = FS::path( mCurrentDirectory );
		FS::path root = FS::path( mRootDirectory );
		if ( current == root )
		{
			return;
		}

		// Set to parent path of directory
		mCurrentDirectory = FS::path( mCurrentDirectory ).parent_path().string();
	}

	//=========================================================================

	const Asset* EditorAssetBrowserView::GetSelectedAsset( )
	{
		if ( mSelectedAssetInfo )
		{
			// Load asset and return it
			return mSelectedAssetInfo->GetAsset( );
		}

		return nullptr;
	}

	//=========================================================================

	void EditorAssetBrowserView::SetSelectedPath( const String& path )
	{
		if ( !FS::is_directory( path ) )
		{ 
			// Want to be able to inspect this asset
			AssetRecordInfo* info =  EngineSubsystem( AssetManager )->GetAssetRecordFromFilePath( Utils::FindReplaceAll( path, "\\", "/" ) );
			//AssetHandle< Asset > asset = EngineSubsystem( AssetManager )->GetAssetFromFilePath( Utils::FindReplaceAll( path, "\\", "/" ) ); 

			// If asset, then set to be inspected...God, this will fall apart so fucking fast...
			if ( info )
			{ 
				// Set selected asset
				mSelectedAssetInfo = info;
			}
			else
			{ 
				// Set selected asset to be null
				mSelectedAssetInfo = nullptr;
			}
		}
		else
		{ 
			// Set selected asset to be null
			mSelectedAssetInfo = nullptr; 
		}

		// Set selected path
		mSelectedPath = path;
	}

	//=========================================================================

	void EditorAssetBrowserView::PushActivePopupWindow( PopupWindow* window )
	{
		mActivePopupWindow = window;
	}

	//=========================================================================

	bool EditorAssetBrowserView::ActivePopupWindowEnabled( )
	{
		if ( mActivePopupWindow == nullptr )
		{
			return false;
		}

		return mActivePopupWindow->Enabled( ); 
	}

	//=========================================================================

	void EditorAssetBrowserView::UpdateView( )
	{ 
		// Release any previously grabbed assets that need release
		if ( mNeedToReleaseAssetNextFrame )
		{
			ReleaseGrabbedAsset( );
		}
 
		// Get input system
		Input* input = EngineSubsystem( Input );

		// Iterate through directory structure of application 
		Project* project = mApp->GetProject( ); 

		if ( project && project->GetApplication() )
		{
			// Attempt to initialize current directory if not already ( NOTE(): hate this )
			InitializeCurrentDirectory( project );

			Application* app = project->GetApplication( );

			// Add new button
			if ( ImGui::Button( "+ Add New" ) )
			{
				mFolderMenuPopup.Activate( input->GetMouseCoords( ) );
			} 

			ImGui::SameLine( );

			// Go back a directory...So fancy
			if ( ImGui::Button( "<-" ) )
			{
				AttemptDirectoryBackTraversal( );
			}

			ImGui::SameLine( );

			String curDir = Utils::SplitString( Utils::FindReplaceAll( mCurrentDirectory, "Assets", "!" ), "!" ).back( );
			curDir = Utils::FindReplaceAll( curDir, "\\", "/" );

			// Display the current directory path
			ImGui::Text( "%s", Utils::format( "Current Dir: Assets%s", curDir.c_str() ).c_str( ) );

			// Separator for formatting
			ImGui::Separator( );

			// Iterate the current directory
			Vec2 padding( 20.0f, 8.0f );
			f32 height = ImGui::GetWindowSize( ).y - ImGui::GetCursorPosY( ) - padding.y;
			ImVec2 listBoxMin = ImGui::GetCursorScreenPos( );
			ImVec2 listBoxSize = ImVec2( ImGui::GetWindowWidth( ) - 20.0f, height );
			bool hoveringItem = false;
			ImGui::ListBoxHeader( "##AssetDirectory", listBoxSize );
			{
				for ( auto& p : FS::directory_iterator( mCurrentDirectory ) )
				{
					bool isDir = FS::is_directory( p );
					bool isSelected = p == FS::path( mSelectedPath ); 

					String pathLabel = Utils::SplitString( p.path( ).string( ), "\\" ).back( );
					pathLabel = Utils::SplitString( pathLabel, "/" ).back();
					ImColor headerHovered = ImColor( ImGui::GetColorU32( ImGuiCol_HeaderHovered ) );
					ImColor textColor = ImColor( ImGui::GetColorU32( ImGuiCol_Text ) );

					// Get file extension
					String fileExtension = isDir ? "" : Utils::SplitString( pathLabel, "." ).back( );

					// Select directory
					ImColor finalColor = isDir ? isSelected ? textColor : headerHovered : textColor ;

					ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( finalColor ) );
					if ( mPathNeedsRename && p == FS::path( mSelectedPath ) )
					{
						ImGui::SetKeyboardFocusHere( -1 );

						char buffer[ 256 ];
						strncpy( buffer, isDir ? pathLabel.c_str( ) : Utils::SplitString( pathLabel, "." ).front( ).c_str( ), 256 );
						if ( ImGui::InputText( "##pathRename", buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll ) )
						{
							pathLabel = buffer;

							String newPath = "";
							Vector<String> splits = Utils::SplitString( p.path( ).string( ), "\\" );
							for ( u32 i = 0; i < splits.size( ) - 1; ++i )
							{
								newPath += splits.at( i );
								newPath += i == splits.size( ) - 2 ? "" : "/";
							}

							// Final path to be created
							String finalPath = isDir ? newPath + "/" + pathLabel : newPath + "/" + pathLabel + "." + fileExtension; 
 
							// If path doesn't exist, then rename this path
							if ( !FS::exists( finalPath ) )
							{ 
								// If not a directory, then rename the asset
								if ( !isDir )
								{
									// Try to get selected asset info from this original selected path
									if ( !mSelectedAssetInfo )
									{ 
										mSelectedAssetInfo = EngineSubsystem( AssetManager )->GetAssetRecordFromFilePath( Utils::FindReplaceAll( mSelectedPath, "\\", "/" ) );
									}

									// Can rename
									if ( mSelectedAssetInfo )
									{
										// Use asset manager to set filepath of selected asset
										EngineSubsystem( AssetManager )->RenameAssetFilePath( mSelectedAssetInfo->GetAsset(), finalPath ); 

										// Rename the filepath
										FS::remove( p ); 

									}
								}
								else
								{
									// Rename the filepath
									FS::rename( p, finalPath ); 
								}

							}

							mPathNeedsRename = false;
						}

						if ( input->IsKeyPressed( KeyCode::LeftMouseButton ) && !ImGui::IsItemHovered( ) )
						{
							mPathNeedsRename = false;
						}
					} 
					else if ( ImGui::Selectable( pathLabel.c_str( ), isSelected ) )
					{
						SetSelectedPath( p.path( ).string( ) );
					}
					ImGui::PopStyleColor( );

					if ( ImGui::IsItemHovered( ) )
					{
						hoveringItem = true;

						if ( ImGui::IsMouseDoubleClicked( 0 ) )
						{
							if ( FS::is_directory( p ) )
							{
								mCurrentDirectory = p.path( ).string( ); 
							} 
							else if ( mSelectedAssetInfo )
							{
								const MetaClass* assetCls = mSelectedAssetInfo->GetAssetClass( );

								if ( assetCls )
								{
									// Open up new window
									if ( assetCls->InstanceOf< Material >( ) )
									{
										// Load asset
										const Asset* mat = mSelectedAssetInfo->GetAsset( ); 

										// Open new editor window for this material
										WindowParams params;
										params.mMetaClassFunc = [&]() -> const MetaClass * { return Object::GetClass< EditorMaterialEditWindow >(); };
										params.mName = mat->GetName( );
										params.mWidth = 800;
										params.mHeight = 400;
										params.mFlags = WindowFlagsMask( ( u32 )WindowFlags::RESIZABLE );
										params.mData = (void*)mat;
										EngineSubsystem( WindowSubsystem )->AddNewWindow( params );
									} 
									else if ( assetCls->InstanceOf< Scene >( ) )
									{
										// Load scene from asset
										EngineSubsystem( SceneManager )->LoadScene( mSelectedAssetInfo->GetAssetUUID( ) );
									} 
									else if ( assetCls->InstanceOf< Archetype >( ) )
									{
										// Load archetype window
										const Asset* archType = mSelectedAssetInfo->GetAsset( );

										// Open new edit window for this archetype
										WindowParams params;
										params.mMetaClassFunc = [&]() -> const MetaClass * { return Object::GetClass< EditorArchetypeEditWindow >(); };
										params.mName = archType->GetName( );
										params.mWidth = 1200;
										params.mHeight = 800;
										params.mFlags = WindowFlagsMask( ( u32 )WindowFlags::RESIZABLE );
										params.mData = ( void* )archType;
										EngineSubsystem( WindowSubsystem )->AddNewWindow( params );
									}
									else if ( assetCls->InstanceOf< Texture >( ) )
									{
										const Asset* asset = mSelectedAssetInfo->GetAsset( );

										printf( "Opening texture window...\n" );

										// Open new params
										WindowParams params;
										params.mMetaClassFunc = [&]() -> const MetaClass * { return Object::GetClass< EditorTextureEditWindow >(); };
										params.mName = asset->GetName( );
										params.mWidth = 1200;
										params.mHeight = 800;
										params.mFlags = WindowFlagsMask( ( u32 )WindowFlags::RESIZABLE );
										params.mData = ( void* )asset;
										EngineSubsystem( WindowSubsystem )->AddNewWindow( params );
									}
									else if ( assetCls->InstanceOf< UIStyleSheet >( ) )
									{
										const Asset* asset = mSelectedAssetInfo->GetAsset();
										// Open new params
										WindowParams params;
										params.mMetaClassFunc = [ & ] () -> const MetaClass * { return Object::GetClass< EditorUIStyleSheetEditWindow >(); };
										params.mName = asset->GetName();
										params.mWidth = 800;
										params.mHeight = 400;
										params.mFlags = WindowFlagsMask( (u32 )WindowFlags::RESIZABLE );
										params.mData = ( void* )asset;
										EngineSubsystem( WindowSubsystem )->AddNewWindow( params ); 

									} 
									else if ( 
										assetCls->InstanceOf< UIStyleConfig >() 
									)
									{
										const Asset* asset = mSelectedAssetInfo->GetAsset();

										// Open new params
										WindowParams params;
										params.mMetaClassFunc = [ & ] () -> const MetaClass * { return Object::GetClass< EditorGenericAssetEditWindow >(); };
										params.mName = asset->GetName();
										params.mWidth = 800;
										params.mHeight = 400;
										params.mFlags = WindowFlagsMask( (u32 )WindowFlags::RESIZABLE );
										params.mData = ( void* )asset;
										EngineSubsystem( WindowSubsystem )->AddNewWindow( params ); 
									}
									else if ( assetCls->InstanceOf< UI >() )
									{
										const Asset* asset = mSelectedAssetInfo->GetAsset();

										// Open new params
										{
											WindowParams params;
											params.mMetaClassFunc = [ & ] () -> const MetaClass * { return Object::GetClass< EditorUIEditWindow >(); };
											params.mName = asset->GetName();
											params.mWidth = 16 * 70;
											params.mHeight = 9 * 70;
											params.mFlags = WindowFlagsMask( (u32)WindowFlags::RESIZABLE );
											params.mData = ( void* )asset;
											EngineSubsystem( WindowSubsystem )->AddNewWindow( params ); 
										}

										{
											WindowParams params;
											params.mMetaClassFunc = [ & ] () -> const MetaClass * { return Object::GetClass< EditorUICanvasWindow >(); };
											params.mName = asset->GetName() + ": Canvas";
											params.mWidth = 16 * 40;
											params.mHeight = 9 * 40;
											params.mFlags = WindowFlagsMask( (u32)WindowFlags::RESIZABLE | (u32)WindowFlags::INVISIBLE );
											params.mData = ( void* )asset;
											EngineSubsystem( WindowSubsystem )->AddNewWindow( params ); 
										}
									}
								}
							}
						} 

						if ( input->IsKeyPressed( KeyCode::RightMouseButton ) )
						{
							SetSelectedPath( p.path( ).string( ) );
						}
 
						if ( ImGui::IsMouseDown( 0 ) )
						{ 
							AssetRecordInfo* info =  EngineSubsystem( AssetManager )->GetAssetRecordFromFilePath( Utils::FindReplaceAll( p.path().string(), "\\", "/" ) );

							if ( info )
							{
								if ( !mMouseHeld )
								{
									mHeldMousePosition = input->GetMouseCoords( );
									mMouseHeld = true;
								}
								else if ( mMouseHeld && !mGrabbedAsset )
								{
									if ( mHeldMousePosition.Distance(input->GetMouseCoords()) >= 5.0f )
									{ 
										mGrabbedAsset = info->GetAsset( );
									} 
								} 
							}
						}
					}
				} 
			} 

			// If the list box is hovered
			if ( ImGui::IsMouseHoveringRect( listBoxMin, ImVec2( listBoxMin.x + listBoxSize.x, listBoxMin.y + listBoxSize.y ) ) )
			{
				// Set active context menu and cache mouse coordinates
				if ( input->IsKeyPressed( KeyCode::RightMouseButton ) )
				{
					if ( !hoveringItem )
					{
						mFolderMenuPopup.Activate( input->GetMouseCoords( ) );
						PushActivePopupWindow( &mFolderMenuPopup );
					}
					else
					{
						if ( FS::is_directory( mSelectedPath ) )
						{
							mFolderOptionsMenuPopup.Activate( input->GetMouseCoords( ) ); 
							PushActivePopupWindow( &mFolderOptionsMenuPopup );
						} 
					}
				} 
			}

			// TODO(): Input states are starting to get wonky - need to unify through one central area
			if ( 
					( ( IsHovered() && 
					!hoveringItem && 
					!mPathNeedsRename && 
					!ActivePopupWindowEnabled() ) || ( !IsHovered() && !ActivePopupWindowEnabled() ) ) && 
					( input->IsKeyPressed(KeyCode::LeftMouseButton ) || input->IsKeyPressed( KeyCode::RightMouseButton ) ) 
				)
			{
				SetSelectedPath( "" );
			} 
 
			// Check for dropped files
			AssetManager* am = EngineSubsystem( AssetManager );
			auto window = EngineSubsystem( GraphicsSubsystem )->GetWindow( )->ConstCast< Window >();
			auto dropFiles = window->GetDroppedFiles( );

			// Only process newly dropped files if we're not currently loading the previously dropped files
			if ( dropFiles.size( ) && !mCurrentAssetLoader )
			{
				std::cout << "Dropped files: \n";
				for ( auto& f : dropFiles )
				{
					// Have to make sure doesn't exit already in asset manager before inserting
					if ( !am->AssetExists( f, mCurrentDirectory ) )
					{
						// Push back file 
						mFilesToImport.insert( f ); 
					} 
				}
			}

			ImGui::ListBoxFooter( ); 
		} 

		// Draw window around mouse for grabbed asset
		if ( mGrabbedAsset )
		{
			HandleDraggingGrabbedAsset( ); 
		}

		// Check for any file drops that have occurred
		ProcessFileDrops( );

		// Do active popup window
		if ( ActivePopupWindowEnabled( ) )
		{
			mActivePopupWindow->DoWidget( );
		} 

		if ( mSelectedPath.compare( "" ) != 0 && input->IsKeyPressed( KeyCode::F2 ) )
		{
			mPathNeedsRename = true;
		}

		if ( input->IsKeyReleased( KeyCode::LeftMouseButton ) )
		{
			mMouseHeld = false;
		}

	}

	//=========================================================================

	const Asset* EditorAssetBrowserView::GetGrabbedAsset( )
	{
		return mGrabbedAsset;
	} 

	//=========================================================================

	void EditorAssetBrowserView::HandleDraggingGrabbedAsset( )
	{
		Input* input = EngineSubsystem( Input );

		if ( !mGrabbedAsset )
		{
			PrepareReleaseGrabbedAsset( );
			return;
		} 

		// Release grabbed asset
		if ( input->IsKeyReleased( KeyCode::LeftMouseButton ) )
		{
			PrepareReleaseGrabbedAsset( );
			return;
		}
	} 

	//=========================================================================

	void EditorAssetBrowserView::PrepareReleaseGrabbedAsset( )
	{
		mNeedToReleaseAssetNextFrame = true;
	}

	//=========================================================================

	void EditorAssetBrowserView::ReleaseGrabbedAsset( )
	{
		mNeedToReleaseAssetNextFrame = false;
		mMouseHeld = false;
		mGrabbedAsset = nullptr;
	}

	void EditorAssetBrowserView::ProcessFileDrops( )
	{
		if ( !mFilesToImport.empty() )
		{
			// If a current asset loader hasn't been processed yet
			if ( mCurrentAssetLoader == nullptr )
			{
				// Get next file to pop off
				auto elem = mFilesToImport.begin( );
				String filePath = *elem;
				mFilesToImport.erase( elem );

				// Grab asset loader
				AssetManager* am = EngineSubsystem( AssetManager );
				mCurrentAssetLoader = am->GetLoaderByResourceFilePath( filePath )->ConstCast< AssetLoader >(); 

				// Begin the import process using the loader
				if ( mCurrentAssetLoader && !mCurrentAssetLoader->IsImporting( ) )
				{
					mCurrentAssetLoader->BeginImport( filePath, mCurrentDirectory );
				}
			}
		}

		// Reset current asset loader if done with importing previous file
		if ( mCurrentAssetLoader )
		{
			// Done, so reset to nullptr
			if ( !mCurrentAssetLoader->IsImporting( ) )
			{
				mCurrentAssetLoader = nullptr; 
			}
			else
			{
				mCurrentAssetLoader->ConstCast< AssetLoader >( )->GetImportOptions( )->ConstCast< ImportOptions >( )->OnEditorUI( );
			}
		}
	}

	//=========================================================================

	void EditorAssetBrowserView::ProcessViewInput( )
	{ 
	}

	//=========================================================================

	void EditorAssetBrowserView::FolderOptionsMenuPopup( )
	{
		Input* input = EngineSubsystem( Input );
		ImGuiManager* igm = EngineSubsystem( ImGuiManager );

		// Folder option group
		ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( ImGui::GetStyle( ).Colors[ImGuiCol_TextDisabled] ) );
		ImGui::PushFont( igm->GetFont( "WeblySleek_10" ) );
		{
			ImGui::Text( "Folder" );
		}
		ImGui::PopFont( ); 
		ImGui::PopStyleColor( );

		// Construct new folder option
		if ( ImGui::Selectable( "\tDelete" ) )
		{ 
			// Attempt to create the directory
			FS::remove_all( mSelectedPath );

			mFolderOptionsMenuPopup.Deactivate( ); 
		} 

		// Construct new folder option
		if ( ImGui::Selectable( "\tRename" ) )
		{ 
			mPathNeedsRename = true; 

			mFolderOptionsMenuPopup.Deactivate( ); 
		} 

		if ( input->IsKeyDown( KeyCode::LeftMouseButton ) && !mFolderOptionsMenuPopup.Hovered( ) )
		{
			mFolderOptionsMenuPopup.Deactivate( );
		}

		// Separator at end of folder menu option
		ImGui::Separator( ); 
	}

	//=========================================================================

	void EditorAssetBrowserView::NewFolderMenuOption( )
	{
		Input* input = EngineSubsystem( Input );
		ImGuiManager* igm = EngineSubsystem( ImGuiManager );

		// Folder option group
		ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( ImGui::GetStyle( ).Colors[ImGuiCol_TextDisabled] ) );
		ImGui::PushFont( igm->GetFont( "WeblySleek_10" ) );
		{
			ImGui::Text( "Folder" );
		}
		ImGui::PopFont( ); 
		ImGui::PopStyleColor( );

		// Construct new folder option
		if ( ImGui::Selectable( "\t+ New Folder" ) )
		{ 
			// Starting folder name
			String folderName = "NewFolder";
			String dir = mCurrentDirectory + "\\NewFolder";

			// Continue to try and create new directory
			u32 index = 0;
			while ( FS::exists( dir ) )
			{
				index++;
				dir = mCurrentDirectory + "\\NewFolder" + std::to_string( index );
			}

			// Attempt to create the directory
			FS::create_directory( dir );

			// Select the path
			SetSelectedPath( dir );

			// Set to needing rename
			mPathNeedsRename = true; 

			mFolderMenuPopup.Deactivate( ); 
		} 

		if ( input->IsKeyDown( KeyCode::LeftMouseButton ) && !mFolderMenuPopup.Hovered( ) )
		{
			mFolderMenuPopup.Deactivate( );
		}

		// Separator at end of folder menu option
		ImGui::Separator( );
	}

	//=========================================================================

	void EditorAssetBrowserView::CreateMenuOption( )
	{
		Input* input = EngineSubsystem( Input );
		ImGuiManager* igm = EngineSubsystem( ImGuiManager );
		AssetManager* am = EngineSubsystem( AssetManager );

		// Folder option group
		ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( ImGui::GetStyle( ).Colors[ImGuiCol_TextDisabled] ) );
		ImGui::PushFont( igm->GetFont( "WeblySleek_10" ) );
		{
			ImGui::Text( "Create Assets" );
		}
		ImGui::PopFont( ); 
		ImGui::PopStyleColor( );

		auto FinishAssetConstruction = [ & ] ( const Asset* asset )
		{
			// Select the path
			SetSelectedPath( asset->GetAssetRecordInfo( )->GetAssetFilePath( ) );

			// Set needing to rename
			mPathNeedsRename = true;

			mFolderMenuPopup.Deactivate( ); 
		};

		// Construct material option
		if ( ImGui::Selectable( "\t+ Material" ) )
		{ 
			// Construct asset with current directory
			AssetHandle< Material > mat = am->ConstructAsset< Material >( "NewMaterial", mCurrentDirectory );
			FinishAssetConstruction( mat.Get( ) ); 
		} 

		if ( ImGui::Selectable( "\t+ UI" ) )
		{
			// Construct asset with current directory
			AssetHandle< UI > ui = am->ConstructAsset< UI >( "NewUI", mCurrentDirectory );
			FinishAssetConstruction( ui.Get() ); 
		}

		if ( ImGui::Selectable( "\t+ UI Style Config" ) )
		{
			// Construct asset with current directory
			AssetHandle< UIStyleConfig > ui = am->ConstructAsset< UIStyleConfig >( "NewUIStyleConfig", mCurrentDirectory );
			FinishAssetConstruction( ui.Get( ) ); 
		}

		if ( ImGui::Selectable( "\t+ UI Style Sheet" ) )
		{
			// Construct asset with current directory
			AssetHandle< UIStyleSheet > ss = am->ConstructAsset< UIStyleSheet >( "NewUIStyleSheet", mCurrentDirectory ); 
			FinishAssetConstruction( ss.Get( ) ); 
		}

		// Construct scene option
		if ( ImGui::Selectable( "\t+ Scene" ) )
		{ 
			// Construct asset with current directory
			AssetHandle< Scene > scene = am->ConstructAsset< Scene >( "NewScene", mCurrentDirectory );
			FinishAssetConstruction( scene.Get( ) );
		} 

		// Construct scene option
		if ( ImGui::Selectable( "\t+ Archetype" ) )
		{ 
			// Construct asset with current directory
			AssetHandle< Archetype > archetype = am->ConstructAsset< Archetype >( "NewArchetype", mCurrentDirectory );
			FinishAssetConstruction( archetype.Get( ) );
		} 

		// Separator at end of folder menu option
		ImGui::Separator( );
	}

	void EditorAssetBrowserView::FolderMenuPopup( ) 
	{ 
		Input* input = EngineSubsystem( Input );
		ImGuiManager* igm = EngineSubsystem( ImGuiManager );

		// Close folder popup menu
		if ( input->IsKeyPressed( KeyCode::LeftMouseButton ) && !mFolderMenuPopup.Hovered() )
		{
			mFolderMenuPopup.Deactivate( );
		} 

		// New folder option
		NewFolderMenuOption( ); 

		// Create menu option
		CreateMenuOption( );
	}

	void EditorAssetBrowserView::Initialize( )
	{ 
		mFolderMenuPopup.SetSize( Vec2( 200.0f, 400.0f ) );
		mFolderOptionsMenuPopup.SetSize( Vec2( 200.0f, 400.0f ) );

		// Set up folder menu popup 
		mFolderMenuPopup.RegisterCallback( [&]( )
		{ 
			// Do folder menu popup function
			FolderMenuPopup( );
		} );

		// Set up folder options menu popup
		mFolderOptionsMenuPopup.RegisterCallback( [ & ] ( )
		{
			FolderOptionsMenuPopup( );
		} );
	} 

	//=========================================================================
}

