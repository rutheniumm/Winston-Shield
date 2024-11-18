#include "Window.h"
#include <iostream>
#include <emscripten/emscripten.h>

Window::Window(int width, int height, std::string title) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "Failed to initialize SDL2: " << SDL_GetError() << std::endl;
        return;
    }

    // Create an SDL window
    m_windowPtr = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,   SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    if (!m_windowPtr) {
        std::cout << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }
     printf("INFO: SDL window created with: %s samples.\n", glGetString(GL_SAMPLES));

    // Create an OpenGL context
    m_glContext = SDL_GL_CreateContext(m_windowPtr); 
    printf("INFO: GL version: %s\n", glGetString(GL_VERSION));
    if (!m_glContext) {
        std::cout << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_windowPtr);
        SDL_Quit();
        return;
    }
    printf("INFO: created GL context: %s\n", glGetString(GL_VENDOR));
    // Initialize OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_params.width = width;
    m_params.height = height;

    // Set the window size callback (in SDL, you need to manually manage the resizing)

    // Set up the Emscripten main loop directly
    SDL_GL_SetSwapInterval(1); // Enable V-Sync by default
}

void Window::Update() {
    // Handle events
    SDL_PollEvent(&m_event);
    if (m_event.type == SDL_QUIT) {
        SDL_QuitRequested();
    }

    // Swap buffers
    SDL_GL_SwapWindow(m_windowPtr);  // Swap buffers
}

Window::~Window() {
    SDL_GL_DeleteContext(m_glContext);  // Delete OpenGL context
    SDL_DestroyWindow(m_windowPtr);     // Destroy the window
    SDL_Quit();                        // Quit SDL
}

double Window::GetTime() {
    return SDL_GetTicks() / 1000.0;  // Get time in seconds
}

void Window::SetVSync(bool val) {
    SDL_GL_SetSwapInterval(val ? 1 : 0); // 1 enables V-Sync, 0 disables
}

void Window::SetTitle(std::string title) {
    SDL_SetWindowTitle(m_windowPtr, title.c_str());
}

bool Window::IsOpen() {
    return !SDL_QuitRequested();
}

SDL_Window* Window::GetPtr() {
    return m_windowPtr;
}

float Window::GetAspect() {
    return (float)m_params.width / m_params.height;
}
