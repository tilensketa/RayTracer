#version 430

int CAMERA_OFFSET =  0;
int VERTICES_OFFSET = 1;
int BVH_OFFSET = 2;
int MATERIAL_OFFSET = 3;
int LIGHTS_OFFSET = 4;

int POINT_LIGHT = 0;
int DIRECTIONAL_LIGHT = 1;

struct Ray {
  vec3 mOrigin;
  vec3 mDirection;
};
struct Vertex {
  vec3 mPosition;
};
int vertexSize = 3;

struct Triangle {
  int mModelIndex;
  int mMeshIndex;
  int mIndices[3];
  Vertex mVertices[3];
  vec3 mNormal;
};

struct Material {
  vec3 mDiffuse;
  //vec3 mAmbient;
};

struct BoundingBox {
  vec3 mMaxVert;
  vec3 mMinVert;
};

struct Camera {
  float mFOV;
  float mAspectRatio;
  vec2 mResolution;
  vec3 mPosition;
  mat3 mMatrix;
};

struct Settings {
  bool mBlack;
};

struct Light {
  int mType;
  float mIntensity;
  vec3 mPosition;
  vec3 mDirection;
  vec3 mColor;
};

struct HitPayload {
  bool mHit;
  Triangle mTriangle;
  float mClosestHit;
  vec3 mWorldPosition;
};

layout(std430, binding = 0) buffer Data
{
  float mData[10000000];
};

out vec4 FragColor;

