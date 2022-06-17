#include "Entity.hpp"

Entity::Entity(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : Object("Entity") {
    this->model = model;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
}