#include "ShaderManager.h"
#include "Debug.hpp"

bool ShaderManager::AddShader(const std::string& shaderName, const Shader& shader)
{
    if (HasShader(shaderName))
    {
        Debug::Log("登録済みのシェーダ : " + shaderName);
        return false;
    }

    m_ShaderList.emplace(shaderName, std::make_shared<Shader>(shader));
    Debug::Log("シェーダ追加 : " + shaderName);
    return true;
}

ShaderManager::ShaderManager(std::string filePath) : m_FilePath(filePath)
{
}

bool ShaderManager::Load(const std::string& shaderName)
{
    if (HasShader(shaderName))
    {
        Debug::Log("既に登録済み : " + shaderName);
        return false;
    }

    const std::string vsPath = m_FilePath + "VS_" + shaderName + ".hlsl";
    const std::string psPath = m_FilePath + "PS_" + shaderName + ".hlsl";

    Shader shader;
    shader.Create(vsPath, psPath);
    m_ShaderList.emplace(shaderName, std::make_shared<Shader>(shader));
    Debug::Log("シェーダ読込 : " + shaderName);
    return true;
}

bool ShaderManager::LoadVertexSahder(const std::string& shaderName)
{
    if (HasShader(shaderName))
    {
        Debug::Log("既に登録済み : " + shaderName);
        return false;
    }

    const std::string vsPath = m_FilePath + "VS_" + shaderName + ".hlsl";
    const std::string psPath = m_FilePath + "PS_Default.hlsl";

    Shader shader;
    shader.Create(vsPath, psPath);
    m_ShaderList.emplace(shaderName, std::make_shared<Shader>(shader));
    Debug::Log("シェーダ読込 : " + shaderName);
    return true;
}

bool ShaderManager::LoadPixelSahder(const std::string& shaderName)
{
    if (HasShader(shaderName))
    {
        Debug::Log("既に登録済み : " + shaderName);
        return false;
    }

    const std::string vsPath = m_FilePath + "VS_" + shaderName + ".hlsl";
    const std::string psPath = m_FilePath + "PS_" + shaderName + ".hlsl";

    Shader shader;
    shader.Create(vsPath, psPath);
    m_ShaderList.emplace(shaderName, std::make_shared<Shader>(shader));
    Debug::Log("シェーダ読込 : " + shaderName);
    return true;
}

Shader* ShaderManager::GetShader(const std::string& shaderName)
{
    auto it = m_ShaderList.find(shaderName);
    if (it != m_ShaderList.end())
    {
        return it->second.get();
    }

    if (Load(shaderName))
    {
        return m_ShaderList[shaderName].get();
    }

    return nullptr;
}

bool ShaderManager::HasShader(const std::string& shaderName) const
{
    return m_ShaderList.find(shaderName) != m_ShaderList.end();
}