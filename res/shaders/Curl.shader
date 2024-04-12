#shader vertex //Sets gl_Position coords, Every shader has both fragment and vertex code in same file
#version 330 core

layout(location = 0) in vec2 aPosition;

void main()
{
	gl_Position = vec4(aPosition, 0, 1.0);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 cell_data;

uniform sampler2D u_Texture;


void main()
{
	//copy data from previous iteration
	vec2 pos = gl_FragCoord.xy;

	//Get data from surrounding texels
	vec4 L = texelFetch(u_Texture, ivec2(pos.x - 1, pos.y), 0);
	vec4 R = texelFetch(u_Texture, ivec2(pos.x + 1, pos.y), 0);
	vec4 U = texelFetch(u_Texture, ivec2(pos.x, pos.y + 1), 0);
	vec4 D = texelFetch(u_Texture, ivec2(pos.x, pos.y - 1), 0);

	//cell_data = vec4(U.x - D.x + L.y - R.y, 0, 0, 1);
	cell_data = vec4(0.5*(R.y - L.y - U.x + D.x), 0, 0, 1);
}

