// Copyright 2016-2017 John Jackson. All Rights Reserved.
// File: SkeletalMeshAssetLoader.h

#pragma  once
#ifndef ENJON_SKELETAL_MESH_ASSET_LOADER_H
#define ENJON_SKELETAL_MESH_ASSET_LOADER_H 

#include "Asset/ImportOptions.h"
#include "Asset/AssetLoader.h"
#include "Graphics/Mesh.h"
#include "Graphics/Skeleton.h"
#include "Graphics/SkeletalMesh.h"

// Assimp specifics
struct aiMesh;
struct aiNode;
struct aiScene;

namespace Enjon
{
	class SkeletalMesh;

	ENJON_CLASS()
	class SkeletalMeshAssetLoader : public AssetLoader
	{
		ENJON_CLASS_BODY( )

		public:

			/**
			* @brief Constructor
			*/
			SkeletalMeshAssetLoader( ); 

			/**
			* @brief Constructor
			*/
			~SkeletalMeshAssetLoader( ); 

			/**
			* @brief 
			*/
			virtual Asset* DirectImport( const ImportOptions* options ) override;

		protected:

			/**
			* @brief
			*/
			virtual void RegisterDefaultAsset( ) override; 

			/**
			* @brief 
			*/
			virtual Asset* LoadResourceFromImporter( const ImportOptions* options ) override;

		private:

			/**
			* @brief 
			*/
			void ProcessSkeletalMesh( aiMesh* aim, const aiScene* scene, const AssetHandle<Skeleton>& skeleton, SkeletalMesh* mesh, Vector< VertexJointData >* vertexJointData );

			/**
			* @brief 
			*/
			void ProcessNodeSkeletal( aiNode* node, const aiScene* scene, const AssetHandle<Skeleton>& skeleton, SkeletalMesh* mesh, Vector< VertexJointData >* vertexJointData ); 
	};
	
}

#endif
