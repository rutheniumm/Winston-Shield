#pragma once

#include <string>
#include <SDL2/SDL.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>

class Window {
public:
    Window(int width, int height, std::string title = "Game");
    ~Window();

    void Update();

    void SetVSync(bool);
    void SetTitle(std::string);

    double GetTime(); // Gets seconds since program started
    SDL_Window* GetPtr();
    glm::vec2 GetSize() { return { m_params.width, m_params.height }; }
    float GetAspect();

    bool IsOpen();

private:
    SDL_Window* m_windowPtr = nullptr;
    SDL_GLContext m_glContext;

    struct WindowParams {
        int width, height;
    } m_params;

    SDL_Event m_event;  // Event handling

    // Callback for the Emscripten main loop
    static void UpdateCallback();
};
