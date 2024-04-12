#shader vertex
#version 330 core

layout (location = 0) in vec2 aPosition;

out vec2 UV;

void main() 
{
	gl_Position = vec4(aPosition, 0, 1.0);
};

#shader fragment
#version 330 core

out vec4 cell_data;

uniform sampler2D u_Texture;

void main()
{
	//copy data from previous iteration
	vec2 pos = gl_FragCoord.xy;
	cell_data = texelFetch(u_Texture, ivec2(pos.x, pos.y), 0);

	//Get data from surrounding texels
	vec4 L = texelFetch(u_Texture, ivec2(pos.x - 1, pos.y), 0);
	vec4 R = texelFetch(u_Texture, ivec2(pos.x + 1, pos.y), 0);
	vec4 U = texelFetch(u_Texture, ivec2(pos.x, pos.y + 1), 0);
	vec4 D = texelFetch(u_Texture, ivec2(pos.x, pos.y - 1), 0);

	if ((L.w > 0 || R.w > 0 || U.w > 0 || D.w > 0) && cell_data.w <= 0.0f)
	{
		cell_data = vec4(0, 0, 0, 1);
	}

};