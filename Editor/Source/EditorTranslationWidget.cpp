// @file EditorTranslationWidget.cpp
// Copyright 2016-2017 John Jackson. All Rights Reserved.

#include <Engine.h>
#include <Asset/AssetManager.h>
#include <Graphics/GraphicsSubsystem.h>
#include <SubsystemCatalog.h>

#include "EditorTranslationWidget.h"

namespace Enjon
{
	void EditorTranslationWidget::Initialize( EditorTransformWidget* owner )
	{ 
		// Set up renderables for each of the axes
		AssetManager* am = EngineSubsystem( AssetManager );

		// Set up renderables
		mRoot.mRenderable.SetMesh( am->GetAsset< Mesh >( "models.unit_sphere" ) );
		mForwardAxis.mRenderable.SetMesh( am->GetAsset< Mesh >( "models.unit_cylinder" ) );
		mForwardAxisArrow.mRenderable.SetMesh( am->GetAsset< Mesh >( "models.unit_cone" ) );
		mRightAxis.mRenderable.SetMesh( am->GetAsset< Mesh >( "models.unit_cylinder" ) );
		mRightAxisArrow.mRenderable.SetMesh( am->GetAsset< Mesh >( "models.unit_cone" ) );
		mUpAxis.mRenderable.SetMesh( am->GetAsset< Mesh >( "models.unit_cylinder" ) );
		mUpAxisArrow.mRenderable.SetMesh( am->GetAsset< Mesh >( "models.unit_cone" ) );
		mXYAxis.mRenderable.SetMesh( am->GetAsset< Mesh >( "models.unit_cube" ) );
		mXZAxis.mRenderable.SetMesh( am->GetAsset< Mesh >( "models.unit_cube" ) );
		mYZAxis.mRenderable.SetMesh( am->GetAsset< Mesh >( "models.unit_cube" ) ); 

		// Set up materials
		mRoot.mRenderable.SetMaterial( am->GetDefaultAsset< Material >( ) );
		mForwardAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.BlueMaterial" ) );
		mRightAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.RedMaterial" ) );
		mUpAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.GreenMaterial" ) ); 
		mForwardAxisArrow.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.BlueMaterial" ) );
		mUpAxisArrow.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.GreenMaterial" ) );
		mRightAxisArrow.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.RedMaterial" ) );

		mXYAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.BlueMaterial" ) );
		mXZAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.GreenMaterial" ) );
		mYZAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.RedMaterial" ) ); 

		mRoot.mLocalTransform.SetScale( 0.2f );

		// Child heirarchies
		mRoot.AddChild( &mForwardAxis );
		mRoot.AddChild( &mUpAxis );
		mRoot.AddChild( &mRightAxis );
		mForwardAxis.AddChild( &mForwardAxisArrow );
		mUpAxis.AddChild( &mUpAxisArrow );
		mRightAxis.AddChild( &mRightAxisArrow ); 
		mUpAxis.AddChild( &mYZAxis );
		mUpAxis.AddChild( &mXYAxis );
		mForwardAxis.AddChild( &mXZAxis );

		// Set up local transforms for each
		f32 yScale = 4.0f;
		f32 xzScale = 0.3f;
		f32 bias = 1.0f;
		f32 coneYOffset = 1.2f;
		Vec3 coneScale = Vec3( 2.0f, 0.3f, 2.0f );
		Vec3 sharedAxisScale = Vec3( 3.0f, 0.4f, 0.2f );
		mForwardAxis.mLocalTransform.SetRotation( Quaternion::AngleAxis( Math::ToRadians( 90.0f ), Vec3::XAxis() ) );
		mRightAxis.mLocalTransform.SetRotation( Quaternion::AngleAxis( Math::ToRadians( -90.0f ), Vec3::ZAxis() ) );
		mForwardAxis.mLocalTransform.SetScale( Vec3( xzScale, yScale, xzScale ) );
		mRightAxis.mLocalTransform.SetScale( Vec3( xzScale, yScale, xzScale ) );
		mUpAxis.mLocalTransform.SetScale( Vec3( xzScale, yScale, xzScale ) );
		mForwardAxis.mLocalTransform.SetPosition( Vec3( 0.0f, 0.0f, yScale + bias ) );
		mRightAxis.mLocalTransform.SetPosition( Vec3( yScale + bias, 0.0f, 0.0f ) );
		mUpAxis.mLocalTransform.SetPosition( Vec3( 0.0f, yScale + bias, 0.0f ) );
		mForwardAxisArrow.mLocalTransform.SetPosition( Vec3( 0.0f, coneYOffset, 0.0f ) );
		mRightAxisArrow.mLocalTransform.SetPosition( Vec3( 0.0f, coneYOffset, 0.0f ) );
		mUpAxisArrow.mLocalTransform.SetPosition( Vec3( 0.0f, coneYOffset, 0.0f ) );
		mForwardAxisArrow.mLocalTransform.SetScale( coneScale );
		mUpAxisArrow.mLocalTransform.SetScale( coneScale );
		mRightAxisArrow.mLocalTransform.SetScale( coneScale ); 

		mXZAxis.mLocalTransform.SetScale( sharedAxisScale );
		mXZAxis.mLocalTransform.SetRotation( Quaternion::AngleAxis( Math::ToRadians( 90.0f ), Vec3::ZAxis( ) ) );
		mXZAxis.mLocalTransform.SetPosition( Vec3( 9.0f, -0.2f, 0.0f ) );

		mXYAxis.mLocalTransform.SetScale( sharedAxisScale );
		mXYAxis.mLocalTransform.SetRotation( Quaternion::AngleAxis( Math::ToRadians( 0.0f ), Vec3::YAxis( ) ) );
		mXYAxis.mLocalTransform.SetPosition( Vec3( 9.0f, 0.0f, 0.0f ) ); 

		mYZAxis.mLocalTransform.SetScale( sharedAxisScale );
		mYZAxis.mLocalTransform.SetRotation( Quaternion::AngleAxis( Math::ToRadians( -90.0f ), Vec3::YAxis( ) ) );
		mYZAxis.mLocalTransform.SetPosition( Vec3( 0.0f, 0.0f, 9.0f ) ); 

		// Set renderable ids 
		mRoot.mRenderable.SetRenderableID( MAX_ENTITIES + (u32)TransformWidgetRenderableType::TranslationRoot );
		mForwardAxis.mRenderable.SetRenderableID( MAX_ENTITIES + (u32)TransformWidgetRenderableType::TranslationForwardAxis );
		mUpAxis.mRenderable.SetRenderableID( MAX_ENTITIES + (u32)TransformWidgetRenderableType::TranslationUpAxis );
		mRightAxis.mRenderable.SetRenderableID( MAX_ENTITIES + (u32)TransformWidgetRenderableType::TranslationRightAxis );
		mForwardAxisArrow.mRenderable.SetRenderableID( MAX_ENTITIES + ( u32 )TransformWidgetRenderableType::TranslationForwardAxis );
		mUpAxisArrow.mRenderable.SetRenderableID( MAX_ENTITIES + (u32)TransformWidgetRenderableType::TranslationUpAxis );
		mRightAxisArrow.mRenderable.SetRenderableID( MAX_ENTITIES + (u32)TransformWidgetRenderableType::TranslationRightAxis );
		mYZAxis.mRenderable.SetRenderableID( MAX_ENTITIES + ( u32 )TransformWidgetRenderableType::TranslationYZAxes );
		mXYAxis.mRenderable.SetRenderableID( MAX_ENTITIES + ( u32 )TransformWidgetRenderableType::TranslationXYAxes );
		mXZAxis.mRenderable.SetRenderableID( MAX_ENTITIES + ( u32 )TransformWidgetRenderableType::TranslationXZAxes );

		// Add transformation hierarchies
		mTransformHeirarchies.push_back( &mRoot );
		mTransformHeirarchies.push_back( &mForwardAxis );
		mTransformHeirarchies.push_back( &mUpAxis );
		mTransformHeirarchies.push_back( &mRightAxis );
		mTransformHeirarchies.push_back( &mForwardAxisArrow );
		mTransformHeirarchies.push_back( &mUpAxisArrow );
		mTransformHeirarchies.push_back( &mRightAxisArrow );
		mTransformHeirarchies.push_back( &mXYAxis );
		mTransformHeirarchies.push_back( &mXZAxis );
		mTransformHeirarchies.push_back( &mYZAxis );

		// Set up scale for root
		//mRoot.mLocalTransform.SetScale( 0.5f );

		mRoot.mLocalTransform.SetPosition( Vec3( 0.0f, 0.0f, 0.0f ) );

		// Set owner
		mOwner = owner;

		// Set root hierarchy
		mRootHeirarchy = &mRoot;
	} 

