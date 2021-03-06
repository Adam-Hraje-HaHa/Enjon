// File: EditorView.h
// Copyright 2016-2018 John Jackson. All Rights Reserved.

#pragma once
#ifndef ENJON_EDITOR_VIEW_H
#define ENJON_EDITOR_VIEW_H 

#include "EditorObject.h" 

namespace Enjon
{
	class Window;
	class EditorApp;

	class EditorView : public EditorObject
	{
		friend EditorApp;

		public:

			EditorView( ) = default;

			/**
			* @brief
			*/
			EditorView( EditorApp* app, Window* window, const String& name = "Editor View", const u32& viewFlags = 0 );

			/**
			* @brief
			*/
			virtual ~EditorView( )
			{ 
			}

			/**
			* @brief
			*/
			virtual void Update( ) override; 

			/**
			* @brief
			*/
			String GetViewName( );

			/**
			* @brief
			*/
			Window* GetWindow( );

			/**
			* @brief
			*/ 
			virtual u32 GetViewFlags( ); 

			/**
			* @brief
			*/ 
			bool GetEnabled( );

			/**
			* @brief
			*/
			virtual void Initialize( );

			/**
			* @brief
			*/
			bool IsFocused( ) const;

			/**
			* @brief
			*/
			bool IsHovered( ) const;

			/**
			* @brief
			*/
			void Enable( const b32& enabled );
 
		protected: 

			/**
			* @brief Must be overridden
			*/
			virtual void UpdateView( ) = 0;

			/**
			* @brief Must be overridden
			*/
			virtual void ProcessViewInput( ) = 0; 

			/**
			* @brief
			*/
			virtual void CaptureState( );

		private: 

			/**
			* @brief
			*/
			virtual void ProcessInput( ) override; 

		protected:

			EditorApp* mApp = nullptr;
			String mName = "EditorView"; 
			u32 mViewFlags = 0;
			bool mViewEnabled = true;
			Window* mWindow = nullptr;
			bool mIsHovered = false;
			bool mIsFocused = false;
	};
}

#endif

/*
	EditorView needs to be able to:
		- Set window focus to itself: 
			. Be able to capture all input
			. Detect when it's in focus/out of focus
*/












