#include "Subsystems/App.h"

constexpr int FPS = 60;
constexpr int DELAY_TIME = static_cast<int>(1000.0f / FPS);

int main(int argc, char *argv[]) {
    App::get()->init();
    //TODO Parse command line arguments
    //app.parse(argc, argv)

    float deltaTime = 0;

    //TODO Mover all this crap to the app itself?
    while (App::get()->running()) {
        const Uint64 frameStart = SDL_GetTicks();

        //Move all this to the EngineStateManager?
        App::get()->handleEvents();
        App::get()->update(deltaTime);
        App::get()->render();

        //Limit FPS so we don't burn out the GPU (kidding...)
        if (const Uint64 frameTime = SDL_GetTicks() - frameStart; frameTime < DELAY_TIME) {
            SDL_Delay(static_cast<int>(DELAY_TIME - frameTime));
        }

        deltaTime = (SDL_GetTicks() - frameStart) / 1000.0f;
    }

    return 0;
}