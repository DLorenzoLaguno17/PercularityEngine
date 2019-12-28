Percularity is a a game engine programmed from scratch with `C++` using open third-party libraries. It has been developed by [Daniel Lorenzo](https://www.linkedin.com/in/daniel-lorenzo-laguno-a2ab35180/) & [Joan Marín](https://www.linkedin.com/in/joan-marín-encinas-6a4693191/), two third course students of the [Bachelor's Degree in Video Games by UPC at CITM](https://www.citm.upc.edu/ing/estudis/graus-videojocs/). The purpouse of the subject was to build a game engine useful enough it could allow us to develop a game with the whole class as a big team. Using pairs, each team developed their own version of the program with the idea of mergeing the best parts of each of them into the best game engine we could get.

You can download Percularity's [latest release](https://github.com/DLorenzoLaguno17/PercularityEngine/releases) or check out our [GitHub](https://github.com/DLorenzoLaguno17/PercularityEngine) repository! 

# About Percularity

## Main core sub-systems
The main core sub-systems that form Percularity are the **resource manager**, the **physics** sub-system

## Specialized sub-system
Since we wanted the engine to be used for creating the game, each pair of students specialyzed the last delivery of the game engine development in one specific subsystem. The sub-system we chose was **physics**. With it, we programmed the possibility of adding **ridigbodies** as GameObject components, and we implemented the possibility of controlling a car in **Game mode**, which can interact with the scene and its collisions. Appart from that, the user can shoot spheres from the camera also to test the physics and we have added a sphere collider around the camera that prevents it from getting inside the meshes of the GameObjects.

## Third-party libraries used
* [SDL v2.0.6](https://www.libsdl.org/)
* [STL v2.0](https://www.geeksforgeeks.org/the-c-standard-template-library-stl/)
* [Dear ImGui v1.72b](https://github.com/ocornut/imgui)
* [MathGeoLib v1.5](https://github.com/juj/MathGeoLib)
* [OpenGL v4.5](https://www.opengl.org/)
* [Glew v2.1](http://glew.sourceforge.net/)
* [Assimp 4.1](http://assimp.org/)
* [DevIL v1.8](http://openil.sourceforge.net/)
* [PhysFS v3.0.2](https://icculus.org/physfs/)
* [Nlohmann's JSON v3.7.3](https://github.com/nlohmann/json)
* [Bullet]()

# Tasks delivered by each team member

## Joan Marín
* Camera component.
* Frustum culling.
* Mousepicking.
* GameObject transformations.
* Quadtrees and octrees.
* Physicalization of the GameObjects.

## Daniel Lorenzo
* User interface.
* GameObjects.
* Import and export of textures and models.
* Own file format.
* Serialization of the scene.
* Resource manager.
* Mesh, material and rigidbody components.
* Car and spheres to test the collisions of the scene.

# License
This game engine is licensed under the [MIT License](LICENSE).
