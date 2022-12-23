#ifndef CUBE_H
#define CUBE_H

#include "Mesh.h"

#include <vector>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

struct Cube : public Mesh
{
    Cube();

    void Create();
    void CreateVertex(vector<vec3>& _vertices);
    virtual void CreateIndices(vector<unsigned int>& _indices);
    void CreateUV();
    void ComputeNormals(const vector<vec3>& vertices, const vector<unsigned int>& _indices, const bool& _reversedNormal = false);
};

#endif