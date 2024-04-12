#include "Fluid.h"

Fluid::Fluid(Renderer* r, unsigned int w, unsigned int h, unsigned int d)
	:renderer(r), width(w/d), height(h/d), divisor(d)
{
    //Initialize textures
	p_TexA = new Texture(width, height);
	p_TexB = new Texture(width, height);
    p_TexC = new Texture(width, height);
    p_TexD = new Texture(width, height);

    p_DtexA = new Texture(w, h);
    p_DtexB = new Texture(w, h);
    p_DtexC = new Texture(w, h);


    //Initialize buffer for reading velocity texture
    buf_size = width * height * 4 * sizeof(float);
    vel_data = new float[buf_size];

    //create vertex data for drawing quad
    float positions[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    //create index data for drawing quad
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    //Setup VA and IB
    p_VA = new VertexArray;
    p_VB = new VertexBuffer(positions, 4 * 4 * sizeof(float));
    p_VA->AddBuffer(*p_VB, 2);
    p_IB = new IndexBuffer(indices, 6);
    
    //Setup framebuffer to hold final texture
    fb.Bind();
    GLCall(glViewport(0, 0, GetVelWidth(), GetVelHeight()));
    fb.AttachTexture(p_TexA->GetID());
    if (!fb.checkOK())
    {
        cout << "Framebuffer not complete" << endl;
    }

    //Setup second framebuffer to hold buffer texture (velocity)
    doubleBuffer_v.Bind();
    doubleBuffer_v.AttachTexture(p_TexC->GetID());
    if (!doubleBuffer_v.checkOK())
    {
        cout << "Double Velocity Framebuffer not complete" << endl;
    }

    //Setup third framebuffer to hold divergence buffer texture
    divBuffer.Bind();
    divBuffer.AttachTexture(p_TexD->GetID());
    if (!divBuffer.checkOK())
    {
        cout << "Divergence Framebuffer not complete" << endl;
    }

    //Setup second framebuffer to hold buffer texture (density)
    doubleBuffer_d.Bind();
    GLCall(glViewport(0, 0, GetDenWidth(), GetDenHeight()));
    doubleBuffer_d.AttachTexture(p_DtexC->GetID());
    if (!doubleBuffer_d.checkOK())
    {
        cout << "Double Density Framebuffer not complete" << endl;
    }

    doubleBuffer_d.UnBind();

}

Fluid::~Fluid()
{
    delete p_TexA;
    delete p_TexB;
    delete p_TexC;
    delete p_TexD;
    delete p_DtexA;
    delete p_DtexB;
    delete p_DtexC;
    delete [] vel_data;
    delete p_VA;
    delete p_IB;
    delete p_VB;
}

//Swap texures held in two given pointers and reattach first texture to framebuffer
void Fluid::SwapTextures(FrameBuffer* buf, Texture*& p_T1, Texture*& p_T2)
{
    buf->Bind();

    //Hold texture in T1 in temp pointer, then point T1 to T2 and T2 to temp
    Texture* p_T = p_T1;
    p_T1 = p_T2;
    p_T2 = p_T;

    //attach texture and check framebuffer is complete 
    buf->AttachTexture(p_T1->GetID());
    if (!buf->checkOK())
    {
        cout << "Framebuffer not complete" << endl;
    }

    buf->UnBind();
}

void Fluid::VorticityConfinement()
{
    SwapTextures(&fb, p_TexA, p_TexB);

    //disable blend so fragment values arent modified when drawn to texture
    GLCall(glDisable(GL_BLEND));

    //use texture C to hold values
    doubleBuffer_v.Bind();
    renderer->Clear();

    //calculate curl values from velocity field
    curlShader.Bind();
    p_TexB->Bind();
    curlShader.SetUniform1i("u_Texture", 0);
    p_VA->Bind();
    p_IB->Bind();
    renderer->Draw(p_IB->GetCount());

    fb.Bind();
    renderer->Clear();

    vortShader.Bind();
    p_TexB->Bind();
    vortShader.SetUniform1i("u_Texture", 0);
    p_TexC->Bind(1);
    vortShader.SetUniform1i("u_Curl", 1);
    vortShader.SetUniform1f("u_Vorticity", vorticity);
    vortShader.SetUniform1f("u_Timestep", time_step);
    p_VA->Bind();
    p_IB->Bind();
    renderer->Draw(p_IB->GetCount());

    //Unbind all
    fb.UnBind();
    vortShader.UnBind();
    p_TexB->Unbind();
    p_TexC->Unbind();
    p_VA->Unbind();
    p_IB->Unbind();

}
//Clear divergence from the fluid by calculating gradient field with a Gauss Seidel method,
//then subtracting from current velocity field
void Fluid::Project()
{
    SwapTextures(&fb, p_TexA, p_TexB);

    //disable blend so fragment values arent modified when drawn to texture
    GLCall(glDisable(GL_BLEND));
    
    //Drawing to texture D held in divBuffer
    divBuffer.Bind();
    renderer->Clear();

    //calculate divergence in texture B and save to texture D
    divShader.Bind();
    p_TexB->Bind();
    divShader.SetUniform1i("u_Texture", 0);
    p_VA->Bind();
    p_IB->Bind();
    renderer->Draw(p_IB->GetCount());

    //Calculate gradient field, can also be held in texture D because of unused values in fragment vectors
    for (int i = 0; i < 20; ++i)
    {
        //Save the previous iteration in Texture C
        BufferData(&doubleBuffer_v, p_TexD);

        //Drawing to texture D held in divBuffer
        divBuffer.Bind();
        renderer->Clear();

        //Poisson step to home in on gradient field using values in Texture C, save to Texture D
        projectShader.Bind();
        p_TexC->Bind();
        projectShader.SetUniform1i("u_Texture", 0);
        p_VA->Bind();
        p_IB->Bind();
        renderer->Draw(p_IB->GetCount());
    }

    //Drawing to to texture A held in fb
    fb.Bind();
    renderer->Clear();
    
    //Subtract final values in Texture D from Texture B
    subShader.Bind();
    p_TexB->Bind();
    subShader.SetUniform1i("u_Prev", 0);
    p_TexD->Bind(1);
    subShader.SetUniform1i("u_Div", 1);
    p_VA->Bind();
    p_IB->Bind();
    renderer->Draw(p_IB->GetCount());

    //Unbind all
    fb.UnBind();
    subShader.UnBind();
    p_TexB->Unbind();
    p_TexD->Unbind();
    p_VA->Unbind();
    p_IB->Unbind();
}

//Calculate diffusion using Gauss Seidel method to spread out velocity and density values
void Fluid::Diffuse(FrameBuffer* buf, Texture*& p_T1, Texture*& p_T2, Texture*& p_T3, bool b)
{
    SwapTextures(&fb, p_T1, p_T2);

    //disable blend so fragment values arent modified when drawn to texture
    GLCall(glDisable(GL_BLEND));

    for (int i = 0; i < 20; ++i)
    {
        //Save data in first texture to buffer texture
        BufferData(buf, p_T1);

        //Drawing to framebuffer
        fb.Bind();
        renderer->Clear();

        //Diffuse the current frame in second texture into the previous frame held in third texture
        diffuseShader.Bind();
        p_T3->Bind(0);
        diffuseShader.SetUniform1i("u_Texture", 0);
        p_T2->Bind(1);
        diffuseShader.SetUniform1i("u_Prev", 1);
        diffuseShader.SetUniform1f("u_Diff", viscosity);
        diffuseShader.SetUniform1f("u_Timestep", time_step);
        if (b) {
            diffuseShader.SetUniform1f("b", 1);
        }
        else {
            diffuseShader.SetUniform1f("b", 0);
        }
        p_VA->Bind();
        p_IB->Bind();
        renderer->Draw(p_IB->GetCount());
    }
    
    //Unbind all
    fb.UnBind();
    diffuseShader.UnBind();
    p_T3->Unbind();
    p_T2->Unbind();
    p_VA->Unbind();
    p_IB->Unbind();
}

//Move velocity values around the fluid according to velocity
void Fluid::VelAdvect()
{
    SwapTextures(&fb, p_TexA, p_TexB);

    //disable blend so fragment values arent modified when drawn to texture
    GLCall(glDisable(GL_BLEND));

    //Drawing to framebuffer
    fb.Bind();
    renderer->Clear();

    float damp = 1.0f - (dampening * time_step) / 1000.0f;
    damp = (damp < 0.0f)? 0 : damp;
    //Advect values in previous texture to framebuffer
    advectVelShader.Bind();
    p_TexB->Bind();
    advectVelShader.SetUniform1i("u_Prev", 0);
    advectVelShader.SetUniform1f("u_Timestep", time_step);
    advectVelShader.SetUniform1f("u_Dampening", damp);
    p_VA->Bind();
    p_IB->Bind();
    renderer->Draw(p_IB->GetCount());

    //Unbind all
    fb.UnBind();
    advectVelShader.UnBind();
    p_TexB->Unbind();
    p_VA->Unbind();
    p_IB->Unbind();
}

//Move density values around the fluid according to velocity
void Fluid::DensAdvect()
{
    SwapTextures(&fb, p_DtexA, p_DtexB);

    //disable blend so fragment values arent modified when drawn to texture
    GLCall(glDisable(GL_BLEND));

    //Drawing to framebuffer
    fb.Bind();
    renderer->Clear();

    float diss = 1.0f - (dissipation * time_step) / 1000.0f;
    diss = (diss < 0.0f) ? 0 : diss;
    //Advect values in previous texture to framebuffer
    advectDenShader.Bind();
    p_TexA->Bind();
    advectDenShader.SetUniform1i("u_Prev", 0);
    p_DtexB->Bind(1);
    advectDenShader.SetUniform1i("u_Prev_d", 1);
    advectDenShader.SetUniform1f("u_Timestep", time_step);
    advectDenShader.SetUniform1f("u_Size_m", divisor);
    advectDenShader.SetUniform1f("u_Dissipation", diss);
    p_VA->Bind();
    p_IB->Bind();
    renderer->Draw(p_IB->GetCount());

    //Unbind all
    fb.UnBind();
    advectDenShader.UnBind();
    p_TexA->Unbind();
    p_DtexB->Unbind();
    p_VA->Unbind();
    p_IB->Unbind();
}

//Inject values into fluid at a specified point
void Fluid::AddForces(glm::vec2 mPos, glm::vec4 vals, Texture*& p_T1, Texture*& p_T2, int m, Texture*& injection)
{
    SwapTextures(&fb, p_T1, p_T2);

    //disable blend so fragment values arent modified when drawn to texture
    GLCall(glDisable(GL_BLEND));

    //Drawing to framebuffer
    fb.Bind();
    renderer->Clear();

    //Copy previous texture to framebuffer with added values
    forceShader.Bind();
    forceShader.SetUniform2f("u_mPos", floor(mPos.x/(divisor/m)), floor(mPos.y/(divisor/m))); //position to inject at
    forceShader.SetUniform4f("u_Values", vals.x/(divisor/m), vals.y/(divisor/m), vals.z, vals.w); //values to set selected cells to
    p_T2->Bind();
    forceShader.SetUniform1i("u_Prev", 0); //Previous texture to copy to framebuffer
    injection->Bind(1);
    forceShader.SetUniform1i("u_Texture", 1); //Previous texture to copy to framebuffer
    forceShader.SetUniform1i("u_Size", inject_size * m); //radius around specified point to specify which cells to select
    p_VA->Bind();
    p_IB->Bind();
    renderer->Draw(p_IB->GetCount());

    //Unbind all
    fb.UnBind();
    forceShader.UnBind();
    injection->Unbind();
    p_VA->Unbind();
    p_IB->Unbind();
}

//render to screen
void Fluid::DrawFluid(int zoom)
{

    //Using blending for lower density cells
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    renderer->Clear();

    //Render texture A to the screen
    quadShader.Bind();
    p_DtexA->Bind();
    quadShader.SetUniform1i("u_renderedTexture", 0);
    quadShader.SetUniform3f("adj", 0, 0, 1); //used to pan and crop final render (x, y, crop (0<->0.99 crop smaller)
    quadShader.SetUniform1f("zoom", zoom); //zoom level
    p_VA->Bind();
    p_IB->Bind();
    renderer->Draw(p_IB->GetCount());

    //Unbind all
    p_VA->Unbind();
    p_IB->Unbind();
    p_DtexA->Unbind();
    quadShader.UnBind();
}



//Drawing lines to show vectors through field (laggy implementation)
void Fluid::DrawVectorField()
{
    GLCall(glDisable(GL_BLEND));

    //read texture A into pixel array
    p_TexA->Bind();
    GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, vel_data));
    glColor3f(1.0, 0.2, 0.2);

    float xUnits = 1 / ((float)width / 2);
    float yUnits = 1 / ((float)height / 2);

    GLCall(glBegin(GL_LINES));

    //Read vel_data in 15 pixel separations
    for (float i = 0; i < width; i += 7) {
        for (float j = 0; j < height; j += 7) {
            glm::vec4 pixel = glm::vec4(GetPixel(i, j));
            //OpenGl takes coordinates from -1 to 1, so convert pixel coords with p.xy - (width/2, height/2) * (xUnits, yUnits)
            GLCall(glVertex2f((i - width / 2) * xUnits, (j - height / 2) * yUnits));
            GLCall(glVertex2f(((i + pixel.r * 10) - width / 2) * xUnits, ((j + pixel.g * 10) - height / 2) * yUnits));
        }
    }
    glEnd();

    p_TexA->Unbind();
}

