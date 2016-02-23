#ifndef FONT_H
#define FONT_H

#include <map>

#include <GLEW/glew.h>

#include "Graphics/SpriteBatch.h"
#include "Graphics/Color.h"

#include "ft2build.h"
#include FT_FREETYPE_H


namespace Enjon { namespace Graphics { namespace Fonts {

	typedef struct 
	{
		GLuint TextureID;
		Enjon::Math::iVec2 Size;
		Enjon::Math::iVec2 Bearing;
		GLuint Advance;
	} Character;

	typedef struct 
	{
		std::map<GLchar, Character> Characters;	
	} Font;

	typedef struct 
	{
		Enjon::Math::Vec4 DestRect;
		Enjon::Math::Vec4 UV;
		GLuint TextureID;
	} CharacterStats;

	/* Inits a particular font with a particular size and stores in a returned map */
	void Init(char* filePath, GLuint size, Font* font);

	/* Gets character stats from given font */
	CharacterStats GetCharacterAttributes(Math::Vec2 Pos, float scale, Font* F, std::string::const_iterator c, float* advance);

	/* Creates and returns new font */
	Font* CreateFont(char* filePath, GLuint size);

	/* Adds a string of tex at (x,y) to given spritebatch */
	void PrintText(GLfloat x, GLfloat y, GLfloat scale, std::string text, Font* F, Enjon::Graphics::SpriteBatch& Batch, 
						Enjon::Graphics::ColorRGBA16 Color = Enjon::Graphics::RGBA16_White());

}}}

#endif