#include "GuiHandler.h"

GuiHandler::GuiHandler(Fluid* f, Renderer* r, unsigned int w, unsigned int h)
    :fluid(f), renderer(r), w_width(w), w_height(h)
{
    origin = vec2(w_width / 5, 20);

    //toolbox
    t_width = origin.x;
    t_height = ((w_height - origin.y )/ 5) ;
    //variable window
    v_width = t_width;
    v_height = w_height - origin.y - t_height ;
    //action panel
    a_height = t_height;
    a_width = 50;
    //render window
    r_width = w_width - v_width;
    r_height = v_height;
    //object window
    o_width = w_width - t_width - a_width;
    o_height = t_height;
    //object panels
    oc_width = 150;
    oc_height = 70;


    //Initialize buffer for reading density texture
    unsigned int buf_size = f->GetDenWidth() * f->GetDenHeight() * 4 * sizeof(float);
    frame = new float[buf_size];

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
}

GuiHandler::~GuiHandler()
{
    delete[] frame;
}

//directs UI render based on the what state the user is in
void GuiHandler::StateController(bool drawVectors, vec2 mousePos) {
    switch (st) {
    //Setting up simulation
    case SETUP:
        SetupRender(mousePos);
        break;

    //Resetting objects before going back to setup state 
    case RESET:
        fluid->ClearBuffer();
        sim_time = 0;
        //If simulation was recorded, do frame processing UI
        if (recording) {
            //wait for processing to finish before going back to setup
            if (processFrames(savePath)) {
                recording = false;
                frames_processed = 0;
                startThread = true;
                st = SETUP;
            }
        }
        else {
            st = SETUP;
        }
        break;

    //Simulation playing/paused
    default:
        FluidRender(zoom, drawVectors);
        break;
    }
}

//Panel showing output render
void GuiHandler::RenderWindow()
{
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    SetNextWindowPos(ImVec2(origin.x, origin.y));
    SetNextWindowSize(ImVec2(r_width, r_height));
    Begin("0", NULL, window_flags);
    GetWindowDrawList()->AddImage((void*)texture.GetID(), ImVec2(GetWindowPos()), ImVec2(w_width, r_height + origin.y));
    End();
}

