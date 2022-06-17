#include "Application.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imfilebrowser.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>

void windowSizeCallback(GLFWwindow*, int, int);
void mouseCallback(GLFWwindow*, double, double);
void scrollCallback(GLFWwindow*, double, double);
void keyCallback(GLFWwindow*, int, int, int, int);

unsigned int width = 800, height = 600;
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;
bool mouseLocked = true;

float dt = 0.0f;
float lastFrame = 0.0f;
float timeElapsed = 0.0f;

Camera camera = Camera();

std::vector<Object*> Application::objects = {};
std::vector<VerletEntity*> Application::updateList = {};
std::vector<VerletLink> Application::updateListLinks = {};
int Application::entityCount = 0;

void Application::run() {
    this->init();
    this->loop();
    this->cleanup();
}

void Application::init() {
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->window = glfwCreateWindow(width, height, "Physics Engine", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }
    glfwMakeContextCurrent(this->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(this->window);
        glfwTerminate();
        std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, width, height);

    // Set glfw callbacks
    glfwSetFramebufferSizeCallback(window, windowSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);

    // Lock mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Setup shader and projection matrix
    this->shader = new Shader("../shaders/vertex.glsl", "../shaders/fragment.glsl");

    glEnable(GL_DEPTH_TEST); // Enable depth testing

    glEnable(GL_CULL_FACE); // Enable face culling
    glCullFace(GL_BACK);

    glfwSwapInterval(1); // Enable VSync

    stbi_set_flip_vertically_on_load(true);

    // Test
    /*
    Model* test = new Model("../assets/zuball/zuball.obj");
    for (int z = 0; z < 12; z++) {
        VerletEntity* prev = nullptr;
        for (int i = 0; i < 12; i++) {
            VerletEntity* e = new VerletEntity(test, glm::vec3(20.0f - i*2.5, -2.5f, 20.0f - z*2.1f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f));
            Application::addEntity(e);
            if (i > 0) {
                VerletLink l;
                l.e1 = prev;
                l.e2 = e;
                l.targetDist = 2.3f;
                updateListLinks.push_back(l);
                if (i == 11)
                    e->pinned = true;
            } else {
                e->pinned = true;
            }
            prev = e;
        }
    }
    */
}

