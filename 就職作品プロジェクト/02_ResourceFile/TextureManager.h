#pragma once
#include "Texture.h"
#include <vector>
#include <memory>
#include <string>

class TextureManager
{
private:
	std::vector<std::string>			  m_TexturePath;
	std::vector<std::shared_ptr<Texture>> m_Textures;
	short m_RegistNumber = 0;

	const char* m_filePath = "01_AssetFile/Texture/" ;

public:
	bool	 CheckTexturePath(std::string chackPath);
	void	 AddTexture		 (std::string addPath);

	/// @brief Texture‚ğGame::GetTextures‚©‚ç’T‚·
	///	       ‚È‚©‚Á‚½ê‡‚Í‚»‚Ìê‚Å’Ç‰Á‚·‚é
	/// @param texturePath 
	/// @return 
	Texture* GetTexture		 (std::string texturePath);

};

