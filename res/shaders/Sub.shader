#shader vertex //Sets gl_Position coords, Every shader has both fragment and vertex code in same file
#version 330 core

layout(location = 0) in vec2 aPosition;

void main()
{
	gl_Position = vec4(aPosition, 0, 1.0);
};

#shader fragment //Subtract gradient field from field in previous frame
#version 330 core

layout(location = 0) out vec4 cell_data;

uniform sampler2D u_Prev; //Texture from previous frame
uniform sampler2D u_Div; //Gradient field


void main()
{
	//copy data from previous frame
	vec2 pos = gl_FragCoord.xy;
	cell_data = texelFetch(u_Prev, ivec2(pos.x, pos.y), 0);
	
	ivec2 size = textureSize(u_Prev, 0);

	//Get values surrounding cell
	vec4 L = texelFetch(u_Div, ivec2(pos.x - 1, pos.y), 0);
	vec4 R = texelFetch(u_Div, ivec2(pos.x + 1, pos.y), 0);
	vec4 U = texelFetch(u_Div, ivec2(pos.x, pos.y + 1), 0);
	vec4 D = texelFetch(u_Div, ivec2(pos.x, pos.y - 1), 0);

	//subtract surrounding values from cell velocity
	cell_data.x -= (R.y - L.y);
	cell_data.y -= (U.y - D.y);
}

