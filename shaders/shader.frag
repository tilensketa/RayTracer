#version 430

struct Ray {
  vec3 mOrigin;
  vec3 mDirection;
};
struct Vertex {
  vec3 mPosition;
};
int sizeOfVertex = 3;

struct Triangle {
  int mModelIndex;
  int mMeshIndex;
  Vertex mVertices[3];
  vec3 mNormal;
};
int sizeOfTriangle = 1 + 1 + 3 + 3 * sizeOfVertex;

struct Material {
  vec3 mDiffuse;
  vec3 mAmbient;
};
int sizeOfMaterial = 3 + 3;

struct BoundingBox {
  vec3 mMaxVert;
  vec3 mMinVert;
};
int sizeOfBoundingBox = 3 + 3;

layout(std430, binding = 0) buffer Data
{
  float mData[10000000];
};

out vec4 FragColor;


vec3 getVec3(inout int offset) {
  vec3 vector = vec3(mData[offset], mData[offset + 1], mData[offset + 2]);
  offset += 3;
  return vector;
}
float getFloat(inout int offset) {
  float value = mData[offset];
  offset++;
  return value;
}
int getInt(inout int offset) {
  int value = int(mData[offset]);
  offset++;
  return value;
}
Triangle getTriangle(inout int offset) {
  Triangle triangle;
  triangle.mModelIndex = getInt(offset);
  triangle.mMeshIndex = getInt(offset);
  for (int l = 0; l < 3; l++) {
    Vertex vert;
    vert.mPosition = getVec3(offset);
    triangle.mVertices[l] = vert;
  }
  triangle.mNormal = getVec3(offset);
  return triangle;
}
bool getBool(inout int offset) {
  bool bol = mData[offset] != 0.0f;
  offset++;
  return bol;
}
BoundingBox getAABB(inout int offset) {
  BoundingBox aabb;
  aabb.mMaxVert = getVec3(offset);
  aabb.mMinVert = getVec3(offset);
  return aabb;
}
Material getMaterial(inout int offset) {
  Material material;
  material.mDiffuse = getVec3(offset);
  material.mAmbient = getVec3(offset);
  return material;
}
void skipMesh(inout int offset) {
  offset+=sizeOfMaterial;
  int triangleCount = int(getFloat(offset));
  for (int k = 0; k < triangleCount; k++) {
    offset+=sizeOfTriangle;
  }
}
void skipModel(inout int offset) {
  int meshCount = int(getFloat(offset));
  for (int j = 0; j < meshCount; j++) {
    offset+=sizeOfBoundingBox;
    skipMesh(offset);
  }
}

float findMinComponent(vec3 vector) {
  return min(min(vector.x, vector.y), vector.z);
}
float findMaxComponent(vec3 vector) {
  return max(max(vector.x, vector.y), vector.z);
}

bool intersectRayAABB(Ray ray, BoundingBox aabb) {
  vec3 invDir = 1.0f / ray.mDirection;
  vec3 t1 = (aabb.mMinVert - ray.mOrigin) * invDir;
  vec3 t2 = (aabb.mMaxVert - ray.mOrigin) * invDir;

  float tNear = findMaxComponent(min(t1, t2));
  float tFar = findMinComponent(max(t1, t2));

  return tNear <= tFar && tFar >= 0;
}

bool intersectRayTriangle(Ray ray, Triangle triangle, out float outT) {
  float EPSILON = 0.000001f;

  vec3 edge1 = triangle.mVertices[1].mPosition - triangle.mVertices[0].mPosition;
  vec3 edge2 = triangle.mVertices[2].mPosition - triangle.mVertices[0].mPosition;
  vec3 h = cross(ray.mDirection, edge2);
  float a = dot(edge1, h);

  if (a > -EPSILON && a < EPSILON)
    return false;

  float f = 1.0f / a;
  vec3 s = ray.mOrigin - triangle.mVertices[0].mPosition;
  float u = f * dot(s, h);

  if (u < 0.0f || u > 1.0f)
    return false;

  vec3 q = cross(s, edge1);
  float v = f * dot(ray.mDirection, q);

  if (v < 0.0f || u + v > 1.0f)
    return false;

  outT = f * dot(edge2, q);

  if (outT > EPSILON)
    return true;

  return false;
}

