#ifndef BASEAPP_H
#define BASEAPP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

struct BaseApp
{
    int displayWidth, displayHeight;
    glm::vec4 clearColor;
    bool isRunning;

    SDL_Window* window = NULL;
    SDL_GLContext context = NULL;
    GLuint defaultVAO = 0;

    virtual bool userInit() = 0;
    virtual void userShutdown() = 0;
    virtual void userRender() = 0;
    virtual void userRenderUI() = 0;

    ~BaseApp()
    {
        if (context)
        {
            SDL_GL_DeleteContext(context);
            context = NULL;
        }

        if (window)
        {
            SDL_DestroyWindow(window);
            window = NULL;
        }

        IMG_Quit();
        SDL_Quit();
    }

    int setup(const char * title, int32_t width, int32_t height)
    {
        //Initialize SDL2
        if(SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            SDL_LogCritical(0, "SDL could not initialize: %s", SDL_GetError());
            return -1;
        }

        //Initialize SDL Image
        if (!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG))
        {
            SDL_LogCritical(0, "SDL Image could not initialize: %s", IMG_GetError());
            return -1;
        }

#ifndef __EMSCRIPTEN__
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        // MSAA (Multi-sample Anti-Aliasing).
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        //Use OpenGL 3.2 core (Minimum for Renderdoc)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

#ifdef __APPLE__
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
#endif

        // Create SDL Window
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        if(!window)
        {
            SDL_LogCritical(0, "Window creation error: %s", SDL_GetError());
            return -1;
        }

        // Create GL Context
        context = SDL_GL_CreateContext(window);
        if (!context)
        {
            SDL_LogCritical(0, "GL context creation error: %s", SDL_GetError());
            SDL_DestroyWindow(window);
            window = NULL;
            return -1;
        }

        //Initialize GLAD
#ifdef __EMSCRIPTEN__
        if(!gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress))
#else
        if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
#endif
        {
            SDL_LogCritical(0,"Error initializing GLAD!");

            SDL_GL_DeleteContext(context);
            context = NULL;

            SDL_DestroyWindow(window);
            window = NULL;

            return -1;
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(window, context);
        ImGui_ImplOpenGL3_Init(NULL);

#ifndef __EMSCRIPTEN__
        //Default Vertex Array Object
        glGenVertexArrays(1, &defaultVAO);
        glBindVertexArray(defaultVAO);
#endif

        //Use Vsync to avoid unwanted screen tearing.
        if (SDL_GL_SetSwapInterval(1) != 0)
        {
            SDL_LogWarn(0, "GL enable Vsync failed: %s", SDL_GetError());
        }

        // populate displayWidth and displayHeight before the user's init()
        // so the user can use these variables if needed.
        SDL_GL_GetDrawableSize(window, &displayWidth, &displayHeight);

        // User Init
        if (!userInit())
        {
            SDL_GL_DeleteContext(context);
            context = NULL;

            SDL_DestroyWindow(window);
            window = NULL;

            return -1;
        }

        // Our state
        clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
        isRunning = true;
        return 0;
    }

    void step()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                isRunning = false;
            }

            ImGui_ImplSDL2_ProcessEvent(&e);
        }

        SDL_GL_GetDrawableSize(window, &displayWidth, &displayHeight);
        glViewport(0, 0, displayWidth, displayHeight);
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        userRender();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        userRenderUI();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    void teardown()
    {

        // User Shutdown
        userShutdown();

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &defaultVAO);
        defaultVAO = 0;

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
};

#endif // BASEAPP_H
