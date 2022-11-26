#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <DArray.h>
#include <FlatBuffer.h>

#include <IO_API/IO_API_3D.h>

void translateScreen2DToGL(float& x, float& y);

#include "GL_IO/Shader.h"
#include "GL_IO/Shapes.h"
#include "GL_IO/VertexBuffer.h"
#include "GL_IO/IndexBuffer.h"
#include "GL_IO/VertexArray.h"
#include "GL_IO/Texture.h"
#include "GL_IO/TexturedQuad.h"
#include "GL_IO/Mesh.h"

static void clearErrors() {
    while (glGetError() != GL_NO_ERROR);
}
static void getErrors() {
    while (GLenum error = glGetError() != GL_NO_ERROR) {
        std::cout << "Opengl_Error: " << error << std::endl;
    }
}
void translateScreen2DToGL(float& x, float& y) {
    IO_Point2Df out;
    unsigned int width, height;
    IO_getCanvasSize(&width, &height);
    out = { x / width, (y / height) * -1 };
    out.x += -1.0f; out.y += 1.0f;
    x = out.x; y = out.y;
}



GLFWwindow* window;
//TexturedQuad rectTexturedQuad;
//std::vector<Quad> rectangles;
std::vector<TexturedQuad> textures;
FlatBuffer<Mesh, 1000000> meshes;
//2D
void IO_drawTexture(void* texture, int x, int y, int w, int h) {
    textures[(uint32_t)texture].draw(x, y, w, h);
}
void IO_recolorTexture(void* texture, uint8_t r, uint8_t g, uint8_t b) {

}
void* IO_getTexture(const char* fileName) {
    TexturedQuad texture;
    texture.init(fileName, 0, 0, 0, 0);
    textures.push_back(texture);
    return (void*)(textures.size() - 1);
}
void IO_releaseTexture(void* texture) {

}
void IO_drawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    //rectTextureQuad.setPosAndSiz((float)x, (float)y, (float)w, (float)h);
    //rectTextureQuad.draw();
    //rectTexturedQuad.draw(x, y, w, h);
}
void IO_getCanvasSize(unsigned int* width, unsigned int* height) {
    glfwGetWindowSize(window, (int*)width, (int*)height);
}
const char* IO_getDirData() {
    return "../data/";
}

//3D
void IO_drawTriangle2DRGB(const IO_Point2Df points[3], IO_ColorR8G8B8 color) {

}
void IO_drawTriangle2DRGBVertex(const IO_Point2Df points[3], const IO_ColorR8G8B8 colors[3]) {

}
void* IO_getMesh(const char* filepath) {
    uint32_t retValue = meshes.count;
    meshes[retValue].init(filepath);
    meshes.count++;
    return (void*)retValue;
}
void IO_drawMesh(void* mesh) {
    uint32_t meshID = (uint32_t)meshID;
    unsigned int winWidth, winHeight;
    IO_getCanvasSize(&winWidth, &winHeight);
    meshes[meshID].draw();
}
void IO_releaseMesh(void* mesh) {
    meshes[(uint32_t)mesh].destruct();
}



int main() {

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        printf("Error: glewInit failed\n");
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  // Accept fragment if it closer to the camera than the former one

    IO_appStart();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        IO_appLoop();
        getErrors();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    IO_appEnd();

    glfwTerminate();
    return 0;
}