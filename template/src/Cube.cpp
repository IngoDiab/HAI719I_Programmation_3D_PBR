#include "Cube.h"

Cube::Cube()
{

}

void Cube::Create()
{
    mNumVertices = 24;
    mNumFaces = 12; //triangles

    vector<vec3> _vertices;
    vector<unsigned int> _indices;
    CreateVertex(_vertices);
    CreateIndices(_indices);
    CreateUV();
    ComputeNormals(_vertices, _indices);
}

void Cube::CreateVertex(vector<vec3>& _vertices)
{
    center = vec3(0,0,0);

    _vertices = 
    {
        #pragma region Face Bottom
        center + vec3(-1,-1,-1), //LeftBotFar
        center + vec3(1,-1,-1), //RightBotFar
        center + vec3(-1,-1,1), //LeftBotNear
        center + vec3(1,-1,1), //RightBotNear
        #pragma endregion

        #pragma region Face Top
        center + vec3(1,1,-1), //RightTopFar
        center + vec3(-1,1,-1), //LeftTopFar
        center + vec3(1,1,1), //RightTopNear
        center + vec3(-1,1,1), //LeftTopNear
        #pragma endregion

        #pragma region Face Front
        center + vec3(-1,-1,1), //LeftBotNear
        center + vec3(1,-1,1), //RightBotNear
        center + vec3(-1,1,1), //LeftTopNear
        center + vec3(1,1,1), //RightTopNear
        #pragma endregion

        #pragma region Face Back
        center + vec3(1,1,-1), //RightTopFar
        center + vec3(-1,-1,-1), //LeftBotFar
        center + vec3(-1,1,-1), //LeftTopFar
        center + vec3(1,-1,-1), //RightBotFar
        #pragma endregion

        #pragma region Face Right
        center + vec3(1,1,-1), //RightTopFar
        center + vec3(1,-1,1), //RightBotNear
        center + vec3(1,-1,-1), //RightBotFar
        center + vec3(1,1,1), //RightTopNear
        #pragma endregion

        #pragma region Face Left
        center + vec3(-1,1,-1), //LeftTopFar
        center + vec3(-1,-1,-1), //LeftBotFar
        center + vec3(-1,-1,1), //LeftBotNear
        center + vec3(-1,1,1), //LeftTopNear
        #pragma endregion
    };

    glGenBuffers(1, &positionsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(vec3), (void*)&_vertices[0], GL_STATIC_DRAW);
}

void Cube::CreateIndices(vector<unsigned int>& _indices)
{
    _indices =
    {
        #pragma region Face Bottom
        0, 1, 2,
        1, 3, 2,
        #pragma endregion

        #pragma region Face Top
        4, 5, 6,
        5, 7, 6,
        #pragma endregion

        #pragma region Face Front
        8, 9, 10,
        9, 11, 10,
        #pragma endregion

        #pragma region Face Back
        12, 13, 14,
        12, 15, 13,
        #pragma endregion

        #pragma region Face Right
        16, 17, 18,
        16, 19, 17,
        #pragma endregion

        #pragma region Face Left
        20, 21, 22,
        20, 22, 23,
        #pragma endregion
    };

    glGenBuffers(1, &elementsBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), (void*)&_indices[0] , GL_STATIC_DRAW);
}

void Cube::CreateUV()
{
    vector<vec3> verticesUV = 
    {
        #pragma region Face Bottom
        vec3(0,0,0), //LeftBotFar
        vec3(1,0,0), //RightBotFar
        vec3(0,1,0), //LeftBotNear
        vec3(1,1,0), //RightBotNear
        #pragma endregion

        #pragma region Face Top
        vec3(1,1,0), //RightTopFar
        vec3(0,1,0), //LeftTopFar
        vec3(1,0,0), //RightTopNear
        vec3(0,0,0), //LeftTopNear
        #pragma endregion

        #pragma region Face Front
        vec3(0,0,0), //LeftBotNear
        vec3(1,0,0), //RightBotNear
        vec3(0,1,0), //LeftTopNear
        vec3(1,1,0), //RightTopNear
        #pragma endregion

        #pragma region Face Back
        vec3(0,1,0), //RightTopFar
        vec3(1,0,0), //LeftBotFar
        vec3(1,1,0), //LeftTopFar
        vec3(0,0,0), //RightBotFar
        #pragma endregion

        #pragma region Face Right
        vec3(1,1,0), //RightTopFar
        vec3(0,0,0), //RightBotNear
        vec3(1,0,0), //RightBotFar
        vec3(0,1,0), //RightTopNear
        #pragma endregion

        #pragma region Face Left
        vec3(1,1,0), //LeftTopFar
        vec3(1,0,0), //LeftBotFar
        vec3(0,0,0), //LeftBotNear
        vec3(0,1,0) //LeftTopNear
        #pragma endregion
    };
    
    glGenBuffers(1, &uv0Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, uv0Buffer);
	glBufferData(GL_ARRAY_BUFFER, verticesUV.size() * sizeof(vec3), (void*)&verticesUV[0], GL_STATIC_DRAW);
}

void Cube::ComputeNormals(const vector<vec3>& vertices, const vector<unsigned int>& _indices, const bool& _reversedNormal)
{
    vector<vec3> _verticesNormals = vector<vec3>(vertices.size());
    vector<vec3> _verticesNbNeighboors = vector<vec3>(vertices.size());

    for(unsigned int _index = 0; _index<_indices.size(); _index += 3)
    {
        const vec3 _vertex0 = vertices[_indices[_index]];
        const vec3 _vertex1 = vertices[_indices[_index+1]];
        const vec3 _vertex2 = vertices[_indices[_index+2]];

        const vec3 _edge10 = _vertex1 - _vertex0;
        const vec3 _edge20 = _vertex2 - _vertex0;

        const vec3 _normalTriangle = normalize(cross(_edge10,_edge20));

        _verticesNormals[_indices[_index]] += _normalTriangle;
        _verticesNormals[_indices[_index+1]] += _normalTriangle;
        _verticesNormals[_indices[_index+2]] += _normalTriangle;

        ++_verticesNbNeighboors[_indices[_index]];
        ++_verticesNbNeighboors[_indices[_index+1]];
        ++_verticesNbNeighboors[_indices[_index+2]];
    }

    for(unsigned int _index = 0; _index<_verticesNormals.size(); ++_index)
    {
        _verticesNormals[_index] /= _verticesNbNeighboors[_index];
        _verticesNormals[_index] = normalize(_verticesNormals[_index]);
    }
    
    glGenBuffers(1, &normalsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, _verticesNormals.size() * sizeof(vec3), (void*)&_verticesNormals[0], GL_STATIC_DRAW);
}