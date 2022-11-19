#include <remotemo/remotemo.hpp>
#include <SDL.h>
#include <SDL_image.h>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

const char* screenImageFile = "terminal_screen.png";
const SDL_Point SCREEN_TEXTURE_SIZE = {946, 732};
const SDL_Rect backgroundMinArea {.x=118, .y=95, .w=700, .h=540};


SDL_Texture* loadTexture(SDL_Renderer *renderer, const std::string &file)
{
    auto texture = IMG_LoadTexture(renderer, file.c_str());
    if (texture == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "IMG_LoadTexture(renderer, \"%s\") failed: %s\n",
                file.c_str(), SDL_GetError());
    }
    return texture;
}

void getClipping(const int win_w, const int win_h,
        const int texture_w, const int texture_h,
        const SDL_Rect &minArea,
        float &scale, SDL_Rect &target)
{
    target.h = texture_h;
    target.w = texture_w;
    float scale_w = (float)win_w / minArea.w;
    float scale_h = (float)win_h / minArea.h;
    scale = scale_w < scale_h ? scale_w : scale_h;

    int border_h = ((win_h / scale) - minArea.h + 1) / 2;
    int border_w = ((win_w / scale) - minArea.w + 1) / 2;
    target.y = border_h - minArea.y;
    target.x = border_w - minArea.x;
}

int test_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                "SDL_Init(SDL_INIT_VIDEO) failed: %s\n", SDL_GetError());
        return 2;
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear")) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                "SDL_SetHint() (scale quality to linear) failed: %s\n",
                SDL_GetError());
    }

    auto window = SDL_CreateWindow(
            "Monitor TEST",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_RESIZABLE
    );
    if (window == nullptr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                "SDL_CreateWindow() failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 2 ;
    }

    auto renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_TARGETTEXTURE);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                "SDL_CreateRenderer() failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 2 ;
    }

    char *basePath = SDL_GetBasePath();
    if (basePath == nullptr) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                "SDL_GetBasePath() failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 2;
    }

    SDL_Log("basePath: %s\n", basePath);
    std::string resImagePath = basePath;
    SDL_free(basePath);
#ifdef _WIN32
    const char PATH_SEP = '\\';
#else
    const char PATH_SEP = '/';
#endif
    resImagePath = resImagePath + "res" + PATH_SEP + "img" + PATH_SEP;
    SDL_Log("resImagePath: %s\n", resImagePath.c_str());

    auto screenTexture = loadTexture(renderer,
            resImagePath + screenImageFile);
    if (screenTexture == nullptr) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 2 ;
    }
    SDL_Rect backgroundTarget;
    float backgroundScale;
    getClipping(WINDOW_WIDTH, WINDOW_HEIGHT,
            SCREEN_TEXTURE_SIZE.x, SCREEN_TEXTURE_SIZE.y,
            backgroundMinArea, backgroundScale, backgroundTarget);

    bool done = false;
    bool fullscreen = false;
    SDL_Event event;
    bool keyHandled_F11 = false;
    while (!done && SDL_WaitEvent(&event)) {
        bool unhandled_event = true;
        while(!done && unhandled_event) {
            switch (event.type) {
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_q:
                        case SDLK_w:
                            if(SDL_GetModState() & KMOD_CTRL) {
                                done = true;
                            }
                            break;
                        case SDLK_F11:
                            if (keyHandled_F11) break;
                            /* Do the following only once per keypress.
                             * Therefore we set keyHandled_F11 when done
                             * and then reset it back to false when KEYUP.
                            */
                            if (!fullscreen) {
                                SDL_SetWindowFullscreen(window,
                                        SDL_WINDOW_FULLSCREEN_DESKTOP);
                            } else {
                                SDL_SetWindowFullscreen(window,
                                        0);
                            }
                            fullscreen = !fullscreen;
                            keyHandled_F11 = true;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_F11:
                            keyHandled_F11 = false;
                            break;
                        default:
                            break;
                    }
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            getClipping(event.window.data1,
                                    event.window.data2,
                                    SCREEN_TEXTURE_SIZE.x,
                                    SCREEN_TEXTURE_SIZE.y,
                                    backgroundMinArea,
                                    backgroundScale, backgroundTarget);
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
            unhandled_event = (SDL_PollEvent(&event) != 0);
        }

        SDL_RenderClear(renderer);
        float tmpScaleX, tmpScaleY;
        SDL_RenderGetScale(renderer, &tmpScaleX, &tmpScaleY);
        SDL_RenderSetScale(renderer, backgroundScale, backgroundScale);
        SDL_RenderCopy(renderer, screenTexture, NULL, &backgroundTarget);
        SDL_RenderSetScale(renderer, tmpScaleX, tmpScaleY);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    SDL_DestroyTexture(screenTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
