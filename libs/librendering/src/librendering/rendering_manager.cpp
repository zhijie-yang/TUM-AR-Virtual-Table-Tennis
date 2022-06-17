#include "librendering/rendering_manager.hpp"

#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_sdlrenderer.h"

#include <iostream>
#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

using namespace librendering;

class rendering_manager::impl {
private:
	SDL_Window* _window;
	SDL_Renderer* _renderer;
    SDL_Texture* _texture;

	SDL_Event e;

	bool _paused;
	bool _quit;
public:
	impl() : e{}, _window{nullptr}, _renderer{nullptr}, _paused{false}, _quit{false}
    {}
    ~impl() = default;

	bool quit_get() const
    {
        return _quit;
    }

	void quit_set()
    {
        _quit = true;
    }

    bool paused_get() const
    {
        return _paused;
    }

    void paused_set(bool value)
    {
        _paused = value;
    }

    std::function<int(void*, int)> capture_deserialize()
    {
        return [&](void* data, int size) -> int
        {
            unsigned char* texture_data = NULL;
            int texture_pitch = 0;

            SDL_LockTexture(_texture, 0, (void**)&texture_data, &texture_pitch);
            memcpy(texture_data, data, size);
            SDL_UnlockTexture(_texture);

            return 0;
        };
    }

	int init(const rendering_settings& settings)
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            return 1;
        }

        // Setup window
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        _window = SDL_CreateWindow("Dear ImGui+SDL2+SDL_Renderer+OpenCV", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, settings.window_width, settings.window_height, window_flags);
        if (_window == NULL)
        {
            std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return 1;
        }

        // Setup SDL_Renderer instance
        _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
        if (_renderer == NULL)
        {
            std::cerr << "Error creating renderer: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(_window);
            SDL_Quit();
            return 1;
        }

        _texture = SDL_CreateTexture(
            _renderer,
            SDL_PIXELFORMAT_BGR24,
            SDL_TEXTUREACCESS_STREAMING,
            settings.frame_width,
            settings.frame_height
        );

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForSDLRenderer(_window, _renderer);
        ImGui_ImplSDLRenderer_Init(_renderer);

        std::cout << "Hot keys:" << std::endl;
        std::cout << "\tESC - quit the program" << std::endl;
        std::cout << "\tp - pause video" << std::endl;

        return 0;
    }

	int run_tick()
    {
        while (SDL_PollEvent(&e))
        {
            ImGui_ImplSDL2_ProcessEvent(&e);
            switch (e.type)
            {
            case SDL_QUIT:
                quit_set();
                break;

            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    quit_set();
                    break;

                case SDLK_p:
                    paused_set(!paused_get());
                    break;
                }
                break;
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Main Menu");

            ImGui::Text("Main Menu");

            if (ImGui::Button("Cool")) {
                //
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        SDL_RenderClear(_renderer);
        SDL_RenderCopy(_renderer, _texture, NULL, NULL);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(_renderer);

        SDL_Delay(10);

        return 0;
    }

	int term()
    {
        // Cleanup
        ImGui_ImplSDLRenderer_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_DestroyTexture(_texture);

        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        SDL_Quit();

        return 0;
    }
};

rendering_manager::rendering_manager()
    : _impl{ std::make_unique<impl>() }
{}

librendering::rendering_manager::~rendering_manager()
{}

bool rendering_manager::quit_get() const
{
    return _impl->quit_get();
}

void rendering_manager::quit_set()
{
    _impl->quit_set();
}

bool rendering_manager::paused_get() const
{
    return _impl->paused_get();
}

void rendering_manager::paused_set(bool value)
{
    _impl->paused_set(value);
}

std::function<int(void*, int)> rendering_manager::capture_deserialize()
{
    return _impl->capture_deserialize();
}

int rendering_manager::init(const rendering_settings& settings)
{
    return _impl->init(settings);
}

int rendering_manager::run_tick()
{
    return _impl->run_tick();
}

int rendering_manager::term()
{
    return _impl->term();
}
