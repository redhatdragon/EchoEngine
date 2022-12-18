#pragma once
#pragma warning(push, 0)

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <DArray.h>
#include <FlatBuffer.h>

#include <HAL/HAL_3D.h>

void translateScreen2DToGL(float& x, float& y);

#include "GL_Utils/Shader.h"
#include "GL_Utils/Shapes.h"
#include "GL_Utils/VertexBuffer.h"
#include "GL_Utils/IndexBuffer.h"
#include "GL_Utils/VertexArray.h"
#include "GL_Utils/Texture.h"
#include "GL_Utils/TexturedQuad.h"
#include "GL_Utils/Mesh.h"
#include "GL_Utils/SceneCamera.h"

//Camera camera;
glm::mat4 perspective;
std::vector<glm::mat4> viewMatrices;
std::vector<SceneCamera*> sceneCameras;
SceneCamera* activeSceneCamera = nullptr;

static void clearErrors() {
    while (glGetError() != GL_NO_ERROR);
}
static void getErrors() {
    while (GLenum error = glGetError() != GL_NO_ERROR) {
        std::cout << "Opengl_Error: " << error << std::endl;
    }
}
inline void translateScreen2DToGL(float& x, float& y) {
    EE_Point2Df out;
    unsigned int width, height;
    EE_getCanvasSize(&width, &height);
    out = { x / width, (y / height) * -1 };
    out.x += -1.0f; out.y += 1.0f;
    x = out.x; y = out.y;
}
inline void translateScreen3DToGL(float& x, float& y, float& z) {
    EE_Point3Df out;
    unsigned int width, height;
    EE_getCanvasSize(&width, &height);
    out = { x / width, (y / height) * -1, (z / ((width + height) / 2)) * -1 };
    out.x += -1.0f; out.y += 1.0f;
    x = out.x; y = out.y; z = out.z;
}



GLFWwindow* window;
//TexturedQuad rectTexturedQuad;
//std::vector<Quad> rectangles;
std::vector<TexturedQuad> textures;
FlatBuffer<Mesh, 1000000> meshes = {};
//2D
void EE_drawTexture(void* texture, int x, int y, int w, int h) {
    textures[(uint32_t)texture].draw(x, y, w, h);
}
void EE_recolorTexture(void* texture, uint8_t r, uint8_t g, uint8_t b) {

}
void* EE_getNewTexture(const char* fileName) {
    TexturedQuad texture;
    texture.init(fileName, 0, 0, 0, 0);
    textures.push_back(texture);
    return (void*)(textures.size() - 1);
}
void EE_releaseTexture(void* texture) {

}
void EE_drawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    //rectTextureQuad.setPosAndSiz((float)x, (float)y, (float)w, (float)h);
    //rectTextureQuad.draw();
    //rectTexturedQuad.draw(x, y, w, h);
}

bool EE_getKeyState(char k) {
    if (k == EE_KEY_ESC)
        return (bool)glfwGetKey(window, GLFW_KEY_ESCAPE);
    if (k == EE_KEY_SPACE)
        return (bool)glfwGetKey(window, GLFW_KEY_SPACE);
    if (k == EE_KEY_TAB)
        return (bool)glfwGetKey(window, GLFW_KEY_TAB);
    return (bool)glfwGetKey(window, k);
}
void EE_getMouseState(uint8_t* leftButton, uint8_t* middleButton, uint8_t* rightButton) {
    int result;
    result = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    *leftButton = (bool)result;
    result = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    *rightButton = (bool)result;
    result = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
    *middleButton = (bool)result;
}
void EE_getMouseCanvasPos(int* x, int* y) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    *x = (int)xpos;
    *y = (int)ypos;
}
void EE_setMouseCanvasPos(int x, int y) {
    glfwSetCursorPos(window, (double)x, (double)y);
}

void EE_getCanvasSize(unsigned int* width, unsigned int* height) {
    glfwGetWindowSize(window, (int*)width, (int*)height);
}
const char* EE_getDirData() {
    return "../data/";
}

//3D
void EE_drawTriangle2DRGB(const EE_Point2Df points[3], EE_ColorR8G8B8 color) {

}
void EE_drawTriangle2DRGBVertex(const EE_Point2Df points[3], const EE_ColorR8G8B8 colors[3]) {

}
void* EE_getNewMesh(const char* filepath) {
    uint32_t retValue = meshes.count;
    meshes[retValue].init(filepath);
    meshes.count++;
    return (void*)retValue;
}
void EE_drawMesh(void* mesh) {
    uint32_t meshID = (uint32_t)mesh;
    //meshes[meshID].draw(viewMatrices.back(), perspective);
    if (activeSceneCamera)
        meshes[meshID].draw(activeSceneCamera->GetViewMatrix(), perspective);
    else
        meshes[meshID].draw(viewMatrices.back(), perspective);
    //meshes[meshID].draw(glm::mat4(1), perspective);
}

