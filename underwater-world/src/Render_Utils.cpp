#include "Render_Utils.h"

#include <algorithm>

#include "glew.h"
#include "freeglut.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


void Core::RenderContext::initFromOBJ(obj::Model& model)
{
    vertexArray = 0;
    vertexBuffer = 0;
    vertexIndexBuffer = 0;
    unsigned int vertexDataBufferSize = sizeof(float) * model.vertex.size();
    unsigned int vertexNormalBufferSize = sizeof(float) * model.normal.size();
    unsigned int vertexTexBufferSize = sizeof(float) * model.texCoord.size();

    size = model.faces["default"].size();
    unsigned int vertexElementBufferSize = sizeof(unsigned short) * size;


    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);


    glGenBuffers(1, &vertexIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexElementBufferSize, &model.faces["default"][0], GL_STATIC_DRAW);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    glBufferData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize, NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataBufferSize, &model.vertex[0]);

    glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize, vertexNormalBufferSize, &model.normal[0]);

    glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize, vertexTexBufferSize, &model.texCoord[0]);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertexDataBufferSize));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(vertexNormalBufferSize + vertexDataBufferSize));
}


void Core::RenderContext::initFromAssimpMesh(aiMesh* mesh) {
    vertexArray = 0;
    vertexBuffer = 0;
    vertexIndexBuffer = 0;

    std::vector<float> textureCoord;
    std::vector<unsigned int> indices;
    //tex coord must be converted to 2d vecs
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        if (mesh->mTextureCoords[0] != nullptr) {
            textureCoord.push_back(mesh->mTextureCoords[0][i].x);
            textureCoord.push_back(mesh->mTextureCoords[0][i].y);
        }
        else {
            textureCoord.push_back(0.0f);
            textureCoord.push_back(0.0f);
        }
    }
    if (mesh->mTextureCoords[0] == nullptr) {
        std::cout << "no uv coords\n";
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    unsigned int vertexDataBufferSize = sizeof(float) * mesh->mNumVertices * 3;
    unsigned int vertexNormalBufferSize = sizeof(float) * mesh->mNumVertices * 3;
    unsigned int vertexTexBufferSize = sizeof(float) * mesh->mNumVertices * 2;
    unsigned int vertexTangentBufferSize = sizeof(float) * mesh->mNumVertices * 3;
    unsigned int vertexBiTangentBufferSize = sizeof(float) * mesh->mNumVertices * 3;

    unsigned int vertexElementBufferSize = sizeof(unsigned int) * indices.size();
    size = indices.size();

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);


    glGenBuffers(1, &vertexIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexElementBufferSize, &indices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    //std::cout << vertexBuffer;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glBufferData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize + vertexTangentBufferSize + vertexBiTangentBufferSize, NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataBufferSize, mesh->mVertices);

    glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize, vertexNormalBufferSize, mesh->mNormals);

    glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize, vertexTexBufferSize, &textureCoord[0]);

    glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize, vertexTangentBufferSize, mesh->mTangents);

    glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize + vertexTangentBufferSize, vertexBiTangentBufferSize, mesh->mBitangents);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertexDataBufferSize));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(vertexNormalBufferSize + vertexDataBufferSize));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize + vertexTangentBufferSize));

}

void Core::DiffuseMaterial::init_data() {
    glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
    Core::SetActiveTexture(texture, "color_texture", program, 0);
}

void Core::DiffuseSpecularMaterial::init_data() {
    glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
    Core::SetActiveTexture(texture, "color_texture", program, 0);
    Core::SetActiveTexture(textureSpecular, "specular_texture", program, 1);
}

void Core::RenderContext::render()
{

    glBindVertexArray(this->vertexArray);
    glDrawElements(
        GL_TRIANGLES,      // mode
        this->size,    // count
        GL_UNSIGNED_INT,   // type
        (void*)0           // element array buffer offset
    );
    glBindVertexArray(0);
}


void Core::DrawVertexArray(const float * vertexArray, int numVertices, int elementSize )
{
	glVertexAttribPointer(0, elementSize, GL_FLOAT, false, 0, vertexArray);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void Core::DrawVertexArrayIndexed( const float * vertexArray, const int * indexArray, int numIndexes, int elementSize )
{
	glVertexAttribPointer(0, elementSize, GL_FLOAT, false, 0, vertexArray);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, indexArray);
}


void Core::DrawVertexArray( const VertexData & data )
{
	int numAttribs = std::min(VertexData::MAX_ATTRIBS, data.NumActiveAttribs);
	for(int i = 0; i < numAttribs; i++)
	{
		glVertexAttribPointer(i, data.Attribs[i].Size, GL_FLOAT, false, 0, data.Attribs[i].Pointer);
		glEnableVertexAttribArray(i);
	}
	glDrawArrays(GL_TRIANGLES, 0, data.NumVertices);
}

void Core::DrawContext(Core::RenderContext& context)
{

	glBindVertexArray(context.vertexArray);
	glDrawElements(
		GL_TRIANGLES,      // mode
		context.size,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);
	glBindVertexArray(0);
}

void Core::initRay(RayContext& rayContext) {
    rayContext.size = 2;
    glGenVertexArrays(1, &rayContext.vertexArray);
    glBindVertexArray(rayContext.vertexArray);

    glGenBuffers(1, &rayContext.vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, rayContext.vertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, 3 * 7 * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
    glBindVertexArray(0);
}
void Core::updateRayPos(RayContext& rayContext, std::vector<glm::vec3> ray) {
    glBindVertexArray(rayContext.vertexArray);
    std::vector<glm::vec3> keyPoints;
    float offset = 4.f;
    float scale = 0.2f;
    float rayEnd = 50.f;
    keyPoints.push_back(ray[0] + ray[1] * offset);
    keyPoints.push_back(ray[0] + ray[1] * rayEnd);

    keyPoints.push_back(ray[0] + ray[1] * offset + scale * glm::vec3(1.f, 1.f, 0.f));
    keyPoints.push_back(ray[0] + ray[1] * offset - scale * glm::vec3(1.f, 1.f, 0.f));
    keyPoints.push_back(ray[0] + ray[1] * offset + scale * glm::vec3(1.f, -1.f, 0.f));
    keyPoints.push_back(ray[0] + ray[1] * offset - scale * glm::vec3(1.f, -1.f, 0.f));

    keyPoints.push_back(ray[0] + ray[1] * offset + scale * glm::vec3(1.f, 1.f, 0.f));
    keyPoints.push_back(ray[0] + ray[1] * rayEnd * scale);
    keyPoints.push_back(ray[0] + ray[1] * offset - scale * glm::vec3(1.f, 1.f, 0.f));
    keyPoints.push_back(ray[0] + ray[1] * rayEnd * scale);
    keyPoints.push_back(ray[0] + ray[1] * offset + scale * glm::vec3(1.f, -1.f, 0.f));
    keyPoints.push_back(ray[0] + ray[1] * rayEnd * scale);
    keyPoints.push_back(ray[0] + ray[1] * offset - scale * glm::vec3(1.f, -1.f, 0.f));
    keyPoints.push_back(ray[0] + ray[1] * rayEnd * scale);
    glBindBuffer(GL_ARRAY_BUFFER, rayContext.vertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, keyPoints.size() * 3 * sizeof(float), &keyPoints[0]);
    glBindVertexArray(0);
}