//Left panel for tweaking variables
void GuiHandler::ToolVars()
{
    SetNextWindowPos(ImVec2(0, origin.y));
    SetNextWindowSize(ImVec2(v_width, v_height));
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize;
    if (st != SETUP) {
        window_flags |= ImGuiWindowFlags_NoInputs;
    }

    Begin("Tool Variables", NULL, window_flags);
    //Text("Test: %i", test);
    Text("%.3f ms/frame (%.1f FPS)", 1000.0f / GetIO().Framerate, GetIO().Framerate);
    Text("Sim Time: %i", sim_time);
    Separator();
    if (selected > -1) {
        Influencer& inf = *influencers[selected];

        std::string n = inf.GetName();
        static char name[50];
        strcpy_s(name, &n[0]);
        InputText(" ", name, IM_ARRAYSIZE(name));
        inf.SetName(name);


        Text("Position");
        vec2 pos = inf.GetPos();

        int posX = (int)((pos.x + 1) / 2 * r_width);
        int posY = (int)((pos.y + 1) / 2 * r_height);
        PushItemWidth(v_width / 2 - 50);
        DragInt("X###1", &posX, 2.0f, 0, (int)r_width, "%i"); SameLine();
        DragInt("Y###2", &posY, 2.0f, 0, (int)r_height, "%i");
        inf.SetPos(vec2(float(posX) / r_width *  2 - 1, float(posY) / r_height * 2 - 1));
        PopItemWidth();

        Text("Scale");
        int scaleX = (int)inf.GetSize().x;
        int scaleY = (int)inf.GetSize().y;
        PushItemWidth(v_width / 2 - 50);
        DragInt("X###3", &scaleX, 0.5f, 0, INT_MAX, "%i"); SameLine();
        DragInt("Y###4", &scaleY, 0.5f, 0, INT_MAX, "%i");
        inf.SetSize(vec2(scaleX, scaleY));
        PopItemWidth();

        Text("Rotation");
        int rot = (int)inf.GetRotation();
        PushItemWidth(v_width / 2 - 50);
        DragInt("###degrees", &rot, 1.0f, 0, 360, "%i");
        inf.SetRotation(rot);
        PopItemWidth();


        Text("Constant"); SameLine();
        HelpMarker("Toggle whether object has a finite lifetime.");
        bool con = inf.GetConstant();
        Checkbox("###constant", &con);
        inf.SetConstant(con);

        if (!inf.GetConstant()) {
            Text("Start time / Lifetime");
            int life[2] = { (int)inf.GetStartTime(), (int)inf.GetLifetime() };
            DragInt2("", life, 0.5f, 0, INT_MAX, "%i");
            inf.SetStartTime(life[0]);
            inf.SetLifetime(life[1]);
        }
        Separator();


        inf.ShowVars(v_width);
    }
    //code for tweaking fluid variables here
    else if (selected == -2)
    {
        Text("Viscocity"); SameLine();
        HelpMarker("The rate at which values diffuse into the surrounding fluid.");
        float vis = fluid->GetViscosity();
        DragFloat("###vis", &vis, 0.1f, 0.0f, 5.0f, "%.1f");
        fluid->SetViscosity(vis);

        /*Text("Time Step"); SameLine();
        HelpMarker("The rate of change in simulation");
        float ts = fluid->GetTimestep();
        DragFloat("###ts", &ts, 0.1f, 0.1f, 5.0f, "%.1f");
        fluid->SetTimestep(ts);*/

        Text("Vorticity"); SameLine();
        HelpMarker("Amount of curling in velocity field.");
        float vort = fluid->GetVorticity();
        DragFloat("###vort", &vort, 0.1f, 0.0f, 1.8f, "%.1f");
        fluid->SetVorticity(vort);

        Text("Dampening"); SameLine();
        HelpMarker("The rate of decay in velocity vectors.");
        int damp = (int)fluid->GetDampening();
        DragInt("###dampening", &damp, 1, 0, 50, "%i");
        fluid->SetDampening((float)damp);

        Text("Dissipation"); SameLine();
        HelpMarker("The rate of decay in density values.");
        int diss = (int)fluid->GetDissipation();
        DragInt("###dissipation", &diss, 1, 0, 50, "%i");
        fluid->SetDissipation((float)diss);

    }
    End();
}

//Bottom left panel for tools
void GuiHandler::ToolBox() 
{
    SetNextWindowPos(ImVec2(0, r_height + origin.y));
    SetNextWindowSize(ImVec2(t_width, t_height));
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize;
    if (st != SETUP) {
        window_flags |= ImGuiWindowFlags_NoInputs;
    }

    Begin("Toolbox", NULL, window_flags);
    ImTextureID thumbs_id = (void*)thumbs.GetID();
    float thumbs_w = (float)thumbs.GetWidth();
    float thumbs_h = (float)thumbs.GetHeight();
    if (BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) 
    {
        if (BeginTabItem("Ovoids")) {
            tool_tabs = 0;
            EndTabItem();
        }
        if (BeginTabItem("Quads")) {
            tool_tabs = 1;
            EndTabItem();
        }
        for (int i = 0; i < 4; i++)
        {
            PushID(i);
            ImVec2 size = ImVec2(32.0f, 32.0f); // Size of the image we want to make visible
            float y_offset = 32.0f / thumbs_h * tool_tabs;
            float x_offset = 32.0f / thumbs_w * i;
            ImVec2 uv0 = ImVec2(0.0f + x_offset, 0.0f + y_offset); // UV coordinates for lower-left
            ImVec2 uv1 = ImVec2((32.0f / thumbs_w) + x_offset, (32.0f / thumbs_h) + y_offset); // UV coordinates for upper-right
            ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);         // Black background
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint
            if (ImageButton(thumbs_id, size, uv0, uv1, -1, bg_col, tint_col)) {
                CreateInfluencer((type)i, (shape)tool_tabs);
            }
            PopID();
            SameLine();
        }
        EndTabBar();
    }
    End();
}

