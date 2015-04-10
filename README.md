# opengl-terrain

## Building, Compiling and running the project
In `CMake`, where is the source:  `.../opengl-terrain` and where is the binaries: `opengl-terrain/build`.
Configure (generator is `Visual Stdio 12 2013 Win64` or similar), generate.
Copy all folders except CMakeFiles of ` opengl-terrain/source/` to `opengl-terrain\build\source`. Some shaders won't be found without this step.

Finally, open `\opengl-terrain\build\opengl_terrain.sln` in Visual Studio or similar, and Set as StartUp Project `source`.
