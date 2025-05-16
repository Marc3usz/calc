#pragma once

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include "common.hpp"


namespace graph {

    void plotFunction(SDL_Renderer*, Function&,
        double, double, double, double,
        int, int, SDL_Color);

    int mapY(double, double, double, int);

    int mapX(double, double, double, int);

    double calculateStepSize(double);

    void drawAxes(SDL_Renderer*, double, double, double, double,
        int, int, const char* fontPath = "C:\\Windows\\Fonts\\arial.ttf");

    void drawGrid(SDL_Renderer*, double, double, double, double,
        int, int);

}

