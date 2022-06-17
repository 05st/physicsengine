#include "VerletEntity.hpp"

VerletEntity::VerletEntity(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : Entity(model, position, rotation, scale) {
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    position_old = position;
}

void VerletEntity::accelerate(glm::vec3 acc) {
    acceleration += acc;
}

void VerletEntity::updatePosition(float dt) {
    glm::vec3 vdt = position - position_old;
    position_old = position;
    position += vdt + acceleration*(dt*dt);
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
}

void VerletEntity::createPropertiesGui() {
    Entity::createPropertiesGui();
}