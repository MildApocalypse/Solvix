#shader vertex //Sets gl_Position coords, Every shader has both fragment and vertex code in same file because lazy
#version 330 core

layout(location = 0) in vec2 aPosition;

out vec2 UV;

void main()
{
	gl_Position = vec4(aPosition, 0, 1.0);
};

#shader fragment //Selects cells around input coordinates and sets them to input values
#version 330 core

layout(location = 0) out vec4 cell_data;

uniform sampler2D u_Texture; //Texture to be modified
uniform sampler2D u_Bounds; //Texture containing boundaries


void main()
{
	vec2 pos = gl_FragCoord.xy;

	vec4 b_cell = texelFetch(u_Bounds, ivec2(pos.x, pos.y), 0);

	vec4 b_L = texelFetch(u_Bounds, ivec2(pos.x - 1, pos.y), 0);
	vec4 b_R = texelFetch(u_Bounds, ivec2(pos.x + 1, pos.y), 0);
	vec4 b_U = texelFetch(u_Bounds, ivec2(pos.x, pos.y + 1), 0);
	vec4 b_D = texelFetch(u_Bounds, ivec2(pos.x, pos.y - 1), 0);
	
	cell_data = texelFetch(u_Texture, ivec2(pos.x, pos.y), 0);

	if (b_L.x == 1 || b_R.x == 1) { 
		cell_data.x = 0;
	}

	if (b_U.x == 1 || b_D.x == 1) {
		cell_data.y = 0;
	}

	if (b_cell.x == 1) {
		cell_data.xy = vec2(0, 0);
	}
}

