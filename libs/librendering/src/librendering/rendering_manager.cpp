#include "librendering/rendering_manager.hpp"

#include <iostream>
#include <SDL.h>

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

        if (SDL_CreateWindowAndRenderer(0, 0, 0, &_window, &_renderer) < 0)
        {
            std::cerr << "Error creating window or renderer: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return 1;
        }

        SDL_SetWindowSize(_window, 800, 480);

        _texture = SDL_CreateTexture(
            _renderer,
            SDL_PIXELFORMAT_BGR24,
            SDL_TEXTUREACCESS_STREAMING,
            settings.frame_width,
            settings.frame_height
        );

        std::cout << "Hot keys:" << std::endl;
        std::cout << "\tESC - quit the program" << std::endl;
        std::cout << "\tp - pause video" << std::endl;

        return 0;
    }

	int run_tick()
    {
        while (SDL_PollEvent(&e))
        {
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

        SDL_RenderClear(_renderer);
        SDL_RenderCopy(_renderer, _texture, NULL, NULL);
        SDL_RenderPresent(_renderer);

        SDL_Delay(10);

        return 0;
    }

	int term()
    {
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
