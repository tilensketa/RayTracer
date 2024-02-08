#include "Data.h"

void Data::update(const Camera &camera) {
  mData[0] = camera.getConfig();
  mData[1] = static_cast<float>(camera.getResolution().x);
  mData[2] = static_cast<float>(camera.getResolution().y);
  mData[3] = camera.getFOV();
  mData[4] = camera.getPosition().x;
  mData[5] = camera.getPosition().y;
  mData[6] = camera.getPosition().z;
  mData[7] = camera.getFront().x;
  mData[8] = camera.getFront().y;
  mData[9] = camera.getFront().z;
}

void Data::update(const Scene &scene) {
  int offset = 10;

  int modelCount = scene.getModelCount();
  mData[offset] = modelCount;
  offset++;
  for (int i = 0; i < modelCount; i++) {
    const Model &model = scene.getModels()[i];
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
          offset+=6;
        }
      }
    }
  }
}
