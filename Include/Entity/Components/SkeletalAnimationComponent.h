// @file SkeletalAnimationComponent.h
// Copyright 2016-2018 John Jackson. All Rights Reserved.

#pragma once
#ifndef ENJON_SKELETAL_ANIMATION_COMPONENT_H
#define ENJON_SKELETAL_ANIMATION_COMPONENT_H

#include "Entity/Component.h"
#include "Asset/Asset.h"
#include "Graphics/SkeletalAnimation.h"

namespace Enjon
{ 
	class AnimationSubsystem;

	ENJON_CLASS( Construct )
	class SkeletalAnimationComponent : public Component
	{ 
		friend AnimationSubsystem;

		ENJON_COMPONENT( SkeletalAnimationComponent, Requires[ SkeletalMeshComponent ] ) 

		protected: 

			/**
			* @brief
			*/
			void UpdateAndCalculateTransforms( ); 

		private: 

			ENJON_PROPERTY( )
			AssetHandle< SkeletalAnimation > mAnimation; 

			ENJON_PROPERTY( )
			f32 mCurrentAnimationTime = 0.0f; 
	};
}

#endif
