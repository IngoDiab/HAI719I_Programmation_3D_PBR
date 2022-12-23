#version 330 core

//In
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv0;

//Matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//Out
out vec3 o_positionWorld;
out vec3 o_normalWorld;
out vec2 o_uv0;

out mat3 o_TBN;


void main() {
  //Normal
  mat3 normalMatrix = mat3(transpose(inverse(model)));
  o_normalWorld = normalize(normalMatrix * normal);

  //Bitangent
  vec3 bitangent = normalize(cross(normal, tangent));

  //TBN
  o_TBN = mat3(normalize(normalMatrix*tangent),normalize(normalMatrix*bitangent),normalize(normalMatrix*normal));

  //Vertex pos
  vec4 positionWorld = model * vec4(position, 1.0);
  o_positionWorld = positionWorld.xyz;
  gl_Position = projection * view * positionWorld;

  //UV
  o_uv0 = vec2(uv0.x, -uv0.y); //Needed because every textures are reversed on y
}
