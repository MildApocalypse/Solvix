#shader vertex //Sets gl_Position coords, Every shader has both fragment and vertex code in same file
#version 330 core

layout(location = 0) in vec2 aPosition;

void main()
{
	gl_Position = vec4(aPosition, 0, 1.0);
};

#shader fragment //Move values in field according to velocity in cells with a linear traceback algorithm
#version 330 core

layout(location = 0) out vec4 cell_data;

uniform sampler2D u_Prev; //texture from previous frame
uniform float u_Timestep; //rate of time
uniform float u_Dampening; //rate of velocity dampening

void main()
{
	//copy data from velocity texture
	ivec2 pos = ivec2(gl_FragCoord.xy);
	vec4 prev_cell = texelFetch(u_Prev, pos, 0);

	//use velocity in cell to trace to location values are coming from
	vec2 old_pos = pos - (prev_cell.xy * u_Timestep);

	//get cells surrounding old position
	vec4 LR = texelFetch(u_Prev, ivec2(int(old_pos.x) + 1, int(old_pos.y)), 0);
	vec4 LL = texelFetch(u_Prev, ivec2(int(old_pos.x), int(old_pos.y)), 0);
	vec4 UR = texelFetch(u_Prev, ivec2(int(old_pos.x) + 1, int(old_pos.y) + 1), 0);
	vec4 UL = texelFetch(u_Prev, ivec2(int(old_pos.x), int(old_pos.y) + 1), 0);

	//interpolate two values between lower and upper pairs of cells
	vec4 z1 = LL + fract(old_pos.x) * (LR - LL);
	vec4 z2 = UL + fract(old_pos.x) * (UR - UL);

	//interpolate value from two values in previous step, save to cell
	cell_data = vec4(z1 + (fract(old_pos.y) * (z2 - z1)));
	cell_data *= u_Dampening;
}

