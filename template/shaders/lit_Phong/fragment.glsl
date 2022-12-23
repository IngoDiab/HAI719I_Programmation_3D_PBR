#version 330 core

in vec3 o_positionWorld;
in vec3 o_normalWorld;
in vec2 o_uv0;
in mat3 o_TBN;

out vec4 FragColor;

uniform vec3 cameraPosition;

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
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

    float shininess;

	sampler2D albedoTexture;
    sampler2D normalMap;
    bool useNormalMap;
};
uniform MaterialData materialData;

vec3 Phong(PointLight _pointLight, vec3 _normal)
{
    vec3 _lightColor = _pointLight.lightPower*_pointLight.lightColor/(length(_pointLight.lightPos - o_positionWorld)*length(_pointLight.lightPos - o_positionWorld));

    vec3 _lightDirection = normalize(_pointLight.lightPos - o_positionWorld);
    float _clampedDotDiffuse = max(dot(_lightDirection, _normal), 0.0);
    vec3 _diffuse = materialData.diffuse * _clampedDotDiffuse * _lightColor;

    vec3 _viewDirection = normalize(cameraPosition-o_positionWorld);
    vec3 _reflectDirection = reflect(-_lightDirection,_normal);
    float _clampedDotSpecular = pow(max(dot(_viewDirection, _reflectDirection), 0.0), materialData.shininess);
    vec3 _specular = materialData.specular * _clampedDotSpecular * _pointLight.lightPower*_lightColor;

    return _diffuse + _specular;
}

void main() 
{
    vec3 _normal = o_normalWorld;
    if(materialData.useNormalMap){
    //Calculate normal
    vec3 texelNormalMap = texture(materialData.normalMap, o_uv0).rgb*2.0-1.0;
    _normal = normalize(o_TBN * texelNormalMap);
    }

    vec3 _resultPhong = vec3(0,0,0);
    for(int _indexPointLight = 0; _indexPointLight<NB_POINT_LIGHTS; ++_indexPointLight)
    {
        PointLight _pointLight = pointLights[_indexPointLight];
        if(!_pointLight.isEnabled) continue;
        _resultPhong += Phong(_pointLight, _normal);
    }
    
    vec3 _result = materialData.ambient + _resultPhong;
    FragColor = vec4(texture(materialData.albedoTexture, o_uv0).rgb * _result,1.0);
}
