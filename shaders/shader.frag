#version 430

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Sphere {
    float center[3];
    float color[3];
    float radius;
};

struct Scene {
    int numberOfSpheres;
    Sphere spheres[100];
};

layout(std430, binding = 0) buffer Data
{
    int resX;
    int resY;
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

                vec3 intersectionPoint = ray.origin + ray.direction * closestT;
                vec3 normal = normalize(intersectionPoint - spherePosition);
                float intensity = dot(normal, -lightDir);// * 0.5 + 0.5;
                closestColor = sphereColor * intensity;
            }
        }
    }
    return closestColor;
}

void main() {
    float aspectRatio = float(data.resX) / float(data.resY);
    vec2 pixelCoords = gl_FragCoord.xy;
    vec2 normalizedCoords = vec2(pixelCoords.x/data.resX, pixelCoords.y/data.resY); // 0 -> 1 range
    vec2 minusToPlus = vec2(normalizedCoords.x * 2 - 1, normalizedCoords.y * 2 - 1); // -1 -> 1 range
    minusToPlus.x *= aspectRatio;

    Ray ray;
    ray.origin = vec3(0.0, 0.0, 2.0);
    ray.direction = vec3(minusToPlus.x, minusToPlus.y, -1.0);

    vec3 color = rayTrace(ray);
    FragColor = vec4(color, 1.0);
}
