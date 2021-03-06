// @file ImportOptions.h
// Copyright 2016-2018 John Jackson. All Rights Reserved.

#pragma once
#ifndef ENJON_IMPORT_OPTIONS_H
#define ENJON_IMPORT_OPTIONS_H

#include "Base/Object.h"
#include "System/Types.h"
#include "Defines.h"

namespace Enjon
{
	class AssetLoader;

	struct AssetStringInformation
	{ 
		String mQualifiedName;

		String mDisplayName;

		String mAssetDestinationPath;
	};

	ENJON_CLASS( Abstract )
	class ImportOptions : public Object
	{
		friend AssetLoader;

		ENJON_CLASS_BODY( ImportOptions )

		public: 

			/*
			* @brief
			*/
			bool IsImporting( ) const;

			/*
			* @brief
			*/
			String GetResourceFilePath( ) const;

			/*
			* @brief
			*/
			String GetDestinationAssetDirectory( ) const; 

			/*
			* @brief
			*/
			virtual Result OnEditorUI( ) override; 

			/*
			* @brief
			*/
			const AssetLoader* GetLoader( ) const; 


			virtual void Reset( )
			{ 
				// Nothing by default
			}

		protected:

			/*
			* @brief
			*/
			virtual Result OnEditorUIInternal( ) = 0;

		protected: 
			String mResourceFilePath;
			String mDestinationAssetDirectory; 
			String mAssetName = "";
			bool mIsImporting = false;
			const AssetLoader* mLoader = nullptr;
	}; 
}

#endif