//play pause stop
void GuiHandler::ActionPanel()
{
    float spacing = (a_height - 20) / 2 - (32 * 3 + 10) / 2; //buffer space between first button and top of window
    SetNextWindowBgAlpha(0.35f); // Transparent background
    SetNextWindowPos(ImVec2(v_width, r_height + origin.y));
    SetNextWindowSize(ImVec2(a_width, a_height));
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    
    ImTextureID thumbs_id = (void*)thumbs.GetID();
    float thumbs_w = (float)thumbs.GetWidth();
    float thumbs_h = (float)thumbs.GetHeight();
    Begin("Title", NULL, window_flags);
    Dummy(ImVec2(0.0f, spacing));
    for (int i = 0; i < 3; ++i)
    {
        PushID(i);
        ImVec2 size = ImVec2(32.0f, 32.0f); // Size of the image we want to make visible
        float x_offset = 32.0f / thumbs_w * i;
        ImVec2 uv0 = ImVec2(0.0f + x_offset, 64.0f / thumbs_h); // UV coordinates for lower-left
        ImVec2 uv1 = ImVec2((32.0f / thumbs_w) + x_offset, 96.0f / thumbs_h); // UV coordinates for upper-right
        ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);         // Black background
        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint
        if (ImageButton(thumbs_id, size, uv0, uv1, 0, bg_col, tint_col))
        {
            st = (state)(i + 1);
        }
        PopID();
    }
    End();
}

//displays list of objects in setup mode
void GuiHandler::ObjectList()
{
    SetNextWindowPos(ImVec2(t_width + a_width, r_height + origin.y));
    SetNextWindowSize(ImVec2(o_width, o_height));
    ImGuiWindowFlags window_flags = 0;
    ImGuiWindowFlags child_window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    child_window_flags |= ImGuiWindowFlags_NoMove;
    child_window_flags |= ImGuiWindowFlags_NoResize;
    child_window_flags |= ImGuiWindowFlags_NoCollapse;
    child_window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (st != SETUP) {
        window_flags |= ImGuiWindowFlags_NoInputs;
        child_window_flags |= ImGuiWindowFlags_NoInputs;
    }

    float thumbs_w = (float)thumbs.GetWidth();
    float thumbs_h = (float)thumbs.GetHeight();

    float aspect = (float)r_width / (float)r_height;
    ImVec2 rendersize = ImVec2(50.0f, 50.0f);

    Begin("Objects", NULL, window_flags);
        if (st == PLAY && recording) {
            Text("Recorded: %i frames.",  gifData.size() );
            Text("Press the stop button to finish recording.");
        }
        else {
            //Fluid object (can't be deleted)
            if (selected == -2) {
                PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2, 0.2, 0.2, 1.0));
                BeginChild("Fluid", ImVec2((float)oc_width, (float)oc_height), true, child_window_flags);
                PopStyleColor();
            }
            else {
                BeginChild("Fluid", ImVec2((float)oc_width, (float)oc_height), true, child_window_flags);
            }
            if (IsWindowFocused()) {
                selected = -2;
            }
            BeginChild(" ", ImVec2(rendersize.x * aspect, rendersize.y), true, child_window_flags);
            GetWindowDrawList()->AddImage((void*)texture.GetID(), ImVec2(GetWindowPos()), ImVec2(GetWindowPos().x + rendersize.x, GetWindowPos().y + rendersize.y));
            EndChild(); SameLine();
            Text("Fluid");
            EndChild(); SameLine();

            //other objects
            unsigned int s = influencers.size();
            int to_delete = -1;
            for (int i = 0; i < s; i++)
            {
                Influencer& inf = *influencers[i];
                bool del = false;

                PushID(i);
                if (selected == i) {
                    del = true;
                    PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2, 0.2, 0.2, 1.0));
                    BeginChild(" ", ImVec2((float)oc_width, (float)oc_height), true, child_window_flags);
                    PopStyleColor();
                }
                else {
                    BeginChild(" ", ImVec2((float)oc_width, (float)oc_height), true, child_window_flags);
                }

                if (IsWindowFocused()) {
                    selected = i;
                }

                //setting up needed distances for image cutting
                float x_offset = 32 * (float)inf.GetType() / thumbs_w;
                float y_offset = 32 * (float)inf.GetShape() / thumbs_h;
                ImVec2 uv0 = ImVec2(0.0f + x_offset, 0.0f + y_offset); // UV coordinates for lower-left
                ImVec2 uv1 = ImVec2((32.0f / thumbs_w) + x_offset, (32.0f / thumbs_h) + y_offset); // UV coordinates for upper-right
                ImVec2 pad = ImVec2((rendersize.x - 32) / 2, (rendersize.y - 32) / 2);

                BeginChild("###icon", rendersize, true, child_window_flags);
                ImVec2 TL = ImVec2(GetWindowPos().x + pad.x, GetWindowPos().y + pad.y);
                ImVec2 LR = ImVec2(TL.x + 32, TL.y + 32);
                if (IsWindowFocused()) {
                    selected = i;
                }
                GetWindowDrawList()->AddImage((void*)thumbs.GetID(), TL, LR, uv0, uv1);
                EndChild(); SameLine();

                BeginChild("###text", ImVec2(rendersize.x * aspect, rendersize.y), true, child_window_flags);
                Text(&inf.GetName()[0]);
                if (IsWindowFocused()) {
                    selected = i;
                }
                if (del) {
                    if (Button("Delete")) {
                        to_delete = i;
                    }
                }
                EndChild();

                EndChild();
                int row = (o_width - 10) / (oc_width + 10);
                if ((i + 2 + row) % row != 0) {
                    SameLine();
                }
                PopID();
            }
            if (to_delete > -1) {
                vector<std::unique_ptr<Influencer>>::iterator it;
                influencers.erase(influencers.begin() + to_delete);
                to_delete = -1;
                selected = -1;
            }
        }
    End();
}

