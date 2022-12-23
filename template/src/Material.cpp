// Local includes
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "Context.h"
// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
// OPENGL includes
#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>

Material::~Material() {
	if (mShaderData.mProgram != 0) {
		glDeleteProgram(mShaderData.mProgram);
	}
}

std::string Material::getPathShader() const
{
	std::string _shaderPath = "../shaders/";
	switch (mShaderType)
	{
	case SHADER_TYPE::SKYBOX:
		_shaderPath += "skybox/";
		return _shaderPath;
		break;

	case SHADER_TYPE::UNLIT:
		_shaderPath += "unlit/";
		return _shaderPath;
		break;

	case SHADER_TYPE::LIT_PHONG:
		_shaderPath += "lit_Phong/";
		return _shaderPath;
		break;

	case SHADER_TYPE::MIROIR:
		_shaderPath += "miroir/";
		return _shaderPath;
		break;

	case SHADER_TYPE::TRANSPARENT:
		_shaderPath += "transparent/";
		return _shaderPath;
		break;

	case SHADER_TYPE::CUSTOM_PBR:
		_shaderPath += "customPBR/";
		return _shaderPath;
		break;

	case SHADER_TYPE::FILAMENT:
		_shaderPath += "Filament/";
		return _shaderPath;
		break;
	
	default:
		throw std::runtime_error("Shader type unknown");
		break;
	}
}

void Material::init(const std::string& _path, const std::string& _format) {
	// TODO : Change shader by your
	//Load shader
	const std::string _shaderPath = getPathShader();
	mShaderData.mProgram = load_shaders((_shaderPath + "vertex.glsl").c_str(), (_shaderPath + "fragment.glsl").c_str());
	check();

	//Get Locations Colors
	mShaderData.mAmbientLocation = getUniform("materialData.ambient");
	mShaderData.mDiffuseLocation = getUniform("materialData.diffuse");
	mShaderData.mSpecularLocation = getUniform("materialData.specular");
	mShaderData.mShininessLocation = getUniform("materialData.shininess");
	mShaderData.mRefractiveIndexLocation = getUniform("materialData.refractiveIndex");


	//Get Locations Textures
	//Albedo
	mShaderData.mAlbedoTextureLocation = getUniform("materialData.albedoTexture");
	//Normal
	mShaderData.mNormalMapLocation = getUniform("materialData.normalMap");
	mShaderData.mbUseNormalMap = getUniform("materialData.useNormalMap");
	//Metalness
	mShaderData.mMetalnessTextureLocation = getUniform("materialData.metalnessTexture");
	mShaderData.mMetalnessValueLocation = getUniform("materialData.metalnessValue");
	//Roughness
	mShaderData.mRoughnessTextureLocation = getUniform("materialData.roughnessTexture");
	mShaderData.mRoughnessValueLocation = getUniform("materialData.roughnessValue");
	//AO
	mShaderData.mAOTextureLocation = getUniform("materialData.AOTexture");
	mShaderData.mAOValueLocation = getUniform("materialData.AOValue");
	//Emissive
	mShaderData.mEmissiveTextureLocation = getUniform("materialData.emissiveTexture");
	mShaderData.mEmissiveValueLocation = getUniform("materialData.emissiveValue");


	//Get Location Camera position
	mShaderData.mSceneCameraLocation = getUniform("cameraPosition");

	//Get Location Skybox
	mShaderData.mSkyboxLocation = getUniform("skybox");

	//Get Location Point Lights
	const unsigned int _nbPointLights = Context::mPointLights.size();
	for(unsigned int _indexPointLight = 0; _indexPointLight < _nbPointLights; ++_indexPointLight)
	{
		mShaderData.pointLightsPositionLocation.push_back(getUniform("pointLights["+to_string(_indexPointLight)+"].lightPos"));
		mShaderData.pointLightsColorLocation.push_back(getUniform("pointLights["+to_string(_indexPointLight)+"].lightColor"));
		mShaderData.pointLightsPowerLocation.push_back(getUniform("pointLights["+to_string(_indexPointLight)+"].lightPower"));
		mShaderData.pointLightsEnableLocation.push_back(getUniform("pointLights["+to_string(_indexPointLight)+"].isEnabled"));
	}

	const std::string _texturesPath = _path.substr(0, _path.find_last_of('/'))+"/Textures/";

	//Load textures if needed
	checkLoadTexture(mShaderData.mAlbedoTextureLocation, mMaterialData.mAlbedoTexture, _texturesPath + "albedo" + _format);
	checkLoadTexture(mShaderData.mNormalMapLocation, mMaterialData.mNormalMap, _texturesPath + "normalMap" + _format);
	checkLoadTexture(mShaderData.mMetalnessTextureLocation, mMaterialData.mMetalnessTexture, _texturesPath + "metalRoughness" + _format); //blue metalness (damaged helmet)
	checkLoadTexture(mShaderData.mRoughnessTextureLocation, mMaterialData.mRoughnessTexture, _texturesPath + "metalRoughness" + _format); //green roughness (damaged helmet)
	checkLoadTexture(mShaderData.mAOTextureLocation, mMaterialData.mAOTexture, _texturesPath + "AO" + _format);
	checkLoadTexture(mShaderData.mEmissiveTextureLocation, mMaterialData.mEmissiveTexture, _texturesPath + "emissive" + _format);
}

void Material::checkLoadTexture(const GLint& _location, GLint& _textureHandler, const std::string& _path)
{
	if(_location == -1) return;
	_textureHandler = loadTexture2DFromFilePath(_path);
}

void Material::checkLoadTextureCubemap(const GLint& _location, GLint& _textureHandler, const std::vector<std::string>& _paths)
{
	if(_location == -1) return;
	_textureHandler = loadTextureCubeMap2DFromFilePath(_paths);
}

