#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <glm/glm.hpp>
using namespace glm;

struct PointLight
{
    vec3 mPosition;
    vec3 mColor;
    float mPower;
    bool mbIsEnabled;

    PointLight() : mColor(vec3(1,1,1)), mPower(1){};
    PointLight(const bool& _enable, const vec3& _position, const vec3& _color = vec3(1,1,1), const float& _power = 1) :
                 mPosition(_position), mColor(_color), mPower(_power), mbIsEnabled(_enable){};
};

#endif