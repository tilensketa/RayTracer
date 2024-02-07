#pragma once

#include "Camera.h"

struct Sphere {
  int Index;
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
  void add(Sphere sphere){
    sphere.Index = numberOfSpheres;
    spheres[numberOfSpheres] = sphere;
    numberOfSpheres++;
  }
};

struct Data {
  bool black;
  Camera camera;
  Scene scene;
};
