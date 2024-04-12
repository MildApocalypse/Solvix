#include "ForceDye.h"

ForceDye::ForceDye(type t, shape s, vec2 p, vec2 si, Shader* shade, std::string n, float a1, float a2, float m, float d, Texture* tex)
	:Influencer(t, s, p, si, shade, n), angle1(a1), angle2(a2), magnitude(m), density(d), texture(tex)
{

}

ForceDye::~ForceDye()
{

}

void ForceDye::Draw(float w, float h)
{
	//convert raw coordnates to relative coords between -1 and 1
	float xPos = GetPos().x;
	float yPos = GetPos().y;
	float xSize = GetSize().x / w;
	float ySize = GetSize().y / h;
	float aspect = w / h;

	//convert angles to radians
	float a1 = angle1 * 3.1416f / 180;
	//float a2 = angle2 * 3.1416f / 180;

	//number used for line representing magnitude
	float mag = magnitude * 20;

	//draw angle indicator
	glBegin(GL_LINES);
	glVertex2f(xPos, yPos);
	glVertex2f(xPos + (xSize * cos(a1)) + mag / w * cos(a1), yPos - (ySize * sin(a1)) - mag / h * sin(a1));
	//glVertex2f(xPos + xSize * cos(a2), yPos - ySize * sin(a2));
	//glVertex2f(xPos + (xSize * cos(a2)) + mag / w * cos(a2), yPos - (ySize * sin(a2)) - mag / h * sin(a2));
	glEnd();

	//draw injection area
	shader->Bind();
	shader->SetUniform2f("u_TexSize", w, h);
	shader->SetUniform2f("u_Pos", GetPos().x, GetPos().y);
	shader->SetUniform2f("u_Scale", GetSize().x, GetSize().y);
	shader->SetUniform1f("u_Rot", GetRotationRad());
	shader->SetUniform1i("u_Draw", 1);
	texture->Bind();
	shader->SetUniform1i("u_Texture", 0);
	p_VA->Bind();
	p_IB->Bind();
	renderer.Draw(p_IB->GetCount());

	texture->Unbind();
	shader->UnBind();
	p_VA->Unbind();
	p_IB->Unbind();

}

void ForceDye::Inject(float w, float h, float d, int textureType)
{
	if (textureType == 1 || textureType == 2) {
		shader->Bind();
		shader->SetUniform2f("u_TexSize", w, h);
		shader->SetUniform2f("u_Pos", GetPos().x, GetPos().y);
		shader->SetUniform2f("u_Scale", GetSize().x / d, GetSize().y / d);
		shader->SetUniform1f("u_Rot", GetRotationRad());
		shader->SetUniform1f("u_Angle1", GetAngle1() * 3.1416f / 180);
		shader->SetUniform1f("u_Magnitude", magnitude);
		shader->SetUniform1i("u_Draw", 0);
		shader->SetUniform1i("u_Overwrite", overwrite);
		texture->Bind();
		shader->SetUniform1i("u_Texture", 0);
		if (textureType == 1)
		{
			shader->SetUniform1i("u_Inject", 1);
		}
		else if (textureType == 2) {
			shader->SetUniform1i("u_Inject", 0);
		}
		p_VA->Bind();
		p_IB->Bind();
		renderer.Draw(p_IB->GetCount());

		texture->Unbind();
		shader->UnBind();
		p_VA->Unbind();
		p_IB->Unbind();
	}
}

void ForceDye::ShowVars(float uiWidth)
{
	Text("Angle");
	float a1 = GetAngle1();
	DragFloat("###angle", &a1, 0.5f, 0, 360, "%.1f");
	SetAngle1(a1);

	Text("Magnitude");
	float m = GetMagnitude();
	DragFloat("###mag", &m, 0.5f, 0, 50, "%.1f");
	SetMagnitude(m);

	Checkbox("Overwrite", &overwrite); SameLine();
	HelpMarker("Force influencers that intersect can either blend their values together or be overwritten by the one on top.");
}
