#ifndef ENJON_COMMON_H
#define ENJON_COMMON_H

#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Constants.h"
#include "Defines.h"
#include "System/Types.h"

namespace Enjon {
	namespace Math {

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

#ifndef PI
#define PI 3.14159265359
#endif

		inline float ToRadians( float degrees )
		{
			return degrees * ( f32 )( ( f32 )PI / 180.0f );
		}

		inline float ToDegrees( float radians )
		{
			return radians * ( 180.0f / ( float )PI );
		}

		inline Enjon::Vec2 IsoToCartesian( const Enjon::Vec2& point )
		{
			return Enjon::Vec2( ( 2 * point.y + point.x ) / 2.0f, ( 2 * point.y - point.x ) / 2.0f );
		}

		inline Enjon::Vec2 CartesianToIso( const Enjon::Vec2& point )
		{
			return Enjon::Vec2( point.x - point.y, ( point.x + point.y ) / 2.0f );
		}

		inline f32 Lerp( f32 a, f32 b, f32 t )
		{
			return a + t * ( b - a );
		}

		template <typename T>
		inline T Clamp( T val, T min, T max )
		{
			if ( val < min ) return min;
			if ( val > max ) return max;
			return val;
		}

		template <typename T>
		inline T MinClamp( T val, T min )
		{
			if ( val < min ) return min;
			return val;
		}

		template <typename T>
		inline T Min( T val, T max )
		{
			return val < max ? val : max;
		}

		template <typename T>
		inline T Max( T val, T max )
		{
			return val > max ? val : max;
		}

		template <typename T>
		inline T Round( T val )
		{
			return std::roundf( val );
		}
	}
}

#endif