void EE_setPositionMesh(void* mesh, float x, float y, float z) {
    uint32_t meshID = (uint32_t)mesh;
    translateScreen3DToGL(x, y, z);
    meshes[meshID].pos = { x, y, z };
}
void EE_setRotationMesh(void* mesh, float x, float y, float z) {
    uint32_t meshID = (uint32_t)mesh;
    meshes[meshID].rot = { x, y, z };
}
void EE_setScaleMesh(void* mesh, float x, float y, float z) {
    uint32_t meshID = (uint32_t)mesh;
    meshes[meshID].siz = { x, y, z };
}

void EE_releaseMesh(void* mesh) {
    meshes[(uint32_t)mesh].destruct();
}

void EE_translate(float x, float y, float z) {
    glm::mat4& matrix = viewMatrices[viewMatrices.size() - 1];
    translateScreen3DToGL(x, y, z);
    matrix = glm::translate(viewMatrices[viewMatrices.size() - 1], { x, y, z });
}
void EE_rotate(float x, float y, float z) {
    glm::mat4& matrix = viewMatrices[viewMatrices.size() - 1];
    //translateScreen3DToGL(x, y, z);  //TODO: needed?
    //glm::lookAt({}, {}, {});
    
    //glm::vec3 position = {0.0f, 0.0f, 0.0f};
    //glm::vec3 front = {x, y, z};
    //matrix = glm::lookAt(position, position + front, { 0.0f, 1.0f, 0.0f });

    matrix = glm::rotate(matrix, glm::radians(x), { 1.0f, 0.0f, 0.0f });
    matrix = glm::rotate(matrix, glm::radians(y), { 0.0f, 1.0f, 0.0f });
    matrix = glm::rotate(matrix, glm::radians(z), { 0.0f, 0.0f, 1.0f });
}
void EE_scale(float x, float y, float z) {
    glm::mat4& matrix = viewMatrices[viewMatrices.size() - 1];
    translateScreen3DToGL(x, y, z);
    matrix = glm::scale(viewMatrices[viewMatrices.size() - 1], { x, y, z });
}
void EE_pushMatrix() {
    viewMatrices.push_back(glm::mat4(1));
}
void EE_popMatrix() {
    viewMatrices.pop_back();
}

void* EE_getNewCamera() {
    sceneCameras.push_back(new SceneCamera());
    return (void*)sceneCameras[sceneCameras.size() - 1];
}
void EE_setCameraRotation(void* self, float x, float y, float z) {
    SceneCamera* cam = (SceneCamera*)self;
    cam->setRotation(x, y, z);
}
void EE_setCameraPos(void* self, float x, float y, float z) {
    SceneCamera* cam = (SceneCamera*)self;
    cam->setPosition(x, y, z);
}
void EE_useCamera(void* self) {
    activeSceneCamera = (SceneCamera*)self;
}
void EE_releaseCamera(void* self) {
    SceneCamera* cam = (SceneCamera*)self;
    uint32_t index = -1;
    uint32_t cameraCount = sceneCameras.size();
    for (uint32_t i = 0; i < cameraCount; i++) {
        if (sceneCameras[i] == cam) {
            index = i;
            break;
        }
    }
    if (index == -1) {

    }
    if (sceneCameras[index] == activeSceneCamera)
        activeSceneCamera = nullptr;
    sceneCameras[index] = sceneCameras[sceneCameras.size() - 1];
    sceneCameras.pop_back();
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

    //camera = Camera();  //TODO: necessary?

    EE_appStart();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        EE_pushMatrix();

        /* Render here */
        glClearColor(0.5f, 0.5f, 0.5f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        unsigned int winWidth, winHeight;
        EE_getCanvasSize(&winWidth, &winHeight);
        perspective = glm::perspective(45.0f, (GLfloat)winWidth / (GLfloat)winHeight, 0.00001f, 150.0f);

        EE_appLoop();
        getErrors();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        viewMatrices.clear();
    }

    EE_appEnd();

    glfwTerminate();
    return 0;
}

#pragma warning(pop)