//Get pixel from 1D pixel array given i,j coordinates
glm::vec4 Fluid::GetPixel(unsigned int i, unsigned int j)
{
    //Pixels are saved in rgba format from GPU, so each pixel takes up 4 elements
    if (i < width && j < height) {
        float r = vel_data[(i * 4) + (j * width * 4)];
        float g = vel_data[(i * 4) + (j * width * 4) + 1] ;
        float b = vel_data[(i * 4) + (j * width * 4) + 2] ;
        float a = vel_data[(i * 4) + (j * width * 4) + 3];
        return glm::vec4(r, g, b, a);
    }
    cout << "index out of bounds, i: " << i << ", j: " << j << endl;
    return glm::vec4(0, 0, 0, 0);
}

//Copy given texture into another texture
void Fluid::BufferData(FrameBuffer* buf, Texture*& p_T)
{
    //disable blend so fragment values arent modified when drawn to texture
    GLCall(glDisable(GL_BLEND));

    //Drawing to given buffer texture
    buf->Bind();

    //Copy data from first texture to second texture
    renderer->Clear();
    copyShader.Bind();
    p_T->Bind();
    copyShader.SetUniform1i("u_renderedTexture", 0);
    p_VA->Bind();
    p_IB->Bind();
    renderer->Draw(p_IB->GetCount());

    //Unbind all
    buf->UnBind();
    copyShader.UnBind();
    p_T->Unbind();
    p_VA->Unbind();
    p_IB->Unbind();
}