//check framerate
void GuiHandler::Diag()
{
    SetNextWindowPos(ImVec2(v_width, v_height + origin.y));
    SetNextWindowSize(ImVec2(380, 80));
    Begin("Readout");
    Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / GetIO().Framerate, GetIO().Framerate);
    End();
}

//create dye, velocity and boundary textures to be injected into fluid object
void GuiHandler::MapForces()
{
    sim_time++;

    dye.Bind();
    renderer->Clear();

    vel.Bind();
    renderer->Clear();

    vel_buf.Bind();
    renderer->Clear();

    bound.Bind();
    renderer->Clear();

    //draw influencers to texture if within lifetime
    for (int i = 0; i < influencers.size(); i++) {
        unsigned int start = influencers[i]->GetStartTime();
        unsigned int life = influencers[i]->GetLifetime();
        bool con = influencers[i]->GetConstant();
        if (con || start < sim_time && start + life > sim_time) {
            //attempt to inject to each texture, influencer will sort out whether to actually do so depending on textureType param
            dye.Bind();
            GLCall(glViewport(0, 0, r_width, r_height));
            influencers[i]->Inject(r_width, r_height, 1, 1); //1 = dye injection

            vel.Bind();
            GLCall(glViewport(0, 0, vel_inj.GetWidth(), vel_inj.GetHeight()));
            influencers[i]->Inject(vel_inj.GetWidth(), vel_inj.GetHeight(), fluid->GetDivisor(), 2); //2 = velocity injection
            textureCopy();
            GLCall(glEnable(GL_BLEND));

            bound.Bind();
            GLCall(glViewport(0, 0, vel_inj.GetWidth(), vel_inj.GetHeight()));
            influencers[i]->Inject(vel_inj.GetWidth(), vel_inj.GetHeight(), fluid->GetDivisor(), 3); //3 = boundary injection
        }
    }

    bound.UnBind();
}

void GuiHandler::textureCopy() {
    GLCall(glDisable(GL_BLEND));
    
    vel_buf.Bind();
    renderer->Clear();
    copyShader.Bind();
    vel_inj.Bind();
    copyShader.SetUniform1i("u_renderedTexture", 0);
    p_VA->Bind();
    p_IB->Bind();
    renderer->Draw(p_IB->GetCount());

    vel_buf.UnBind();
    copyShader.UnBind();
    vel_inj.Unbind();
    p_VA->Unbind();
    p_IB->Unbind();
}

