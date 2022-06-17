#ifndef APPLICATION_H
#define APPLICATION_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "Renderer.hpp"
#include "Camera.hpp"

#include "Object.hpp"
#include "VerletEntity.hpp"
#include "VerletLink.hpp"

#include <vector>

class Application {
public:
    void run();
private:
    GLFWwindow* window;
    Shader* shader;

    static std::vector<Object*> objects;
    static std::vector<VerletEntity*> updateList;
    static std::vector<VerletLink> updateListLinks;
    static int entityCount;
    
    void init();
    void loop();
    void cleanup();

    void stepPhysics(float dt);

    // void windowSizeCallback(int width, int height);
    // void mouseCallback(double xOffset, double yOffset);
    // void scrollCallback(double xOffset, double yOffset);
    // void keyCallback(int key, int scancode, int action, int mods);
    void processInput();

    void createGui();
    void renderGui();

    static void addObject(Object* o);
    static void addEntity(Entity* e);
    static void addEntity(VerletEntity* e);
};

#endif