#version 430

struct Ray {
    vec3 mOrigin;
    vec3 mDirection;
};
struct Vertex {
    vec3 mPosition;
    vec3 mNormal;
};
struct Triangle {
    Vertex mVertices[3];
};

layout(std430, binding = 0) buffer Data
{
    float mData[100000];
};

out vec4 FragColor;


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

    bool black = mData[0] != 0;
    vec3 lightDir = normalize(vec3(-1));

    int offset = 10;
    int modelCount = int(mData[offset]);
    offset++;
    for (int i = 0; i < modelCount; i++) {
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
                    vert.mNormal = vec3(mData[offset+3], mData[offset+4], mData[offset+5]);
                    triangle.mVertices[l] = vert;
                    offset+=6;
                }
                float t;
                if (intersectRayTriangle(ray, triangle, t)) {
                    if (t < closestT) {
                        closestT = t;
                        vec3 modelColor = vec3(0,1,0);
                        if(black) {
                            closestColor = modelColor;
                            continue;
                        }
                        vec3 normal = triangle.mVertices[0].mNormal;
                        float intensity = dot(normal, -lightDir) * 0.5 + 0.5;
                        closestColor = modelColor * intensity;
                    }
                }
            }
        }
    }
    return closestColor;
}

mat3 calculateCameraMatrix(vec3 cameraFront) {
    vec3 cameraUp = vec3(0, 1, 0);
    vec3 cameraRight = normalize(cross(cameraFront, cameraUp));
    vec3 newUp = cross(cameraRight, cameraFront);
    return mat3(cameraRight, newUp, -cameraFront);
}

vec3 calculateRayDirection(vec2 screenCoords, vec2 cameraResolution, float FOV, vec3 cameraFront) {
    float aspectRatio = float(cameraResolution.x) / float(cameraResolution.y);
    vec2 normalizedCoords = screenCoords / cameraResolution;
    vec2 ndc = vec2(normalizedCoords.x * 2 - 1, normalizedCoords.y * 2 - 1); // -1 -> 1 range
    ndc.x *= aspectRatio;

    // Calculate ray direction in view space
    vec3 rayDirectionView = normalize(vec3(ndc.x, ndc.y, -1.0 / tan(0.5 * radians(FOV))));
    //return rayDirectionView;

    // Transform the ray direction to world space using camera orientation
    mat3 cameraMatrix = calculateCameraMatrix(cameraFront);
    vec3 rayDirectionWorld = cameraMatrix * rayDirectionView;

    return rayDirectionWorld;
}

void main() {
    vec2 pixelCoords = gl_FragCoord.xy;

    Ray ray;
    ray.mOrigin = vec3(mData[4], mData[5], mData[6]);
    vec2 resolution = vec2(mData[1], mData[2]);
    float FOV = mData[3];
    vec3 front = vec3(mData[7], mData[8], mData[9]);
    ray.mDirection = calculateRayDirection(pixelCoords, resolution, FOV, front);

    vec3 color = rayTrace(ray);
    FragColor = vec4(color, 1.0);
    vec2 nc = pixelCoords / resolution;
    FragColor = vec4(color, 1);
}
