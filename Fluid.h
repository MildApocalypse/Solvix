#ifndef FLUID_H
#define FLUID_H

#pragma once
#include "Framebuffer.h"
#include "Texture.h"

using namespace std;

//holds all objects, functions and variables needed to simulate a fluid field.
class Fluid
{
private:
	//Fluid field variables
	int inject_size = 7; //radius around inject point to be filled with density
	float viscosity = 1.0f; //viscosity of fluid
	float time_step = 1.0f; //rate of evolution of field
	float vorticity = 0.5f; //amount of vortex exaggeration
	float dampening = 0.0; //rate of velocity dampening
	float dissipation = 0.0; //rate of dye dissipation
	float divisor; //number of times smaller velocity field is to density field

	//size of field
	unsigned int width;
	unsigned int height;

	//Shaders
	Shader forceShader = Shader("res/shaders/AddForce.shader"); //Injects force into fluid
	Shader diffuseShader = Shader("res/shaders/Diffuse.shader"); //Diffuses values in fluid
	Shader advectVelShader = Shader("res/shaders/AdvectVel.shader"); //Moves velocity values in fluid
	Shader advectDenShader = Shader("res/shaders/AdvectDens.shader"); //Moves density values in fluid
	Shader divShader = Shader("res/shaders/Div.shader"); //Calculates and stores divergence in fluid cells
	Shader projectShader = Shader("res/shaders/Project.shader"); //Calculates gradient field from divergence values
	Shader subShader = Shader("res/shaders/Sub.shader"); //Subtracts gradient field from main field
	Shader quadShader = Shader("res/shaders/Quad.shader"); //Renders final texture to screen
	Shader copyShader = Shader("res/shaders/Copy.shader"); //Copies chosen texture into C texture
	Shader boundShader = Shader("res/shaders/SetBounds.shader"); //Stops velocity passing through boundary fields
	Shader curlShader = Shader("res/shaders/Curl.shader"); //Calculates and stores curl values in fluid cells
	Shader vortShader = Shader("res/shaders/Vorticity.shader"); //Evolves fluid to exaggerate curls in velocity field

	//Responsible for draw calls
	Renderer* renderer;

	//Veclocity textures (pointers for easy texture swapping)
	Texture* p_TexA; //Holds the final velocity texture that is written to after calculation
	Texture* p_TexB; //First buffer for holding values to read from
	Texture* p_TexC; //Second buffer for holding values to read from
	Texture* p_TexD; //Used to hold divergence and pressure values

	//Density Textures
	Texture* p_DtexA; //Holds the final density texture that is written to after calculation
	Texture* p_DtexB; //First buffer for holding values to read from
	Texture* p_DtexC; //Second buffer for holding values to read from

	//Texture buffers
	FrameBuffer fb = FrameBuffer(); //holds texture used for final calculation, routinely swaps textures
	FrameBuffer doubleBuffer_v = FrameBuffer(); //Holds third texture, used when needing to hold extra information (velocity)
	FrameBuffer doubleBuffer_d = FrameBuffer(); //Holds third texture, used when needing to hold extra information (density)
	FrameBuffer divBuffer = FrameBuffer(); //Holds forth texture, used to hold divergence information

	//Vertex Array and associated buffers
	VertexArray* p_VA;
	VertexBuffer* p_VB;
	IndexBuffer* p_IB;

	//used to read from GPU for debugging purposes
	unsigned int buf_size = 0;
	float *vel_data;
	//gets pixel out of pixels array given i and j coordinate
	glm::vec4 GetPixel(unsigned int i, unsigned int j);

	//Copies chosen texture into C texture
	void BufferData(FrameBuffer* buf, Texture*& p_T);

public:
	Fluid(Renderer* r, unsigned int w, unsigned int h, unsigned int d);
	~Fluid();

	//Swaps textures in A and B pointers and attaches to fb framebuffer
	void SwapTextures(FrameBuffer* buf, Texture*& p_T1, Texture*& p_T2);

	void VorticityConfinement(); //Adds vorticity to velocity field
	void Project(); //Clears divergence in fluid
	void Diffuse(FrameBuffer* buf, Texture*& p_T1, Texture*& p_T2, Texture*& p_T3, bool b); //Diffuses values in fluid
	void VelAdvect(); //Moves velocity values through fluid
	void DensAdvect(); //Moves density values through fluid
	void AddForces(glm::vec2 mPos, glm::vec4 vals, Texture*& p_T1, Texture*& p_T2, int m, Texture*& injection); //injects values into fluid

	inline void SetViscosity(float v) { viscosity = v; }
	inline void SetTimestep(float t) { time_step = t; }
	inline void SetVorticity(float v) { vorticity = v; }
	inline void SetDampening(float d) { dampening = d; }
	inline void SetDissipation(float d) { dissipation = d; }


	inline unsigned int GetVelWidth() { return width; }
	inline unsigned int GetVelHeight() { return height; }
	inline unsigned int GetDenWidth() { return width * divisor; }
	inline unsigned int GetDenHeight() { return height * divisor; }
	inline float GetViscosity() { return viscosity; }
	inline float GetTimestep() { return time_step; }
	inline float GetVorticity() { return vorticity; }
	inline float GetDampening() { return dampening; }
	inline float GetDissipation() { return dissipation; }
	inline float GetDivisor() { return divisor; }

	//Draw fluid to screen
	void DrawFluid(int zoom);

	//Read vectors in fluid to CPU and draw to screen (big lag)
	void DrawVectorField();

	void VelStep(double xpos, double ypos, glm::vec2 vel, float density, int zoom, Texture* injection);
	void DensStep(double xpos, double ypos, float density, int zoom, Texture* injection);

	//Uses boundary texture to block velocity values in velocity texture
	void BoundStep(Texture* bound);

	//resets fluid
	void ClearBuffer();

	void ClearTextures();
};

#endif