//evolve velocity values
void Fluid::VelStep(double xpos, double ypos, glm::vec2 vel, float density, int zoom, Texture* injection)
{
    GLCall(glViewport(0, 0, GetVelWidth(), GetVelHeight()));

    //Inject values into fluid
    AddForces(glm::vec2(xpos / zoom, ypos / zoom), glm::vec4(vel, 0, density), p_TexA, p_TexB, 1, injection);

    //Diffuse(spread out) the values in the fluid
    Diffuse(&doubleBuffer_v, p_TexA, p_TexB, p_TexC, 1);
    VorticityConfinement();

    //Clear divergence caused by diffusion step
    //Project();

    //Advect(move values according to velocity) in the fluid
    VelAdvect();

    //Clear divergence caused by advection step
    Project();
}

//evolve density values
void Fluid::DensStep(double xpos, double ypos, float density, int zoom, Texture* injection)
{
    GLCall(glViewport(0, 0, GetDenWidth(), GetDenHeight()));
    
    //Inject values into fluid
    AddForces(glm::vec2(xpos / zoom, ypos / zoom), glm::vec4(0, 0, 0, density), p_DtexA, p_DtexB, divisor, injection);

    //Diffuse(spread out) the values in the fluid
    Diffuse(&doubleBuffer_d, p_DtexA, p_DtexB, p_DtexC, 0);

    //Advect(move values according to velocity) in the fluid
    DensAdvect();
}

