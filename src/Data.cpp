#include "Data.h"
#include <iostream>

void Data::update(const Camera &camera) {
  mData[0] = 11;
  mData[1] = camera.getConfig();
  mData[2] = static_cast<float>(camera.getResolution().x);
  mData[3] = static_cast<float>(camera.getResolution().y);
  mData[4] = camera.getFOV();
  mData[5] = camera.getPosition().x;
  mData[6] = camera.getPosition().y;
  mData[7] = camera.getPosition().z;
  mData[8] = camera.getFront().x;
  mData[9] = camera.getFront().y;
  mData[10] = camera.getFront().z;
}

void Data::update(const Scene &scene) {
  int offset = mData[0];

  int modelCount = scene.getModelCount();
  mData[offset] = modelCount;
  offset++;
  for (int i = 0; i < modelCount; i++) {
    const Model &model = scene.getModels()[i];
    mData[offset] = model.getIndex();
    offset++;
    mData[offset] = model.getMaxVert().x;
    mData[offset + 1] = model.getMaxVert().y;
    mData[offset + 2] = model.getMaxVert().z;
    mData[offset + 3] = model.getMinVert().x;
    mData[offset + 4] = model.getMinVert().y;
    mData[offset + 5] = model.getMinVert().z;
    std::cout << model.getMaxVert().x << " " << model.getMaxVert().y << " " << model.getMaxVert().z << std::endl;
    std::cout << model.getMinVert().x << " " << model.getMinVert().y << " " << model.getMinVert().z << std::endl;
    offset+=6;

    int meshCount = model.getMeshCount();
    mData[offset] = meshCount;
    offset++;
    for (int j = 0; j < meshCount; j++) {
      const Mesh &mesh = model.getMeshes()[j];
      int triangleCount = mesh.getTriangleCount();
      mData[offset] = triangleCount;
      offset++;
      for (int k = 0; k < triangleCount; k++) {
        const Triangle &triangle = mesh.getTriangles()[k];
        for (int l = 0; l < 3; l++) {
          const Vertex &vert = triangle.mVertices[l];
          mData[offset] = vert.mPosition.x;
          mData[offset + 1] = vert.mPosition.y;
          mData[offset + 2] = vert.mPosition.z;
          mData[offset + 3] = vert.mNormal.x;
          mData[offset + 4] = vert.mNormal.x;
          mData[offset + 5] = vert.mNormal.x;
          offset += 6;
        }
      }
    }
  }
}
