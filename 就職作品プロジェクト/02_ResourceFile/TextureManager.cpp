#include "TextureManager.h"
#include "Game.h"
#include "Debug.hpp"

bool TextureManager::CheckTexturePath(std::string checkPath)
{
    m_RegistNumber = 0;
    for (auto registedPath : m_TexturePath)
    {
        if (checkPath == registedPath)
        {
            return true;
        }
        m_RegistNumber++;
    }
    return false;
}

void TextureManager::AddTexture(std::string addPath)
{
    if (CheckTexturePath(addPath)) { 
        Debug::Log("éÊìæçœÇ› : " + addPath);
        return;
    }
    
    Texture* registTex = new Texture;
    std::string texDirectory = m_filePath + addPath;
    
    if (registTex->LoadFromFile(texDirectory))
    {
        m_TexturePath.emplace_back(addPath);
        m_Textures   .emplace_back(registTex);
        Debug::Log("ê¨å˜ Textureìoò^ : " + addPath);
        return;
    }

    Debug::Log("é∏îs Textureìoò^ : " + addPath);
    return;
}

Texture* TextureManager::GetTexture(std::string texturePath)
{
    if (CheckTexturePath(texturePath)) {
        return m_Textures[m_RegistNumber].get();
    }

    AddTexture(texturePath);
    return m_Textures[m_Textures.size() - 1].get();
}
