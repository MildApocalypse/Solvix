#shader vertex
#version 330 core

layout (location = 0) in vec2 aPosition;

out vec2 UV;

void main() 
{
	gl_Position = vec4(aPosition, 0, 1.0);
	UV = (aPosition + vec2(1, 1)) / 2.0;
};

#shader fragment
#version 330 core

in vec2 UV;

out vec4 cell_data;

uniform sampler2D u_renderedTexture;

void main()
{
	cell_data = texture(u_renderedTexture, UV);
};