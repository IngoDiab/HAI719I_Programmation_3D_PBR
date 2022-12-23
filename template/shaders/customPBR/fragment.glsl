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
	vec3 ambient;

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

vec3 Fresnel_Schlick(vec3 _specularColor, vec3 _fragToLight, vec3 _halfVector)
{
    return _specularColor + (1.0 - _specularColor)*pow((1.0-(dot(_fragToLight,_halfVector))),5.0);
}

float Geometrie_Schlick_GGX(vec3 _halfVector, vec3 _fragToView, float _roughness)
{
    float _k = (1.0+_roughness)*(1.0+_roughness)/8.0;
    float _dot = dot(_halfVector,_fragToView);
    return _dot/(_dot*(1.0-_k)+_k);
}

float Distribution_GGX(vec3 _normal, vec3 _halfVector, float _roughness)
{
    float _dot = max(dot(_normal, _halfVector), 0.0);
    float _denom = _dot*_dot*(_roughness*_roughness -1) + 1;
    return _roughness * _roughness/(radians(180.0) * _denom * _denom);
}

vec3 Cook_Torrance_Microfacet(vec3 _fragToView, vec3 _fragToLight, vec3 _normal, vec3 _halfVector, vec3 _specularColor, float _roughness)
{
    vec3 _fresnel_schlick = Fresnel_Schlick(_specularColor, _fragToLight, _halfVector);
    float _geometrie_schlick_ggx = Geometrie_Schlick_GGX(_halfVector, _fragToView, _roughness);
    float _distribution_ggx = Distribution_GGX(_normal, _halfVector, _roughness);
    return  _fresnel_schlick*_geometrie_schlick_ggx*_distribution_ggx/(4*dot(_normal,_fragToLight)*dot(_normal,_fragToView));
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
    vec3 _colorAlbedo = texture(materialData.albedoTexture, o_uv0).rgb * materialData.ambient;

    vec3 _ambiant = _colorAlbedo * 0.2;

    vec3 _diffuse = vec3(0.0);

    vec3 _specular = vec3(0.0);

    vec3 _reflection = reflect(_incident, _normal);

    //Metalness
    float _metalness = texture(materialData.metalnessTexture, o_uv0).b * materialData.metalnessValue;
    vec3 _colorSpecularMetal = _colorAlbedo;

    //Reflection skybox
    vec3 _environnement = _colorSpecularMetal * texture(skybox, _reflection).rgb * _metalness;

    //Roughness
    float _roughness = texture(materialData.roughnessTexture, o_uv0).g * (materialData.roughnessValue);
    vec3 _colorSpecularDielectrics = vec3(0.1);//Plastic;   
    vec3 _colorDiffuseDielectrics = 1-_colorSpecularDielectrics;

    //Not metallic, not rough
    float _notMetallicNotRough = (1 - _roughness)*(1 - _metalness)  * (materialData.roughnessValue);
    vec3 _colorSpecular_NoDielNoMetal = vec3(0.1);//Plastic;   
    vec3 _colorDiffuse_NoDielNoMetal  = 1-_colorSpecular_NoDielNoMetal;

    for(int _indexPointLight = 0; _indexPointLight<NB_POINT_LIGHTS; ++_indexPointLight)
    {
        PointLight _pointLight = pointLights[_indexPointLight];
        if(!_pointLight.isEnabled) continue;

        //Light
        vec3 _fragToView = normalize(-_incident);
        vec3 _fragToLight = normalize(_pointLight.lightPos - o_positionWorld);
        vec3 _halfVector = normalize(_fragToView + _fragToLight);
        vec3 _lightColor = _pointLight.lightPower * _pointLight.lightColor/pow(length(_pointLight.lightPos - o_positionWorld),2.0);
        
        //Metalness Calcul
        //Specular
        vec3 _fresnel_schlick_metal = Fresnel_Schlick(_colorSpecularMetal, _fragToLight, _normal) * max(dot(_normal,_fragToLight),0.0);
        vec3 _specularMetal = _lightColor * _fresnel_schlick_metal;
    
        //Roughness Calcul
        //Diffuse
        _diffuse += _lightColor * _colorDiffuseDielectrics * _colorAlbedo * _roughness * max(dot(_normal,_fragToLight),0.0);
        //Specular
        vec3 _cook_Torrance_Specular_Dielectric = Cook_Torrance_Microfacet(_fragToView, _fragToLight, _normal, _halfVector, _colorSpecularDielectrics, _roughness);
        vec3 _specularDielec = _lightColor * _colorSpecularDielectrics * _cook_Torrance_Specular_Dielectric * max(dot(_normal,_fragToLight),0.0);

        //Not metallic, not rough
        //Diffuse
        _diffuse += _lightColor * _colorDiffuse_NoDielNoMetal * _colorAlbedo * _notMetallicNotRough * max(dot(_normal,_fragToLight),0.0);
        //Specular
        vec3 _cook_Torrance_Specular_NoDielNoMetal = Cook_Torrance_Microfacet(_fragToView, _fragToLight, _normal, _halfVector, _colorSpecularDielectrics, _roughness);
        vec3 _specularNoDielNoMetal = _lightColor * _colorSpecular_NoDielNoMetal * _cook_Torrance_Specular_NoDielNoMetal * max(dot(_normal,_fragToLight),0.0);

        //Combine all specular
        _specular += _roughness * _specularDielec + _metalness *_specularMetal + _notMetallicNotRough * _specularNoDielNoMetal;
    }
    
    //AO
    float _AO = pow(texture(materialData.AOTexture, o_uv0).r, materialData.AOValue);

    //Emissive
    vec3 _emissive = texture(materialData.emissiveTexture, o_uv0).rgb;

    vec3 _result = (_ambiant + _diffuse + _specular + _environnement) * _AO + _emissive*materialData.emissiveValue;

    //Tonemapping 
    //_result = _result/(_result + vec3(1.0));

    //Gamma
    //_result = pow(_result.rgb, vec3(1.0/2.2));
    
    //Debug Metal  
    //_result = vec3(0.0, 0.0, texture(materialData.metalnessTexture, o_uv0).b);

    //Debug Roughness  
    //_result = vec3(0.0, texture(materialData.roughnessTexture, o_uv0).g, 0.0);

    //Debug ni metal ni rough  
    //_result = vec3((1-texture(materialData.roughnessTexture, o_uv0).g)*(1 - texture(materialData.metalnessTexture, o_uv0).b), 0.0, 0.0);

    FragColor = vec4(_result,1.0);
}
