#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "DArray.h"

#include "Shader.h"
#include "Shapes.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Billboard.h"
#include "Mesh.h"

static void clearErrors() {
    while (glGetError() != GL_NO_ERROR);
}
static void getErrors() {
    while (GLenum error = glGetError() != GL_NO_ERROR) {
        std::cout << "Opengl_Error: " << error << std::endl;
    }
}

static void drawTriangle(const Triangle2D& triangle) {
    //_globalVertexBuffer.push_back(((Vertex*)&triangle)[0]);
    //_globalVertexBuffer.push_back(((Vertex*)&triangle)[1]);
    //_globalVertexBuffer.push_back(((Vertex*)&triangle)[2]);
}
static void drawBillboard(const Billboard* mesh) {
    /*size_t triangleCount = mesh->vertexBuffer.size() / 3;
    std::cout << triangleCount << std::endl;
    for (uint32_t i = 0; i < triangleCount; i++) {
        const Triangle& triangle = *(Triangle*)&mesh->vertexBuffer[i*3];
        drawTriangle(triangle);
    }
    for (size_t i = 0; i < mesh->vertexBuffer.size(); i++)
        _globalVertexBuffer.push_back(mesh->vertexBuffer[i]);
    for (size_t i = 0; i < mesh->indexBuffer.size(); i++)
        _globalIndicies.push_back(mesh->indexBuffer[i]);
        */
}

#ifdef IO_IMPL


void appStart();
void appLoop();
void appEnd();

int main() {

    GLFWwindow* window;

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

    appStart();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        appLoop();
        getErrors();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    appEnd();

    glfwTerminate();
    return 0;
}
#endif