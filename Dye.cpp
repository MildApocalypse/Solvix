#include "Dye.h"

Dye::Dye(type t, shape s, vec2 p, vec2 si, Shader* shade, std::string n, float dens)
	:Influencer(t, s, p, si, shade, n), density(dens) {

}

Dye::~Dye(){

}

void Dye::Draw(float width, float height)
{
    shader->Bind();
    shader->SetUniform2f("u_TexSize", width, height);
    shader->SetUniform2f("u_Pos", GetPos().x, GetPos().y);
    shader->SetUniform2f("u_Scale", GetSize().x, GetSize().y);
    shader->SetUniform1f("u_Rot", GetRotationRad());

    p_VA->Bind();
    p_IB->Bind();
    renderer.Draw(p_IB->GetCount());

    shader->UnBind();
    p_VA->Unbind();
    p_IB->Unbind();
}

void Dye::Inject(float width, float height, float d, int textureType)
{
    if (textureType == 1) {
        shader->Bind();
        shader->SetUniform2f("u_TexSize", width, height);
        shader->SetUniform2f("u_Pos", GetPos().x, GetPos().y);
        shader->SetUniform2f("u_Scale", GetSize().x / d, GetSize().y / d);
        shader->SetUniform1f("u_Rot", GetRotationRad());

        p_VA->Bind();
        p_IB->Bind();
        renderer.Draw(p_IB->GetCount());

        shader->UnBind();
        p_VA->Unbind();
        p_IB->Unbind();
    }
}

void Dye::ShowVars(float uiWidth) {

}
