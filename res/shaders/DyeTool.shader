#shader vertex //Sets gl_Position coords, Every shader has both fragment and vertex code in same file
#version 330 core

layout(location = 0) in vec2 aPosition;
uniform vec2 u_Pos;
uniform vec2 u_TexSize;
uniform vec2 u_Scale;
uniform float u_Rot;

void main()
{
	gl_Position = vec4(aPosition, 0, 1.0);
	
	float aspect = u_TexSize.x / u_TexSize.y;

	gl_Position.xy *= vec2(u_Scale / u_TexSize);

	vec2 pos1 = gl_Position.xy;
	gl_Position.x = (cos(u_Rot) * pos1.x) - (sin(u_Rot) * (pos1.y / aspect));
	gl_Position.y = (sin(u_Rot) * (pos1.x * aspect)) + (cos(u_Rot) * pos1.y);
	gl_Position.xy += u_Pos;
	
};

#shader fragment 
#version 330 core
 
layout(location = 0) out vec4 color;

void main()
{

	color = vec4(1.0, 1.0, 1.0, 1.0);

}

