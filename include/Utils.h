struct Sphere {
  float Position[3];
  float Color[3];
  float Size;
};

struct Scene {
  int numberOfSpheres;
  Sphere spheres[100];

  Scene() {
    numberOfSpheres = 0;
  }
  void add(const Sphere& sphere){
    spheres[numberOfSpheres] = sphere;
    numberOfSpheres++;
  }
};

struct Data {
  unsigned int ResolutionX;
  unsigned int ResolutionY;

  Scene scene;
};
