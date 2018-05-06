# Duck Game Engine
Duck is a C++ game engine designed for high performance. It uses a multithreaded [Vulkan](https://www.khronos.org/vulkan/) renderer with a PBR workflow. Physics is powered by [Bullet Physics](http://bulletphysics.org) for an accurate and performant physics simulation. A custom entity component system is implemented to minimize cache misses and offer opportunity  for multi threading.

Examples of the engine in action are located in the examples folder.

Building
========
### Requirements
1. [LunarG Vulkan SDK](https://www.lunarg.com/vulkan-sdk/). Minimum version 1.1.70.0.
2. [Bullet Physics](https://github.com/bulletphysics/bullet3). Minimum version 2.85.
3. [SDL2](https://www.libsdl.org/download-2.0.php). Minimum version 2.0.7.
4. [GLM](https://glm.g-truc.net/0.9.8/index.html). Minimum version 0.9.7.

All other dependencies are included.

### CMake
1. Download or clone the Duck Engine repository.
2. Use CMake to build the engine for your prefered IDE or build tool.
3. Enjoy!

Sometimes CMake will be unable to locate the installation locations for different libraries (Especially on Windows systems.) To get around this, define the following variables when building with CMake.

+ **VULKAN_INCLUDE_DIR** - Vulkan include folder.
+ **VULKAN_LIBRARY** - vulkan-1.lib.
+ **SDL2MAIN_LIBRARY** - SDL2main.lib.
+ **SDL2_LIBRARY** - SDL2.lib.
+ **SDL2_INCLUDE_DIR** - SDL2 include folder.
+ **GLM_INCLUDE_DIR** - GLM include folder.
+ **Bullet_BASE** - Bullet Physics include/source folder.
+ **BulletDynamics_LIBRARY** - Bullet Physics BulletDynamics.lib release version.
+ **BulletCollision_LIBRARY** - Bullet Physics BulletCollision.lib release version.
+ **LinearMath_LIBRARY** - Bullet Physics LinearMath.lib release version.
+ **BulletDynamics_LIBRARY_DEBUG** - Bullet Physics BulletDynamics.lib debug version.
+ **BulletCollision_LIBRARY_DEBUG** - Bullet Physics BulletCollision.lib debug version.
+ **LinearMath_LIBRARY_DEBUG** - Bullet Physics LinearMath.lib debug version.