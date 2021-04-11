#include <stdlib.h>
#include <math.h>
#include <SDL/SDL.h>
#include "vTexture.h"

int main(int argc, char *argv[])
{   
    SDL_Surface *hwSurface;
    SDL_Surface *renderSurface;
    SDL_Surface *image;
    SDL_Event event;
    Uint8 *buffer;
    int i, j;
    int done = 0;
    int surfaceWidth, surfaceHeight, surfaceDepth;

    // Init SDL Video
    SDL_Init(SDL_INIT_VIDEO);

	surfaceWidth = 240;
	surfaceHeight = 240;
	surfaceDepth = 32;

    // Open HW screen and set video mode 240x240, with double buffering 
    hwSurface =
        SDL_SetVideoMode(surfaceWidth, surfaceHeight, surfaceDepth,
                         SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);

    // Hide the cursor, FunKey doesn't come with a mouse
    SDL_ShowCursor(0);

    SDL_LockSurface(hwSurface);
    buffer = (Uint8 *)hwSurface->pixels;
    for (i = 0; i < hwSurface->h; i++) {
        memset(buffer, (i * 255) / hwSurface->h, hwSurface->pitch);
        buffer += hwSurface->pitch;
    }
    SDL_UnlockSurface(hwSurface);
    SDL_UpdateRect(hwSurface, 0, 0, 0, 0);
    SDL_Flip(hwSurface);
    // Sleep for 1 seconds
    SDL_Delay(1000);

	if ((image = SDL_LoadBMP("logo128a.bmp")) == NULL) {
		printf("Could not load image: %s\n", SDL_GetError());
		return 1;
	}

	vTexture *texture = new vTexture;
	texture->tPtr = (Uint8 *)image->pixels;
	texture->tw = image->w;
	texture->th = image->h;
	texture->screen = (Uint8 *)hwSurface->pixels;
	texture->screenWidth = hwSurface->w;
	texture->screenHeight = hwSurface->h;

	vVec3 v[3];
	v[0].set(120.0f, 50.0f, 0.0f);
	v[1].set(170.0f, 150.0f, 0.0f);
	v[2].set(70.0f, 150.0f, 0.0f);

	texture->tri->v[0].svtx = v[0];
	texture->tri->v[1].svtx = v[1];
	texture->tri->v[2].svtx = v[2];

	texture->tri->v[0].txc.set(63.0f, 127.0f);
	texture->tri->v[1].txc.set(0.0f, 0.0f);
	texture->tri->v[2].txc.set(127.0f, 0.0f);

	if (SDL_LockSurface(hwSurface) < 0) {
		printf("Could not lock surface: %s\n", SDL_GetError());
		return 1;
	}
	texture->render();
	SDL_UnlockSurface(hwSurface);
	SDL_UpdateRect(hwSurface, 0, 0, 0, 0);
    SDL_Flip(hwSurface);
    // Sleep for 1 seconds
    SDL_Delay(1000);

	vVec3 nv[3];
	float angle = 0.0f;
	float scale = 1.0f;
	float sv = -0.01f;
	float sn, cs;
	float nx, ny;
	int count = 0;
	Uint32 elapseTime = 0;

	while(!done) {
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
				{
					SDL_Rect dst;
					dst.x = event.button.x;
					dst.y = event.button.y;
					dst.w = image->w;
					dst.h = image->h;
					SDL_BlitSurface(image, NULL, hwSurface, &dst);
					SDL_UpdateRects(hwSurface, 1, &dst);
				}
				break;
			
			case SDL_KEYDOWN:
				done = 1;
				break;

			case SDL_QUIT:
				done = 1;
				break;
			
			default:
				break;
			}
		}

		sn = sinf(angle / 100.0f * M_PI);
		cs = cosf(angle / 100.0f * M_PI);
		for (i = 0; i < 3; i++) {
			nv[i] = v[i];
			nx = (nv[i][0] - (surfaceWidth >> 1)) * scale;
			ny = (nv[i][1] - (surfaceHeight >> 1)) * scale;
			nv[i][0] = (nx * cs - ny * sn) + (surfaceWidth >> 1);
			nv[i][1] = (nx * sn + ny * cs) + (surfaceHeight >> 1);
		}
		texture->tri->v[0].svtx = nv[0];
		texture->tri->v[1].svtx = nv[1];
		texture->tri->v[2].svtx = nv[2];
		Uint32 startTime = SDL_GetTicks();
		if (SDL_LockSurface(hwSurface) < 0) {
			printf("Could not lock surface: %s\n", SDL_GetError());
			return 1;
		}
		memset(hwSurface->pixels, 0, hwSurface->h * hwSurface->pitch);
		texture->render();
		SDL_UnlockSurface(hwSurface);
		SDL_UpdateRect(hwSurface, 0, 0, 0, 0);
        SDL_Flip(hwSurface);
		Uint32 endTime = SDL_GetTicks();
		elapseTime += (endTime - startTime);
		if (++count == 100) {
			//printf("%f ms (%f fps)\n", (float)elapseTime / 100.0f, 1000.0f / ((float)elapseTime / 100.0f));
			elapseTime = 0;
			count = 0;
		}

		scale += sv;
		if ((scale > 1.2f) || (scale < 0.1f)) sv = -sv;
		angle += 1.0f;
		if (angle > 360.0f) angle -= 360.0f;
	}

    /// Deinit SDL
    SDL_Quit();

    return 0;
}
