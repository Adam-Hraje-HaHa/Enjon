// Copyright 2016-2017 John Jackson. All Rights Reserved.
// File: AssetUtils.h

#include "Asset/AssetUtils.h"
#include "Asset/AssetManager.h"
#include "Engine.h"
#include "SubsystemCatalog.h"

namespace Enjon
{ 
	Asset* AssetUtils::GetDefaultAsset( const MetaClass* cls )
	{
		if ( !Engine::GetInstance( ) )
		{
			return nullptr;
		}

		const AssetManager* am = Engine::GetInstance( )->GetSubsystemCatalog( )->Get<AssetManager>( ); 
		return am->GetDefaultAsset( cls );
	}
} 
