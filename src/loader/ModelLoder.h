#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <memory>

#include "math/Vector2f.h"
#include "render/Model.h"

class ModelLoader {

public:
  static std::shared_ptr<Model> load(const std::string& path,
                                     QOpenGLFunctions_4_5_Core *gl) {

    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

    if (!scene || !scene->mRootNode) {
      std::cout << importer.GetErrorString();

      return nullptr;
    }

    // 提取文件所在目录，供后续加载纹理时拼接路径
    std::string directory = path.substr(0, path.find_last_of('/') + 1);

    auto model = std::make_shared<Model>();

    processNode(scene->mRootNode, scene, model, gl, directory);

    return model;
  }

  static void processNode(aiNode *node, const aiScene *scene,
                          std::shared_ptr<Model> model,
                          QOpenGLFunctions_4_5_Core *gl,
                          const std::string& directory) {

    for (int i = 0; i < node->mNumMeshes; i++) {

      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

      model->m_meshes.push_back(processMesh(mesh, scene, gl, directory));
    }

    for (int i = 0; i < node->mNumChildren; i++) {

      processNode(node->mChildren[i], scene, model, gl, directory);
    }
  }

  static std::shared_ptr<Mesh> processMesh(aiMesh *aiMesh, const aiScene *scene,
                                           QOpenGLFunctions_4_5_Core *gl,
                                           const std::string& directory) {
    std::vector<Vertex> vertices;

    std::vector<unsigned int> indices;

    std::shared_ptr<Material> material = nullptr;

    for (int i = 0; i < aiMesh->mNumVertices; i++) {

      Vertex vertex;

      vertex.m_position.x = aiMesh->mVertices[i].x;

      vertex.m_position.y = aiMesh->mVertices[i].y;

      vertex.m_position.z = aiMesh->mVertices[i].z;

      if (aiMesh->HasNormals()) {

        vertex.m_normal.x = aiMesh->mNormals[i].x;

        vertex.m_normal.y = aiMesh->mNormals[i].y;

        vertex.m_normal.z = aiMesh->mNormals[i].z;
      }

      if (aiMesh->HasTextureCoords(0)) {

        vertex.m_texCoords.x = aiMesh->mTextureCoords[0][i].x;

        vertex.m_texCoords.y = aiMesh->mTextureCoords[0][i].y;
      } else {

        vertex.m_texCoords = Vector2f(0.0f, 0.0f);
      }

      vertices.push_back(vertex);
    }

    for (int i = 0; i < aiMesh->mNumFaces; i++) {

      aiFace face = aiMesh->mFaces[i];

      for (int j = 0; j < face.mNumIndices; j++) {
        indices.push_back(face.mIndices[j]);
      }
    }

    unsigned int materialIndex = aiMesh->mMaterialIndex;
    if (materialIndex >= 0 && materialIndex < scene->mNumMaterials) {
      aiMaterial *aiMaterial = scene->mMaterials[materialIndex];

      material = processMaterial(aiMaterial, directory, gl);
    }

    return std::make_shared<Mesh>(gl, vertices, indices, material);
  }

  static std::shared_ptr<Material> processMaterial(aiMaterial *aiMaterial,
                                                    const std::string& directory,
                                                    QOpenGLFunctions_4_5_Core *gl) {
    auto material = std::make_shared<Material>();

    auto loadTexture = [&](aiTextureType type) -> std::shared_ptr<Texture> {
      if (aiMaterial->GetTextureCount(type) > 0) {
        aiString str;
        aiMaterial->GetTexture(type, 0, &str);
        std::string fullPath = directory + str.C_Str();
        return std::make_shared<Texture>(fullPath.c_str(), gl);
      }
      return nullptr;
    };

    material->setDiffuseTexture(loadTexture(aiTextureType_DIFFUSE));
    material->setSpecularTexture(loadTexture(aiTextureType_SPECULAR));
    material->setNormalTexture(loadTexture(aiTextureType_NORMALS));
    return material;
  }
};
