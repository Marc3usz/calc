#define SDL_MAIN_HANDLED

#include "graphHandler.hpp"
#include "functionFactory.hpp"

#include <vector>
#include <iostream>
#include <sstream>
#include <string>

#define MESSAGE "Calc\nm: toggle help\n\n<arrows>: navigate\n+/-: zoom\nr: reset view\n\n1-6: toggle function definition\n<shift>1-6: edit function definiton\n<esc>: exit edit mode\n\n<shift>s: save\n<ctrl><shift>s: export roots\n\n<shift><esc>: exit"

int WinMain() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    TTF_Init();

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

        TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 20);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }

        bool showMenu = true;
    functionMapping fs = {
        {"sin", [](ld x) {return std::sin(x); }},
        {"cos", [](ld x) {return std::cos(x); }}
    };

    try {
        double minX = -10.0;
        double maxX = 10.0;
        double minY = -5.0;
        double maxY = 5.0;

        std::vector<std::string> expressions{ "a2^x" };
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

                    double rangeX = maxX - minX;
                    double rangeY = maxY - minY;
                    double centerX = (minX + maxX) / 2.0;
                    double centerY = (minY + maxY) / 2.0;

                    switch (e.key.keysym.sym) {
                    case SDLK_m:
                                                showMenu = !showMenu;
                        break;
                    case SDLK_UP:
                        minY += rangeY * 0.05;
                        maxY += rangeY * 0.05;
                        break;
                    case SDLK_DOWN:
                        minY -= rangeY * 0.05;
                        maxY -= rangeY * 0.05;
                        break;
                    case SDLK_LEFT:
                        minX -= rangeX * 0.05;
                        maxX -= rangeX * 0.05;
                        break;
                    case SDLK_RIGHT:
                        minX += rangeX * 0.05;
                        maxX += rangeX * 0.05;
                        break;
                    case SDLK_KP_PLUS:
                    case SDLK_EQUALS:
                    {
                        double newRangeX = rangeX * 0.9;
                        double newRangeY = rangeY * 0.9;
                        minX = centerX - newRangeX / 2.0;
                        maxX = centerX + newRangeX / 2.0;
                        minY = centerY - newRangeY / 2.0;
                        maxY = centerY + newRangeY / 2.0;
                    }
                    break;
                    case SDLK_KP_MINUS:
                    case SDLK_MINUS:
                    {
                        double newRangeX = rangeX * 1.1;
                        double newRangeY = rangeY * 1.1;
                        minX = centerX - newRangeX / 2.0;
                        maxX = centerX + newRangeX / 2.0;
                        minY = centerY - newRangeY / 2.0;
                        maxY = centerY + newRangeY / 2.0;
                    }
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

                    if (maxX <= minX) {
                        double midX = (minX + maxX) / 2.0;
                        minX = midX - 0.1;
                        maxX = midX + 0.1;
                    }

                    if (maxY <= minY) {
                        double midY = (minY + maxY) / 2.0;
                        minY = midY - 0.1;
                        maxY = midY + 0.1;
                    }
                }
            }

                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

                        graph::drawGrid(renderer, minX, maxX, minY, maxY, SCREEN_WIDTH, SCREEN_HEIGHT);
            graph::drawAxes(renderer, minX, maxX, minY, maxY, SCREEN_WIDTH, SCREEN_HEIGHT);

            SDL_Color functionColor = { 255, 0, 0, 255 };
            graph::plotFunction(renderer, fn, minX, maxX, minY, maxY, SCREEN_WIDTH, SCREEN_HEIGHT, functionColor);

                        if (showMenu && font) {
                                std::vector<std::string> menuLines;
                std::istringstream stream(MESSAGE);
                std::string line;

                while (std::getline(stream, line, '\n')) {
                    menuLines.push_back(line);
                }

                                int lineHeight = TTF_FontHeight(font);
                int menuHeight = lineHeight * menuLines.size() + 30; 
                                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
                SDL_Rect menuBackground = { 20, 20, SCREEN_WIDTH - 40, menuHeight };
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(renderer, &menuBackground);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

                                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &menuBackground);

                                SDL_Color textColor = { 255, 255, 255, 255 };
                for (size_t i = 0; i < menuLines.size(); i++) {
                    SDL_Surface* textSurface = TTF_RenderText_Blended(font, menuLines[i].c_str(), textColor);
                    if (textSurface) {
                        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                        if (textTexture) {
                            SDL_Rect textRect = {
                                30,
                                35 + static_cast<int>(i) * lineHeight,
                                textSurface->w,
                                textSurface->h
                            };
                            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                            SDL_DestroyTexture(textTexture);
                        }
                        SDL_FreeSurface(textSurface);
                    }
                }
            }

                        if (font) {
                SDL_Color textColor = { 255, 255, 255, 255 };
                std::string expressionText = "f(x) = " + expressions[0];
                SDL_Surface* exprSurface = TTF_RenderText_Blended(font, expressionText.c_str(), textColor);
                if (exprSurface) {
                    SDL_Texture* exprTexture = SDL_CreateTextureFromSurface(renderer, exprSurface);
                    if (exprTexture) {
                        SDL_Rect exprRect = {
                            10,
                            SCREEN_HEIGHT - exprSurface->h - 10,
                            exprSurface->w,
                            exprSurface->h
                        };
                        SDL_RenderCopy(renderer, exprTexture, NULL, &exprRect);
                        SDL_DestroyTexture(exprTexture);
                    }
                    SDL_FreeSurface(exprSurface);
                }
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

        if (font) {
        TTF_CloseFont(font);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
