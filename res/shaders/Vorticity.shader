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
uniform sampler2D u_Curl;
uniform float u_Vorticity;
uniform float u_Timestep;

void main()
{
	//copy data from previous iteration
	vec2 pos = gl_FragCoord.xy;
	vec4 C = texelFetch(u_Curl, ivec2(pos.x, pos.y), 0);
	vec2 V = texelFetch(u_Texture, ivec2(pos.x, pos.y), 0).xy;

	//Get data from surrounding texels
	vec4 L = texelFetch(u_Curl, ivec2(pos.x - 1, pos.y), 0);
	vec4 R = texelFetch(u_Curl, ivec2(pos.x + 1, pos.y), 0);
	vec4 U = texelFetch(u_Curl, ivec2(pos.x, pos.y + 1), 0);
	vec4 D = texelFetch(u_Curl, ivec2(pos.x, pos.y - 1), 0);

	//vec2 force = vec2(abs(D.x) - abs(U.x), abs(R.x) - abs(L.x));
	vec2 force = 0.5 * vec2(abs(U.x) - abs(D.x), abs(R.x) - abs(L.x));

	//float len = length(force) + 0.0001;
	//force *= u_Vorticity/len;
	force = force / (length(force) + 0.0001);
	force *= u_Vorticity * C.x;
	force.y *= -1.0;

	V += force * u_Timestep;
	V = min(max(V, -1000.0), 1000.0);
	cell_data = vec4(V, 0, 1.0);
	//V.xy += force * u_Timestep * C.x;
	//V = min(max(V, -1000.0), 1000.0);
	//cell_data = vec4(V.xy, 0, 1.0);
}

