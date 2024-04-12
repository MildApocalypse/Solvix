#include "Force.h"

Force::Force(type t, shape s, vec2 p, vec2 si, Shader* shade, std::string n, float a1, float a2, float m, Texture* tex)
	:Influencer(t, s, p, si, shade, n), angle1(a1), angle2(a2), magnitude(m), texture(tex)
{
	
}

Force::~Force()
{

}

void Force::Draw(float w, float h)
{
	//convert raw coordnates to relative coords between -1 and 1
	float xPos = GetPos().x;
	float yPos = GetPos().y;
	float xSize = GetSize().x / w;
	float ySize = GetSize().y / h;
	float aspect = w / h;

	if (GetShape() == OVOID) {
		float x, y, angle;

		//draw circle
		GLCall(glColor4f(1.0, 1.0, 1.0, 1.0));
		glBegin(GL_LINE_LOOP);
		for (angle = 0.0f; angle <= (2.0f * M_PI); angle += 0.01f)
		{
			x = xSize * cos(angle);
			y = ySize * sin(angle);
			float x1 = cos(GetRotationRad()) * x - sin(GetRotationRad()) * (y / aspect);
			float y1 = sin(GetRotationRad()) * (x * aspect) + cos(GetRotationRad()) * y;
			x = x1 + xPos;
			y = y1 + yPos;
			GLCall(glVertex2f(x, y));
		}
		glEnd();
	}
	else if (GetShape() == QUAD) {

		//draw square
		GLCall(glColor4f(1.0, 1.0, 1.0, 1.0));
		vec2 TL = vec2(cos(GetRotationRad()) * xSize - sin(GetRotationRad()) * (-ySize / aspect), sin(GetRotationRad()) * (xSize * aspect) + cos(GetRotationRad()) * -ySize);
		vec2 TR = vec2(cos(GetRotationRad()) * -xSize - sin(GetRotationRad()) * (-ySize / aspect), sin(GetRotationRad()) * (-xSize * aspect) + cos(GetRotationRad()) * -ySize);
		vec2 BL = vec2(cos(GetRotationRad()) * xSize - sin(GetRotationRad()) * (ySize / aspect), sin(GetRotationRad()) * (xSize * aspect) + cos(GetRotationRad()) * ySize);
		vec2 BR = vec2(cos(GetRotationRad()) * -xSize - sin(GetRotationRad()) * (ySize / aspect), sin(GetRotationRad()) * (-xSize * aspect) + cos(GetRotationRad()) * ySize);

		glBegin(GL_LINE_LOOP);
		GLCall(glVertex2f(TL.x + xPos, TL.y + yPos));
		GLCall(glVertex2f(TR.x + xPos, TR.y + yPos));
		GLCall(glVertex2f(BR.x + xPos, BR.y + yPos));
		GLCall(glVertex2f(BL.x + xPos, BL.y + yPos));
		glEnd();
	}
	//convert angles to radians
	float a1 = angle1 * 3.1416f / 180;
	float a2 = angle2 * 3.1416f / 180;

	//number used for line representing magnitude
	float mag = (magnitude * 20);
	
	//draw angle indicator
	glBegin(GL_LINES);
	glVertex2f(xPos, yPos);
	glVertex2f(xPos + (xSize + (mag / w)) * cos(a1), yPos - (ySize + (mag / h)) * sin(a1));
	/*glVertex2f(xPos + xSize * cos(a2), yPos - ySize * sin(a2));
	glVertex2f(xPos + (xSize + (mag / w)) * cos(a2), yPos - (ySize + (mag / h)) * sin(a2));*/
	glEnd();

	//draw arc
	//shader->Bind();
	//shader->SetUniform2f("u_TexSize", w, h);
	//shader->SetUniform2f("u_Pos", GetPos().x, GetPos().y);
	//shader->SetUniform2f("u_Scale", GetSize().x, GetSize().y);
	//shader->SetUniform1f("u_Angle1", a1);
	////shader->SetUniform1f("u_Angle2", a2);
	//shader->SetUniform1f("u_Magnitude", magnitude);
	//shader->SetUniform1i("u_Draw", 1);
	//p_VA->Bind();
	//p_IB->Bind();
	//renderer.Draw(p_IB->GetCount());

	//shader->UnBind();
	//p_VA->Unbind();
	//p_IB->Unbind();
}

void Force::Inject(float width, float height, float d, int textureType)
{
	if (textureType == 2) {
		shader->Bind();
		shader->SetUniform2f("u_TexSize", width, height);
		shader->SetUniform2f("u_Pos", GetPos().x, GetPos().y);
		shader->SetUniform2f("u_Scale", GetSize().x / d, GetSize().y / d);
		shader->SetUniform1f("u_Rot", GetRotationRad());

		shader->SetUniform1f("u_Angle1", angle1 * 3.1416f / 180);
		//shader->SetUniform1f("u_Angle2", angle2 * 3.1416f / 180);
		shader->SetUniform1f("u_Magnitude", magnitude);
		shader->SetUniform1i("u_Overwrite", overwrite);
		texture->Bind();
		shader->SetUniform1i("u_Texture", 0);
		p_VA->Bind();
		p_IB->Bind();
		renderer.Draw(p_IB->GetCount());

		//texture->Unbind();
		shader->UnBind();
		p_VA->Unbind();
		p_IB->Unbind();
	}
}

void Force::ShowVars(float uiWidth)
{
	Text("Angle");
	float a1 = GetAngle1();
	//float a2 = GetAngle2();
	//PushItemWidth(uiWidth / 2 - 50);
	DragFloat("###angle", &a1, 0.5f, 0, 360, "%.1f");
	//DragFloat("a2", &a2, 0.5f, 0, a1, "%.1f");
	SetAngle1(a1);
	//SetAngle2(a2);
	//PopItemWidth();

	Text("Magnitude");
	float m = GetMagnitude();
	DragFloat("###mag", &m, 0.5f, 0, 50, "%.1f"); 
	SetMagnitude(m);

	Checkbox("Overwrite", &overwrite); SameLine();
	HelpMarker("Force influencers that intersect can either blend their values together or be overwritten by the one on top.");
}
