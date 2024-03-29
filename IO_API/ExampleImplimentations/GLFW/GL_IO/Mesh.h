#pragma once
#include "DArray.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "Shapes.h"
#include "Shader.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <OBJ_Loader.h>

GLfloat cube_vertices[] = {
    // front
    -1.0, -1.0,  1.0,   0.0f, 0.0f,
     1.0, -1.0,  1.0,   0.0f, 0.0f,
     1.0,  1.0,  1.0,   0.0f, 0.0f,
    -1.0,  1.0,  1.0,   0.0f, 0.0f,
    // top
    -1.0,  1.0,  1.0,   0.0f, 0.0f,
     1.0,  1.0,  1.0,   0.0f, 0.0f,
     1.0,  1.0, -1.0,   0.0f, 0.0f,
    -1.0,  1.0, -1.0,   0.0f, 0.0f,
    // back
     1.0, -1.0, -1.0,   0.0f, 0.0f,
    -1.0, -1.0, -1.0,   0.0f, 0.0f,
    -1.0,  1.0, -1.0,   0.0f, 0.0f,
     1.0,  1.0, -1.0,   0.0f, 0.0f,
     // bottom
     -1.0, -1.0, -1.0,  0.0f, 0.0f,
      1.0, -1.0, -1.0,  0.0f, 0.0f,
      1.0, -1.0,  1.0,  0.0f, 0.0f,
     -1.0, -1.0,  1.0,  0.0f, 0.0f,
     // left
     -1.0, -1.0, -1.0,  0.0f, 0.0f,
     -1.0, -1.0,  1.0,  0.0f, 0.0f,
     -1.0,  1.0,  1.0,  0.0f, 0.0f,
     -1.0,  1.0, -1.0,  0.0f, 0.0f,
     // right
      1.0, -1.0,  1.0,  0.0f, 0.0f,
      1.0, -1.0, -1.0,  0.0f, 0.0f,
      1.0,  1.0, -1.0,  0.0f, 0.0f,
      1.0,  1.0,  1.0,  0.0f, 0.0f
};
uint16_t cube_indices[24] = {
    0, 1, 2, 3,
    4, 5, 6, 7,
    8, 9, 10, 11,
    12, 13, 14, 15,
    16, 17, 18, 19,
    20, 21, 22, 23
};
uint32_t cube_vertCount = 24;
uint32_t cube_indexCount = 24;

struct TEXTURE_ENUM {
    enum TYPE {
        DIFFUSE,
        BUMP
    };
};

struct SubMesh {
    VertexBuffer vertexBuffer;
    IndexBuffer indexBuffer;
    VertexBufferLayout vertexLayout;
    VertexArray vertexArray;
    Shader shader;
    std::vector<Texture> textures;
    //float colors[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    Pos3D pos = { .5, .5, 0 };
    float siz[3];
    float rot[2];  //ON HOLD

    void init(const Vertex3D* verticies, const void* indices, uint32_t vertCount, uint32_t indexCount,
            const std::vector<Texture>& _textures) {
        vertexBuffer.init(verticies, vertCount * sizeof(Vertex3D));
        if(vertCount <= 256*256)
            indexBuffer.init(indices, indexCount, 2);
        else
            indexBuffer.init(indices, indexCount, 4);
        
        //vertexBuffer.init(cube_vertices, cube_vertCount * sizeof(Vertex3D));
        //indexBuffer.init(cube_indices, cube_indexCount, 2);
        vertexArray.init();
        vertexLayout.push<float>(3);
        vertexLayout.push<float>(2);
        vertexArray.addBuffer(vertexBuffer, vertexLayout);
        std::string shaderPath = IO_getDirData(); shaderPath += "ShadersGL/BasicMesh.shader";
        shader.init(shaderPath);
        textures.push_back(Texture());
        std::string texturePath = IO_getDirData(); texturePath += "Textures/RightArrow.png";
        textures[0].init(texturePath);
        textures[0].bind();
        shader.setUniform1i("u_texture", 0);
        shader.setUniform4f("u_color", 1, 1, 1, 1);
        shader.setUniformMat4f("u_MVP", {});
    }
    void destruct() {
        vertexBuffer.destruct();
        indexBuffer.destruct();
        vertexArray.destruct();
        shader.destruct();
        for(auto i = 0; i < textures.size(); i++)
            textures[i].destruct();
    }
    void draw() {
        vertexBuffer.bind();
        indexBuffer.bind();
        //for(uint32_t i = 0; i < textures.size(); i++)
            //textures[i].bind(i);
        textures[0].bind();
        //shader.setUniform4f("u_color", colors[0], colors[1], colors[2], colors[3]);
        shader.setUniform4f("u_color", 1, 1, 1, 1);
        shader.setUniform1i("u_texture", 0);
        //glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
        //glm::mat4 newMVP = glm::scale(MVP, {0.8f, 0.8f, 0.8f});
        //glm::mat4 newMVP = glm::scale(MVP, { 1, 1, 1 });
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, { 0.0f, 0.0f, 2.0f });
        static float y = 1.0f;
        model = glm::rotate(model, glm::radians(y), { 0.0f, 1.0f, 0.0f });
        model = glm::rotate(model, glm::radians(y), { 0.0f, 0.0f, 1.0f });
        y += 0.1;
        model = glm::scale(model, { 0.01f, 0.01f, 0.01f });
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, { 0.0f, 0.0f, -3.0f });
        //view = glm::rotate(view, glm::radians(y), { 0.0f, 1.0f, 0.0f });
        unsigned int winWidth, winHeight;
        IO_getCanvasSize(&winWidth, &winHeight);
        glm::mat4 perspective = glm::perspective(45.0f, (GLfloat)winWidth / (GLfloat)winHeight, 1.0f, 150.0f);
        //glm::mat4 mvp = model * view * perspective;
        glm::mat4 mvp = perspective * view * model;
        shader.setUniformMat4f("u_MVP", mvp);
        shader.bind();
        vertexArray.bind();

        glDrawElements(GL_TRIANGLES, indexBuffer.getCount(), GL_UNSIGNED_SHORT, nullptr);
    }
};

