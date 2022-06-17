#include "Object.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

Object::Object(std::string name) {
    this->name = name;
}

void Object::createPropertiesGui() {
    ImGui::InputText("Name", name.data(), 256);
}