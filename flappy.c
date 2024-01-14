#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 670
#define BIRD_WIDTH 50
#define BIRD_HEIGHT 50
#define PIPE_WIDTH 100
#define PIPE_GAP 200

typedef struct {
    float x, y;
    float vy;
} Bird;

typedef struct Pipe {
    float x;
    float gap_y;
    struct Pipe* next;
} Pipe;

Bird bird = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0 };
Pipe* pipes = NULL;
int score = 0;

SDL_Texture* bgTexture = NULL;
SDL_Texture* pipeTexture = NULL;
SDL_Texture* birdTexture = NULL;
Mix_Music *backgroundSound = NULL;

void add_pipe() {
    Pipe* pipe = malloc(sizeof(Pipe));
    pipe->x = SCREEN_WIDTH;
    pipe->gap_y = rand() % (SCREEN_HEIGHT - PIPE_GAP);
    pipe->next = pipes;
    pipes = pipe;
}

void remove_pipe() {
    Pipe* pipe = pipes;
    pipes = pipes->next;
    free(pipe);
}

void update_pipes() {
    Pipe* pipe = pipes;
    while (pipe != NULL) {
        pipe->x -= 2;
        if (pipe->x < -PIPE_WIDTH) {
            remove_pipe();
            add_pipe();
            score++;
        }
        pipe = pipe->next;
    }
}

void render_pipes(SDL_Renderer* renderer) {
    Pipe* pipe = pipes;
    while (pipe != NULL) {
        SDL_Rect upper_pipe_dst = { pipe->x, 0, PIPE_WIDTH, pipe->gap_y };
        SDL_Rect lower_pipe_dst = { pipe->x, pipe->gap_y + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT - pipe->gap_y - PIPE_GAP };
        SDL_RenderCopy(renderer, pipeTexture, NULL, &upper_pipe_dst);
        SDL_RenderCopy(renderer, pipeTexture, NULL, &lower_pipe_dst);
        pipe = pipe->next;
    }
}

void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window* window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    TTF_Font* font = TTF_OpenFont("C:/Users/lenovo/Desktop/flappy bird/arial.ttf", 52);
    bgTexture = IMG_LoadTexture(renderer, "C:/Users/lenovo/Desktop/flappy bird/assets/background.jpeg");
    pipeTexture = IMG_LoadTexture(renderer, "C:/Users/lenovo/Desktop/flappy bird/assets/pipes.jpeg");
    birdTexture = IMG_LoadTexture(renderer, "C:/Users/lenovo/Desktop/flappy bird/assets/flappybird.png");
    backgroundSound = Mix_LoadMUS("C:/Users/lenovo/Desktop/flappy bird/assets/song.mp3");

    if (backgroundSound == NULL) {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
    }

    Mix_PlayMusic(backgroundSound, -1);

    SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

    SDL_Color yellow = { 255, 255, 0, 255 };

    SDL_Surface* openingSurface1 = TTF_RenderText_Solid(font, "FLAPPY", yellow);
    SDL_Texture* openingTexture1 = SDL_CreateTextureFromSurface(renderer, openingSurface1);
    SDL_Rect openingDst1 = { SCREEN_WIDTH / 2 - openingSurface1->w / 2, SCREEN_HEIGHT / 2 - openingSurface1->h, openingSurface1->w, openingSurface1->h };
    SDL_RenderCopy(renderer, openingTexture1, NULL, &openingDst1);
    SDL_FreeSurface(openingSurface1);
    SDL_DestroyTexture(openingTexture1);

    SDL_Surface* openingSurface2 = TTF_RenderText_Solid(font, "The Game", yellow);
    SDL_Texture* openingTexture2 = SDL_CreateTextureFromSurface(renderer, openingSurface2);
    SDL_Rect openingDst2 = { SCREEN_WIDTH / 2 - openingSurface2->w / 2, SCREEN_HEIGHT / 2, openingSurface2->w, openingSurface2->h };
    SDL_RenderCopy(renderer, openingTexture2, NULL, &openingDst2);
    SDL_FreeSurface(openingSurface2);
    SDL_DestroyTexture(openingTexture2);

    
    
    SDL_RenderPresent(renderer);
    SDL_Delay(3000);

    add_pipe();

    int gameRunning = 1;
    while (gameRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameRunning = 0;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        bird.vy = -10;
                        break;
                }
            }
        }

        bird.y += bird.vy;
        bird.vy += 0.5f;
        
        if (bird.y < 0 || bird.y > SCREEN_HEIGHT) {
            gameRunning = 0;
        }

        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        SDL_Rect bird_dst = { bird.x - BIRD_WIDTH / 2, bird.y - BIRD_HEIGHT / 2, BIRD_WIDTH, BIRD_HEIGHT };
        SDL_RenderCopy(renderer, birdTexture, NULL, &bird_dst);

        update_pipes();
        render_pipes(renderer);

        Pipe* pipe = pipes;
        while (pipe != NULL) {
            if (pipe->x < bird.x + BIRD_WIDTH / 2 && pipe->x + PIPE_WIDTH > bird.x - BIRD_WIDTH / 2 &&
                (bird.y - BIRD_HEIGHT / 2 < pipe->gap_y || bird.y + BIRD_HEIGHT / 2 > pipe->gap_y + PIPE_GAP)) {
                gameRunning = 0;
            }
            pipe = pipe->next;
        }

        char score_text[64];
        sprintf(score_text, "Score: %d", score);
        render_text(renderer, font, score_text, 10, 10);

        SDL_RenderPresent(renderer);
    }
    
    SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, "GAME OVER", white);
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
    SDL_Rect gameOverDst = { SCREEN_WIDTH / 2 - gameOverSurface->w / 2, SCREEN_HEIGHT / 2 - gameOverSurface->h - 20, gameOverSurface->w, gameOverSurface->h };
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverDst);
    SDL_FreeSurface(gameOverSurface);
    SDL_DestroyTexture(gameOverTexture);

    char final_score_text[64];
    sprintf(final_score_text, "Final Score: %d", score);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, final_score_text, white);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreDst = { SCREEN_WIDTH / 2 - scoreSurface->w / 2, SCREEN_HEIGHT / 2 - gameOverSurface->h - 80, scoreSurface->w, scoreSurface->h };
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreDst);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    SDL_RenderPresent(renderer);
    SDL_Delay(10000);

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(pipeTexture);
    SDL_DestroyTexture(birdTexture);
    Mix_FreeMusic(backgroundSound);
    backgroundSound = NULL;

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}