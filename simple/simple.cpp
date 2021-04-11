#include <SDL/SDL.h>

int main(int argc, char *argv[])
{   
    // Init SDL Video
    SDL_Init(SDL_INIT_VIDEO);

    // Open HW screen and set video mode 240x240, with double buffering 
    SDL_Surface* hw_surface =
        SDL_SetVideoMode(240, 240, 32,
                         SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);

    // Hide the cursor, FunKey doesn't come with a mouse
    SDL_ShowCursor(0);

    // Draw a green square in the middle of the screen
    SDL_Rect draw_rect = {.x=70, .y=70, .w=100, .h=100};
    Uint32 color = SDL_MapRGB(hw_surface->format, 0, 255, 0);
    SDL_FillRect(hw_surface, &draw_rect, color);

    // Switch buffers to show the square we just drew
    SDL_Flip(hw_surface);

    // Sleep for 5 seconds
    SDL_Delay(5000);

    /// Deinit SDL
    SDL_Quit();

    return 0;
}
