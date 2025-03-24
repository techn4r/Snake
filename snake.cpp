#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>std::

const int BLOCK_SIZE = 20;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int GRID_WIDTH = SCREEN_WIDTH / BLOCK_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / BLOCK_SIZE;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

struct Segment {
    int x;
    int y;
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL не может быть инициализирован: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Snake Game",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Не удалось создать окно: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Не удалось создать рендерер: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool gameOver = false;
    Direction dir = RIGHT;
    int score = 0;

    std::vector<Segment> snake;
    Segment head = { GRID_WIDTH / 2, GRID_HEIGHT / 2 };
    snake.push_back(head);

    srand(static_cast<unsigned int>(time(0)));
    Segment fruit = { rand() % GRID_WIDTH, rand() % GRID_HEIGHT };

    Uint32 lastMove = SDL_GetTicks();
    const int MOVE_DELAY = 100;

    while (!gameOver) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                gameOver = true;
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (dir != RIGHT) dir = LEFT;
                        break;
                    case SDLK_RIGHT:
                        if (dir != LEFT) dir = RIGHT;
                        break;
                    case SDLK_UP:
                        if (dir != DOWN) dir = UP;
                        break;
                    case SDLK_DOWN:
                        if (dir != UP) dir = DOWN;
                        break;
                    case SDLK_ESCAPE:
                        gameOver = true;
                        break;
                }
            }
        }

        Uint32 current = SDL_GetTicks();
        if (current - lastMove >= MOVE_DELAY) {
            lastMove = current;
            Segment newHead = snake[0];
            switch (dir) {
                case LEFT:  newHead.x--; break;
                case RIGHT: newHead.x++; break;
                case UP:    newHead.y--; break;
                case DOWN:  newHead.y++; break;
                default: break;
            }

            if (newHead.x < 0) newHead.x = GRID_WIDTH - 1;
            if (newHead.x >= GRID_WIDTH) newHead.x = 0;
            if (newHead.y < 0) newHead.y = GRID_HEIGHT - 1;
            if (newHead.y >= GRID_HEIGHT) newHead.y = 0;

            for (size_t i = 1; i < snake.size(); ++i) {
                if (snake[i].x == newHead.x && snake[i].y == newHead.y) {
                    gameOver = true;
                    break;
                }
            }
            if (gameOver) break;

            snake.insert(snake.begin(), newHead);

            if (newHead.x == fruit.x && newHead.y == fruit.y) {
                score += 10;
                fruit.x = rand() % GRID_WIDTH;
                fruit.y = rand() % GRID_HEIGHT;
            } else {
                snake.pop_back();
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect fruitRect = { fruit.x * BLOCK_SIZE, fruit.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &fruitRect);

        for (const auto& segment : snake) {
            SDL_Rect segRect = { segment.x * BLOCK_SIZE, segment.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &segRect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    std::cout << "Game Over! Your score: " << score << std::endl;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
