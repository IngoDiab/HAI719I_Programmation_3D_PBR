#version 330 core

in vec2 o_uv0;

out vec4 FragColor;

struct MaterialData
{
	vec3 diffuse;

	sampler2D albedoTexture;
};

uniform MaterialData materialData;

void main() 
{
    FragColor = vec4(texture(materialData.albedoTexture, o_uv0).rgb * materialData.diffuse,1.0);
}
