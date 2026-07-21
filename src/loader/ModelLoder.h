#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

#include "render/Model.h"

class ModelLoader {

public:
  static std::shared_ptr<Model> load(std::string path,
                                     QOpenGLFunctions_4_5_Core *gl) {

    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

    if (!scene || !scene->mRootNode) {
      std::cout << importer.GetErrorString();

      return nullptr;
    }

    auto model = std::make_shared<Model>();

    processNode(scene->mRootNode, scene, model, gl);

    return model;
  }

  static void processNode(aiNode *node, const aiScene *scene,
                          std::shared_ptr<Model> model,
                          QOpenGLFunctions_4_5_Core *gl) {

    for (int i = 0; i < node->mNumMeshes; i++) {

      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

      model->m_meshes.push_back(processMesh(mesh, scene, gl));
    }

    for (int i = 0; i < node->mNumChildren; i++) {

      processNode(node->mChildren[i], scene, model, gl);
    }
  }

  static std::shared_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene,
                                           QOpenGLFunctions_4_5_Core *gl) {

    std::vector<Vertex> vertices;

    std::vector<unsigned int> indices;

    for (int i = 0; i < mesh->mNumVertices; i++) {

      Vertex vertex;

      vertex.m_position.x = mesh->mVertices[i].x;

      vertex.m_position.y = mesh->mVertices[i].y;

      vertex.m_position.z = mesh->mVertices[i].z;

      if (mesh->HasNormals()) {

        vertex.m_normal.x = mesh->mNormals[i].x;

        vertex.m_normal.y = mesh->mNormals[i].y;

        vertex.m_normal.z = mesh->mNormals[i].z;
      }

      vertices.push_back(vertex);
    }

    for (int i = 0; i < mesh->mNumFaces; i++) {

      aiFace face = mesh->mFaces[i];

      for (int j = 0; j < face.mNumIndices; j++) {
        indices.push_back(face.mIndices[j]);
      }
    }

    return std::make_shared<Mesh>(gl, vertices, indices);
  }
};