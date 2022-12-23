#ifndef SKYBOX_H
#define SKYBOX_H

#include "Cube.h"

struct Skybox : public Cube
{
    virtual void CreateIndices(vector<unsigned int>& _indices) override;
};

#endif