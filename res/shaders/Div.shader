#shader vertex //Sets gl_Position coords, Every shader has both fragment and vertex code in same file
#version 330 core

layout(location = 0) in vec2 aPosition;

void main()
{
	gl_Position = vec4(aPosition, 0, 1.0);
};

#shader fragment //Calculate difference in velocity flowing into and out of cell
#version 330 core

layout(location = 0) out vec4 cell_data;

uniform sampler2D u_Texture; //Texture from previous frame


void main()
{
	vec2 pos = gl_FragCoord.xy;

	//Get data of left, right up and down texels
	vec4 R = texelFetch(u_Texture, ivec2(pos.x + 1, pos.y), 0);
	vec4 L = texelFetch(u_Texture, ivec2(pos.x - 1, pos.y), 0);
	vec4 U = texelFetch(u_Texture, ivec2(pos.x, pos.y + 1), 0);
	vec4 D = texelFetch(u_Texture, ivec2(pos.x, pos.y - 1), 0);
	 
	//calculate divergence and to texture to be used in projection step
	float div = 0.5 * (R.x - L.x + U.y - D.y);
	cell_data = vec4(div, 0, 0, 0);
}

