#version 330 core

in vec3 o_position;

out vec4 FragColor;

uniform samplerCube skybox;

void main() 
{
    FragColor = texture(skybox, o_position);
}
