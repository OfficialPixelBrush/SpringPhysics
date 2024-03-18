#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
SDL_Renderer *renderer;

// Utility macros
#define CHECK_ERROR(test, message) \
    do { \
        if((test)) { \
            fprintf(stderr, "%s\n", (message)); \
            exit(1); \
        } \
    } while(0)

struct float2 {
    float x,y;
};

typedef struct float2 float2;

struct spring {
    float2 posStart;
    float2 posEnd;
    float2 posEndVelocity;
    float elasticity;
    int desiredLength;
};
typedef struct spring spring;

spring* sp1;
int renderSizeX = 1280;
int renderSizeY = 720;
int running = 1;
int gravity = 3;
int frame_start_time;
int target_frame_time = 16;


int startRenderer() {
	float renderScale = 1.0f;
	
	// Initialize SDL
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    CHECK_ERROR(SDL_Init(SDL_INIT_VIDEO) != 0, SDL_GetError());

	// Get Desktop size
	SDL_DisplayMode dm;

	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		 SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
		 return 1;
	}

    // Create an SDL window
    SDL_Window *window = SDL_CreateWindow("Springs", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dm.w/3*2, dm.h/3*2, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
    CHECK_ERROR(window == NULL, SDL_GetError());

    // Create a renderer (accelerated and in sync with the display refresh rate)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); 
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderSetLogicalSize(renderer, renderSizeX, renderSizeY);   
    CHECK_ERROR(renderer == NULL, SDL_GetError());

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);	
	SDL_RenderClear(renderer);
    return 0;
}

int renderFrame() {
    if (SDL_GetTicks() - frame_start_time > target_frame_time) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);	
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, sp1->posStart.x, sp1->posStart.y, sp1->posEnd.x, sp1->posEnd.y);
        
        SDL_RenderPresent(renderer);
        // Reset frame start time for next iteration
        frame_start_time = SDL_GetTicks();
    }
    // Delay for the remaining time
    SDL_Delay(target_frame_time);
    return 0;
}

float getDistanceBetweenPositions(float2 pos1, float2 pos2) {
  float squared_distance_x = pow(pos2.x - pos1.x, 2);
  float squared_distance_y = pow(pos2.y - pos1.y, 2);

  // Add the squared distances and calculate the square root to get the distance
  return sqrt(squared_distance_x + squared_distance_y);
}

// Apparently someshit called Hookes Law is needed?
int simulate() {
    renderFrame();
    sp1->posEndVelocity.y += gravity;
    float distance = getDistanceBetweenPositions(sp1->posStart, sp1->posEnd);
    float2 difference;
    difference.x = sp1->posEnd.x - sp1->posStart.x;
    difference.y = sp1->posEnd.y - sp1->posStart.y;
    if (getDistanceBetweenPositions(sp1->posStart, sp1->posEnd) > sp1->desiredLength) {
        sp1->posEndVelocity.x += difference.x*sp1->elasticity;
        sp1->posEndVelocity.y += difference.y*sp1->elasticity;
    }
    sp1->posEnd.x -= sp1->posEndVelocity.x;
    sp1->posEnd.y -= sp1->posEndVelocity.y;
    /*
    sp1->posEnd.x += sp1->posEndVelocity.x;
    sp1->posEnd.y += sp1->posEndVelocity.y;
    */
    return 0;
}

int WinMain() {
    frame_start_time = SDL_GetTicks();
    printf("Springs!\n");
    startRenderer();
    sp1 = (spring*)malloc(sizeof(spring));
    sp1->posStart.x = renderSizeX/2;
    sp1->posStart.y = renderSizeY/4;
    sp1->posEnd.x = renderSizeX/3;
    sp1->posEnd.y = renderSizeY/4*3;
    sp1->posEndVelocity.x = 0;
    sp1->posEndVelocity.y = 0;
    sp1->elasticity = 0.1f;
    sp1->desiredLength = 10;
    SDL_Event event;
    while (running) {
        simulate();
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = 0;
            } else if(event.type == SDL_KEYDOWN) {
                const char *key = SDL_GetKeyName(event.key.keysym.sym);
				// Exit
                if(strcmp(key, "Q") == 0) {
                    running = 0;
					SDL_DestroyRenderer(renderer);
					SDL_Quit();
					return 0;
				}              				
            }
        }
    }
    return 0;
}