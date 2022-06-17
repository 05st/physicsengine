#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Renderer.hpp"

#include <iostream>

std::vector<Entity*> Renderer::renderList = {};
glm::vec4 Renderer::clearColor = glm::vec4(72.0f/255.0f, 126.0f/255.0f, 176.0f/255.0f, 1.0f);

void Renderer::prepare() {
    glClearColor(Renderer::clearColor.x, Renderer::clearColor.y, Renderer::clearColor.z, Renderer::clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render(Shader* shader) {
    shader->bind();
    for (Entity* e : Renderer::renderList) {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, e->position);
        transform = glm::scale(transform, e->scale);
        transform = glm::rotate(transform, glm::radians(e->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(e->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(e->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        
        unsigned int transformLoc = shader->getUniformLocation("transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        e->model->draw(shader);
    }
    shader->unbind();
}

void Renderer::addEntity(Entity* entity) {
    Renderer::renderList.push_back(entity);
}