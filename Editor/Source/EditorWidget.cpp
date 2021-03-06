// @file EditorWidget.cpp
// Copyright 2016-2017 John Jackson. All Rights Reserved.

#include "EditorWidget.h"
#include "EditorTransformWidget.h"

#include <Engine.h>
#include <SubsystemCatalog.h>
#include <Graphics/GraphicsSubsystem.h>

namespace Enjon
{
	void EditorWidget::Enable( )
	{
		// Add renderables to graphics scene
		GraphicsSubsystem* gfx = EngineSubsystem( GraphicsSubsystem );

		for ( auto& t : mTransformHeirarchies )
		{
			gfx->GetGraphicsScene( )->AddNonDepthTestedStaticMeshRenderable( &t->mRenderable );
		} 
	}

	void EditorWidget::Disable( )
	{
		// Remove renderables from graphics scene
		GraphicsSubsystem* gfx = EngineSubsystem( GraphicsSubsystem );

		for ( auto& t : mTransformHeirarchies )
		{
			gfx->GetGraphicsScene( )->RemoveNonDepthTestedStaticMeshRenderable( &t->mRenderable );
		} 
	}

	void EditorWidget::Update( )
	{
		GraphicsSubsystem* gfx = EngineSubsystem( GraphicsSubsystem );
		const Camera* cam = gfx->GetGraphicsSceneCamera( ); 

		// Set scale of root based on distance from camera
		f32 dist = Vec3::Distance( cam->GetPosition( ), mRootHeirarchy->mLocalTransform.GetPosition() );
		f32 scale = cam->GetProjectionType() == ProjectionType::Perspective ?  Math::Clamp( dist / 60.0f, 0.001f, 100.0f ) : Math::Max( cam->GetOrthographicScale(), 1.f ) / 40.f;
		mRootHeirarchy->mLocalTransform.SetScale( scale ); 

		// Calculate world transforms for all heirarchies
		for ( auto& t : mTransformHeirarchies )
		{
			t->CalculateWorldTransform( );
		}

		// Set world transform for renderable
		for ( auto& t : mTransformHeirarchies )
		{
			t->mRenderable.SetTransform( t->mWorldTransform );
		} 
	} 

	Transform EditorWidget::GetWorldTransform( )
	{
		Update( );
		return mRootHeirarchy->mWorldTransform;
	}

	void EditorWidget::SetPosition( const Vec3& position )
	{
		// Set position and update everything else
		mRootHeirarchy->mLocalTransform.SetPosition( position );
		Update( );
	}

	// Sets root scale
	void EditorWidget::SetScale( const f32& scale )
	{
		mRootHeirarchy->mLocalTransform.SetScale( scale );
		Update( );
	}

	void EditorWidget::SetRotation( const Quaternion& rotation )
	{
		mRootHeirarchy->mLocalTransform.SetRotation( rotation );
		Update( );
	} 
 
	void EditorWidget::SetRotation( const Vec3& eulerAngles )
	{
		mRootHeirarchy->mLocalTransform.SetEulerRotation( eulerAngles );
		Update( );
	}

	void EditorWidget::SetTransform( const Transform& transform )
	{
		mRootHeirarchy->mLocalTransform = transform;
		Update( );
	}
}
