#ifndef INFLUENCER_H
#define INFLUENCER_H

#pragma once
#include <string>

#include "Texture.h"

#define _USE_MATH_DEFINES
#include "math.h"

using namespace glm;

enum type { DYE, FORCE, BOUNDARY, FORCEDYE };
enum shape{ OVOID, QUAD };


#ifndef HELP_MARKER
#define HELP_MARKER

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

#endif

class Influencer
{
private:
	std::string name = std::string(50, '\0');

	type ty;
	shape sh;
	vec2 pos;
	vec2 size;
	float rotation = 0;

	float time_start = 0;
	float lifetime = 100;

protected:
	Shader* shader;

	VertexArray* p_VA;
	VertexBuffer* p_VB;
	IndexBuffer* p_IB;

	Renderer renderer;

public:
	bool constant = false;

	Influencer(type t, shape s, vec2 p, vec2 si, Shader* shade, std::string n);
	~Influencer();
	
	virtual void Draw(float width, float height) = 0;
	virtual void Inject(float width, float height, float d, int textureType) = 0;
	virtual void ShowVars(float uiWidth) = 0;

	inline type GetType() { return ty; }
	inline shape GetShape() { return sh; }
	inline vec2 GetPos() { return pos; }
	inline vec2 GetSize() { return size; }
	inline float GetRotation() { return rotation * (180 / M_PI); }
	inline float GetRotationRad() { return rotation; }
	inline std::string GetName() { return name; }
	inline float GetStartTime() { return time_start; }
	inline float GetLifetime() { return lifetime; }
	inline bool GetConstant() { return constant; }

	inline void SetPos(vec2 p) { pos = p; }
	inline void SetSize(vec2 s) { size = s; }
	inline void SetRotation(float r) { rotation = r * (M_PI / 180); }
	inline void SetRotationRad(float r) { rotation = r; }
	inline void SetName(std::string n) { name = n; }
	inline void SetStartTime(float start) { time_start = start; }
	inline void SetLifetime(float lt) { lifetime = lt; }
	inline void SetConstant(bool c) { constant = c; }
};

#endif