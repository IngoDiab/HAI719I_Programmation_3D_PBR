#ifndef MATERIAL_H_DEFINED
#define MATERIAL_H_DEFINED
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <stdexcept>

#include <vector>
using namespace std;

enum SHADER_TYPE
{
	SKYBOX,
	UNLIT,
	LIT_PHONG,
	MIROIR,
	TRANSPARENT,
	CUSTOM_PBR,
	FILAMENT
};

struct MaterialData
{
	glm::vec3 mAmbient;
	glm::vec3 mDiffuse;
	glm::vec3 mSpecular;
	float mShininess;
	float mRefractiveIndex;


	GLint mAlbedoTexture = -1;
	//Use mDiffuse as albedo color

	GLint mNormalMap = -1;

	GLint mMetalnessTexture = -1;
	float mMetalnessValue = 1;
	
	GLint mRoughnessTexture = -1;
	float mRoughnessValue = 1;

	GLint mAOTexture = -1;
	float mAOValue = 1;

	GLint mEmissiveTexture = -1;
	float mEmissiveValue = 1;
};

struct ShaderData
{
	GLuint mProgram;
	GLint mSceneCameraLocation = -1;
	GLint mSkyboxLocation = -1;
	
	GLint mAmbientLocation = -1;
	GLint mDiffuseLocation = -1;
	GLint mSpecularLocation = -1;
	GLint mShininessLocation = -1;
	GLint mRefractiveIndexLocation = -1;

	GLint mAlbedoTextureLocation = -1;
	GLint mNormalMapLocation = -1;
	GLint mbUseNormalMap = -1;
	GLint mMetalnessTextureLocation = -1;
	GLint mMetalnessValueLocation = -1;
	GLint mRoughnessTextureLocation = -1;
	GLint mRoughnessValueLocation = -1;
	GLint mAOTextureLocation = -1;
	GLint mAOValueLocation = -1;
	GLint mEmissiveTextureLocation = -1;
	GLint mEmissiveValueLocation = -1;

	vector<GLint> pointLightsPositionLocation;
	vector<GLint> pointLightsColorLocation;
	vector<GLint> pointLightsPowerLocation;
	vector<GLint> pointLightsEnableLocation;
};

struct Material {
	// Shader program
	SHADER_TYPE mShaderType;

	//Ref on shader
	ShaderData mShaderData;

	// Material parameters
	MaterialData mMaterialData;

	inline void check() {
		if (mShaderData.mProgram == 0) {
			throw std::runtime_error("Shader program not initialized");
		}
	}

	Material() {
		mShaderData.mProgram = 0;
	}

	virtual ~Material();

	std::string getPathShader() const;

	virtual void init(const std::string& _path, const std::string& _format);

	void checkLoadTexture(const GLint& _location, GLint& _textureHandler, const std::string& _path);
	void checkLoadTextureCubemap(const GLint& _location, GLint& _textureHandler, const std::vector<std::string>& _paths);
	
	/// @brief check if we have shader location for position & color & power
	/// @param _indexLight index from Context's pointLight to test
	/// @return true if we have shader location for position & color & power & enabled
	bool checkShaderLocation(const unsigned char& _indexLight) const;

	virtual void clear();

	void bind();

	virtual void internalBind();

	bool checkSendTexture(const GLuint& _refShader, const GLuint& _textureHandler, const GLuint& _offsetTexture, const GLenum& _typeTexture);

	void setMatrices(glm::mat4& projectionMatrix, glm::mat4& viewMatrix, glm::mat4& modelMatrix);

	GLint getAttribute(const std::string& in_attributeName);

	GLint getUniform(const std::string& in_uniformName);
};

#endif
