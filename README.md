# Mandelbrot Set
## General

A simple C++ OpenGL realtime Mandelbrot set renderer. Everything is rendered on the GPU <br> 
using shaders and the OpenGL-Shading-Language (GLSL).

<b> How does it work?</b> <br>
The Mandelbrot Set is defined as a set of points in the complex plain. <br>
Each pixel value is calculated iteratively with a function defined as:

f<sub>c</sub>(z) = z<sup>2</sup> + c

When iteratively processing this function we want to find out if the value of the function converges to some <br> constant or diverges to infinity

When taking a closer look at the fragment shader when can see that we first retrieve the pixels<br> position using gl_FragCoord. After taking the zoom and offset in account we then pass the <br>
coordinates to the mandelbrot function which returns the appropriate pixel value. <br>
After that the final colour is retrieved by the function MapToColor.

```
float Mandelbrot(dvec2 fragCoord)
{
	int n = 0;
	dvec2 z = vec2(0.0, 0.0);
	for (n = 0; n < u_MaxIterations; n++)
	{
		dvec2 znew;
		znew.x = (z.x * z.x) - (z.y * z.y) + fragCoord.x;
		znew.y = (2.0 * z.x * z.y) + fragCoord.y;
		z = znew;
		if ((z.x * z.x) + (z.y * z.y) > 16.0)
			break;
	}
	return n / float(u_MaxIterations);
}
```
The full source code of the shader is available [here](/MandelbrotSet/Shaders/Mandelbrot.glsl)<br>
If you want to know more about the Mandelbrot set: https://en.wikipedia.org/wiki/Mandelbrot_set


## Screenshots

![Screenshot1](/MandelbrotSet/Screenshot1.png?raw=true) <br>
![Screenshot2](/MandelbrotSet/Screenshot2.png?raw=true) <br>
![Screenshot3](/MandelbrotSet/Screenshot3.png?raw=true) <br>
![Screenshot4](/MandelbrotSet/Screenshot4.png?raw=true) <br>
![Screenshot5](/MandelbrotSet/Screenshot5.png?raw=true) <br>
![Screenshot6](/MandelbrotSet/Screenshot6.png?raw=true) <br>
![Screenshot7](/MandelbrotSet/Screenshot7.png?raw=true) <br>


## Building

The project uses [CMake](https://cmake.org/) (3.16+) and a C++17 compiler.
All third-party dependencies are vendored under `MandelbrotSet/External/`, so no
package installation is required beyond the OS development packages listed
below.

### Linux prerequisites

GLFW links against X11; install the corresponding `-dev` packages, e.g. on
Debian/Ubuntu:

```
sudo apt install build-essential libx11-dev libxrandr-dev libxinerama-dev \
                 libxcursor-dev libxi-dev libgl1-mesa-dev
```

### Configure & build

```
cmake -S . -B build
cmake --build build --config Release -j
```

This produces the executable at
`Binaries/Release-<system>-<arch>/MandelbrotSet[.exe]`. The `Shaders/` and
`Fonts/` directories are copied next to the binary as a post-build step, so
the executable can be launched directly from the output directory.

When working inside Visual Studio or Xcode, F5/Run uses
`MandelbrotSet/` as the working directory automatically.

## License
This project is under the MIT license. For full license text see [LICENSE.txt](LICENSE.txt).


## Third Party Libraries
* [GLFW](https://www.glfw.org/) for window creation and events
* [Glad](https://glad.dav1d.de/) as OpenGL loader
* [ImGui](https://github.com/ocornut/imgui) for UI
* [stb_image, stb_image_write](https://github.com/nothings/stb) for saving screenshots