//Uses boundary texture to block velocity values in velocity texture
void Fluid::BoundStep(Texture* bound)
{
    GLCall(glDisable(GL_BLEND));

    GLCall(glViewport(0, 0, GetVelWidth(), GetVelHeight()));

    SwapTextures(&fb, p_TexA, p_TexB);
    
    fb.Bind();
    renderer->Clear();

    boundShader.Bind();
    p_TexB->Bind();
    boundShader.SetUniform1i("u_Texture", 0);
    bound->Bind(1);
    boundShader.SetUniform1i("u_Bounds", 1);
    p_VA->Bind();
    p_IB->Bind();
    renderer->Draw(p_IB->GetCount());

    boundShader.UnBind();
    bound->Unbind();
    p_VA->Unbind();
    p_IB->Unbind();
}

//clears data in velocity and density textures
//janky method due to the way i've done texture swapping, but gets the job done
void Fluid::ClearBuffer() {
    
    
    fb.Bind();
    renderer->Clear();

    GLCall(glViewport(0, 0, GetVelWidth(), GetVelHeight()));

    SwapTextures(&fb, p_TexA, p_TexB);
    fb.Bind();
    renderer->Clear();

    SwapTextures(&fb, p_TexA, p_TexB);
    fb.Bind();
    renderer->Clear();

    fb.UnBind();
}

void Fluid::ClearTextures() {
    p_TexA->Delete();
    p_TexB->Delete();
    p_TexC->Delete();
    p_TexD->Delete();

    p_DtexA->Delete();
    p_DtexB->Delete();
    p_DtexC->Delete();
}