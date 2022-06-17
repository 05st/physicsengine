#ifndef MODEL_H
#define MODEL_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Mesh.hpp"

#include <string>
#include <vector>

class Model {
public:
    Model(std::string const& path);
    void draw(Shader* shader);
private:
    static std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string dir;

    void loadModel(std::string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif