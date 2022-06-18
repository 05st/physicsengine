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
    ImGui::DragFloat3("Position", glm::value_ptr(position), 0.2f);
    ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.2f);
    ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.2f, 0.1f);
    Object::createPropertiesGui();
}