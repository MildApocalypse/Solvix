#include "Boundary.h"

Boundary::Boundary(type t, shape s, vec2 p, vec2 si, Shader* shade, std::string n)
	:Influencer(t, s, p, si, shade, n)
{
}

Boundary::~Boundary()
{
}

void Boundary::Draw(float width, float height)
{
	shader->Bind();
	shader->SetUniform2f("u_TexSize", width, height);
	shader->SetUniform2f("u_Pos", GetPos().x, GetPos().y);
	shader->SetUniform2f("u_Scale", GetSize().x, GetSize().y);
	shader->SetUniform1f("u_Rot", GetRotationRad());
	shader->SetUniform1i("u_Outer", 1);
	shader->SetUniform1i("u_Draw", 1);

	p_VA->Bind();
	p_IB->Bind();
	renderer.Draw(p_IB->GetCount());

	shader->SetUniform2f("u_Scale", GetSize().x * 0.9f, GetSize().y * 0.9f);
	shader->SetUniform1i("u_Outer", 0);

	renderer.Draw(p_IB->GetCount());

	shader->UnBind();
	p_VA->Unbind();
	p_IB->Unbind();
}

void Boundary::Inject(float width, float height, float d, int textureType)
{
    if (textureType == 3) {
        shader->Bind();
        shader->SetUniform2f("u_TexSize", width, height);
        shader->SetUniform2f("u_Pos", GetPos().x, GetPos().y);
        shader->SetUniform2f("u_Scale", GetSize().x / d, GetSize().y / d);
        shader->SetUniform1f("u_Rot", GetRotationRad());
        shader->SetUniform1i("u_Draw", 0);
        p_VA->Bind();
        p_IB->Bind();
        renderer.Draw(p_IB->GetCount());

        shader->UnBind();
        p_VA->Unbind();
        p_IB->Unbind();
    }
}

void Boundary::ShowVars(float uiWidth)
{
}
