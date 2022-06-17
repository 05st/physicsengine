#include "Entity.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

Entity::Entity(Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : Object("Entity") {
    this->model = model;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
}

void Entity::createPropertiesGui() {
    ImGui::SliderFloat3("Position", glm::value_ptr(position), -100.0f, 100.0f);
    ImGui::SliderFloat3("Rotation", glm::value_ptr(rotation), -360.0f, 360.0f);
    ImGui::SliderFloat3("Scale", glm::value_ptr(scale), -0.0f, 10.0f);
    Object::createPropertiesGui();
}