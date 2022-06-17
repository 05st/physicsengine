#include "VerletLink.hpp"

#include "glm/glm.hpp"

void VerletLink::apply() {
    glm::vec3 axis = e1->position - e2->position;
    float dist = glm::distance(e1->position, e2->position);
    glm::vec3 norm = glm::normalize(axis);
    float delta = targetDist - dist;
    if (!e1->pinned && !e2->pinned) {
        e1->position += 0.5f * delta * norm;
        e2->position -= 0.5f * delta * norm;
    } else if (!e2->pinned) {
        e2->position -= delta * norm;
    } else {
        e1->position += delta * norm;
    }
}