struct Mesh {
    std::vector<SubMesh> subMeshes;

    void init(const char* filepath);
    void destruct();
    void draw();
private:
    SubMesh loadSubMeshData(const objl::Mesh& mesh);
};

void Mesh::init(const char* filepath) {
    //Vertex3D* vertices;
    //void* indices;
    //uint32_t vertCount;
    //uint32_t indexCount;

    objl::Loader loader;
    loader.LoadFile(filepath);
    auto& meshes = loader.LoadedMeshes;
    loader.LoadedIndices;
    meshes[0].MeshMaterial;

    for (auto& mesh : meshes) {
        //subMeshes.push_back(SubMesh());
        //SubMesh* subMeshPtr = &subMeshes[subMeshes.size()-1];
        //std::vector<Vertex3D> vertices;
        //std::vector<void*> indices;
        //loadSubMeshData(mesh, vertices, indices);
        subMeshes.push_back(loadSubMeshData(mesh));
    }
}
void Mesh::destruct() {
    for (uint32_t i = 0; i < subMeshes.size(); i++) {
        subMeshes[i].destruct();
    }
}
void Mesh::draw() {
    for (uint32_t i = 0; i < subMeshes.size(); i++) {
        //glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
        subMeshes[i].draw();
    }
}
SubMesh Mesh::loadSubMeshData(const objl::Mesh& mesh) {
    std::vector<Vertex3D> vertices;
    void* indices;
    uint32_t indexCount = mesh.Indices.size();
    uint32_t vertexCount = mesh.Vertices.size();
    SubMesh retValue;
    if (indexCount <= 256 * 256) {
        indices = malloc(indexCount * sizeof(uint16_t));
        uint16_t* indicesAs16 = (uint16_t*)indices;
        for (uint32_t i = 0; i < indexCount; i++) {
            indicesAs16[i] = (uint16_t)(mesh.Indices[i]);
        }
    } else {
        indices = malloc(indexCount * sizeof(uint32_t));
        uint32_t* indicesAs32 = (uint32_t*)indices;
        for (uint32_t i = 0; i < indexCount; i++) {
            indicesAs32[i] = (uint32_t)(mesh.Indices[i]);
        }
    }
    /*uint32_t meshVertexCount = mesh.Vertices.size();
    for (uint32_t i = 0; i < meshVertexCount; i++) {
        vertices.push_back({});
        Vertex3D& toVert = vertices[i];
        auto& fromVert = mesh.Vertices[i];
        toVert.x = fromVert.Position.X;
        toVert.y = fromVert.Position.Y;
        toVert.z = fromVert.Position.Z;
        toVert.u = fromVert.TextureCoordinate.X;
        toVert.v = fromVert.TextureCoordinate.Y;
    }*/

    std::cout << vertexCount << std::endl;
    std::cout << indexCount << std::endl;
    for (uint32_t i = 0; i < vertexCount; i++) {
        Vertex3D toVert;
        auto& fromVert = mesh.Vertices[i];
        toVert.x = fromVert.Position.X;
        toVert.y = fromVert.Position.Y;
        toVert.z = fromVert.Position.Z;
        toVert.u = fromVert.TextureCoordinate.X;
        toVert.v = fromVert.TextureCoordinate.Y;
        std::cout << " Vert: " << toVert.x << ", " << toVert.y << ", " << toVert.z;
        std::cout << " UV: " << toVert.u << ", " << toVert.v << std::endl;
        vertices.push_back(toVert);
    }

    for (uint32_t i = 0; i < indexCount; i++) {
        std::cout << "Index: " << mesh.Indices[i] << std::endl;
    }

    std::cout << "HIT?" << std::endl;
    retValue.init(vertices.data(), indices, vertexCount, indexCount, {});
    free(indices);
    return retValue;

    /*uint32_t meshIndexCount = mesh.Indices.size();
    if (meshIndexCount <= 256 * 256) {
        indices = malloc(meshIndexCount * sizeof(uint16_t));
        uint16_t* indicesAs16 = (uint16_t*)indices;
        for (uint32_t i = 0; i < meshIndexCount; i++) {
            indicesAs16[i] = (uint16_t)mesh.Indices[i];
        }
    }
    else {
        indices = malloc(meshIndexCount * sizeof(uint32_t));
        uint32_t* indicesAs32 = (uint32_t*)indices;
        for (uint32_t i = 0; i < meshIndexCount; i++) {
            indicesAs32[i] = (uint32_t)mesh.Indices[i];
        }
    }
    uint32_t meshVertexCount = mesh.Vertices.size();
    vertices = (Vertex3D*)malloc(meshVertexCount * sizeof(Vertex3D));
    for (uint32_t i = 0; i < meshVertexCount; i++) {
        auto& vert = mesh.Vertices[i];
        vertices[i].x = vert.Position.X;
        vertices[i].y = vert.Position.Y;
        vertices[i].z = vert.Position.Z;
        vertices[i].u = vert.TextureCoordinate.X;
        vertices[i].v = vert.TextureCoordinate.Y;
    }*/
    //subMeshPtr->init(vertices, indices, meshVertexCount, meshIndexCount, {});
}