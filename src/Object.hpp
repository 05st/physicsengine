#ifndef OBJECT_H
#define OBJECT_H

#include <string>

class Object {
public:
    Object(std::string name);
    std::string name;

    virtual void createPropertiesGui();
};

#endif