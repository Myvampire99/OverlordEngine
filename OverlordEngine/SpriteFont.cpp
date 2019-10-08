#include "stdafx.h"
#include "SpriteFont.h"

SpriteFont::SpriteFont():
	m_TextCache(std::vector<TextCache>()),
	m_FontName(L""),
	m_CharacterCount(0),
	m_CharacterSpacing(1),
	m_TextureWidth(0),
	m_TextureHeight(0),
	m_BufferStart(0),
	m_BufferSize(0),
	m_pTexture(nullptr),
	m_FontSize(0),
	m_IsAddedToRenderer(false)
{
	for (auto & i : m_CharTable)
	{
		ZeroMemory(static_cast<void*>(&i), sizeof(FontMetric));
		i.IsValid = false;
	}
}

SpriteFont::SpriteFont(SpriteFont* sprite) 
	: m_TextCache(std::vector<TextCache>()),
	m_FontName(L""),
	m_CharacterCount(0),
	m_CharacterSpacing(1),
	m_TextureWidth(0),
	m_TextureHeight(0),
	m_BufferStart(0),
	m_BufferSize(0),
	m_pTexture(nullptr),
	m_FontSize(0),
	m_IsAddedToRenderer(false)
{
	for (auto & i : m_CharTable)
	{
		ZeroMemory(static_cast<void*>(&i), sizeof(FontMetric));
		i.IsValid = false;
	}
	for (int i{}; i < 256; ++i) {
		m_CharTable[i] = sprite->m_CharTable[i];
	}
	//m_CharTable[0] = *sprite->m_CharTable;
	m_TextCache;// = sprite->m_TextCache;
	m_FontName = sprite->GetFontName();
	m_CharacterCount = sprite->m_CharacterCount;
	m_CharacterSpacing = sprite->m_CharacterSpacing;
		m_TextureWidth = sprite->m_TextureWidth;
		m_TextureHeight = sprite->m_TextureHeight;
		m_BufferStart = sprite->m_BufferStart;
		m_BufferSize = sprite->m_BufferSize;
		m_pTexture = sprite->m_pTexture;
		m_FontSize = sprite->m_FontSize;
		m_IsAddedToRenderer = false;



}

bool SpriteFont::IsCharValid(const wchar_t& character)
{
	if (character >= MIN_CHAR_ID && character <= MAX_CHAR_ID)
		return true;

	return false;
}

void SpriteFont::AddToTextCache(TextCache&& cache)
{
	m_TextCache.emplace_back(cache);
}

void SpriteFont::ClearCache()
{
	m_TextCache.clear();
}