void Application::loop() {
    while (!glfwWindowShouldClose(this->window)) {
        float cf = (float)glfwGetTime();
        dt = cf - lastFrame;
        lastFrame = cf;

        createGui();
        processInput();
        stepPhysics(dt);
        Renderer::prepare();
        shader->bind();

        glm::mat4 proj = glm::mat4(1.0f);
        proj = glm::perspective(glm::radians(camera.fov), (float)width / (float)height, 0.1f, 100.0f);
        glUniformMatrix4fv(shader->getUniformLocation("projection"), 1, GL_FALSE, &proj[0][0]);

        glm::mat4 view = camera.getViewMatrix();
        glUniformMatrix4fv(shader->getUniformLocation("view"), 1, GL_FALSE, &view[0][0]);

        shader->unbind();
        Renderer::render(shader);
        renderGui();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Application::addObject(Object* o) {
    Application::objects.push_back(o);
}

void Application::addEntity(Entity* e) {
    Application::addObject(e);
    Renderer::addEntity(e);
    Application::entityCount++;
}

void Application::addEntity(VerletEntity* e) {
    Application::addEntity((Entity*)e);
    Application::updateList.push_back(e);
}

// Temporary physics stuff
float colliderRadius = 50.0f;
glm::vec3 colliderPos = glm::vec3(0.0f, 0.0f, 0.0f);
void Application::stepPhysics(float dt) {
    // Links
    for (VerletLink link : updateListLinks)
        link.apply();

    for (VerletEntity* object : updateList) {
        if (!object->pinned) {
            object->accelerate(glm::vec3(0.0f, -20.0f, 0.0f)); // Gravity

            // Collider
            glm::vec3 to_obj = object->position - colliderPos;
            float dist = glm::distance(object->position, colliderPos);
            if (dist > (colliderRadius - 1.0f)) {
                glm::vec3 n = glm::normalize(to_obj);
                object->position = colliderPos + n * (colliderRadius - 1.0f);
            }

            // Collisions
            for (VerletEntity* object2 : updateList) {
                if (object != object2) {
                    glm::vec3 coll_axis = object->position - object2->position;
                    float dist = glm::distance(object->position, object2->position);
                    float radii = 2.0f;
                    if (dist < radii) {
                        glm::vec3 n = glm::normalize(coll_axis);
                        float d = radii - dist;
                        if (!object2->pinned) {
                            object->position += d * n / 2.0f;
                            object2->position -= d * n / 2.0f;
                        } else {
                            object->position = object2->position + radii * n;
                        }
                    }
                }
            }

            object->updatePosition(dt);
        }
    }
}

void Application::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void Application::createGui() {
    // ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Debug window
    {
        static bool wireframe = false;
        static bool vsync = true;

        static const char* themes[]{"Dark", "Light", "Classic"};
        static int selectedTheme = 0;

        static char pathBuf[512];
        static ImGui::FileBrowser fileDialog;
        fileDialog.SetTitle("Select .obj file");
        fileDialog.SetTypeFilters({".obj"});

        ImGui::Begin("Debug");

        ImGui::Text("%s", "Camera");
        ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 90.0f);
        ImGui::SliderFloat3("Position", glm::value_ptr(camera.position), -100.0f, 100.0f);

        ImGui::Spacing();

        ImGui::Text("%s", "Insert VerletEntity");
        if (ImGui::Button("Open"))
            fileDialog.Open();
        ImGui::SameLine();
        ImGui::InputText("Model Path", pathBuf, 512);
        if (ImGui::Button("Insert")) {
            Model* test = new Model(std::string(pathBuf));
            VerletEntity* e = new VerletEntity(test, camera.position, glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f));
            Application::addEntity(e);
        }
        if (ImGui::Button("Insert 5x5x10 Softbody")) {
            Model* test = new Model(std::string(pathBuf));
            VerletEntity* es[5][5][10];
            for (int x = 0; x < 5; x++) {
                for (int y = 0; y < 5; y++) {
                    for (int z = 0; z < 10; z++) {
                        VerletEntity* e = new VerletEntity(test, glm::vec3(camera.position.x + x*2.4f, camera.position.y - y*2.4f, camera.position.z + z*2.4f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f));
                        es[x][y][z] = e;
                        Application::addEntity(e);
                        if (x-1 >= 0) {
                            VerletLink l;
                            l.e1 = es[x-1][y][z];
                            l.e2 = e;
                            l.targetDist = 2.3f;
                            updateListLinks.push_back(l);
                        }
                        if (y-1 >= 0) {
                            VerletLink l;
                            l.e1 = es[x][y-1][z];
                            l.e2 = e;
                            l.targetDist = 2.3f;
                            updateListLinks.push_back(l);
                        }
                        if (z-1 >= 0) {
                            VerletLink l;
                            l.e1 = es[x][y][z-1];
                            l.e2 = e;
                            l.targetDist = 2.3f;
                            updateListLinks.push_back(l);
                        }
                        if (y-1 >= 0 && x-1 >= 0) {
                            VerletLink l;
                            l.e1 = es[x-1][y-1][z];
                            l.e2 = e;
                            l.targetDist = sqrtf(2.0f * 2.3f * 2.3f);
                            updateListLinks.push_back(l);
                        }
                        if (y-1 >= 0 && z-1 >= 0) {
                            VerletLink l;
                            l.e1 = es[x][y-1][z-1];
                            l.e2 = e;
                            l.targetDist = sqrtf(2.0f * 2.3f * 2.3f);
                            updateListLinks.push_back(l);
                        }
                        if (y-1 >= 0 && z+1 <= 9) {
                            VerletLink l;
                            l.e1 = es[x][y-1][z+1];
                            l.e2 = e;
                            l.targetDist = sqrtf(2.0f * 2.3f * 2.3f);
                            updateListLinks.push_back(l);
                        }
                    }
                }
            }
        }
        ImGui::Text("Entity Count: %d", Application::entityCount);

        ImGui::Spacing();

        ImGui::Text("%s", "Miscellaneous");
        ImGui::ColorEdit3("Sky Colour", (float*)&Renderer::clearColor);
        if (ImGui::Checkbox("Wireframe", &wireframe))
            glPolygonMode(GL_FRONT_AND_BACK, (wireframe ? GL_LINE : GL_FILL));
        if (ImGui::Checkbox("VSync", &vsync))
            glfwSwapInterval(vsync ? 1 : 0);
        if (ImGui::Combo("UI Theme", &selectedTheme, themes, IM_ARRAYSIZE(themes))) {
            if (selectedTheme == 0)
                ImGui::StyleColorsDark();
            else if (selectedTheme == 1)
                ImGui::StyleColorsLight();
            else if (selectedTheme == 2)
                ImGui::StyleColorsClassic();
        }

        ImGui::Spacing();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        fileDialog.Display();
        if (fileDialog.HasSelected()) {
            std::string sel = fileDialog.GetSelected().string();
            std::replace(sel.begin(), sel.end(), '\\', '/');
            strcpy(pathBuf, sel.c_str());
            fileDialog.ClearSelected();
        }
    }

    // Editor window
    {
        ImGui::Begin("Editor");

        static Object* selectedObj = nullptr;
        if (ImGui::ListBoxHeader("Objects")) {
            for (Object* obj : Application::objects) {
                std::string& obj_name = obj->name;
                if (ImGui::Selectable(obj_name.c_str(), obj->selected)) {
                    selectedObj = obj;
                }
            }
            ImGui::ListBoxFooter();
        }

        ImGui::Spacing();

        ImGui::Text("Properties");
        if (selectedObj) {
            ImGui::InputText("Name", selectedObj->name.data(), 256);
        }

        ImGui::End();
    }
}

void Application::renderGui() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::processInput() {
    GLFWwindow* w = this->window;

    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, dt);
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, dt);
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, dt);
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, dt);
}

void windowSizeCallback(GLFWwindow* window, int width_, int height_) {
    width = width_;
    height = height_;
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
    if (mouseLocked) {
        if (firstMouse) {
            lastX = (float)xPos;
            lastY = (float)yPos;
            firstMouse = false;
        }

        float xOffset = (float)xPos - lastX;
        float yOffset = lastY - (float)yPos;

        lastX = (float)xPos;
        lastY = (float)yPos;

        camera.processMouse(xOffset, yOffset);
    }
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    camera.processScroll((float)yOffset);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, (mouseLocked ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED));
        mouseLocked = !mouseLocked;
        firstMouse = true;
    }
}