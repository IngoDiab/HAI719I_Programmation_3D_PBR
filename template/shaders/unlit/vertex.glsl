#version 330 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv0;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 o_positionWorld;
out vec2 o_uv0;


void main() {
  //Position
  vec4 positionWorld = model * vec4(position, 1.0);
  o_positionWorld = positionWorld.xyz;
  gl_Position = projection * view * positionWorld;

  //UVs
  o_uv0 = vec2(uv0.x, -uv0.y); //Needed because every textures are reversed on y
}
