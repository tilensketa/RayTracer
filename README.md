# RayTracer

Ray Tracer with OpenGL and compute shaders.

---

![basicRayTracer](./renders/basicRayTracing.png)
Implemented basic sphere/ray intersection, calculating hit position, normal and shading of spheres.

<p align="center">
  <img src="./renders/modelLoadingFlatColor.png" width="49%" alt="Flat">
  <img src="./renders/modelLoadingShading.png" width="49%" alt="Shaded">
</p>
Added model loading support and option for switching from flat to shaded view.

![fixedNormalsBug](./renders/fixedNormalsBug.png)
Improved memory layout on GPU and fixed normals.

![bvhWithMaterials](./renders/bvhWithMaterials.png)
Bounding volume hierarchy with materials. Major fps improvment.

![gui](./renders/gui.png)
Graphical user interface with imgui. Functionality for loading and deleting models from scene. A bit of info of scene.

![modelTransformsAndCameraGui](./renders/modelTransformsAndCameraGui.png)
Added more options to modify and transform model. And added properties of camera.

<p align="center">
  <img src="./renders/flat.png" width="32%" alt="Flat">
  <img src="./renders/shaded.png" width="32%" alt="Shaded">
  <img src="./renders/wireframe.png" width="32%" alt="Wireframe">
</p>
Added wireframe view, downsampling, faster scene updating and some refactoring.
