#include <Utils/Timing.h>
#include <iostream>
#include <Math/Maths.h>


namespace Enjon { namespace Utils { 

		FPSLimiter::FPSLimiter()
		{

		}

		FPSLimiter::~FPSLimiter()
		{

		}

		void FPSLimiter::Init(float maxFPS)
		{
			//Set targetFPS when initialized
			SetMaxFPS(maxFPS);
		}

		float FPSLimiter::GetFPS( bool calculateFPS )
		{
			if ( calculateFPS )
			{
				CalculateFPS( );
			}

			return m_FPS;
		} 

		void FPSLimiter::Begin()
		{
			//Set starting ticks
			m_startTicks = SDL_GetTicks();				
		}

		bool FPSLimiter::End()
		{
			static float second = SECOND;
			static f32 t = 0.0f;

			//Calculate the FPS to be limited and returned 
			CalculateFPS();

			u32 ticks = SDL_GetTicks( );

			//Calculate ticks
			Uint32 frameTicks = ticks - m_startTicks; 

			//mDT = (f32)frameTicks / 1000.0f;
			mDT = ( f32 )frameTicks;
			t += mDT; 

			//TODO:: Implement this using time-steps
			// if (m_FPS > m_maxFPS + 2.0f) second = SECOND * 2.0f;
			// else if (m_FPS < m_maxFPS - 2.0f) second = SECOND / 2.0f;
			// else second = SECOND; 

			//std::cout << ticks << ", " << m_startTicks << "\n";
			//if ((f32)second / m_maxFPS > frameTicks)
			//{
			//	//SDL_Delay(Uint32(second / m_maxFPS - frameTicks));
			//	return true;
			//}	 

			if ( ( ( f32 )second / ( f32 )m_maxFPS ) / 60.0f > t )
			{
				return false;
			}

			//if ( ( (f32)second / (f32)m_maxFPS ) > (f32)frameTicks )
			//{
			//	return false;
			//	//return true;
			//}

			t = 0.0f;
			return true; 
						
			//Limit the FPS to maxFPS
			// if (SECOND / m_maxFPS > frameTicks)
			// {
			// 	SDL_Delay(Uint32(SECOND / m_maxFPS - frameTicks));
			// } 
			

			//return FPS
		} 

		void FPSLimiter::CalculateFPS()
		{ 
			static const int NUM_SAMPLES = 20;
			static Uint32 frameTimes[NUM_SAMPLES];
			static int currentFrame = 0;

			static Uint32 prevTicks = SDL_GetTicks();
			
			currentFrame++;
			Uint32 currentTicks;
			currentTicks = SDL_GetTicks();

			Uint32(frameTime_) = currentTicks - prevTicks;
			frameTimes[currentFrame % NUM_SAMPLES] = frameTime_;

			prevTicks = currentTicks;

			int count;
			if( currentFrame < NUM_SAMPLES )
			{
				count = currentFrame;
			}
			else
			{
				count = NUM_SAMPLES;
			}

			float frameTimeAvg = 0;
			for( int i = 0; i < count; i++ )
			{
				frameTimeAvg += frameTimes[i];
			}

			frameTimeAvg /= count;

			if( frameTimeAvg > 0 )
			{
				m_FPS = SECOND / frameTimeAvg;
			}
			else
			{
				m_FPS = 60.0f;
			} 
		}
							









}}