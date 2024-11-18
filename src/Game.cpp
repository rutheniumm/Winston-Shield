#include "Game.h"
#include <emscripten.h>
#include <glm/glm.hpp>

// Static init
std::unique_ptr<Window> Game::m_window;
std::unique_ptr<Input> Game::m_input;
std::unique_ptr<Camera> Game::m_camera;
std::unique_ptr<Scene> Game::m_scene;

Game::Game() {
    m_window = std::make_unique<Window>(1600, 1100, "Winston shield");
    m_input = std::make_unique<Input>();
    m_camera = std::make_unique<Camera>(glm::vec3(0, 0, 5), m_window->GetAspect());
    m_scene = std::make_unique<Scene>();
}

static float last = 0.0f;
static float now = 0.0f;
static float deltaTime = 0.0f;

void Game::mainLoop(void *mainLoopArg) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    now = m_window->GetTime();
    deltaTime = now - last;
    last = now;

    m_camera->Update(deltaTime);
    m_scene->Update(deltaTime);

    m_scene->Render();

    // POST FRAME
    m_window->Update();
}

// Static entry point for Emscripten
static void staticMainLoop(void *arg) {
    Game *game = static_cast<Game *>(arg);
    game->mainLoop(arg);
}

void Game::Run() {
    void* mainLoopArg = m_window->GetPtr();
    emscripten_set_main_loop_arg(staticMainLoop, this, 0, true);
}