//creates an influencer of a type and shape and pushes onto array
void GuiHandler::CreateInfluencer(type t, shape s)
{
    if (t == DYE) {
        std::string name = "Dye " + to_string(dyeCount);
        influencers.push_back(std::make_unique<Dye>(t, s, vec2(0, 0), vec2(50, 50), &shaders[t], name, 1.0f));
        dyeCount++;
    }
    else if (t == FORCE) {
        std::string name = "Force " + to_string(forceCount);
        influencers.push_back(std::make_unique<Force>(t, s, vec2(0, 0), vec2(50, 50), &shaders[t], name, 135, 0, 2.2f, &vel_inj_buf));
        forceCount++;
    }
    else if (t == BOUNDARY) {
        std::string name = "Bound " + to_string(bCount);
        influencers.push_back(std::make_unique<Boundary>(t, s, vec2(0, 0), vec2(50, 50), &shaders[t], name));
        bCount++;
    }
    else if (t == FORCEDYE) {
        std::string name = "Force/Dye " + to_string(forceDyeCount);
        influencers.push_back(std::make_unique<ForceDye>(t, s, vec2(0, 0), vec2(50, 50), &shaders[t], name, 135, 0, 2.2f, 1.0f, &vel_inj_buf));
        forceDyeCount++;
    }
    selected = influencers.size() - 1;
    held = influencers.size() - 1;
}

//Renders the screen for setting up the influencer tools before starting the simulation
void GuiHandler::SetupRender(vec2 mousePos)
{
    GLCall(glEnable(GL_BLEND));

    GLCall(glViewport(0, 0, r_width, r_height));
    fb.Bind();
    renderer->Clear();
    
    //place influencers when clicking mouse
    if (held > -1)
    {
        influencers[held]->SetPos(vec2((mousePos.x/r_width) * 2 - 1, (mousePos.y/r_height) * 2 - 1));
        if (IsMouseClicked(0))
        {
            held = -1;
        }
    }

    //draw all influencers in array
    for (int i = 0; i < influencers.size(); i++)
    {    
        prepro.Bind();
        renderer->Clear();
        influencers[i]->Draw(r_width, r_height);

        fb.Bind();
        outliner.Bind();
        buf.Bind();
        outliner.SetUniform1i("u_Texture", 0);
        p_VA->Bind();
        p_IB->Bind();
        renderer->Draw(p_IB->GetCount());

        outliner.UnBind();
    }
   
    fb.UnBind();
    outliner.UnBind();
    buf.Unbind();
    p_VA->Unbind();
    p_IB->Unbind();
}

//renders the fluid simulation
void GuiHandler::FluidRender(int zoom, bool drawVectors)
{
    fb.Bind();
    GLCall(glViewport(0, 0, fluid->GetDenWidth(), fluid->GetDenHeight()));
    fluid->DrawFluid(zoom);
    if (recording && st == PLAY) {
        if (sim_time % frame_spacing == 0) {
            gifData.push_back(getFrame());
            //cout << "frame " << sim_time / frame_spacing << " recorded" << endl;
        }
    }
    if (drawVectors)
    {
        fluid->DrawVectorField();
    }
    fb.UnBind();
}

//gets frame from fluid sim and saves to vector
vector<uint8_t> GuiHandler::getFrame() {

    //convert 0-1 float values into 256 rgb colour format
    GLCall(glDisable(GL_BLEND));
    prepro.Bind();
    renderer->Clear();
    
    convertShader.Bind();
    texture.Bind();
    convertShader.SetUniform1i("u_renderedTexture", 0);
    p_VA->Bind();
    p_IB->Bind();
    renderer->Draw(p_IB->GetCount());

    //read texture into pixel array
    buf.Bind();
    GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, frame));
    
    //need to convert to uint8_t
    int size = fluid->GetDenWidth() * fluid->GetDenHeight() * 4;
    vector<uint8_t> data(size);
    copy(frame, frame + size, data.begin());

    convertShader.UnBind();
    buf.Unbind();
    p_VA->Unbind();
    p_IB->Unbind();
    
    return data;
}

//Start ImGui frame
void GuiHandler::GuiStart()
{
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();
}

//Render ImGui
void GuiHandler::GuiRender()
{
    
    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

//End ImGui frame
void GuiHandler::GuiEnd()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    DestroyContext();
}

