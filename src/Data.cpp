#include "Data.h"

void Data::update(const Camera &camera) {
  mData[0] = 18;
  mData[1] = camera.getConfig();
  mData[2] = camera.getFOV();
  mData[3] = camera.getAspectRatio();
  mData[4] = static_cast<float>(camera.getResolution().x);
  mData[5] = static_cast<float>(camera.getResolution().y);
  mData[6] = camera.getPosition().x;
  mData[7] = camera.getPosition().y;
  mData[8] = camera.getPosition().z;
  mData[9] = camera.getMatrix()[0][0];
  mData[10] = camera.getMatrix()[0][1];
  mData[11] = camera.getMatrix()[0][2];
  mData[12] = camera.getMatrix()[1][0];
  mData[13] = camera.getMatrix()[1][1];
  mData[14] = camera.getMatrix()[1][2];
  mData[15] = camera.getMatrix()[2][0];
  mData[16] = camera.getMatrix()[2][1];
  mData[17] = camera.getMatrix()[2][2];
}

void Data::update(const Scene &scene) {
  int offset = (int)mData[0];

  int modelCount = scene.getModelCount();
  mData[offset] = (float)modelCount;
  offset++;
  for (int i = 0; i < modelCount; i++) {
    const Model &model = scene.getModels()[i];
    mData[offset] = (float)model.getIndex();
    offset++;
    mData[offset] = model.getMaxVert().x;
    mData[offset + 1] = model.getMaxVert().y;
    mData[offset + 2] = model.getMaxVert().z;
    mData[offset + 3] = model.getMinVert().x;
    mData[offset + 4] = model.getMinVert().y;
    mData[offset + 5] = model.getMinVert().z;
    offset += 6;

    int meshCount = model.getMeshCount();
    mData[offset] = (float)meshCount;
    offset++;
    for (int j = 0; j < meshCount; j++) {
      const Mesh &mesh = model.getMeshes()[j];
      int triangleCount = mesh.getTriangleCount();
      mData[offset] = (float)triangleCount;
      offset++;
      for (int k = 0; k < triangleCount; k++) {
        const Triangle &triangle = mesh.getTriangles()[k];
        for (int l = 0; l < 3; l++) {
          const Vertex &vert = triangle.mVertices[l];
          mData[offset] = vert.mPosition.x;
          mData[offset + 1] = vert.mPosition.y;
          mData[offset + 2] = vert.mPosition.z;
          offset += 3;
        }
        mData[offset] = triangle.mVertices[0].mNormal.x;
        mData[offset + 1] = triangle.mVertices[0].mNormal.y;
        mData[offset + 2] = triangle.mVertices[0].mNormal.z;
        offset += 3;
      }
    }
  }
}
