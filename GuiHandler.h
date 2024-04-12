#ifndef GUIHANDLER_H
#define GUIHANDLER_H

#pragma once
#include <string>
#include <sstream>
#include <shobjidl.h> 
#include <windows.h>
#include <thread>

#include "gif-h/gif.h"

#include "Fluid.h"
#include "Dye.h"
#include "Force.h"
#include "Boundary.h"
#include "ForceDye.h"

using namespace ImGui;

enum state { SETUP, RESET, PLAY, PAUSE };

class GuiHandler
{
private:
	state st = SETUP;
	unsigned int r_width, r_height; //width and height of render window
	unsigned int w_width, w_height; //width and height of app window
	unsigned int t_width, t_height; //width and height of toolbox
	unsigned int v_width, v_height; //width and height of variable settings
	unsigned int a_width, a_height; //width and height of action panel
	unsigned int o_width, o_height; //width and height of object list
	unsigned int oc_width, oc_height; //width and height of object list
	
	unsigned int tool_tabs = 0; //keep which tooltab is selected to help with getting tool icons
	int zoom = 1;
	vec2 origin;

	//number of times each tool has been placed
	unsigned int dyeCount = 0;
	unsigned int forceCount = 0;
	unsigned int bCount = 0;
	unsigned int forceDyeCount = 0;
	
	unsigned int sim_time = 0; //number of frames since start of simulation
	int test = 0;

	//Gif export options
	int frame_spacing = 6; //number of frames between each capture
	int frame_speed = 1; //time between frames in 100ths of a second
	string savePath = "./MySim.gif";
	bool recording = false;
	int frames_processed = 0;

	vector<vector<uint8_t>> gifData; //holds all of the gif frames to be processed
	float* frame;
	int framesProcessed = 0;

	Shader shaders[4] = { 
	Shader("res/shaders/DyeTool.shader"),
	Shader("res/shaders/ForceTool.shader"),
	Shader("res/shaders/BoundaryTool.shader"),
	Shader("res/shaders/ForceDyeTool.shader")  };

	//List of every fluid field influencer
	std::vector<std::unique_ptr<Influencer>> influencers;

	Fluid* fluid; //pointer to fluid to modify values

	//framebuffers and texture to hold data for renderwindow
	Texture texture			= Texture(fluid->GetDenWidth(), fluid->GetDenHeight());
	FrameBuffer fb			= FrameBuffer(&texture, "Framebuffer");

	//buffers to hold injection data
	Texture dye_inj			= Texture(fluid->GetDenWidth(), fluid->GetDenHeight());
	Texture vel_inj			= Texture(fluid->GetVelWidth(), fluid->GetVelHeight());
	Texture vel_inj_buf		= Texture(fluid->GetVelWidth(), fluid->GetVelHeight());
	Texture bound_inj		= Texture(fluid->GetVelWidth(), fluid->GetVelHeight());
	FrameBuffer vel			= FrameBuffer(&vel_inj, "Vel");
	FrameBuffer vel_buf		= FrameBuffer(&vel_inj_buf, "Vel buffer");
	FrameBuffer dye			= FrameBuffer(&dye_inj, "Dye");
	FrameBuffer bound		= FrameBuffer(&bound_inj, "Boundary");
	
	//preprocess items to add black outlines
	Texture buf				= Texture(fluid->GetDenWidth(), fluid->GetDenHeight());
	FrameBuffer prepro		= FrameBuffer(&buf, "Pre processing");
	
	Shader outliner = Shader("res/shaders/Outline.shader");
	//use same texture for data conversion in gif making process
	Shader convertShader = Shader("res/shaders/Convert.shader");

	//Vertex Array and associated buffers
	VertexArray* p_VA;
	VertexBuffer* p_VB;
	IndexBuffer* p_IB;

	Texture thumbs = Texture("res/textures/thumbs.png");

	Renderer* renderer;

	void CreateInfluencer(type t, shape s);

	int selected = -1;
	int held = -1;

	bool export_popup = false;

	//UI render in setup mode
	void SetupRender(vec2 mousePos);
	//UI render in simulation mode
	void FluidRender(int zoom, bool drawVectors);

	//read data in texture
	std::vector<uint8_t> getFrame();

	//ImGui render call
	void GuiRender();
	
	//UI windows
	void RenderWindow();
	void ToolBox();
	void ToolVars();
	void ActionPanel();
	void ObjectList();
	void Diag();
	
	//Transferring UI elements to inputs that can be used by fluid
	void MapForces();

	//Handles state switching
	void StateController(bool drawVectors, vec2 mousePos);

	//handles file choosing
	string FileCoItem();

	//process the frame data that was recorded and save to GIF format
	bool processFrames(string path);
	bool startThread = true;
	bool cancel_process = false;

	Shader copyShader = Shader("res/shaders/Copy.shader");
	void textureCopy();

public:
	GuiHandler(Fluid* f, Renderer* r, unsigned int w, unsigned int h);
	~GuiHandler();

	void GuiStart();
	void GuiEnd();
	void GuiMain(bool drawVectors, vec2 mousePos);

	void GifTest();

	inline state GetState() { return st; }
	inline vec2 GetOrigin() { return origin; }
	inline int GetZoom() { return zoom; }
	inline Texture* GetVelInjection() { return &vel_inj; }
	inline Texture* GetDyeInjection() { return &dye_inj; }
	inline Texture* GetBoundInjection() { return &bound_inj; }

	void ClearTextures();
};

#endif