#shader vertex //Sets gl_Position coords, Every shader has both fragment and vertex code in same file because lazy
#version 330 core

layout(location = 0) in vec2 aPosition;

out vec2 UV;

void main()
{
	gl_Position = vec4(aPosition, 0, 1.0);
	UV = (aPosition + vec2(1, 1)) / 2.0;
};

#shader fragment //Selects cells around input coordinates and sets them to input values
#version 330 core

layout(location = 0) out vec4 cell_data;

in vec2 UV;

uniform sampler2D u_Prev; //Texture from previous frame
uniform sampler2D u_Texture; //Texture containing data to be injected

uniform vec2 u_mPos; //Coodinates to inject values around
uniform vec4 u_Values; //values to inject

uniform int u_Size; //radius around coordinates to inject values into

void add_externals();

void main()
{
	//copy data from previous frame
	cell_data = texture(u_Prev, UV);
	
	//data to be injected
	vec4 inj_data = texture(u_Texture, UV);

	if (inj_data.w > 0) {
		cell_data = inj_data;
	}

	//Shader is called every frame, so treat density = 0 as no user input
	if (u_Values.w > 0)
	{
		add_externals();
	}
}

//if fragment is within area around source, update cell with input force and density
void add_externals()
{
	vec2 size = textureSize(u_Prev, 0);

	if (gl_FragCoord.x < u_mPos.x + u_Size && gl_FragCoord.x > u_mPos.x - u_Size
		&& gl_FragCoord.y < u_mPos.y + u_Size && gl_FragCoord.y > u_mPos.y - u_Size)
	{
		cell_data = vec4(u_Values.x, u_Values.y, u_Values.z, u_Values.w);
	}
}