/*
vec3 rayTrace(Ray ray) {
  float closestT = 1e30;
  vec3 closestColor = vec3(0.0);

  bool black = mData[1] != 0.0f;
  vec3 lightDir = normalize(vec3(-1));

  int offset = int(mData[0]);
  int modelCount = int(getFloat(offset));
  for (int i = 0; i < modelCount; i++) {
    int modelIndex = int(getFloat(offset));
    BoundingBox modelAABB = getAABB(offset);

    if(!intersectRayAABB(ray, modelAABB)) {
      skipModel(offset);
      continue;
    }

    int meshCount = int(getFloat(offset));
    for (int j = 0; j < meshCount; j++) {
      BoundingBox meshAABB = getAABB(offset);

      if(!intersectRayAABB(ray, meshAABB)) {
        skipMesh(offset);
        continue;
      }

      Material material = getMaterial(offset);
      int triangleCount = int(getFloat(offset));
      for (int k = 0; k < triangleCount; k++) {
        Triangle triangle = getTriangle(offset);
        float t;
        if (intersectRayTriangle(ray, triangle, t)) {
          if (t < closestT) {
            closestT = t;
            if(black) {
              float red = (float(modelIndex)+1) / float(modelCount);
              closestColor = vec3(red, 0, 0);
              continue;
            }
            float intensity = dot(triangle.mNormal, -lightDir) * 0.5 + 0.5;
            closestColor = material.mDiffuse * intensity;
          }
        }
      }
    }
  }
  return closestColor;
}
*/

bool traverseBVH(Ray ray, int nodeIndex, int constOffset) {
  int stack[10000];
  int stackPointer = 0;

  stack[stackPointer++] = nodeIndex;

  while (stackPointer > 0) {
    int currentIndex = stack[--stackPointer];
    int offset = int(mData[constOffset + currentIndex]);
    BoundingBox aabb = getAABB(offset);

    if (intersectRayAABB(ray, aabb)) {
      bool isLeaf = getBool(offset);

      if (isLeaf) {
        int triangleCount = getInt(offset);
        for (int i = 0; i < triangleCount; i++) {
          Triangle triangle = getTriangle(offset);
          float t;
          if (intersectRayTriangle(ray, triangle, t)) {
            return true;
          }
        }
      } else {
        int leftIndex = getInt(offset);
        int rightIndex = getInt(offset);
        stack[stackPointer++] = rightIndex;
        stack[stackPointer++] = leftIndex;
      }
    }
  }
  return false;
}

vec3 rayTrace(Ray ray) {
  float closestT = 1e30;
  vec3 closestColor = vec3(0.0);

  bool black = mData[1] != 0.0f;
  vec3 lightDir = normalize(vec3(-1));

  int offset = int(mData[0]);
  int constOffset = offset;
  if (traverseBVH(ray, 0, constOffset)) {
    closestColor = vec3(1,0,0);
  }
  return closestColor;
}


vec3 calculateRayDirection(vec2 screenCoords, vec2 cameraResolution, float FOV, float aspectRatio, mat3 cameraMatrix) {
  vec2 normalizedCoords = screenCoords / cameraResolution;
  vec2 ndc = vec2(normalizedCoords.x * 2 - 1, normalizedCoords.y * 2 - 1); // -1 -> 1 range
  ndc.x *= aspectRatio;

  // Calculate ray direction in view space
  vec3 rayDirectionView = normalize(vec3(ndc.x, ndc.y, -1.0 / tan(0.5 * radians(FOV))));

  // Transform the ray direction to world space using camera orientation
  vec3 rayDirectionWorld = cameraMatrix * rayDirectionView;

  return rayDirectionWorld;
}

void main() {
  vec2 pixelCoords = gl_FragCoord.xy;

  Ray ray;
  ray.mOrigin = vec3(mData[6], mData[7], mData[8]);
  vec2 resolution = vec2(mData[4], mData[5]);
  mat3 camMatrix = mat3(mData[9], mData[10], mData[11], mData[12], mData[13], mData[14], mData[15], mData[16], mData[17]);
  ray.mDirection = calculateRayDirection(pixelCoords, resolution, mData[2], mData[3], camMatrix);

  vec3 color = rayTrace(ray);
  FragColor = vec4(color, 1.0);
}
