# Percularity Engine
Percularity is a game engine that has been developed in the Video Games degree in Universitat Politècnica de Catalunya for student purposes.

Go to our GitHub: https://github.com/DLorenzoLaguno17/PercularityEngine

## Main core sub-systems

## Libraries used
* [SDL v2.0.6](https://www.libsdl.org/)
* [STL v2.0](https://www.geeksforgeeks.org/the-c-standard-template-library-stl/)
* [Dear ImGui v1.72b](https://github.com/ocornut/imgui)
* [MathGeoLib v1.5](https://github.com/juj/MathGeoLib)
* [OpenGL v4.5](https://www.opengl.org/)
* [Glew v2.1](http://glew.sourceforge.net/)
* [Assimp 4.1](http://assimp.org/)
* [DevIL v1.8](http://openil.sourceforge.net/)
* [PhysFS v3.0.2](https://icculus.org/physfs/)

## Innovation

We have implemented a debug window with which the user can decide wether to activate or not the frustum culling and there is another menu that shows the current performance of the engine and how the changes on the debug window affect it.

## Controls

### Camera movement

* F - Focus the camera on the selected GameObject

* Hold right click - rotate the camera
* Hold middle click - Move the camera at will
* Mouse scroll - Zoom in & out

* F1 - In Play mode, switch between car camera and free camera

While right clicking:
* W - Move camera up
* S - Move camera down
* A - Move camera left
* D - Move camera right
* T - Rise the camera
* G - Descend the camera

* Left shift - Duplicate camera movement speed

### Save & Load

* CTRL + S - Save the scene
* CTRL + L - Load the scene

Note: for the moment, each time a scene is saved, it is placed at Assets/Scenes/ and it's always called Scene.json, and only scenes named like that can be loaded by now.

## Tasks delivered by each member

### Joan Marín
* Camera component.
* Frustum culling.
* Mousepicking.
* GameObject transformations.
* Quadtrees and octrees.

### Daniel Lorenzo
* Import and export of files.
* Own file format.
* Serialization of the scene.
* Resource manager.

## License
This game engine has been created by third course students [Joan Marín](https://github.com/X0KA) and [Daniel Lorenzo](https://github.com/DLorenzoLaguno17).
Licensed under the [MIT License](LICENSE).