bool getBool(inout int offset) {
  bool bol = mData[offset] != 0.0f;
  offset++;
  return bol;
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
vec2 getVec2(inout int offset) {
  vec2 vector = vec2(mData[offset], mData[offset + 1]);
  offset += 2;
  return vector;
}
vec3 getVec3(inout int offset) {
  vec3 vector = vec3(mData[offset], mData[offset + 1], mData[offset + 2]);
  offset += 3;
  return vector;
}
mat3 getMat3(inout int offset) {
  mat3 matrix = mat3(mData[offset], mData[offset + 1], mData[offset + 2], mData[offset + 3], mData[offset + 4], mData[offset + 5], mData[offset + 6], mData[offset + 7], mData[offset + 8]);
  offset += 9;
  return matrix;
}
Vertex getVertex(inout int offset) {
  Vertex vertex;
  vertex.mPosition = getVec3(offset);
  return vertex;
}
Triangle getTriangle(inout int offset) {
  Triangle triangle;
  triangle.mModelIndex = getInt(offset);
  triangle.mMeshIndex = getInt(offset);
  triangle.mIndices[0] = getInt(offset);
  triangle.mIndices[1] = getInt(offset);
  triangle.mIndices[2] = getInt(offset);
  triangle.mNormal = getVec3(offset);

  int verticesOffset = int(mData[VERTICES_OFFSET]);
  for (int i = 0; i < 3; i++) {
    int vertOffset = verticesOffset + triangle.mIndices[i] * vertexSize;
    triangle.mVertices[i] = getVertex(vertOffset);
  }
  return triangle;
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
  // material.mAmbient = getVec3(offset);
  return material;
}
Settings getSettings(inout int offset) {
  Settings settings;
  settings.mBlack = getBool(offset);
  return settings;
}
Camera getCamera(inout int offset) {
  Camera camera;
  camera.mFOV = getFloat(offset);
  camera.mAspectRatio = getFloat(offset);
  camera.mResolution = getVec2(offset);
  camera.mPosition = getVec3(offset);
  camera.mMatrix = getMat3(offset);
  return camera;
}
Light getLight(inout int offset) {
  Light light;
  light.mType = getInt(offset);
  light.mIntensity = getFloat(offset);
  light.mPosition = getVec3(offset);
  light.mDirection = getVec3(offset);
  light.mColor = getVec3(offset);
  return light;
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

HitPayload miss() {
  HitPayload payload;
  payload.mHit = false;
  return payload;
}

HitPayload closestHit(float t, Triangle triangle, vec3 worldPosition) {
  HitPayload payload;
  payload.mHit = true;
  payload.mClosestHit = t;
  payload.mTriangle = triangle;
  payload.mWorldPosition = worldPosition;
  return payload;
}

HitPayload traverseBVH(Ray ray, int nodeIndex) {
  int BVHOffset = int(mData[BVH_OFFSET]);

  float closestT = 1e30;
  Triangle closestTriangle;
  bool hit;
  vec3 worldPosition;

  int stack[100];
  int stackPointer = 0;

  stack[stackPointer++] = nodeIndex;

  while (stackPointer > 0) {
    int currentIndex = stack[--stackPointer];
    int offset = int(mData[BVHOffset + currentIndex]);
    BoundingBox aabb = getAABB(offset);

    if (intersectRayAABB(ray, aabb)) {
      bool isLeaf = getBool(offset);
      if (isLeaf) {
        int triangleCount = getInt(offset);
        for (int i = 0; i < triangleCount; i++) {
          Triangle triangle = getTriangle(offset);
          float t;
          if (intersectRayTriangle(ray, triangle, t)) {
            if(t < closestT){
              closestT = t;
              closestTriangle = triangle;
              worldPosition = ray.mOrigin + ray.mDirection * t;
            }
          }
        }
      } else {
        int leftIndex = getInt(offset);
        int rightIndex = getInt(offset);
        stack[stackPointer++] = leftIndex;
        stack[stackPointer++] = rightIndex;
      }
    }
  }
  if (closestT < 1e30) {
    return closestHit(closestT, closestTriangle, worldPosition);
  }
  return miss();
}

vec3 rayTrace(Ray ray, Settings settings) {
  vec3 closestColor = vec3(0.0);

  vec3 lightDir = normalize(vec3(-1));

  HitPayload payload = traverseBVH(ray, 0);
  if (payload.mHit) {
    int materialOffset = int(mData[MATERIAL_OFFSET]);
    int modelMaterialOffset = int(mData[materialOffset + payload.mTriangle.mModelIndex]);
    int meshMaterialOffset = modelMaterialOffset + payload.mTriangle.mMeshIndex * 3; // 3 -> material size
    Material material = getMaterial(meshMaterialOffset);

    if (settings.mBlack) {
      return material.mDiffuse;
    }

    vec3 totalColor = vec3(0.0f);

    int lightsOffset = int(mData[LIGHTS_OFFSET]);
    int lightsCount = getInt(lightsOffset);

    for(int i = 0; i < lightsCount; i++) {
      Light light = getLight(lightsOffset);

      vec3 lightDirection;
      float intensity;

      if (light.mType == POINT_LIGHT) {
        lightDirection = normalize(light.mPosition - payload.mWorldPosition);
        float distance = length(light.mPosition - payload.mWorldPosition);
        intensity = 1.0 / (light.mDirection.x + light.mDirection.y * distance + light.mDirection.z * distance * distance);
      }
      else if (light.mType == DIRECTIONAL_LIGHT) {
        lightDirection = normalize(-light.mDirection);
        intensity = light.mIntensity;
      }

      float diffuseIntensity = dot(payload.mTriangle.mNormal, -lightDirection) * 0.5 + 0.5;
      vec3 diffuseColor = material.mDiffuse * diffuseIntensity;

      vec3 lightContribution = diffuseColor * light.mColor * intensity;
      totalColor += lightContribution;
    }
    closestColor = totalColor;
  }
  return closestColor;
}


vec3 calculateRayDirection(vec2 screenCoords, Camera camera) {
  vec2 normalizedCoords = screenCoords / camera.mResolution;
  vec2 ndc = vec2(normalizedCoords.x * 2 - 1, normalizedCoords.y * 2 - 1); // -1 -> 1 range
  ndc.x *= camera.mAspectRatio;

  // Calculate ray direction in view space
  vec3 rayDirectionView = normalize(vec3(ndc.x, ndc.y, -1.0 / tan(0.5 * radians(camera.mFOV))));

  // Transform the ray direction to world space using camera orientation
  vec3 rayDirectionWorld = camera.mMatrix * rayDirectionView;

  return rayDirectionWorld;
}

void main() {
  vec2 pixelCoords = gl_FragCoord.xy;

  int cameraOffset = int(mData[CAMERA_OFFSET]);
  Settings settings = getSettings(cameraOffset);
  Camera cam = getCamera(cameraOffset);

  Ray ray;
  ray.mOrigin = cam.mPosition;
  ray.mDirection = calculateRayDirection(pixelCoords, cam);

  vec3 color = rayTrace(ray, settings);
  FragColor = vec4(color, 1.0);
}
