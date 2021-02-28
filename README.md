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
![Screenshot4](/MandelbrotSet/Screenshot5.png?raw=true) <br>


## License
This project is under the MIT license. For full license text see LICENCE.txt

    premake5 <project-type>
    
    
## Third Party Libraries
* [Premake5](https://premake.github.io/) as a build system
* [GLFW](https://www.glfw.org/) for window creation and events
* [Glad](https://glad.dav1d.de/) as OpenGL loader
* [ImGui](https://github.com/ocornut/imgui) for UI
* [stb_image, stb_image_write](https://github.com/nothings/stb) for saving screenshots
