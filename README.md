Percularity is a a game engine programmed from scratch with `C++` using open third-party libraries. It has been developed by [Daniel Lorenzo](https://www.linkedin.com/in/daniel-lorenzo-laguno-a2ab35180/) & [Joan Marín](https://www.linkedin.com/in/joan-marín-encinas-6a4693191/), two third course students of the [Bachelor's Degree in Video Games by UPC at CITM](https://www.citm.upc.edu/ing/estudis/graus-videojocs/). The purpouse of the subject was to build a game engine useful enough it could allow us to develop a game with the whole class as a big team. Using pairs, each team developed their own version of the program with the idea of mergeing the best parts of each of them into the best game engine we could get.

<p align="center">
  <iframe width="600" height="335" src="https://www.youtube.com/embed/1fTSsjieH-Q" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>

You can download Percularity's [latest release](https://github.com/DLorenzoLaguno17/PercularityEngine/releases) or check out our [GitHub](https://github.com/DLorenzoLaguno17/PercularityEngine) repository! 

# About Percularity

<p align="center">
  <img src="https://github.com/DLorenzoLaguno17/PercularityEngine/blob/master/Page%20assets/PercularityIcon.png?raw=true" alt="Icon" width="100" height="100">
</p>

### Main core sub-systems

The main core sub-systems that form Percularity are the following:
 * The **resource manager** handles all the external files that are loaded into the engine. Whenever the user drags and drops a texture, an image, a JSON file or an FBX, this subsystem reads it using Assimp and DevIL, importing them into Percularity's own file format and stores it in its library. 
 * The **camera** manages the frustum culling, which renders only what is inside the view of the camera and the mousepicking, that allows the user select GameObjects clicking in them.
 * The **scene** has all the GameObjects and is in charge of **Game mode**, all the hierarchy of the GameObjects and the save and load of projects.
 * The **renderer** is in charge of the UI (wich uses ImGui) and controls the order of what is visible on the screen.
 * The **physics** sub-system is in charge of managing the dynamics of the GameObjects, such as gravity and interactions between them.
 * The **application** is the most important sub-system. Is in control of all the modules and makes its loop.

### Specialized sub-system

<p align="center">
  <img src="https://github.com/DLorenzoLaguno17/PercularityEngine/blob/master/Page%20assets/Gif2.gif?raw=true" alt="Gif1" width="600" height="335"/>
</p>

Since we wanted the engine to be used for creating the game, each pair of students specialized the last delivery of the game engine development in one specific subsystem. The sub-system we chose was **physics**. With it, we programmed the possibility of adding **ridigbodies** as GameObject components (either box, sphere or capsule colliders), and we implemented the possibility of controlling a car in **Game mode**, which can interact with the scene and its collisions. Appart from that, the user can shoot spheres from the camera also to test the physics and we have added a sphere collider around the camera that prevents it from getting inside the meshes of the GameObjects.

<p align="center">
  <img src="https://github.com/DLorenzoLaguno17/PercularityEngine/blob/master/Page%20assets/Gif1.gif?raw=true" alt="Citizen BT" width="600" height="335"/>
</p>

### Third-party libraries used
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

# Tasks delivered by each team member

### Joan Marín

<p align="center">
  <img src="https://github.com/DLorenzoLaguno17/PercularityEngine/blob/master/Page%20assets/Joan.jpg?raw=true" alt="Joan" width="200" height="250">
</p>

* Camera component.
* Frustum culling.
* Mousepicking.
* GameObject transformations.
* Quadtrees and octrees.
* Physicalization of the GameObjects.

### Daniel Lorenzo

<p align="center">
  <img src="https://github.com/DLorenzoLaguno17/PercularityEngine/blob/master/Page%20assets/Dani.jpg?raw=true" alt="Dani" width="200" height="300">
</p>

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