void Material::clear() {
	// nothing to clear
	// TODO: Add the texture or buffer you want to destroy for your material
}

void Material::bind() {
	check();
	glUseProgram(mShaderData.mProgram);
	internalBind();
}

void Material::internalBind() {
	// bind parameters
	//Send colors
	glUniform3fv(mShaderData.mAmbientLocation, 1, glm::value_ptr(mMaterialData.mAmbient));
	glUniform3fv(mShaderData.mDiffuseLocation, 1, glm::value_ptr(mMaterialData.mDiffuse));
	glUniform3fv(mShaderData.mSpecularLocation, 1, glm::value_ptr(mMaterialData.mSpecular));
	glUniform1f(mShaderData.mShininessLocation, mMaterialData.mShininess);
	glUniform1f(mShaderData.mRefractiveIndexLocation, mMaterialData.mRefractiveIndex);

	//Send scene camera position
	glUniform3fv(mShaderData.mSceneCameraLocation, 1, glm::value_ptr(Context::camera.position));

	//Send scene point lights
	const unsigned char _nbPointLights = Context::mPointLights.size();
	for(unsigned short _indexPointLight = 0; _indexPointLight < _nbPointLights; ++_indexPointLight)
	{
		if(!checkShaderLocation(_indexPointLight)) break;
		const PointLight& _pointLight = Context::mPointLights[_indexPointLight];
		glUniform3fv(mShaderData.pointLightsPositionLocation[_indexPointLight], 1, glm::value_ptr(_pointLight.mPosition));
		glUniform3fv(mShaderData.pointLightsColorLocation[_indexPointLight], 1, glm::value_ptr(_pointLight.mColor));
		glUniform1f(mShaderData.pointLightsPowerLocation[_indexPointLight], _pointLight.mPower);
		glUniform1i(mShaderData.pointLightsEnableLocation[_indexPointLight], _pointLight.mbIsEnabled);
	}

	//Send textures if needed
	glActiveTexture(GL_TEXTURE0);
	checkSendTexture(mShaderData.mAlbedoTextureLocation, mMaterialData.mAlbedoTexture, 0, GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	bool _normalMapSent = checkSendTexture(mShaderData.mNormalMapLocation, mMaterialData.mNormalMap, 1, GL_TEXTURE_2D);
	if(_normalMapSent) glUniform1i(mShaderData.mbUseNormalMap, true);

	glActiveTexture(GL_TEXTURE2);
	bool _metalnessSent = checkSendTexture(mShaderData.mMetalnessTextureLocation, mMaterialData.mMetalnessTexture, 2, GL_TEXTURE_2D);
	if(_metalnessSent) glUniform1f(mShaderData.mMetalnessValueLocation, mMaterialData.mMetalnessValue);

	glActiveTexture(GL_TEXTURE3);
	bool _roughnessSent = checkSendTexture(mShaderData.mRoughnessTextureLocation, mMaterialData.mRoughnessTexture, 3, GL_TEXTURE_2D);
	if(_roughnessSent) glUniform1f(mShaderData.mRoughnessValueLocation, mMaterialData.mRoughnessValue);

	glActiveTexture(GL_TEXTURE4);
	bool _AOSent = checkSendTexture(mShaderData.mAOTextureLocation, mMaterialData.mAOTexture, 4, GL_TEXTURE_2D);
	if(_AOSent) glUniform1f(mShaderData.mAOValueLocation, mMaterialData.mAOValue);

	glActiveTexture(GL_TEXTURE5);
	bool _emissiveSent = checkSendTexture(mShaderData.mEmissiveTextureLocation, mMaterialData.mEmissiveTexture, 5, GL_TEXTURE_2D);
	if(_emissiveSent) glUniform1f(mShaderData.mEmissiveValueLocation, mMaterialData.mEmissiveValue);

	//Send scene skybox
	const Material* _materialSkybox = Context::mSkybox.material;
	if(_materialSkybox) 
	{
		glActiveTexture(GL_TEXTURE6);
		checkSendTexture(mShaderData.mSkyboxLocation, _materialSkybox->mMaterialData.mAlbedoTexture, 6, GL_TEXTURE_CUBE_MAP);
	}
}

bool Material::checkSendTexture(const GLuint& _refShader, const GLuint& _textureHandler, const GLuint& _offsetTexture, const GLenum& _typeTexture)
{
	check();
	if(_refShader == -1 || _textureHandler == -1) return false;
	glBindTexture(_typeTexture, _textureHandler);
	//glActiveTexture(GL_TEXTURE0+_offsetTexture); //not working
	glUniform1i(_refShader, _offsetTexture);
	return true;
}

bool Material::checkShaderLocation(const unsigned char& _indexLight) const
{
	return _indexLight < mShaderData.pointLightsPositionLocation.size() && 
		   _indexLight < mShaderData.pointLightsPositionLocation.size() && 
		   _indexLight < mShaderData.pointLightsPositionLocation.size() &&
		   _indexLight < mShaderData.pointLightsEnableLocation.size();
}


void Material::setMatrices(glm::mat4& projectionMatrix, glm::mat4& viewMatrix, glm::mat4& modelMatrix)
{
	check();
	glUniformMatrix4fv(getUniform("projection"), 1, false, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(getUniform("view"), 1, false, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(getUniform("model"), 1, false, glm::value_ptr(modelMatrix));
}

GLint Material::getAttribute(const std::string& in_attributeName) {
	check();
	return glGetAttribLocation(mShaderData.mProgram, in_attributeName.c_str());
}

GLint Material::getUniform(const std::string& in_uniformName) {
	check();
	return glGetUniformLocation(mShaderData.mProgram, in_uniformName.c_str());
}
