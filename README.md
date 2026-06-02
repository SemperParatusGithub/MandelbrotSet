# Mandelbrot Set
## General

A simple C++ OpenGL realtime Mandelbrot / Julia set renderer. Everything is rendered
on the GPU using shaders and the OpenGL Shading Language (GLSL). Targets
**OpenGL 3.3 Core**, so it runs on Windows, Linux and macOS (including
Apple Silicon).

<b> How does it work?</b> <br>
The Mandelbrot Set is defined as a set of points in the complex plain. <br>
Each pixel value is calculated iteratively with a function defined as:

f<sub>c</sub>(z) = z<sup>2</sup> + c

When iteratively processing this function we want to find out if the value of the function converges to some <br> constant or diverges to infinity

When taking a closer look at the fragment shader when can see that we first retrieve the pixels<br> position using gl_FragCoord. After taking the zoom and offset in account we then pass the <br>
coordinates to the mandelbrot function which returns the appropriate pixel value. <br>
After that the final colour is retrieved by the function MapToColor.

```glsl
float Mandelbrot(vec2 fragCoord)
{
	int n = 0;
	vec2 z = vec2(0.0);
	for (n = 0; n < u_MaxIterations; n++)
	{
		vec2 znew;
		znew.x = (z.x * z.x) - (z.y * z.y) + fragCoord.x;
		znew.y = (2.0 * z.x * z.y) + fragCoord.y;
		z = znew;
		if ((z.x * z.x) + (z.y * z.y) > 16.0)
			break;
	}
	return n / float(u_MaxIterations);
}
```
The full source code of the shader is available [here](/MandelbrotSet/Shaders/Mandelbrot.glsl).
If you want to know more about the Mandelbrot set: <https://en.wikipedia.org/wiki/Mandelbrot_set>

### A note on precision

The shader uses single‑precision `float` everywhere, which gives a useful zoom
range up to ~5×10⁶ before fp32 quantization becomes visible (the cap is
enforced as `MaxZoomLevel`). Going deeper requires either *emulated double‑float*
arithmetic in the shader (about 2× the mantissa) or *perturbation theory* with a
CPU reference orbit (effectively unlimited zoom). The latter is the same
technique used by Kalles Fraktaler / Mandel Machine and is portable to any
GPU / API.


## Screenshots

![Screenshot1](/MandelbrotSet/Screenshot1.png?raw=true) <br>
![Screenshot2](/MandelbrotSet/Screenshot2.png?raw=true) <br>
![Screenshot3](/MandelbrotSet/Screenshot3.png?raw=true) <br>
![Screenshot4](/MandelbrotSet/Screenshot4.png?raw=true) <br>
![Screenshot5](/MandelbrotSet/Screenshot5.png?raw=true) <br>
![Screenshot6](/MandelbrotSet/Screenshot6.png?raw=true) <br>
![Screenshot7](/MandelbrotSet/Screenshot7.png?raw=true) <br>


## Controls

| Input | Action |
|---|---|
| Mouse drag | Pan |
| **W** / **A** / **S** / **D** | Pan (frame‑rate‑independent, scales with zoom) |
| Scroll wheel | Zoom |

## Requirements

- A C++17 compiler (MSVC 2019+, Clang 10+, GCC 9+)
- [CMake](https://cmake.org/) 3.16+
- A GPU driver supporting **OpenGL 3.3 Core** (effectively every desktop GPU made since ~2010)
- **Linux only**: X11 development packages — on Debian/Ubuntu:
  ```
  sudo apt install build-essential libx11-dev libxrandr-dev libxinerama-dev \
                   libxcursor-dev libxi-dev libgl1-mesa-dev
  ```

All third‑party dependencies (GLFW, Glad, ImGui, stb) are vendored under
`MandelbrotSet/External/`, so no further package installation is required.

## Building

```
cmake -S . -B build
cmake --build build --config Release -j
```

The executable lands at
`Binaries/Release-<system>-<arch>/MandelbrotSet[.exe]`. The `Shaders/` and
`Fonts/` directories are copied next to the binary as a post‑build step so the
program can be launched directly from the output directory:

```
./Binaries/Release-Linux-x86_64/MandelbrotSet
```

When running inside Visual Studio or Xcode, F5/Run uses `MandelbrotSet/` as the
working directory automatically. If you launch the executable from somewhere
else and see "Could not open shader file 'Shaders/Mandelbrot.glsl'", set the
working directory to either `MandelbrotSet/` or the binary's output directory.

## License
This project is under the MIT license. For full license text see [LICENSE.txt](LICENSE.txt).


## Third Party Libraries
* [GLFW](https://www.glfw.org/) for window creation and events
* [Glad](https://glad.dav1d.de/) as OpenGL loader
* [ImGui](https://github.com/ocornut/imgui) for UI
* [stb_image, stb_image_write](https://github.com/nothings/stb) for saving screenshots