	void EditorTranslationWidget::BeginInteraction( TransformWidgetRenderableType type )
	{
		AssetManager* am = EngineSubsystem( AssetManager );

		// Change material 
		switch ( type )
		{
			default: break;

			case TransformWidgetRenderableType::TranslationRoot:
			{
				mRoot.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.YellowMaterial" ) );
			} break; 

			case TransformWidgetRenderableType::TranslationForwardAxis:
			{
				mForwardAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.YellowMaterial" ) );
				mForwardAxisArrow.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.YellowMaterial" ) );
			} break;

			case TransformWidgetRenderableType::TranslationRightAxis:
			{
				mRightAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.YellowMaterial" ) );
				mRightAxisArrow.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.YellowMaterial" ) ); 
			} break;

			case TransformWidgetRenderableType::TranslationUpAxis:
			{
				mUpAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.YellowMaterial" ) );
				mUpAxisArrow.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.YellowMaterial" ) ); 
			} break; 

			case TransformWidgetRenderableType::TranslationXYAxes:
			{
				mXYAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.YellowMaterial" ) );
			} break; 

			case TransformWidgetRenderableType::TranslationXZAxes:
			{
				mXZAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.YellowMaterial" ) );
			} break; 

			case TransformWidgetRenderableType::TranslationYZAxes:
			{
				mYZAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.YellowMaterial" ) );
			} break; 
		}
	}

