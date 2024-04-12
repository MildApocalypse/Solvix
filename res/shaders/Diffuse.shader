#shader vertex //Sets gl_Position coords, Every shader has both fragment and vertex code in same file
#version 330 core

layout(location = 0) in vec2 aPosition;

void main()
{
	gl_Position = vec4(aPosition, 0, 1.0);
};

#shader fragment //Calculate diffusion of values with stable gauss seidel method
#version 330 core

layout(location = 0) out vec4 cell_data;

uniform sampler2D u_Texture; //copy of texture being written into/iterated on
uniform sampler2D u_Prev; //texture from previous frame
uniform float u_Diff; //rate of diffusion/viscocity
uniform float u_Timestep; //rate of time
uniform bool b;

void main()
{
	//copy data from previous iteration
	ivec2 pos = ivec2(gl_FragCoord.xy);
	cell_data = texelFetch(u_Texture, pos, 0);
	
	//get data from this cell in previous frame
	vec4 prev_cell = texelFetch(u_Prev, pos, 0);

	ivec2 size = textureSize(u_Texture, 0);

	float a = u_Diff * u_Timestep; //constant affecting rate of diffusion

	//get data from surrounding cells
	vec4 L = texelFetch(u_Texture, ivec2(pos.x - 1, pos.y), 0);
	vec4 R = texelFetch(u_Texture, ivec2(pos.x + 1, pos.y), 0);
	vec4 U = texelFetch(u_Texture, ivec2(pos.x, pos.y + 1), 0);
	vec4 D = texelFetch(u_Texture, ivec2(pos.x, pos.y - 1), 0);

	//set values in this cell based on surrounding cells and diffusion constant
	cell_data = (prev_cell + a * (L + R + U + D)) / (1 + 4 * a);

	//zero border cells
	if (b) {
		if (pos.x <= 0 || pos.x >= size.x - 5) {
			cell_data.x = 0;
		}
		if (pos.y <= 0 || pos.y >= size.y - 5) {
			cell_data.y = 0;
		}
	}
}

