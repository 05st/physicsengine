#ifndef RENDERER_H
#define RENDERER_H

#include "GLFW/glfw3.h"

#include "Entity.hpp"
#include "Shader.hpp"

#include <vector>

class Renderer {
public:
    static void prepare();
    static void render(Shader* shader);
    
    static void addEntity(Entity* entity);

    static glm::vec4 clearColor;
private:
    static std::vector<Entity*> renderList;
};

#endif