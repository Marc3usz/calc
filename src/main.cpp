#include "functionFactory.hpp"
#include <iostream>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <cmath>
#include <vector>


int mapX(double x, double minX, double maxX, int screenWidth) {
    return static_cast<int>((x - minX) / (maxX - minX) * screenWidth);
}

int mapY(double y, double minY, double maxY, int screenHeight) {
    return static_cast<int>(screenHeight - ((y - minY) / (maxY - minY) * screenHeight));
}

void drawAxes(SDL_Renderer* renderer, double minX, double maxX, double minY, double maxY,
    int screenWidth, int screenHeight) {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);

    if (minY <= 0 && maxY >= 0) {
        int y0 = mapY(0, minY, maxY, screenHeight);
        SDL_RenderDrawLine(renderer, 0, y0, screenWidth, y0);
    }

    if (minX <= 0 && maxX >= 0) {
        int x0 = mapX(0, minX, maxX, screenWidth);
        SDL_RenderDrawLine(renderer, x0, 0, x0, screenHeight);
    }
}

void drawGrid(SDL_Renderer* renderer, double minX, double maxX, double minY, double maxY,
    int screenWidth, int screenHeight) {
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);

    double rangeX = maxX - minX;
    double rangeY = maxY - minY;

    double gridSpacingX = 1.0;
    double gridSpacingY = 1.0;

    if (rangeX > 20) gridSpacingX = std::ceil(rangeX / 20);
    if (rangeY > 20) gridSpacingY = std::ceil(rangeY / 20);

    double startX = std::ceil(minX / gridSpacingX) * gridSpacingX;
    for (double x = startX; x <= maxX; x += gridSpacingX) {
        int screenX = mapX(x, minX, maxX, screenWidth);
        SDL_RenderDrawLine(renderer, screenX, 0, screenX, screenHeight);
    }

    double startY = std::ceil(minY / gridSpacingY) * gridSpacingY;
    for (double y = startY; y <= maxY; y += gridSpacingY) {
        int screenY = mapY(y, minY, maxY, screenHeight);
        SDL_RenderDrawLine(renderer, 0, screenY, screenWidth, screenY);
    }
}

void plotFunction(SDL_Renderer* renderer, Function& func,
    double minX, double maxX, double minY, double maxY,
    int screenWidth, int screenHeight, SDL_Color color) {

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    const int numPoints = screenWidth * 2;
    double step = (maxX - minX) / numPoints;

    std::vector<SDL_Point> points;
    bool lastPointValid = false;
    SDL_Point lastPoint = { 0, 0 };

    for (int i = 0; i <= numPoints; i++) {
        double x = minX + i * step;
        double y;

        try {
            y = func(x);

            bool pointValid = y >= minY && y <= maxY &&
                !std::isinf(y) && !std::isnan(y);

            if (pointValid) {
                SDL_Point point = {
                    mapX(x, minX, maxX, screenWidth),
                    mapY(y, minY, maxY, screenHeight)
                };

                if (lastPointValid) {
                    SDL_RenderDrawLine(renderer, lastPoint.x, lastPoint.y, point.x, point.y);
                }

                lastPoint = point;
                lastPointValid = true;
            }
            else {
                lastPointValid = false;
            }
        }
        catch (...) {
            lastPointValid = false;
        }
    }
}

int WinMain() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    SDL_Window* window = SDL_CreateWindow(
        "Function Grapher",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    functionMapping fs = {
        {"sin", [](ld x) {return std::sin(x); }},
        {"cos", [](ld x) {return std::cos(x); }}
    };

    try {
        double minX = -10.0;
        double maxX = 10.0;
        double minY = -5.0;
        double maxY = 5.0;

        std::vector<std::string> expressions{ "asin(x) * cos(x^2)" };
        FunctionFactory fns(fs, expressions);
        std::map<std::string, Function> functions = fns.getFunctions();

        auto it = functions.find("a");
        if (it == functions.end()) {
            throw std::runtime_error("Function not found");
        }
        Function& fn = it->second;

        bool quit = false;
        SDL_Event e;

        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
                else if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        minY += 0.5;
                        maxY += 0.5;
                        break;
                    case SDLK_DOWN:
                        minY -= 0.5;
                        maxY -= 0.5;
                        break;
                    case SDLK_LEFT:
                        minX -= 0.5;
                        maxX -= 0.5;
                        break;
                    case SDLK_RIGHT:
                        minX += 0.5;
                        maxX += 0.5;
                        break;
                    case SDLK_KP_PLUS:
                    case SDLK_EQUALS:
                        minX = minX * 0.9;
                        maxX = maxX * 0.9;
                        minY = minY * 0.9;
                        maxY = maxY * 0.9;
                        break;
                    case SDLK_KP_MINUS:
                    case SDLK_MINUS:
                        minX = minX * 1.1;
                        maxX = maxX * 1.1;
                        minY = minY * 1.1;
                        maxY = maxY * 1.1;
                        break;
                    case SDLK_r:
                        minX = -10.0;
                        maxX = 10.0;
                        minY = -5.0;
                        maxY = 5.0;
                        break;
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    }
                }
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            drawGrid(renderer, minX, maxX, minY, maxY, SCREEN_WIDTH, SCREEN_HEIGHT);
            drawAxes(renderer, minX, maxX, minY, maxY, SCREEN_WIDTH, SCREEN_HEIGHT);

            SDL_Color functionColor = { 255, 0, 0, 255 };
            plotFunction(renderer, fn, minX, maxX, minY, maxY, SCREEN_WIDTH, SCREEN_HEIGHT, functionColor);

            SDL_RenderPresent(renderer);

            SDL_Delay(16);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}