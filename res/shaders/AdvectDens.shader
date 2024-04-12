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
uniform sampler2D u_Prev_d; //density texture from previous frame (same as above for vel step)
uniform float u_Size_m; //difference in size between read and write texture
uniform float u_Timestep; //rate of time
uniform float u_Dissipation; //rate of time

void main()
{
	//copy data from velocity texture
	vec2 pos = vec2(gl_FragCoord.x/u_Size_m, gl_FragCoord.y/u_Size_m);
	vec4 prev_cell = texelFetch(u_Prev, ivec2(int(pos.x), int(pos.y)), 0);

	//use velocity in cell to trace to location values are coming from
	vec2 old_pos = gl_FragCoord.xy - (prev_cell.xy * u_Size_m * u_Timestep) - vec2(0.5, 0.5);

	//get cells surrounding old position
	vec4 LR = texelFetch(u_Prev_d, ivec2(int(old_pos.x) + 1, int(old_pos.y)), 0);
	vec4 LL = texelFetch(u_Prev_d, ivec2(int(old_pos.x), int(old_pos.y)), 0);
	vec4 UR = texelFetch(u_Prev_d, ivec2(int(old_pos.x) + 1, int(old_pos.y) + 1), 0);
	vec4 UL = texelFetch(u_Prev_d, ivec2(int(old_pos.x), int(old_pos.y) + 1), 0);

	//interpolate two values between lower and upper pairs of cells
	vec4 z1 = LL + fract(old_pos.x) * (LR - LL);
	vec4 z2 = UL + fract(old_pos.x) * (UR - UL);

	//interpolate value from two values in previous step, save to cell
	cell_data = vec4(z1 + (fract(old_pos.y) * (z2 - z1)));
	cell_data.w *= u_Dissipation;
}

