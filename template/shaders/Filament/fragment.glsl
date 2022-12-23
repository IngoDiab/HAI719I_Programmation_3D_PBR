#version 330 core

in vec3 o_positionWorld;
in vec3 o_normalWorld;
in vec2 o_uv0;
in mat3 o_TBN;

out vec4 FragColor;

uniform vec3 cameraPosition;
uniform samplerCube skybox;

struct PointLight
{
    vec3 lightPos;
    vec3 lightColor;
    float lightPower;
    bool isEnabled;
};
#define NB_POINT_LIGHTS 16
uniform PointLight pointLights[NB_POINT_LIGHTS];

struct MaterialData
{
	sampler2D albedoTexture;
	vec3 diffuse;

    sampler2D normalMap;
    bool useNormalMap;

    sampler2D metalnessTexture;
	float metalnessValue;
	
	sampler2D roughnessTexture;
	float roughnessValue;

	sampler2D AOTexture;
	float AOValue;

	sampler2D emissiveTexture;
	float emissiveValue;
};
uniform MaterialData materialData;

vec3 Fresnel_Schlick(float _dotLH, vec3 _colorAtNormalIncidence, vec3 _colorAtGrazingAngles)
{
    return _colorAtNormalIncidence + (_colorAtGrazingAngles - _colorAtNormalIncidence)*pow(1.0-(_dotLH),5.0);
}

float Geometrie_Smith_GGX(float _NoV, float _NoL, float _roughness)
{
    float _GGXV = _NoL * (_NoV * (1.0 - _roughness) + _roughness);
    float _GGXL = _NoV * (_NoL * (1.0 - _roughness) + _roughness);
    return 0.5 / (_GGXV + _GGXL);
}

float Geometrie_Schlick_GGX(vec3 _halfVector, vec3 _fragToView, float _roughness)
{
    float _k = (1.0+_roughness)*(1.0+_roughness)/8.0;
    float _dot = dot(_halfVector,_fragToView);
    return _dot/(_dot*(1.0-_k)+_k);
}

#define MEDIUMP_FLT_MAX    65504.0
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)
float Distribution_GGX(float _roughness, float _NoH, vec3 _nomal, vec3 _halfVector)
{
    vec3 _NxH = cross(_nomal, _halfVector);
    float _a = _NoH * _roughness;
    float _k = _roughness / (dot(_NxH, _NxH) + _a * _a);
    float _d = _k * _k * (1.0 / radians(180.0));
    return saturateMediump(_d);
}

float Visibility_Kelemen(float LoH) {
    return 0.25 / (LoH * LoH);
}

vec3 BRDF(vec3 _normal, vec3 _fragToLight, vec3 _fragToView, float _roughness, vec3 _colorSpecular)
{
    vec3 _halfVector = normalize(_fragToView + _fragToLight);

    float _NoV = abs(dot(_normal, _fragToView)) + 0.00001;
    float _NoL = clamp(dot(_normal, _fragToLight), 0.0, 1.0);
    float _NoH = clamp(dot(_normal, _halfVector), 0.0, 1.0);
    float _LoH = clamp(dot(_fragToLight, _halfVector), 0.0, 1.0);

    float _realRoughness = _roughness * _roughness;

    float _distribution_ggx = Distribution_GGX(_realRoughness, _NoH, _normal, _halfVector);
    vec3 _fresnel_schlick = Fresnel_Schlick(_LoH, _colorSpecular, vec3(1.0));
    //float _geometrie_smith_ggx = Geometrie_Smith_GGX(_NoV, _NoL, _realRoughness);
    float _geometrie_schlick_ggx = Geometrie_Schlick_GGX(_halfVector, _fragToView, _realRoughness);

    vec3 _diffuse = (1-_colorSpecular)/radians(180.0);

    //vec3 _specular = _distribution_ggx*_geometrie_smith_ggx*_fresnel_schlick;
    vec3 _specular = _distribution_ggx*_geometrie_schlick_ggx*_fresnel_schlick;
    
    //return _diffuse + _specular;

    //clearCoat
    float clearCoatPerceptualRoughness = 0.5;
    float clearCoatRoughness = clearCoatPerceptualRoughness*clearCoatPerceptualRoughness;

    float _distribution_ggx_coat = Distribution_GGX(clearCoatRoughness, _NoH, _normal, _halfVector);
    vec3 _fresnel_schlick_coat = Fresnel_Schlick(_LoH, vec3(0.04), vec3(1.0));
    float _visibility_kelemen_coat = Visibility_Kelemen(_LoH);
 
    vec3 _specularCoat = _distribution_ggx_coat * _fresnel_schlick_coat * _visibility_kelemen_coat;

    return _colorSpecular * ((_diffuse + _specular * (1.0 - _fresnel_schlick_coat)) * (1.0 - _fresnel_schlick) + _specularCoat);
}

void main() 
{
    vec3 _incident = normalize(o_positionWorld-cameraPosition);

    //Normal
    vec3 _normal = o_normalWorld;
    if(materialData.useNormalMap)
    {
        vec3 texelNormalMap = texture(materialData.normalMap, o_uv0).rgb*2.0-1.0;
        _normal = normalize(o_TBN * texelNormalMap);
    }
    vec3 _colorAlbedo = texture(materialData.albedoTexture, o_uv0).rgb * materialData.diffuse;

    vec3 _ambiant = _colorAlbedo * 0.2;
    
    vec3 _reflection = reflect(_incident, _normal);

    //Metalness
    float _metalness = texture(materialData.metalnessTexture, o_uv0).b * materialData.metalnessValue;
    vec3 _colorSpecularMetal = _colorAlbedo;

    //Reflection skybox
    vec3 _environnement = _colorSpecularMetal * texture(skybox, _reflection).rgb * _metalness;

    //Roughness
    float _roughness = texture(materialData.roughnessTexture, o_uv0).g * (materialData.roughnessValue);
    vec3 _colorSpecularDielectrics = vec3(0.045); //Plastic
    vec3 _colorDiffuseDielectrics = 1 - _colorSpecularDielectrics;

    vec3 _brdf = vec3(0.0);
    for(int _indexPointLight = 0; _indexPointLight<NB_POINT_LIGHTS; ++_indexPointLight)
    {
        PointLight _pointLight = pointLights[_indexPointLight];
        if(!_pointLight.isEnabled) continue;

        //Light
        vec3 _fragToView = normalize(-_incident);
        vec3 _fragToLight = normalize(_pointLight.lightPos - o_positionWorld);
        vec3 _halfVector = normalize(_fragToView + _fragToLight);
        vec3 _lightColor = _pointLight.lightPower * _pointLight.lightColor/pow(length(_pointLight.lightPos - o_positionWorld),2.0);

        _brdf += _lightColor * BRDF(_normal, _fragToLight, _fragToView, _roughness, _colorSpecularDielectrics) * max(dot(_normal,_halfVector),0.0); 
    }
    
    //AO
    float _AO = texture(materialData.AOTexture, o_uv0).r;

    //Emissive
    vec3 _emissive = texture(materialData.emissiveTexture, o_uv0).rgb;

    vec3 _result = (_ambiant + _brdf + _environnement) * pow(_AO,materialData.AOValue) + _emissive*materialData.emissiveValue;

    FragColor = vec4(_result,1.0);
}