//Main function collates calling all functions
void GuiHandler::GuiMain(bool drawVectors, vec2 mousePos)
{
    StateController(drawVectors, mousePos);

    ToolVars();
    RenderWindow();
    if (st == PLAY) {
        MapForces();
    }
    ToolBox();
    ActionPanel();
    ObjectList();
    //ShowDemoWindow();

    //Do export menu here
    if (BeginMainMenuBar())
    {
        if (BeginMenu("File"))
        {
            if (st == SETUP) {
                if (MenuItem("Export")) {
                    export_popup = true;
                }
            }
            else {
                if (MenuItem("Export", " ", false, false)) {}
            }
            ImGui::EndMenu();
        }
        EndMainMenuBar();
    }

    if (export_popup) {
        OpenPopup("Export settings");
    }
    ImVec2 center = GetMainViewport()->GetCenter();
    SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (BeginPopupModal("Export settings", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char p[100];
        strcpy_s(p, &savePath[0]);
        InputText(" ", &savePath[0], savePath.size()); SameLine();

        if (Button("...")) {
            savePath = FileCoItem();
        }
        SliderInt("Frame Spacing", &frame_spacing, 0, 50, "%i");
        SameLine(); HelpMarker("The number of frames between frame captures in the recording of the simulation.");
        SliderInt("Frame Speed", &frame_speed, 0, 50, "%i");
        SameLine(); HelpMarker("The amount of time in 100ths of a second between frames in the final .gif.");

        
        if (Button("Start Recording", ImVec2(120, 0))) { 
            gifData.clear();
            export_popup = false; 
            CloseCurrentPopup();
            recording = true;
            st = PLAY;
        }
        SetItemDefaultFocus();
        SameLine();
        if (Button("Cancel", ImVec2(120, 0))) { export_popup = false; CloseCurrentPopup();  }
        EndPopup(); 
    }
    GuiRender();
}

string GuiHandler::FileCoItem() {
    string path = "";
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {

        IFileSaveDialog* pFile;
        //IFileOpenDialog* pFileOpen;
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
            IID_IFileSaveDialog, reinterpret_cast<void**>(&pFile));

        if (SUCCEEDED(hr))
        {
            // Set default extension
            hr = pFile->SetDefaultExtension(L"gif");
            // Show the Open dialog box.
            hr = pFile->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFile->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    if (SUCCEEDED(hr))
                    {
                        char buffer[500];
                        size_t s = wcslen(pszFilePath);
                        if (s > 500) {
                            cout << "Maximum length of path is 500 characters. Please try another location or use a shorter file name." << endl;
                        }
                        wcstombs_s(&s, buffer, pszFilePath, 500);
                        path = string(buffer);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFile->Release();
        }
    }
    CoUninitialize();
    return path;
}


//process the frame data that was recorded and save to GIF format and show popup to track progress
bool GuiHandler::processFrames(string path) {

    auto f = [this, path]() {
        GifWriter writer = {};
        GifBegin(&writer, path.c_str(), fluid->GetDenWidth(), fluid->GetDenHeight(), frame_speed, 8, false);
        for (int i = 0; i < gifData.size(); ++i) {
            GifWriteFrame(&writer, &gifData[i][0], fluid->GetDenWidth(), fluid->GetDenHeight(), frame_speed, 8, false);
            frames_processed = i + 1;
            if (cancel_process) {
                cancel_process = false;
                break;
            }
        }
        GifEnd(&writer);
    };

    if (startThread) {
        thread (f).detach();
        startThread = false;
    }

    OpenPopup("Processing");
    ImVec2 center = GetMainViewport()->GetCenter();
    SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        
    if (BeginPopupModal("Processing", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        Text("Frames processed: %i / %i", frames_processed, gifData.size() );
        if (frames_processed >= gifData.size() - 1) {
            if (Button("Done")) {
                CloseCurrentPopup();
                EndPopup();
                return true;
            }
        }
        else {
            if (Button("Stop")) {
                cancel_process = true;
                CloseCurrentPopup();
                EndPopup();
                return true;
            }
        }
        EndPopup();
    }
    return false;
}

void GuiHandler::ClearTextures() {
    texture.Delete();
    dye_inj.Delete();
    vel_inj.Delete();
    vel_inj_buf.Delete();
    bound_inj.Delete();
    buf.Delete();
}

