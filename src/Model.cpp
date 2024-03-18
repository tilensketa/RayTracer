#include "Model.h"
#include <glm/glm.hpp>
#include <iostream>

Model::Model(const std::string objPath) {
  mName = objPath;
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
  std::cout << "Model created: " << objPath << std::endl;
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
      /* std::cout << "------------------------------" << std::endl;
      std::cout << "Mesh name: " << meshName << std::endl;
      std::cout << "Number of vertices: " << numVertices << std::endl; */
      name = meshName;

      for (int j = 0; j < mesh->mNumVertices; j++) {
        Vertex vert;

        aiVector3D vertex = mesh->mVertices[j];
        aiVector3D normal = mesh->mNormals[j];
        vert.mPosition = glm::vec3(vertex.x, vertex.y, vertex.z);
        vert.mModedPosition = glm::vec3(vertex.x, vertex.y, vertex.z);
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
      aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
      Material newMaterial = processNodeMaterial(material);

      Mesh newMesh(vertices, indices);
      newMesh.setIndex(meshIndex);
      newMesh.setMaterial(newMaterial);
      mMeshes.push_back(newMesh);
    }
  }
  // Recursively process child nodes
  for (int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Material Model::processNodeMaterial(const aiMaterial *material) {
  Material newMaterial;

  aiColor3D diffuseColor;
  if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS) {
    glm::vec3 diffuse =
        glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    newMaterial.setDiffuse(diffuse);
  }
  aiColor3D ambientColor;
  if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == AI_SUCCESS) {
    glm::vec3 ambient =
        glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b);
    newMaterial.setAmbient(ambient);
  }
  return newMaterial;
}

void Model::setIndex(int id) {
  mIndex = id;
  for (Mesh &mesh : mMeshes) {
    for (Triangle &triangle : mesh.modTriangles()) {
      triangle.mModelIndex = id;
    }
  }
}

void Model::createBoundingBox() {
  float max = std::numeric_limits<float>::max();
  glm::vec3 minVert = glm::vec3(max, max, max);
  glm::vec3 maxVert = glm::vec3(-max, -max, -max);
  for (int i = 0; i < mMeshes.size(); i++) {
    const Mesh &mesh = mMeshes[i];
    const glm::vec3 &meshMaxVert = mesh.getMaxVert();
    const glm::vec3 &meshMinVert = mesh.getMinVert();
    for (int j = 0; j < 3; j++) {
      minVert[j] = glm::min(minVert[j], meshMinVert[j]);
      maxVert[j] = glm::max(maxVert[j], meshMaxVert[j]);
    }
  }
  mMaxVert = maxVert;
  mMinVert = minVert;
}

void Model::updatePosition() {
  for (Mesh &mesh : mMeshes) {
    mesh.setPosition(mPosition);
    mesh.updatePosition();
  }
  createBoundingBox();
}
