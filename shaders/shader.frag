#version 430

struct Ray {
  vec3 mOrigin;
  vec3 mDirection;
};
struct Vertex {
  vec3 mPosition;
};
struct Triangle {
  Vertex mVertices[3];
  vec3 mNormal;
};

layout(std430, binding = 0) buffer Data
{
  float mData[10000000];
};

out vec4 FragColor;

float findMinComponent(vec3 vector) {
  return min(min(vector.x, vector.y), vector.z);
}
float findMaxComponent(vec3 vector) {
  return max(max(vector.x, vector.y), vector.z);
}
bool intersectRayAABB(Ray ray, vec3 maxVert, vec3 minVert) {
  vec3 invDir = 1.0f / ray.mDirection;
  vec3 t1 = (minVert - ray.mOrigin) * invDir;
  vec3 t2 = (maxVert - ray.mOrigin) * invDir;

  float tNear = findMaxComponent(min(t1, t2));
  float tFar = findMinComponent(max(t1, t2));

  return tNear <= tFar && tFar >= 0;
}

bool intersectRayTriangle(Ray ray, Triangle triangle, out float outT) {
  float EPSILON = 0.000001f;
  vec3 edge1, edge2, h, s, q;
  float a, f, u, v;

  edge1 = triangle.mVertices[1].mPosition - triangle.mVertices[0].mPosition;
  edge2 = triangle.mVertices[2].mPosition - triangle.mVertices[0].mPosition;
  h = cross(ray.mDirection, edge2);
  a = dot(edge1, h);

  if (a > -EPSILON && a < EPSILON)
    return false;

  f = 1.0f / a;
  s = ray.mOrigin - triangle.mVertices[0].mPosition;
  u = f * dot(s, h);

  if (u < 0.0f || u > 1.0f)
    return false;

  q = cross(s, edge1);
  v = f * dot(ray.mDirection, q);

  if (v < 0.0f || u + v > 1.0f)
    return false;

  outT = f * dot(edge2, q);

  if (outT > EPSILON)
    return true;

  return false;
}

vec3 rayTrace(Ray ray) {
  // Find the closest intersection
  float closestT = 1e30;
  vec3 closestColor = vec3(0.0);

  bool black = mData[1] != 0.0f;
  vec3 lightDir = normalize(vec3(-1));

  int offset = int(mData[0]);
  int modelCount = int(mData[offset]);
  offset++;
  for (int i = 0; i < modelCount; i++) {
    int modelIndex = int(mData[offset]);
    offset++;
    vec3 maxVert = vec3(mData[offset], mData[offset+1], mData[offset+2]);
    vec3 minVert = vec3(mData[offset+3], mData[offset+4], mData[offset+5]);
    offset+=6;

    if(!intersectRayAABB(ray, maxVert, minVert)) {
      int meshCount = int(mData[offset]);
      offset++;
      for (int j = 0; j < meshCount; j++) {
        int triangleCount = int(mData[offset]);
        offset++;
        for (int k = 0; k < triangleCount; k++) {
          offset+=12;
        }
      }
      continue;
    }

    int meshCount = int(mData[offset]);
    offset++;
    for (int j = 0; j < meshCount; j++) {
      int triangleCount = int(mData[offset]);
      offset++;
      for (int k = 0; k < triangleCount; k++) {
        Triangle triangle;
        for (int l = 0; l < 3; l++) {
          Vertex vert;
          vert.mPosition = vec3(mData[offset], mData[offset+1], mData[offset+2]);
          triangle.mVertices[l] = vert;
          offset+=3;
        }
        triangle.mNormal = vec3(mData[offset], mData[offset+1], mData[offset+2]);
        offset+=3;
        float t;
        if (intersectRayTriangle(ray, triangle, t)) {
          if (t < closestT) {
            closestT = t;
            vec3 modelColor = vec3(0,1,0);
            if(black) {
              float red = (float(modelIndex)+1) / float(modelCount);
              closestColor = vec3(red, 0, 0);
              continue;
            }
            vec3 normal = triangle.mNormal;
            float intensity = dot(normal, -lightDir) * 0.5 + 0.5;
            closestColor = modelColor * intensity;
          }
        }
      }
    }
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
