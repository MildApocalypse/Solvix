#shader vertex
#version 330 core

layout (location = 0) in vec2 aPosition;

out vec2 UV;

uniform vec3 adj; //x = x offset, y = y offset, z = crop size
uniform float zoom;

void main() 
{
	gl_Position = vec4(aPosition, 0, 1.0);
	UV = (aPosition + vec2(1, 1)) / 2.0;
};

#shader fragment
#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D u_renderedTexture;

void main()
{
	vec4 data = texture(u_renderedTexture, UV);
	color = data * 255.0f;
};