// @file CapsuleCollisionShape.h
// Copyright 2016-2018 John Jackson. All Rights Reserved.

#pragma once
#ifndef ENJON_CAPSULECOLLISIONSHAPE_H
#define ENJON_CAPSULECOLLISIONSHAPE_H

#include "Physics/CollisionShape.h"
#include "Math/Vec3.h"

namespace Enjon
{ 
	ENJON_CLASS( Construct )
	class CapsuleCollisionShape : public CollisionShape
	{
		ENJON_CLASS_BODY( )

		public:

			/**
			* @brief
			*/
			CapsuleCollisionShape( );

			/**
			* @brief
			*/
			~CapsuleCollisionShape( );

		private:

			/**
			* @brief
			*/
			virtual void Base( ) override;

		private: 
			ENJON_PROPERTY( )
			f32 mRadius = 1.0f;

			ENJON_PROPERTY( )
			f32 mHeight = 1.0f;

	};
}

#endif