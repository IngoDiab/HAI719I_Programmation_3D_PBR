#ifndef MATERIALSKYBOX_H
#define MATERIALSKYBOX_H

#include "Material.h"

struct MaterialSkybox : public Material
{
    virtual void init(const std::string& _path, const std::string& _format) override;
};

#endif