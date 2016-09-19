#ifndef ENJON_COMMON_H
#define ENJON_COMMON_H

#include <Math/Vec2.h>
#include <Math/Vec3.h>
#include <Math/Constants.h>
#include <Defines.h>

namespace Enjon { namespace Math {
		
	//Cardinal directions
	#define NORTH		Enjon::Math::Vec2(0, 1)
	#define NORTHEAST   Enjon::Math::Vec2(1, 1)
	#define NORTHWEST   Enjon::Math::Vec2(-1, 1)
	#define EAST		Enjon::Math::Vec2(1, 0)
	#define WEST		Enjon::Math::Vec2(-1, 0)
	#define SOUTH		Enjon::Math::Vec2(0, -1)
	#define SOUTHEAST   Enjon::Math::Vec2(1, -1)
	#define SOUTHWEST   Enjon::Math::Vec2(-1, -1)

	// Bools
	#ifndef TRUE 		
		#define TRUE 	1
	#endif
	#ifndef FALSE
		#define FALSE	0
	#endif
		
	inline float ToRadians(float degrees)
	{
		return degrees * (float)(PI / 180.0f);
	} 

	inline float ToDegrees(float radians)
	{
		return radians * (180.0f / (float)PI); 
	}
	
	inline Math::Vec2 IsoToCartesian(const Enjon::Math::Vec2& point)
	{
		return Enjon::Math::Vec2((2 * point.y + point.x) / 2.0f, (2 * point.y - point.x) / 2.0f);
	}

	inline Math::Vec2 CartesianToIso(const Enjon::Math::Vec2& point)
	{
		return Enjon::Math::Vec2(point.x - point.y, (point.x + point.y) / 2.0f); 
	}

	inline float Lerp(float goal, float current, float dt)
	{
		float difference = goal - current;

		if (difference > dt)
			return current + dt;
		if (difference < -dt)
			return current - dt;

		return goal;
	}

}}

#endif