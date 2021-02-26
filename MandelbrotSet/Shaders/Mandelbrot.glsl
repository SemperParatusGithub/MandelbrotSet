#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
}

#shader fragment
#version 450 core
layout(location = 0) out vec4 o_Color;

uniform int u_MaxIterations;
uniform dvec2 u_ScreenSize;
uniform double u_Zoom;
uniform dvec2 u_Offset;
uniform vec4 u_Color;

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

vec3 MapToColor(float a)
{
	float r = 10.0 * u_Color.x * (1.0 - a) * a * a * a;
	float g = 10.0 * u_Color.y * (1.0 - a) * (1.0 - a) * a * a;
	float b = 10.0 * u_Color.z * (1.0 - a) * (1.0 - a) * (1.0 - a) * a;
	
	return vec3(r, g, b);
}

void main()
{
	float pixelValue = Mandelbrot(((gl_FragCoord.xy - u_ScreenSize / 2) / u_Zoom) - u_Offset);
	vec3 color = MapToColor(pixelValue);
	o_Color = vec4(color, 1.0);
}