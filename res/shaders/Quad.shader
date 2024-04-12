#shader vertex
#version 330 core

layout (location = 0) in vec2 aPosition;

out vec2 UV;

uniform vec3 adj; //x = x offset, y = y offset, z = crop size
uniform float zoom;

void main() 
{
	gl_Position = vec4(aPosition * adj.z + adj.xy, 0, 1.0);
	UV = (aPosition + (vec2(1, 1) * zoom)) / ((2.0 * zoom) / adj.z);
};

#shader fragment
#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D u_renderedTexture;

void main()
{
	vec4 data = texture(u_renderedTexture, UV);
	vec3 rgb = vec3(1.0, 1.0, 1.0) * data.w;
	color = vec4(rgb, 1.0);
};