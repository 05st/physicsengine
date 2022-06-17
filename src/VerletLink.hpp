#ifndef VERLETLINK_H
#define VERLETLINK_H

#include "VerletEntity.hpp"

struct VerletLink {
    VerletEntity* e1;
    VerletEntity* e2;
    float targetDist;

    void apply();
};

#endif