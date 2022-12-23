#ifndef CONTEXT_H_DEFINED
#define CONTEXT_H_DEFINED

#define NB_POINT_LIGHTS 16

#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "PointLight.h"
#include "Skybox.h"
// #include "Texture.h"

#include <glm/glm.hpp>

#include <vector>

static unsigned int SCREENWIDTH = 1600;
static unsigned int SCREENHEIGHT = 900;

struct Instance {
  glm::vec3 mPosition = glm::vec3(0,0,0);
  glm::vec3 mRotation = glm::vec3(0,0,0);
  glm::vec3 mScale = glm::vec3(1,1,1);

  glm::mat4 matrix;
  Mesh* mesh;
  Material* material;
};

struct Context {
  static Camera camera;
  static bool fullScreen;
  static bool refreshMatrices;

  static Instance mSkybox;
  static std::vector<PointLight> mPointLights;
  static std::vector<Instance> instances;
  static std::vector<Mesh*> meshes;
  static std::vector<Material*> materials;
  // std::vector<Texture*> textures;
};

void idle();
void reshape(int w, int h);
void clearContext();


#endif
