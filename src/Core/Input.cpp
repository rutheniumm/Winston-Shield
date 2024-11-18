#include "Input.h"
#include "Game.h"

bool Input::IsKeyPressed(int keyCode) {
    const Uint8* state = SDL_GetKeyboardState(NULL);  // Get the current keyboard state
    return state[SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(keyCode))] == 1; // Check if key is pressed
}

bool Input::IsMouseButtonPressed(int mouseButton) {
    Uint32 mouseState = SDL_GetMouseState(NULL, NULL);  // Get the current mouse state
    if (mouseButton == SDL_BUTTON_LEFT) {
        return (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;  // Check if left mouse button is pressed
    }
    else if (mouseButton == SDL_BUTTON_RIGHT) {
        return (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0; // Check if right mouse button is pressed
    }
    return false;
}

glm::vec2 Input::GetMousePos() {
    int xpos = 0, ypos = 0;
    SDL_GetMouseState(&xpos, &ypos);  // Get the current mouse position
    return glm::vec2(xpos, ypos);     // Return as glm::vec2
}
