#ifndef ENTITY_H
#define ENTITY_H

#include "glm/glm.hpp"

#include "Object.hpp"
#include "Model.hpp"

class Entity : public Object {
public:
    Entity(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
    ~Entity() = default;

    Model* model;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

#endif