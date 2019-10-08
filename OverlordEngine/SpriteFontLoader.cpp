#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "BinaryReader.h"
#include "ContentManager.h"
#include "TextureData.h"

SpriteFont* SpriteFontLoader::LoadContent(const std::wstring& assetFile)
{
	auto pBinReader = new BinaryReader();
	pBinReader->Open(assetFile);

	if (!pBinReader->Exists())
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	if (pBinReader->Read<char>() == 'B' && pBinReader->Read<char>() == 'M' && pBinReader->Read<char>() == 'F') {
		///Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Not a valid .fnt font", assetFile.c_str());==============
		///return nullptr;
	}
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	//...

	//Parse the version (version 3 required)
	if (pBinReader->Read<char>() < 3) {
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported", assetFile.c_str());
		return nullptr;
	}
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
	//...

	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)
	///pSpriteFont->m_FontName() = 


	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	int ID = pBinReader->Read<char>();
	int size = pBinReader->Read<unsigned int>();
	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	pSpriteFont->m_FontSize = pBinReader->Read<short>();
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	pBinReader->MoveBufferPosition(12);
	//Retrieve the FontName [SpriteFont::m_FontName]
	pSpriteFont->m_FontName += pBinReader->ReadNullString();
	//...


	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	ID = pBinReader->Read<char>();
	size = pBinReader->Read<unsigned int>();

	pBinReader->MoveBufferPosition(4);
	//Retrieve Texture Width & Height [SpriteFont::m_TextureWidth/m_TextureHeight]
	pSpriteFont->m_TextureWidth = pBinReader->Read<uint16_t>();
	pSpriteFont->m_TextureHeight = pBinReader->Read<uint16_t>();
	//Retrieve PageCount
	int pageCount = pBinReader->Read<short>();
	//> if pagecount > 1
	if (pageCount > 1) {
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed", assetFile.c_str());
		return nullptr;
		//> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed)
	}
	//Advance to Block2 (Move Reader)
	pBinReader->MoveBufferPosition(5);
	//...

	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	ID = pBinReader->Read<char>();
	size = pBinReader->Read<unsigned int>();
	//Retrieve the PageName (store Local)
	std::wstring pageName;
	pageName = pBinReader->ReadNullString();
	//	> If PageName is empty
	if (pageName.empty()) {
		//	> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty])
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]", assetFile.c_str());
		return nullptr;
	}
	//>Retrieve texture filepath from the assetFile path
	//> (ex. c:/Example/somefont.fnt => c:/Example/) [Have a look at: wstring::rfind()]
	std::size_t found = assetFile.rfind(L"/");
	std::wstring path = assetFile.substr(0, found+1);
	path += pageName;
	//>Use path and PageName to load the texture using the ContentManager [SpriteFont::m_pTexture]assetFile
	pageName = assetFile.substr(0, assetFile.rfind('/') + 1) + pageName;
	pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(path);
	//> (ex. c:/Example/ + 'PageName' => c:/Example/somefont_0.png)
	//...

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	ID = pBinReader->Read<char>();
	size = pBinReader->Read<unsigned int>();
	//Retrieve Character Count (see documentation)
	int numChars = size / 20;
	//Retrieve Every Character, For every Character:
	for (int i{}; i < numChars; ++i) {
		//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		wchar_t charID = (wchar_t)pBinReader->Read<uint32_t>();
		//> Check if CharacterId is valid (SpriteFont::IsCharValid), Log Warning and advance to next character if not valid
		if (!SpriteFont::IsCharValid(charID)) {
			Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > CharacterId not valid", assetFile.c_str());
			return nullptr;
		}
		//> Retrieve the corresponding FontMetric (SpriteFont::GetMetric) [REFERENCE!!!]
		FontMetric &fontMetric = pSpriteFont->GetMetric(charID);
		//> Set IsValid to true [FontMetric::IsValid]
		fontMetric.IsValid = true;
		//> Set Character (CharacterId) [FontMetric::Character]
		fontMetric.Character = charID;
		//> Retrieve Xposition (store Local)
		uint16_t xPos = pBinReader->Read<uint16_t>();
		//> Retrieve Yposition (store Local)
		uint16_t yPos = pBinReader->Read<uint16_t>();
		//> Retrieve & Set Width [FontMetric::Width]
		fontMetric.Width = pBinReader->Read<uint16_t>();
		//> Retrieve & Set Height [FontMetric::Height]
		fontMetric.Height = pBinReader->Read<uint16_t>();
		//> Retrieve & Set OffsetX [FontMetric::OffsetX]
		fontMetric.OffsetX = pBinReader->Read<uint16_t>();
		//> Retrieve & Set OffsetY [FontMetric::OffsetY]
		fontMetric.OffsetY = pBinReader->Read<uint16_t>();
		//> Retrieve & Set AdvanceX [FontMetric::AdvanceX]
		fontMetric.AdvanceX = pBinReader->Read<uint16_t>();
		//> Retrieve & Set Page [FontMetric::Page]
		fontMetric.Page = pBinReader->Read<char>();
		//> Retrieve Channel (BITFIELD!!!) 
		uint8_t channel = pBinReader->Read<uint8_t>();
		//	> See documentation for BitField meaning [FontMetrix::Channel]
		//blue
		if ((channel & 0b00000001) > 0)
		{
			fontMetric.Channel = 2;
		}
		//green
		if ((channel & 0b00000010) > 0)
		{
			fontMetric.Channel = 1;
		}
		//red
		if ((channel & 0b00000100) > 0)
		{
			fontMetric.Channel = 0;
		}
		//alpha
		if ((channel & 0b00001000) > 0)
		{
			fontMetric.Channel = 3;
		}

		//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
		fontMetric.TexCoord = { float(xPos) / float(pSpriteFont->m_TextureWidth) ,float(yPos) / float(pSpriteFont->m_TextureHeight) };
		//...
	}
	//DONE :)

	delete pBinReader;
	return pSpriteFont;
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
