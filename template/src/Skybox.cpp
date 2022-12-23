#include "Skybox.h"

void Skybox::CreateIndices(vector<unsigned int>& _indices)
{
    _indices =
    {
        #pragma region Face Bottom
        2, 1, 0,
        2, 3, 1,
        #pragma endregion

        #pragma region Face Top
        6, 5, 4,
        6, 7, 5,
        #pragma endregion

        #pragma region Face Front
        10, 9, 8,
        10, 11, 9,
        #pragma endregion

        #pragma region Face Back
        14, 13, 12,
        13, 15, 12,
        #pragma endregion

        #pragma region Face Right
        18, 17, 16,
        17, 19, 16,
        #pragma endregion

        #pragma region Face Left
        22, 21, 20,
        23, 22, 20,
        #pragma endregion
    };

    glGenBuffers(1, &elementsBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), (void*)&_indices[0] , GL_STATIC_DRAW);
}