	void EditorTranslationWidget::Interact( )
	{

	}

	void EditorTranslationWidget::EndInteraction( TransformWidgetRenderableType type )
	{
		AssetManager* am = EngineSubsystem( AssetManager );

		// Change material 
		switch ( type )
		{
			default: break;
			
			case TransformWidgetRenderableType::TranslationRoot:
			{
				mRoot.mRenderable.SetMaterial( am->GetDefaultAsset< Material >( ) );
			} break; 

			case TransformWidgetRenderableType::TranslationXYAxes:
			{
				mXYAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.BlueMaterial" ) );
			} break; 

			case TransformWidgetRenderableType::TranslationXZAxes:
			{
				mXZAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.GreenMaterial" ) );
			} break; 

			case TransformWidgetRenderableType::TranslationYZAxes:
			{
				mYZAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.RedMaterial" ) );
			} break; 

			case TransformWidgetRenderableType::TranslationForwardAxis:
			{
				mForwardAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.BlueMaterial" ) );
				mForwardAxisArrow.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.BlueMaterial" ) );
			} break; 

			case TransformWidgetRenderableType::TranslationRightAxis:
			{
				mRightAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.RedMaterial" ) );
				mRightAxisArrow.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.RedMaterial" ) ); 
			} break;

			case TransformWidgetRenderableType::TranslationUpAxis:
			{
				mUpAxis.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.GreenMaterial" ) );
				mUpAxisArrow.mRenderable.SetMaterial( am->GetAsset< Material >( "Cache.GreenMaterial" ) ); 
			} break; 
		}
	}
}
