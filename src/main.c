#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#define GRID_WIDTH  200
#define GRID_HEIGHT 100
#define CELL_SIZE 4

#define EMPTY 0
#define SAND  1
#define WALL  2
#define WATER 3

#define WALL_THICK 2

int grid[GRID_HEIGHT][GRID_WIDTH];

int isEmpty(int y, int x) {
    if (y < 0 || y >= GRID_HEIGHT || x < 0 || x >= GRID_WIDTH)
        return 0;  // outside grid is not empty

    for (int dy = 0; dy < WALL_THICK; dy++)
        for (int dx = 0; dx < WALL_THICK; dx++)
            if (y+dy < GRID_HEIGHT && x+dx < GRID_WIDTH && grid[y+dy][x+dx] == WALL)
                return 0;
    return grid[y][x] == EMPTY || grid[y][x] == WATER;
}

int main(void) {
    srand(SDL_GetTicks());
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Falling Sand",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        GRID_WIDTH * CELL_SIZE,   // 200*4 = 800
        GRID_HEIGHT * CELL_SIZE,  // 100*4 = 400
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP
    );

    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED
    );

    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = 0;
        }
    }    

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_MOUSEMOTION &&
                (event.motion.state & SDL_BUTTON_LMASK)) {
            
                int mx = event.motion.x / CELL_SIZE;
                int my = event.motion.y / CELL_SIZE;
            
                if (mx >= 0 && mx < GRID_WIDTH &&
                    my >= 0 && my < GRID_HEIGHT) {
            
                        SDL_Keymod mods = SDL_GetModState();

                        for (int dy = -1; dy <= 1; dy++)
                            for (int dx = -1; dx <= 1; dx++)
                                if (mx+dx >= 0 && mx+dx < GRID_WIDTH && my+dy >=0 && my+dy < GRID_HEIGHT)
                                    if (!(mods & KMOD_SHIFT) || grid[my+dy][mx+dx] != WALL)
                                        grid[my+dy][mx+dx] = mods & KMOD_SHIFT ? WALL :
                                                              mods & KMOD_GUI ? WATER :
                                                              mods & KMOD_ALT ? EMPTY : SAND;
                }
            }
        }

        for (int y = GRID_HEIGHT - 2; y >= 0; y--) {
            for (int x = 0; x < GRID_WIDTH; x++) {    
                if (grid[y][x] == SAND) {
                    // Fall straight down if empty or water
                    if (isEmpty(y + 1, x)) {
                        // If water below, swap sand and water
                        if (grid[y + 1][x] == WATER) {
                            grid[y + 1][x] = SAND;
                            grid[y][x] = WATER;
                        } else {
                            grid[y + 1][x] = SAND;
                            grid[y][x] = EMPTY;
                        }
                    } else {
                        // slide down-left or down-right but prevent corner squeeze
                        int first = rand() & 1;
                        int dx1 = first ? -1 : 1;
                        int dx2 = -dx1;

                        // first direction
                        if (x + dx1 >= 0 && x + dx1 < GRID_WIDTH &&
                            (grid[y + 1][x + dx1] == EMPTY || grid[y + 1][x + dx1] == WATER) &&
                            (isEmpty(y, x + dx1))) {
                            if (grid[y + 1][x + dx1] == WATER) {
                                grid[y + 1][x + dx1] = SAND;
                                grid[y][x] = WATER;
                            } else {
                                grid[y + 1][x + dx1] = SAND;
                                grid[y][x] = EMPTY;
                            }
                        }
                        // second direction
                        else if (x + dx2 >= 0 && x + dx2 < GRID_WIDTH &&
                                (grid[y + 1][x + dx2] == EMPTY || grid[y + 1][x + dx2] == WATER) &&
                                (grid[y][x + dx2] == EMPTY)) {
                            if (grid[y + 1][x + dx2] == WATER) {
                                grid[y + 1][x + dx2] = SAND;
                                grid[y][x] = WATER;
                            } else {
                                grid[y + 1][x + dx2] = SAND;
                                grid[y][x] = EMPTY;
                            }
                        }
                    }
                }
                
                if (grid[y][x] == WATER) {
                    // Fall down if empty
                    if (grid[y + 1][x] == EMPTY) {
                        grid[y + 1][x] = WATER;
                        grid[y][x] = EMPTY;
                    } else {
                        // Flow sideways if empty
                        int dirs[2] = {-1, 1};
                        // Randomize which side we try first
                        if (rand() & 1) { int t = dirs[0]; dirs[0]=dirs[1]; dirs[1]=t; }
                
                        for (int i = 0; i < 2; i++) {
                            int nx = x + dirs[i];
                            if (nx >= 0 && nx < GRID_WIDTH &&
                                grid[y][nx] == EMPTY &&
                                grid[y + 1][nx] != WALL) { 
                                grid[y][nx] = WATER;
                                grid[y][x] = EMPTY;
                                break;
                            }
                        }
                    }
                }                
            }
        }

        // Clear screen (dark gray)
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
        
                if (grid[y][x] == SAND) {
                    SDL_Rect cell = {
                        x * CELL_SIZE,
                        y * CELL_SIZE,
                        CELL_SIZE,
                        CELL_SIZE
                    };
                    SDL_SetRenderDrawColor(renderer, 220, 200, 50, 255); // sand color
                    SDL_RenderFillRect(renderer, &cell);
                }

                if (grid[y][x] == WALL) {
                    for (int dy = 0; dy < 2; dy++) {
                        for (int dx = 0; dx < 2; dx++) {
                            SDL_Rect cell = {
                                x * CELL_SIZE + dx,
                                y * CELL_SIZE + dy,
                                CELL_SIZE,
                                CELL_SIZE
                            };
                            SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
                            SDL_RenderFillRect(renderer, &cell);
                        }
                    }
                }
                
                if (grid[y][x] == WATER) {
                    SDL_Rect cell = {
                        x * CELL_SIZE,
                        y * CELL_SIZE,
                        CELL_SIZE,
                        CELL_SIZE
                    };
                    SDL_SetRenderDrawColor(renderer, 50, 100, 220, 255); // blue water
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
        }        

        // Present frame
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
