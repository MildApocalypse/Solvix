#ifndef FORCEDYE_H
#define FORCEDYE_H

#pragma once
#include "Influencer.h"

using namespace ImGui;

class ForceDye : public Influencer
{
private:
	float angle1;
	float angle2;
	float magnitude;
	float density;

	bool overwrite = false;

	Texture* texture;
public:
	ForceDye(type t, shape s, vec2 p, vec2 si, Shader* shade, std::string n, float a1, float a2, float m, float d, Texture *tex);
	~ForceDye();

	inline float GetAngle1() { return angle1; }
	inline float GetAngle2() { return angle2; }
	inline float GetMagnitude() { return magnitude; }
	inline float GetDensity() { return density; }

	inline void SetAngle1(float a) { angle1 = a; }
	inline void SetAngle2(float a) { angle2 = a; }
	inline void SetMagnitude(float m) { magnitude = m; }
	inline void SetDensity(float d) { density = d; }

	void Draw(float width, float height);
	void Inject(float width, float height, float d, int textureType);
	void ShowVars(float uiWidth);
};

#endif