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
  mOffset = (int)mData[0];

  int modelCount = scene.getModelCount();
  add(modelCount);
  for (int i = 0; i < modelCount; i++) {
    const Model &model = scene.getModels()[i];
    add(model.getIndex());
    add(model.getMaxVert());
    add(model.getMinVert());
    int meshCount = model.getMeshCount();
    add(meshCount);
    for (int j = 0; j < meshCount; j++) {
      const Mesh &mesh = model.getMeshes()[j];
      add(mesh.getMaxVert());
      add(mesh.getMinVert());
      const Material &material = mesh.getMaterial();
      add(material.getDiffuse());
      add(material.getAmbient());
      int triangleCount = mesh.getTriangleCount();
      add(triangleCount);
      for (int k = 0; k < triangleCount; k++) {
        const Triangle &triangle = mesh.getTriangles()[k];
        for (int l = 0; l < 3; l++) {
          const Vertex &vert = triangle.mVertices[l];
          add(vert.mPosition);
        }
        add(triangle.mVertices[0].mNormal);
      }
    }
  }
}

void Data::add(const glm::vec3 &vec) {
  mData[mOffset] = vec.x;
  mData[mOffset + 1] = vec.y;
  mData[mOffset + 2] = vec.z;
  mOffset += 3;
}

void Data::add(const float &value) {
  mData[mOffset] = value;
  mOffset++;
}

void Data::add(const int &value) {
  mData[mOffset] = (int)value;
  mOffset++;
}
