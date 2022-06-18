#ifndef VERLETENTITY_H
#define VERLETENTITY_H

#include "Entity.hpp"

#include "glm/glm.hpp"

class VerletEntity : public Entity {
public:
    VerletEntity(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

    void updatePosition(float dt);
    void accelerate(glm::vec3 acc);

    float colliderRadius = 1.0f;
    bool pinned = false;
    
    virtual void createPropertiesGui();
private:
    glm::vec3 acceleration;
    glm::vec3 position_old;
};

#endif