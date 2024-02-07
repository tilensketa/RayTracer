#version 430

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Sphere {
    int index;
    float center[3];
    float color[3];
    float radius;
};

struct Scene {
    int numberOfSpheres;
    Sphere spheres[100];
};

struct Camera {
    int resX;
    int resY;
    float FOV;
    float position[3];
    float front[3];

    float moveSpeed;
    float rotateSpeed;
    float lastMousePosition[2];
};

layout(std430, binding = 0) buffer Data
{
    bool black;
    Camera camera;
    Scene scene;
} data;

out vec4 FragColor;


bool intersectRaySphere(Ray ray, Sphere sphere, out float t) {
    // Vector from ray origin to sphere center
    vec3 sphereCenter = vec3(sphere.center[0], sphere.center[1], sphere.center[2]);
    vec3 oc = ray.origin - sphereCenter;

    // Coefficients of the quadratic equation
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;

    // Calculate the discriminant
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0) {
        // No intersection
        return false;
    } else {
        // Two possible intersection points
        float t1 = (-b - sqrt(discriminant)) / (2.0 * a);
        float t2 = (-b + sqrt(discriminant)) / (2.0 * a);

        // Check if the intersection points are in front of the ray
        if (t1 >= 0.0 && t2 >= 0.0) {
            // Return the closest intersection point
            t = min(t1, t2);
            return true;
        } else {
            // Both intersection points are behind the ray
            return false;
        }
    }
}

vec3 rayTrace(Ray ray) {
    // Find the closest intersection
    float closestT = 1e30;
    vec3 closestColor = vec3(0.0);

    vec3 lightDir = normalize(vec3(-1));

    for (int i = 0; i < data.scene.numberOfSpheres; i++) {
        float t;
        Sphere sphere = data.scene.spheres[i];
        if (intersectRaySphere(ray, sphere, t)) {
            if (t < closestT) {
                closestT = t;
                vec3 sphereColor = vec3(sphere.color[0], sphere.color[1], sphere.color[2]);
                vec3 spherePosition = vec3(sphere.center[0], sphere.center[1], sphere.center[2]);

                if(data.black){
                    float red = 1.0f / data.scene.numberOfSpheres * (sphere.index + 1);
                    closestColor = vec3(red, 0, 0);
                    continue;
                }

                vec3 intersectionPoint = ray.origin + ray.direction * closestT;
                vec3 normal = normalize(intersectionPoint - spherePosition);
                float intensity = dot(normal, -lightDir);// * 0.5 + 0.5;
                closestColor = sphereColor * intensity;
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
    Camera camera = data.camera;
    vec2 cameraResolution = vec2(camera.resX, camera.resY);
    vec3 cameraPosition = vec3(camera.position[0], camera.position[1], camera.position[2]);
    vec3 cameraFront = vec3(camera.front[0], camera.front[1], camera.front[2]);
    float cameraFOV = camera.FOV;

    vec2 pixelCoords = gl_FragCoord.xy;

    Ray ray;
    ray.origin = cameraPosition;
    ray.direction = calculateRayDirection(pixelCoords, cameraResolution, cameraFOV, cameraFront);

    vec3 color = rayTrace(ray);
    FragColor = vec4(color, 1.0);
}
