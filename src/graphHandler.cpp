#include "graphHandler.hpp"

#include <fmt/core.h>


namespace graph{
    int mapX(double x, double minX, double maxX, int screenWidth) {
        return static_cast<int>((x - minX) / (maxX - minX) * screenWidth);
    }

    int mapY(double y, double minY, double maxY, int screenHeight) {
        return static_cast<int>(screenHeight - ((y - minY) / (maxY - minY) * screenHeight));
    }


    double calculateStepSize(double range) {
        double power = std::pow(10, floor(log10(range)));

        if (range / power < 2) {
            return power / 5;
        }
        else if (range / power < 5) {
            return power / 2;
        }
        else {
            return power;
        }
    }

    void drawAxes(SDL_Renderer* renderer, double minX, double maxX, double minY, double maxY,
        int screenWidth, int screenHeight, const char* fontPath) {
        
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        if (minY <= 0 && maxY >= 0) {
            int y0 = mapY(0, minY, maxY, screenHeight);
            SDL_RenderDrawLine(renderer, 0, y0, screenWidth, y0);
        }
        if (minX <= 0 && maxX >= 0) {
            int x0 = mapX(0, minX, maxX, screenWidth);
            SDL_RenderDrawLine(renderer, x0, 0, x0, screenHeight);
        }

        
        double xRange = maxX - minX;
        double yRange = maxY - minY;

        
        int xPrecision = 0; 
        int yPrecision = 0;

        
        if (xRange < 10) xPrecision = 1;
        if (xRange < 1) xPrecision = 2;
        if (xRange < 0.1) xPrecision = 3;
        if (xRange < 0.01) xPrecision = 4;
        if (yRange < 0.001) yPrecision = 5;
        if (yRange < 0.0001) yPrecision = 6;


        
        if (yRange < 10) yPrecision = 1;
        if (yRange < 1) yPrecision = 2;
        if (yRange < 0.1) yPrecision = 3;
        if (yRange < 0.01) yPrecision = 4;
        if (yRange < 0.001) yPrecision = 5;
        if (yRange < 0.0001) yPrecision = 6;


        
        double xStep = calculateStepSize(xRange);
        double yStep = calculateStepSize(yRange);
        double xStart = std::ceil(minX / xStep) * xStep;
        double yStart = std::ceil(minY / yStep) * yStep;

        
        TTF_Font* font = TTF_OpenFont(fontPath, 10);
        SDL_Color textColor = { 255, 255, 255, 255 };

        
        for (double x = xStart; x <= maxX; x += xStep) {
            if (fabs(x) < 1e-10) continue; 
            int screenX = mapX(x, minX, maxX, screenWidth);
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            int axisY = (minY <= 0 && maxY >= 0) ? mapY(0, minY, maxY, screenHeight) : screenHeight - 20;
            SDL_RenderDrawLine(renderer, screenX, axisY - 5, screenX, axisY + 5);

            
            std::string label = fmt::format("{0:.{1}f}", x, xPrecision);


            if (font) {
                SDL_Surface* surface = TTF_RenderText_Solid(font, label.c_str(), textColor);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    if (texture) {
                        SDL_Rect dest = { screenX - surface->w / 2, axisY + 8, surface->w, surface->h };
                        SDL_RenderCopy(renderer, texture, NULL, &dest);
                        SDL_DestroyTexture(texture);
                    }
                    SDL_FreeSurface(surface);
                }
            }
        }

        
        for (double y = yStart; y <= maxY; y += yStep) {
            if (fabs(y) < 1e-10) continue; 
            int screenY = mapY(y, minY, maxY, screenHeight);
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            int axisX = (minX <= 0 && maxX >= 0) ? mapX(0, minX, maxX, screenWidth) : 32;
            SDL_RenderDrawLine(renderer, axisX - 5, screenY, axisX + 5, screenY);

            
            std::string label = fmt::format("{0:.{1}f}", y, yPrecision);


            if (font) {
                SDL_Surface* surface = TTF_RenderText_Solid(font, label.c_str(), textColor);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    if (texture) {
                        SDL_Rect dest = { axisX - surface->w - 8, screenY - surface->h / 2, surface->w, surface->h };
                        SDL_RenderCopy(renderer, texture, NULL, &dest);
                        SDL_DestroyTexture(texture);
                    }
                    SDL_FreeSurface(surface);
                }
            }
        }

        
        if (font) {
            TTF_CloseFont(font);
        }
    }



    void drawGrid(SDL_Renderer* renderer, double minX, double maxX, double minY, double maxY,
        int screenWidth, int screenHeight) {
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);

        
        double xRange = maxX - minX;
        double yRange = maxY - minY;

        
        double xStep = calculateStepSize(xRange);
        double yStep = calculateStepSize(yRange);

        
        double xStart = std::ceil(minX / xStep) * xStep;
        double yStart = std::ceil(minY / yStep) * yStep;

        
        for (double x = xStart; x <= maxX; x += xStep) {
            
            if (fabs(x) < 1e-10) continue;

            int screenX = mapX(x, minX, maxX, screenWidth);
            SDL_RenderDrawLine(renderer, screenX, 0, screenX, screenHeight);
        }

        
        for (double y = yStart; y <= maxY; y += yStep) {
            
            if (fabs(y) < 1e-10) continue;

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
}