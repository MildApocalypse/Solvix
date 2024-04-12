#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Fluid.h"
#include "GuiHandler.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace glm;

static bool drawVectors = false;

static const unsigned int w_width = 1080, w_height = 740; //window
static const unsigned int divisor = 3; //number to divide velocity field size by for performance
static const unsigned int f_width = w_width - (w_width/5), f_height = (w_height - 20) - ((w_height - 20)/5); //fluid

static Renderer renderer;

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(w_width, w_height, "SolVix", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

    if (glewInit() != GLEW_OK) {
        cout << "Error!" << endl;
    }

    //create fluid
    Fluid fluid(&renderer, f_width, f_height, divisor);

    //setup GUI handler
    GuiHandler gui(&fluid, &renderer, w_width, w_height);
    
    //GLFW context
    {
        //setup blend mode
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        //variables to hold cursor position and previous position
        double xpos = gui.GetOrigin().x, ypos = gui.GetOrigin().y, xpos0 = gui.GetOrigin().x, ypos0 = gui.GetOrigin().y;
        
        //density to be injected at mouse position
        float density = 0;

        //velocity to be injected at mouse position
        vec2 vel = vec2(0, 0);

        GLCall(glClearColor(0.0, 0.0, 0.0, 0.0));

        //gui.GifTest();
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            //open ImGui frame
            gui.GuiStart();

            //getting cursor position
            xpos0 = xpos;
            ypos0 = ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            float f = gui.GetOrigin().x;
            xpos -= gui.GetOrigin().x;
            ypos -= gui.GetOrigin().y;

            //Draw the representation of the density in the fluid and the velocity field if flagged

            //listen for mouse click
            int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
            if (state == GLFW_PRESS)
            {
                density = 1;
                vel = vec2(xpos, ypos) - vec2(xpos0, ypos0);
            }
            else {
                density = 0.0;
                vel = vec2(0, 0);
            }

            if (gui.GetState() == PLAY)
            {
                fluid.VelStep(xpos, ypos, vel, density, gui.GetZoom(), gui.GetVelInjection());
                fluid.BoundStep(gui.GetBoundInjection());
                fluid.DensStep(xpos, ypos, density, gui.GetZoom(), gui.GetDyeInjection());
            }
            

            
            gui.GuiMain(drawVectors, vec2(xpos, ypos));

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        
    }

    //fluid.ClearTextures();
    //gui.ClearTextures();

    //shutdown ImGui and glfw processes 
    gui.GuiEnd();
    glfwTerminate();
    return 0;
}
