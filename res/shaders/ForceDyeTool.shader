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

uniform float u_Angle1;
uniform float u_Magnitude;
uniform bool u_Draw;
uniform bool u_Inject;
uniform bool u_Overwrite;
uniform sampler2D u_Texture;


void main()
{
	if (u_Draw) 
	{
		color = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else
	{
		if (u_Inject) {
			color = vec4(0, 0, 0, 1);
		}
		else {
			float x = u_Magnitude * cos(u_Angle1);
			float y = -u_Magnitude * sin(u_Angle1);
			if (u_Overwrite) {
				color = vec4(x, y, 0, 1);
			}
			else {
				vec4 data = texelFetch(u_Texture, ivec2(gl_FragCoord.xy), 0);
				data.xy = (data.xy + vec2(x, y));
				color = vec4(data.xy, 0, 1);
			}
		}
	}
}

//void draw(float dist)
//{
//	if (u_Shape == 0) {
//		color = vec4(step(dist, 1.0));
//	}
//	else if (u_Shape == 1) {
//		if (gl_FragCoord.x < u_Pos.x + u_Scale.x / 2 && gl_FragCoord.x > u_Pos.x - u_Scale.x / 2
//			&& gl_FragCoord.y < u_Pos.y + u_Scale.y / 2 && gl_FragCoord.y > u_Pos.y - u_Scale.y / 2) 
//		{
//			color = vec4(1.0, 1.0, 1.0, 1.0);
//		}
//	}
//}
//
////inject into density field
//void dens_inject(float dist)
//{
//	//shape is ovoid
//	if (u_Shape == 0) {
//		if (dist < 1.0) {
//			color = vec4(0, 0, 0, 1);
//		}
//	}
//	//shape is quad
//	else if (u_Shape == 1) {
//		if (gl_FragCoord.x < u_Pos.x + u_Scale.x / 2 && gl_FragCoord.x > u_Pos.x - u_Scale.x / 2
//			&& gl_FragCoord.y < u_Pos.y + u_Scale.y / 2 && gl_FragCoord.y > u_Pos.y - u_Scale.y / 2) {
//
//			color = vec4(0, 0, 0, 1);
//		}
//	}
//};
//
////inject into velocity field
//void vel_inject(float dist) {
//	//shape is ovoid
//	if (u_Shape == 0) {
//		if (dist < 1.0) {
//
//			float x = u_Magnitude * cos(u_Angle1);
//			float y = -u_Magnitude * sin(u_Angle1);
//			if (u_Overwrite) {
//				color = vec4(x, y, 0, 1);
//			}
//			else {
//				vec4 data = texelFetch(u_Texture, ivec2(gl_FragCoord.xy), 0);
//				data.xy = (data.xy + vec2(x, y));
//				color = vec4(data.xy, 0, 1);
//			}
//		}
//	}
//	//shape is quad
//	else if (u_Shape == 1) {
//		if (gl_FragCoord.x < u_Pos.x + u_Scale.x / 2 && gl_FragCoord.x > u_Pos.x - u_Scale.x / 2
//			&& gl_FragCoord.y < u_Pos.y + u_Scale.y / 2 && gl_FragCoord.y > u_Pos.y - u_Scale.y / 2) {
//
//			float x = u_Magnitude * cos(u_Angle1);
//			float y = -u_Magnitude * sin(u_Angle1);
//			if (u_Overwrite) {
//				color = vec4(x, y, 0, 1);
//			}
//			else {
//				vec4 data = texelFetch(u_Texture, ivec2(gl_FragCoord.xy), 0);
//				data.xy = (data.xy + vec2(x, y));
//				color = vec4(data.xy, 0, 1);
//			}
//		}
//	}
//};


