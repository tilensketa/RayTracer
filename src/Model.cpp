#include "Model.h"
#include <glm/glm.hpp>
#include <iostream>

Model::Model(const std::string &objPath) {
  Assimp::Importer importer;

  const aiScene *scene =
      importer.ReadFile(objPath, aiProcess_CalcTangentSpace |
                                     aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "Error loading model " << objPath << std::endl;
  } else {
    // Start processing the model data beginning with the root node
    processNode(scene->mRootNode, scene);
  }

  createBoundingBox();
}

void Model::processNode(const aiNode *node, const aiScene *scene) {
  for (int i = 0; i < node->mNumMeshes; i++) {

    int meshIndex = node->mMeshes[i];

    if (meshIndex < scene->mNumMeshes) {
      std::string name;
      std::vector<Vertex> vertices;
      std::vector<int> indices;

      aiMesh *mesh = scene->mMeshes[meshIndex];

      // Access mesh information
      std::string meshName = mesh->mName.C_Str();
      int numVertices = mesh->mNumVertices;
      std::cout << "------------------------------" << std::endl;
      std::cout << "Mesh name: " << meshName << std::endl;
      std::cout << "Number of vertices: " << numVertices << std::endl;
      name = meshName;

      for (int j = 0; j < mesh->mNumVertices; j++) {
        Vertex vert;

        aiVector3D vertex = mesh->mVertices[j];
        aiVector3D normal = mesh->mNormals[j];
        vert.mPosition = glm::vec3(vertex.x, vertex.y, vertex.z);
        vert.mNormal = glm::vec3(normal.x, normal.y, normal.z);

        if (mesh->HasTextureCoords(0)) {
          aiVector3D texCoord = mesh->mTextureCoords[0][j];
          // spdlog::info("TexCoord: ({}, {})", texCoord.x, texCoord.y);
          // vert.UV[0] = texCoord.x;
          // vert.UV[1] = texCoord.y;
        }
        vertices.push_back(vert);
      }

      // Access the indices to construct triangles.
      for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
        const aiFace &face = mesh->mFaces[j];
        if (face.mNumIndices != 3) {
          // Handle non-triangle faces if necessary.
        } else {
          indices.push_back(face.mIndices[0]);
          indices.push_back(face.mIndices[1]);
          indices.push_back(face.mIndices[2]);
        }
      }

      // Process material information for this mesh
      // aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

      Mesh newMesh(vertices, indices);
      mMeshes.push_back(newMesh);
    }
  }
  // Recursively process child nodes
  for (int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

void Model::createBoundingBox() {
  float max = std::numeric_limits<float>::max();
  float min = std::numeric_limits<float>::min();
  glm::vec3 minVert = glm::vec3(max, max, max);
  glm::vec3 maxVert = glm::vec3(-max, -max, -max);
  for (int i = 0; i < mMeshes.size(); i++) {
    const Mesh &mesh = mMeshes[i];
    for (int j = 0; j < mesh.getTriangleCount(); j++) {
      const Triangle &triangle = mesh.getTriangles()[j];
      for (int k = 0; k < 3; k++) {
        const Vertex &vert = triangle.mVertices[k];
        minVert.x = glm::min(minVert.x, vert.mPosition.x);
        minVert.y = glm::min(minVert.y, vert.mPosition.y);
        minVert.z = glm::min(minVert.z, vert.mPosition.z);
        maxVert.x = glm::max(maxVert.x, vert.mPosition.x);
        maxVert.y = glm::max(maxVert.y, vert.mPosition.y);
        maxVert.z = glm::max(maxVert.z, vert.mPosition.z);
      }
    }
  }
  mMaxVert = maxVert;
  mMinVert = minVert;
}
