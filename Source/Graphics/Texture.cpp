// Copyright 2016-2017 John Jackson. All Rights Reserved.
// File: Texture.cpp

#include "Graphics/Texture.h"
#include "Asset/TextureAssetLoader.h"
#include "Asset/AssetManager.h"
#include "Serialize/ObjectArchiver.h"
#include "Utils/FileUtils.h"
#include "Engine.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION 
#include <STB/stb_image_write.h> 

#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

#include <random>
#include <GLEW/glew.h>
#include <vector> 

namespace Enjon
{
	//=================================================

	Texture::Texture()
		: mId(0), mWidth(0), mHeight(0)
	{ 
	}

	//=================================================
			
	Texture::Texture( u32 width, u32 height, u32 textureID )
		: mWidth( width ), mHeight( height ), mId( textureID )
	{ 
	}

	//=================================================

	Texture::~Texture()
	{ 

	}

	//=================================================

	Texture* Texture::Construct( const String& filePath )
	{
		// Get file extension of file
		Enjon::String fileExtension = Utils::SplitString( filePath, "." ).back( ); 

		// Fields to load and store
		s32 width, height, nComps, len; 

		void* textureData = nullptr; 

		// Construct new texture to fill out
		Texture* tex = new Texture( );

		// Load HDR format
		if ( fileExtension.compare( "hdr" ) == 0 )
		{
			stbi_set_flip_vertically_on_load( true );
			f32* data = stbi_loadf( filePath.c_str( ), &width, &height, &nComps, 0 ); 

			glGenTextures( 1, &tex->mId );
			glBindTexture( GL_TEXTURE_2D, tex->mId );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_FLOAT, data );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			s32 MAG_PARAM = GL_LINEAR;
			s32 MIN_PARAM = GL_LINEAR_MIPMAP_LINEAR;
			b8 genMips = true;

			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MAG_PARAM );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MIN_PARAM );

			if ( genMips )
			{
				glGenerateMipmap( GL_TEXTURE_2D );
			} 

			tex->mFormat = TextureFormat::HDR;

			// Free image data once done
			//stbi_image_free( data );
		}

		// Otherwise load standard format
		else
		{
			// Load texture data
			stbi_set_flip_vertically_on_load( false );

			// For now, this data will always have 4 components, since STBI_rgb_alpha is being passed in as required components param
			// Could optimize this later
			u8* data = stbi_load( filePath.c_str( ), &width, &height, &nComps, STBI_rgb_alpha );
			textureData = (u8*)data;

			// TODO(): For some reason, required components is not working, so just default to 4 for now
			nComps = 4;

			// Generate texture
			glGenTextures( 1, &( tex->mId ) );

			// Bind and create texture
			glBindTexture( GL_TEXTURE_2D, tex->mId );

			// Generate texture depending on number of components in texture data
			switch ( nComps )
			{
				case 3: 
				{
					glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data ); 
				} break;

				default:
				case 4: 
				{
					glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data ); 
				} break;
			}

			s32 MAG_PARAM = GL_LINEAR;
			s32 MIN_PARAM = GL_LINEAR_MIPMAP_LINEAR;
			b8 genMips = true;

			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MAG_PARAM );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MIN_PARAM );

			if ( genMips )
			{
				glGenerateMipmap( GL_TEXTURE_2D );
			}

			tex->mFormat = TextureFormat::LDR;

			glBindTexture( GL_TEXTURE_2D, 0 ); 
		} 

		// Set texture attributes
		tex->mWidth = width;
		tex->mHeight = height;
		tex->mNumberOfComponents = nComps; 

		// Store file extension type of texture
		tex->mFileExtension = Texture::GetFileExtensionType( fileExtension ); 

		// Store texture data
		switch ( tex->mFormat )
		{
			case TextureFormat::HDR:
			{
				tex->mSourceData = new TextureSourceData< f32 >( (f32*)textureData, tex ); 
			} break;
			case TextureFormat::LDR: 
			{
				tex->mSourceData = new TextureSourceData< u32 >( (u32*)textureData, tex ); 
			} break;
		}

		return tex; 
	}

	//=================================================

	TextureFileExtension Texture::GetFileExtensionType( const Enjon::String& fileExtension )
	{
		if ( fileExtension.compare( "png" ) == 0 )
		{
			return TextureFileExtension::PNG;
		}
		else if ( fileExtension.compare( "tga" ) == 0 )
		{
			return TextureFileExtension::TGA;
		}
		else if ( fileExtension.compare( "bmp" ) == 0 )
		{
			return TextureFileExtension::BMP;
		}
		else if ( fileExtension.compare( "jpeg" ) == 0 )
		{
			return TextureFileExtension::JPEG;
		}
		else if ( fileExtension.compare( "hdr" ) == 0 )
		{
			return TextureFileExtension::HDR;
		}
		else
		{
			return TextureFileExtension::UNKNOWN;
		}
	}

	//=================================================

	TextureFormat Texture::GetFormat( ) const
	{
		return mFormat;
	}

	//=================================================

	u32 Texture::GetWidth() const
	{
		return mWidth;
	}

	//=================================================

	u32 Texture::GetHeight() const
	{
		return mHeight;
	}

	//=================================================

	u32 Texture::GetTextureId() const
	{
		return mId;
	} 

	//================================================= 
			
	Enjon::Result Texture::CacheFile( Enjon::ByteBuffer& buffer )
	{
		// Get engine
		Enjon::Engine* engine = Enjon::Engine::GetInstance( );

		// Get asset manager from subsystem catalog
		const Enjon::AssetManager* am = engine->GetSubsystemCatalog( )->Get< Enjon::AssetManager >( );

		// Get project directory
		const Enjon::String& cachePath =  am->GetCachedAssetsPath( );

		// Write out to buffer
		buffer.Write( mWidth );
		buffer.Write( mHeight );
		buffer.Write( (u32)mFileExtension );

		return Enjon::Result::SUCCESS;
	}

	//================================================= 

	Result Texture::SerializeData( ObjectArchiver* archiver ) const 
	{
		std::cout << "Texture serializing!\n";

		// Keep around texture data resource object for serialization purposes
		// Will be compiled out with release of application and only defined as being with editor data

		// Write out basic header info for texture 
		archiver->WriteToBuffer< u32 >( mWidth );					// Texture width
		archiver->WriteToBuffer< u32 >( mHeight );					// Texture height
		archiver->WriteToBuffer< u32 >( mNumberOfComponents );		// Texture components per pixel
		archiver->WriteToBuffer< u32 >( ( u32 )mFormat );			// Texture format
		archiver->WriteToBuffer< u32 >( ( u32 )mFileExtension );	// Texture file extension

		switch ( mFormat )
		{
			case TextureFormat::HDR:
			{
				const f32* rawData = static_cast< TextureSourceData<f32>* >( mSourceData )->GetData( ); 
			} break;

			case TextureFormat::LDR:
			{
				// Get raw data from source
				const u8* rawData = mSourceData->Cast< u8 >()->GetData( ); 

				// The pixel data consists of *y scanlines of *x pixels,
				//	with each pixel consisting of N interleaved 8-bit components with no padding in between; the first
				//	pixel pointed to is top-left-most in the image.  
				u32 totalWidth = mWidth * mNumberOfComponents;

				for ( u32 h = 0; h < mHeight; ++h )
				{
					for ( u32 w = 0; w < totalWidth; ++w )
					{
						// Get index of indvidual interleaved pixel
						u32 pixelIndex = totalWidth * h + w; 
						// Raw pixel 
						u8 pixel = rawData[ pixelIndex ]; 
						// Write individual pixel to archive
						archiver->WriteToBuffer< u8 >( pixel );
					}
				}

				// Release source data after serializing
				const_cast< TextureSourceData< u8 >* >( mSourceData->Cast< u8 >( ) )->ReleaseData( );

			} break;
		} 

		return Result::SUCCESS;
	} 
	
	Result Texture::DeserializeData( ObjectArchiver* archiver )
	{
		std::cout << "Texture Deserializing!\n";

		// Read properties from buffer - THIS SHOULD BE USED WITH A VERSIONING STRUCT!
		mWidth = archiver->ReadFromBuffer< u32 >( );										// Texture width
		mHeight = archiver->ReadFromBuffer< u32 >( );										// Texture height
		mNumberOfComponents = archiver->ReadFromBuffer< u32 >( );							// Texture components per pixel
		mFormat = TextureFormat( archiver->ReadFromBuffer< u32 >( ) );						// Texture format
		mFileExtension = TextureFileExtension( archiver->ReadFromBuffer< u32 >( ) );		// Texture format

		switch ( mFormat )
		{
			case TextureFormat::HDR:
			{ 
			} break;

			case TextureFormat::LDR:
			{ 
				// Buffer for pixel data
				u8* pixelData = new u8[ mHeight * mWidth * mNumberOfComponents ];

				u32 totalWidth = mWidth * mNumberOfComponents;

				for ( u32 h = 0; h < mHeight; ++h ) 
				{
					for ( u32 w = 0; w < totalWidth; ++w )
					{
						// Get index of indvidual interleaved pixel
						u32 pixelIndex = totalWidth * h + w; 
						// Read individual pixel from archive
						pixelData[ pixelIndex ] = archiver->ReadFromBuffer< u8 >( );
					}
				} 

				// Generate texture
				glGenTextures( 1, &( mId ) ); 
				// Bind texture to be created
				glBindTexture( GL_TEXTURE_2D, mId );

				switch ( mNumberOfComponents )
				{
					default:
					case 3: 
					{
						glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelData ); 
					} break;

					case 4: 
					{
						glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData ); 
					} break;
				}

				s32 MAG_PARAM = GL_LINEAR;
				s32 MIN_PARAM = GL_LINEAR_MIPMAP_LINEAR;
				b8 genMips = true;

				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MAG_PARAM );
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MIN_PARAM );

				if ( genMips )
				{
					glGenerateMipmap( GL_TEXTURE_2D );
				} 

				glBindTexture( GL_TEXTURE_2D, 0 ); 

				// Clean up pixel data once done
				delete pixelData; 

			} break;
		} 

		return Result::SUCCESS;
	}
}














