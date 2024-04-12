#ifndef FORCE_H
#define FORCE_H

#pragma once
#include "Influencer.h"

using namespace ImGui;

class Force : public Influencer
{
private:
	float angle1;
	float angle2; //unused, may add back later
	float magnitude;

	bool overwrite = false;

	Texture* texture;

public:
	Force(type t, shape s, vec2 p, vec2 si, Shader* shade, std::string n, float a1, float a2, float m, Texture* tex);
	~Force();

	inline float GetAngle1() { return angle1; }
	inline float GetAngle2() { return angle2; }
	inline float GetMagnitude() { return magnitude; }

	inline void SetAngle1(float a) { angle1 = a; }
	inline void SetAngle2(float a) { angle2 = a; }
	inline void SetMagnitude(float m) { magnitude = m; }

	void Draw(float width, float height);
	void Inject(float width, float height, float d, int textureType);
	void ShowVars(float uiWidth);
};

#endif