#include "MaterialSkybox.h"
#include "Shader.h"
#include "Context.h"
#include "Texture.h"

#include <vector>
using namespace std;

void MaterialSkybox::init(const std::string& _path, const std::string& _format) 
{
	mShaderType = SHADER_TYPE::SKYBOX;
	const std::string _shaderPath = getPathShader();
	mShaderData.mProgram = load_shaders((_shaderPath + "vertex.glsl").c_str(), (_shaderPath + "fragment.glsl").c_str());
	check();

    vector<string> _texturesCubemapPaths
    {
        _path + "right" + _format,
        _path + "left" + _format,
        _path + "top" + _format,
        _path + "bottom" + _format,
        _path + "front" + _format,
        _path + "back" + _format,
    };

    mShaderData.mAlbedoTextureLocation = getUniform("skybox");
    checkLoadTextureCubemap(mShaderData.mAlbedoTextureLocation, mMaterialData.mAlbedoTexture, _texturesCubemapPaths);
}