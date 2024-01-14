#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

// Constants for screen dimensions, bird dimensions, pipe dimensions, and gap between pipes
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BIRD_WIDTH 50
#define BIRD_HEIGHT 50
#define PIPE_WIDTH 100
#define PIPE_GAP 200

// Structure to represent the bird
typedef struct {
    float x, y;
    float vy; // velocity in the y direction
} Bird;

// Structure to represent a pipe
typedef struct Pipe {
    float x;
    float gap_y; // y-coordinate of the gap between pipes
    struct Pipe* next; // pointer to the next pipe in the linked list
} Pipe;

Bird bird = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0 }; // Initialize the bird at the center of the screen
Pipe* pipes = NULL; // Head of the linked list representing pipes
int score = 0; // Player's score

SDL_Texture* bgTexture = NULL; // Background texture
SDL_Texture* pipeTexture = NULL; // Pipe texture
SDL_Texture* birdTexture = NULL; // Bird texture

// Function to add a new pipe to the linked list
void add_pipe() {
    Pipe* pipe = malloc(sizeof(Pipe)); // Allocate memory for a new pipe
    pipe->x = SCREEN_WIDTH; // Set the initial x-coordinate of the pipe offscreen
    pipe->gap_y = rand() % (SCREEN_HEIGHT - PIPE_GAP); // Set a random y-coordinate for the gap
    pipe->next = pipes; // Set the next pointer to the current head of the list
    pipes = pipe; // Update the head of the list to the new pipe
}

// Function to remove the first pipe in the linked list
void remove_pipe() {
    Pipe* pipe = pipes; // Point to the head of the list
    pipes = pipes->next; // Update the head of the list to the next pipe
    free(pipe); // Free the memory of the removed pipe
}

// Function to update the position of all pipes in the linked list
void update_pipes() {
    Pipe* pipe = pipes; // Start with the head of the list
    while (pipe != NULL) {
        pipe->x -= 2; // Move the pipe to the left
        if (pipe->x < -PIPE_WIDTH) { // Check if the pipe is offscreen
            remove_pipe(); // Remove the offscreen pipe
            add_pipe(); // Add a new pipe to replace the removed one
            score++; // Increase the player's score
        }
        pipe = pipe->next; // Move to the next pipe in the list
    }
}

// Function to render all pipes on the screen
void render_pipes(SDL_Renderer* renderer) {
    Pipe* pipe = pipes; // Start with the head of the list
    while (pipe != NULL) {
        // Render the upper part of the pipe
        SDL_Rect upper_pipe_dst = { pipe->x, 0, PIPE_WIDTH, pipe->gap_y };
        // Render the lower part of the pipe
        SDL_Rect lower_pipe_dst = { pipe->x, pipe->gap_y + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT - pipe->gap_y - PIPE_GAP };
        SDL_RenderCopy(renderer, pipeTexture, NULL, &upper_pipe_dst); // Render upper pipe
        SDL_RenderCopy(renderer, pipeTexture, NULL, &lower_pipe_dst); // Render lower pipe
        pipe = pipe->next; // Move to the next pipe in the list
    }
}

// Function to render text on the screen
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

    // Create window and renderer
    SDL_Window* window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    TTF_Font* font = TTF_OpenFont("C:/Users/lenovo/Desktop/flappy bird/arial.ttf", 52);

    // Load textures
    bgTexture = IMG_LoadTexture(renderer, "C:/Users/lenovo/Desktop/flappy bird/assets/background.jpeg");
    pipeTexture = IMG_LoadTexture(renderer, "C:/Users/lenovo/Desktop/flappy bird/assets/pipes.jpeg");
    birdTexture = IMG_LoadTexture(renderer, "C:/Users/lenovo/Desktop/flappy bird/assets.png");

    SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

    SDL_Color yellow = { 255, 255, 0, 255 };

    // Render opening screen
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

    add_pipe(); // Add the first pipe

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
                        bird.vy = -10; // Apply upward velocity when the spacebar is pressed
                        break;
                }
            }
        }

        bird.y += bird.vy; // Update bird's position based on velocity
        bird.vy += 0.5f; // Apply gravity

        if (bird.y < 0 || bird.y > SCREEN_HEIGHT) {
            gameRunning = 0; // End the game if the bird hits the top or bottom of the screen
        }

        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        // Render the bird
        SDL_Rect bird_dst = { bird.x - BIRD_WIDTH / 2, bird.y - BIRD_HEIGHT / 2, BIRD_WIDTH, BIRD_HEIGHT };
        SDL_RenderCopy(renderer, birdTexture, NULL, &bird_dst);

        update_pipes(); // Update pipe positions
        render_pipes(renderer); // Render pipes

        // Check for collision with pipes
        Pipe* pipe = pipes;
        while (pipe != NULL) {
            if (pipe->x < bird.x + BIRD_WIDTH / 2 && pipe->x + PIPE_WIDTH > bird.x - BIRD_WIDTH / 2 &&
                (bird.y - BIRD_HEIGHT / 2 < pipe->gap_y || bird.y + BIRD_HEIGHT / 2 > pipe->gap_y + PIPE_GAP)) {
                gameRunning = 0; // End the game if the bird hits a pipe
            }
            pipe = pipe->next;
        }

        // Render the player's score
        char score_text[64];
        sprintf(score_text, "Score: %d", score);
        render_text(renderer, font, score_text, 10, 10);

        SDL_RenderPresent(renderer);
    }

    // Render the game over screen
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
    SDL_Rect scoreDst = { SCREEN_WIDTH / 2 - scoreSurface->w / 2, SCREEN_HEIGHT / 2 + gameOverSurface->h - 30, scoreSurface->w, scoreSurface->h };
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreDst);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    SDL_RenderPresent(renderer);
    SDL_Delay(10000);

    // Cleanup resources
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(pipeTexture);
    SDL_DestroyTexture(birdTexture);

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
