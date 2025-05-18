#define SDL_MAIN_HANDLED

#include "graphHandler.hpp"
#include "functionFactory.hpp"
#include "cli.hpp"
#include "fileHandler.hpp"
#include "getZeroes.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <fmt/core.h>


#define MESSAGE "Calc\nm: toggle help\n\n<arrows>: navigate\n+/-: zoom\nr: reset view\n\n1-6: toggle function definition\n<shift>1-6: edit function definiton\n<esc>: exit edit mode\na: show all functions\n\n<shift>s: save\n<ctrl><shift>s: export roots\n\n<shift><esc>: exit"

#ifdef __WIN32__
#define ENTRYPOINT int WinMain()
#define CliHandlerDef CliHandler cli(__argc, __argv)
#else
#define ENTRYPOINT int main(int argc, char** argv)
#define CliHandlerDef CliHandler cli(argc, argv)
#endif


ENTRYPOINT{
    CliHandlerDef;
    const int SCREEN_WIDTH = cli.width();
    const int SCREEN_HEIGHT = cli.height();
    const std::string FONT_PATH = cli.fontFilePath();
    const std::optional<std::string> LOAD_PATH = cli.loadPath();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    TTF_Init();

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

    TTF_Font* font = TTF_OpenFont(FONT_PATH.c_str(), 20);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    bool showMenu = true;
    functionMapping fs = {
        {"sin", [](ld x) {return std::sin(x); }},
        {"cos", [](ld x) {return std::cos(x); }},
        {"log", [](ld x) {return std::log(x); }},
        {"exp", [](ld x) {return std::exp(x); }},
        {"logtwo", [](ld x) {return std::log2(x); }},
        {"tan", [](ld x) {return std::tan(x); }},
        {"pi", [](ld x) {return M_PI * x; }}
    };

    try {
        double minX = -10.0;
        double maxX = 10.0;
        double minY = -5.0;
        double maxY = 5.0;
        bool editing = false;
        char toDisplay = '\0';
        bool showAllFunctions = false;
        char editingFunctionId = '\0';
        std::string currentInput = "";
        std::string errorMessage = "";
        bool showError = false;
        int errorDisplayTime = 0;
        std::string statusMessage = "";
        int statusDisplayTime = 0;


        std::array<SDL_Color, 6> functionColors = {
            SDL_Color{255, 0, 0, 255},
            SDL_Color{0, 255, 0, 255},
            SDL_Color{0, 128, 255, 255},
            SDL_Color{255, 255, 0, 255},
            SDL_Color{255, 0, 255, 255},
            SDL_Color{0, 255, 255, 255}
        };

        std::cout << "cam";

        FunctionFactory fns = [&]() {
            if (LOAD_PATH.has_value()) {
                std::vector<std::string> loaded = fileHandler::loadFunctions(LOAD_PATH.value());
                return FunctionFactory(fs, loaded);
            }
            else {
                return FunctionFactory(fs);
            }
            }();

        bool quit = false;
        SDL_Event e;

        char lastEditingFunctionId = '\0';

        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
                else if (e.type == SDL_KEYDOWN && editing) {

                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        editing = false;

                        editingFunctionId = '\0';
                        currentInput = "";
                        SDL_StopTextInput();

                        showError = false;
                        errorMessage = "";
                    }
                    else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {

                        if (!currentInput.empty()) {
                            try {

                                fns.parseFunction(currentInput, editingFunctionId);


                                editing = false;
                                editingFunctionId = '\0';
                                currentInput = "";
                                SDL_StopTextInput();


                                showError = false;
                                errorMessage = "";
                            }
                            catch (const std::exception& ex) {

                                errorMessage = ex.what();
                                showError = true;
                                errorDisplayTime = 120;
                                currentInput = "";

                            }
                        }
                    }
                    else if (e.key.keysym.sym == SDLK_BACKSPACE && !currentInput.empty()) {

                        currentInput.pop_back();

                        showError = false;
                    }
                }
                else if (e.type == SDL_TEXTINPUT && editing) {

                    currentInput += e.text.text;

                    showError = false;
                }
                else if (e.type == SDL_KEYDOWN && !editing) {
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
                    case SDLK_a:

                        showAllFunctions = !showAllFunctions;
                        if (showAllFunctions) {
                            toDisplay = '\0';
                        }
                        break;
                    case SDLK_s:
                        if ((e.key.keysym.mod & KMOD_CTRL) && (e.key.keysym.mod & KMOD_SHIFT)) {
                            try {
                                std::vector<std::string> all_roots_formatted_lines;
                                const functionMapping& current_functions = fns.getFunctions();
                                bool functions_found_for_export = false;

                                for (const auto& pair : current_functions) {
                                    if (pair.first.length() == 1 && pair.first[0] >= 'a' && pair.first[0] <= 'f') {
                                        functions_found_for_export = true;
                                        char functionIdChar = pair.first[0];
                                        const auto& func_to_eval = pair.second;
                                        std::vector<ld> roots = getZeroes(func_to_eval);

                                        all_roots_formatted_lines.push_back(std::string(1, functionIdChar) + ":");
                                        for (ld root_val : roots) {
                                            all_roots_formatted_lines.push_back(fmt::format("{}", root_val));
                                        }
                                    }
                                }

                                if (functions_found_for_export) {
                                    fileHandler::saveFile(all_roots_formatted_lines, "roots.txt");
                                    statusMessage = "Roots exported to roots.txt";
                                }
                                else {
                                    statusMessage = "No functions (a-f) to export roots for.";
                                }
                                statusDisplayTime = 120;

                            }
                            catch (const std::exception& ex) {
                                    statusMessage = "Error exporting roots: ";
                                    statusMessage += ex.what();
                                    statusDisplayTime = 180;
                            }
}
                        else if (e.key.keysym.mod & KMOD_SHIFT) {
                            try {
                                std::vector<std::string> functionsToSave = fns.exportFunctions();
                                std::string savePath = "functions.txt";
                                fileHandler::saveFile(functionsToSave, savePath);
                                statusMessage = "Functions saved to " + savePath;
                                statusDisplayTime = 120;
                            }
                            catch (const std::exception& ex) { 
                                statusMessage = "Error saving functions: ";
                                statusMessage += ex.what();
                                statusDisplayTime = 180;
                            }
                        }
                        break;
                    case SDLK_ESCAPE:
                        if (e.key.keysym.mod & KMOD_SHIFT)
                            quit = true;
                        break;
                    case SDLK_1:
                    case SDLK_2:
                    case SDLK_3:
                    case SDLK_4:
                    case SDLK_5:
                    case SDLK_6:
                    {
                        showAllFunctions = false;
                        char functionId = 'a' + (e.key.keysym.sym - SDLK_1);
                        std::string fnKey(1, functionId);

                        if (e.key.keysym.mod & KMOD_SHIFT) {
                            editing = true;

                            if (editingFunctionId != functionId) {
                                showError = false;
                                errorMessage = "";
                            }

                            editingFunctionId = functionId;

                            if (!showAllFunctions) {
                                toDisplay = functionId;
                            }


                            std::vector<std::string> expressions = fns.exportFunctions();
                            currentInput = "";


                            for (const auto& expr : expressions) {
                                if (!expr.empty() && expr[0] == functionId) {

                                    currentInput = expr.substr(1);
                                    break;
                                }
                            }


                            SDL_StartTextInput();
                        }
                        else if (!showAllFunctions) {

                            showAllFunctions = false;

                            if (toDisplay == functionId) {
                                toDisplay = '\0';
                            }
                            else {
                                toDisplay = functionId;
                            }
                        }
                    }
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
            graph::drawAxes(renderer, minX, maxX, minY, maxY, SCREEN_WIDTH, SCREEN_HEIGHT, FONT_PATH.c_str());


            const functionMapping& functions = fns.getFunctions();

            if (toDisplay == '\0') {

                for (const auto& pair : functions) {
                    if (pair.first.size() == 1) {
                        char functionId = pair.first[0];
                        if (functionId >= 'a' && functionId <= 'f') {
                            int colorIndex = functionId - 'a';

                            auto fn = pair.second;
                            graph::plotFunction(renderer, fn, minX, maxX, minY, maxY,
                                SCREEN_WIDTH, SCREEN_HEIGHT, functionColors[colorIndex]);
                        }
                    }
                }
            }
            else {

                std::string fnKey(1, toDisplay);
                auto it = functions.find(fnKey);
                if (it != functions.end()) {
                    int colorIndex = toDisplay - 'a';
                    auto fn = it->second;
                    graph::plotFunction(renderer, fn, minX, maxX, minY, maxY,
                        SCREEN_WIDTH, SCREEN_HEIGHT, functionColors[colorIndex]);
                }
            }

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

                std::vector<std::string> expressions = fns.exportFunctions();

                if (showAllFunctions) {

                    int yPos = SCREEN_HEIGHT - 10;




                    for (auto it = expressions.rbegin(); it != expressions.rend(); ++it) {
                        const auto& expr = *it;
                        if (!expr.empty() && expr[0] >= 'a' && expr[0] <= 'f') {
                            char functionId = expr[0];


                            std::string fnKey(1, functionId);
                            if (functions.find(fnKey) != functions.end()) {
                                int colorIndex = functionId - 'a';
                                SDL_Color textColor = functionColors[colorIndex];

                                std::string displayText = fmt::format("{}(x) = {}", functionId, expr.substr(1));
                                SDL_Surface* exprSurface = TTF_RenderText_Blended(font, displayText.c_str(), textColor);
                                if (exprSurface) {
                                    SDL_Texture* exprTexture = SDL_CreateTextureFromSurface(renderer, exprSurface);
                                    if (exprTexture) {
                                        SDL_Rect exprRect = {
                                            10,
                                            yPos - exprSurface->h,
                                            exprSurface->w,
                                            exprSurface->h
                                        };
                                        SDL_RenderCopy(renderer, exprTexture, NULL, &exprRect);
                                        SDL_DestroyTexture(exprTexture);
                                    }
                                    yPos -= exprSurface->h + 5;
                                    SDL_FreeSurface(exprSurface);
                                }
                            }
                        }
                    }
                }
                else {

                    SDL_Color textColor = { 255, 255, 255, 255 };
                    std::string displayText;

                    if (editing) {

                        int colorIndex = editingFunctionId - 'a';
                        textColor = functionColors[colorIndex];
                        displayText = fmt::format("{}(x) = {} _", editingFunctionId, currentInput);
                    }
                    else if (toDisplay != '\0') {

                        std::string fnKey(1, toDisplay);
                        if (functions.find(fnKey) != functions.end()) {
                            int colorIndex = toDisplay - 'a';
                            textColor = functionColors[colorIndex];


                            std::string expressionText;
                            for (const auto& expr : expressions) {
                                if (!expr.empty() && expr[0] == toDisplay) {
                                    expressionText = expr;
                                    break;
                                }
                            }

                            if (!expressionText.empty()) {
                                displayText = fmt::format("{}(x) = {}", expressionText[0], expressionText.substr(1));
                            }
                        }
                    }

                    if (!displayText.empty()) {
                        SDL_Surface* exprSurface = TTF_RenderText_Blended(font, displayText.c_str(), textColor);
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
                }

                if (statusDisplayTime > 0) {
                    SDL_Color statusColor = { 100, 255, 100, 255 };
                    SDL_Surface* statusSurface = TTF_RenderText_Blended(font, statusMessage.c_str(), statusColor);
                    if (statusSurface) {
                        SDL_Texture* statusTexture = SDL_CreateTextureFromSurface(renderer, statusSurface);
                        if (statusTexture) {
                            SDL_Rect statusRect = {
                                (SCREEN_WIDTH - statusSurface->w) / 2,
                                20,
                                statusSurface->w,
                                statusSurface->h
                            };
                            SDL_RenderCopy(renderer, statusTexture, NULL, &statusRect);
                            SDL_DestroyTexture(statusTexture);
                        }
                        SDL_FreeSurface(statusSurface);
                    }

                    statusDisplayTime--;
                }

                if (editing) {
                    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 200);
                    SDL_Rect editBox = {
                        0,
                        SCREEN_HEIGHT - 40,
                        SCREEN_WIDTH,
                        40
                    };
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_RenderFillRect(renderer, &editBox);
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

                    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
                    SDL_RenderDrawRect(renderer, &editBox);


                    if (showError && !errorMessage.empty()) {
                        SDL_Color errorColor = { 255, 100, 100, 255 };
                        std::string errorText = fmt::format("Error: {}", errorMessage);
                        SDL_Surface* errorSurface = TTF_RenderText_Blended(font, errorText.c_str(), errorColor);
                        if (errorSurface) {
                            SDL_Texture* errorTexture = SDL_CreateTextureFromSurface(renderer, errorSurface);
                            if (errorTexture) {
                                SDL_Rect errorRect = {
                                    10,
                                    SCREEN_HEIGHT - 80,
                                    errorSurface->w,
                                    errorSurface->h
                                };
                                SDL_RenderCopy(renderer, errorTexture, NULL, &errorRect);
                                SDL_DestroyTexture(errorTexture);
                            }
                            SDL_FreeSurface(errorSurface);
                        }


                        if (errorDisplayTime > 0) {
                            errorDisplayTime--;
                            if (errorDisplayTime == 0) {
                                showError = false;
                            }
                        }
                    }
                }
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(16);


            lastEditingFunctionId = editingFunctionId;
        }


        if (SDL_IsTextInputActive()) {
            SDL_StopTextInput();
        }
    }
    catch (std::exception& ex_outer) {

        std::cerr << "Error: " << ex_outer.what() << std::endl;


        if (SDL_IsTextInputActive()) {
            SDL_StopTextInput();
        }
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