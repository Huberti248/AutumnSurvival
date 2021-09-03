#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_net.h>
#include <SDL_ttf.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
//#include <SDL_gpu.h>
//#include <SFML/Network.hpp>
//#include <SFML/Graphics.hpp>
#include <algorithm>
#include <atomic>
#include <codecvt>
#include <functional>
#include <locale>
#include <mutex>
#include <thread>
#ifdef __ANDROID__
#include "vendor/PUGIXML/src/pugixml.hpp"
#include <android/log.h> //__android_log_print(ANDROID_LOG_VERBOSE, "AutumnLeafCollector", "Example number log: %d", number);
#include <jni.h>
#else
#include <filesystem>
#include <pugixml.hpp>
#ifdef __EMSCRIPTEN__
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif
using namespace std::chrono_literals;
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

// NOTE: Remember to uncomment it on every release
//#define RELEASE

#if defined _MSC_VER && defined RELEASE
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

//240 x 240 (smart watch)
//240 x 320 (QVGA)
//360 x 640 (Galaxy S5)
//640 x 480 (480i - Smallest PC monitor)

#define LEAF_INIT_SPAWN_DELAY_IN_MS 1000
#define LEAF_ROT_INIT_DELAY_IN_MS 5000

int windowWidth = 240;
int windowHeight = 320;
SDL_Point mousePos;
SDL_Point realMousePos;
bool keys[SDL_NUM_SCANCODES];
bool buttons[SDL_BUTTON_X2 + 1];
SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* robotoF;
bool running = true;

void logOutputCallback(void* userdata, int category, SDL_LogPriority priority, const char* message)
{
    std::cout << message << std::endl;
}

int random(int min, int max)
{
    return min + rand() % ((max + 1) - min);
}

int SDL_QueryTextureF(SDL_Texture* texture, Uint32* format, int* access, float* w, float* h)
{
    int wi, hi;
    int result = SDL_QueryTexture(texture, format, access, &wi, &hi);
    if (w) {
        *w = wi;
    }
    if (h) {
        *h = hi;
    }
    return result;
}

SDL_bool SDL_PointInFRect(const SDL_Point* p, const SDL_FRect* r)
{
    return ((p->x >= r->x) && (p->x < (r->x + r->w)) && (p->y >= r->y) && (p->y < (r->y + r->h))) ? SDL_TRUE : SDL_FALSE;
}

std::ostream& operator<<(std::ostream& os, SDL_FRect r)
{
    os << r.x << " " << r.y << " " << r.w << " " << r.h;
    return os;
}

std::ostream& operator<<(std::ostream& os, SDL_Rect r)
{
    SDL_FRect fR;
    fR.w = r.w;
    fR.h = r.h;
    fR.x = r.x;
    fR.y = r.y;
    os << fR;
    return os;
}

SDL_Texture* renderText(SDL_Texture* previousTexture, TTF_Font* font, SDL_Renderer* renderer, const std::string& text, SDL_Color color)
{
    if (previousTexture) {
        SDL_DestroyTexture(previousTexture);
    }
    SDL_Surface* surface;
    if (text.empty()) {
        surface = TTF_RenderUTF8_Blended(font, " ", color);
    }
    else {
        surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    }
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
    }
    else {
        return 0;
    }
}

struct Text {
    std::string text;
    SDL_Surface* surface = 0;
    SDL_Texture* t = 0;
    SDL_FRect dstR{};
    bool autoAdjustW = false;
    bool autoAdjustH = false;
    float wMultiplier = 1;
    float hMultiplier = 1;

    ~Text()
    {
        if (surface) {
            SDL_FreeSurface(surface);
        }
        if (t) {
            SDL_DestroyTexture(t);
        }
    }

    Text()
    {
    }

    Text(const Text& rightText)
    {
        text = rightText.text;
        if (surface) {
            SDL_FreeSurface(surface);
        }
        if (t) {
            SDL_DestroyTexture(t);
        }
        if (rightText.surface) {
            surface = SDL_ConvertSurface(rightText.surface, rightText.surface->format, SDL_SWSURFACE);
        }
        if (rightText.t) {
            t = SDL_CreateTextureFromSurface(renderer, surface);
        }
        dstR = rightText.dstR;
        autoAdjustW = rightText.autoAdjustW;
        autoAdjustH = rightText.autoAdjustH;
        wMultiplier = rightText.wMultiplier;
        hMultiplier = rightText.hMultiplier;
    }

    Text& operator=(const Text& rightText)
    {
        text = rightText.text;
        if (surface) {
            SDL_FreeSurface(surface);
        }
        if (t) {
            SDL_DestroyTexture(t);
        }
        if (rightText.surface) {
            surface = SDL_ConvertSurface(rightText.surface, rightText.surface->format, SDL_SWSURFACE);
        }
        if (rightText.t) {
            t = SDL_CreateTextureFromSurface(renderer, surface);
        }
        dstR = rightText.dstR;
        autoAdjustW = rightText.autoAdjustW;
        autoAdjustH = rightText.autoAdjustH;
        wMultiplier = rightText.wMultiplier;
        hMultiplier = rightText.hMultiplier;
        return *this;
    }

    void setText(SDL_Renderer* renderer, TTF_Font* font, std::string text, SDL_Color c = { 255, 255, 255 })
    {
        this->text = text;
#if 1 // NOTE: renderText
        if (surface) {
            SDL_FreeSurface(surface);
        }
        if (t) {
            SDL_DestroyTexture(t);
        }
        if (text.empty()) {
            surface = TTF_RenderUTF8_Blended(font, " ", c);
        }
        else {
            surface = TTF_RenderUTF8_Blended(font, text.c_str(), c);
        }
        if (surface) {
            t = SDL_CreateTextureFromSurface(renderer, surface);
        }
#endif
        if (autoAdjustW) {
            SDL_QueryTextureF(t, 0, 0, &dstR.w, 0);
        }
        if (autoAdjustH) {
            SDL_QueryTextureF(t, 0, 0, 0, &dstR.h);
        }
        dstR.w *= wMultiplier;
        dstR.h *= hMultiplier;
    }

    void setText(SDL_Renderer* renderer, TTF_Font* font, int value, SDL_Color c = { 255, 255, 255 })
    {
        setText(renderer, font, std::to_string(value), c);
    }

    void draw(SDL_Renderer* renderer)
    {
        if (t) {
            SDL_RenderCopyF(renderer, t, 0, &dstR);
        }
    }
};

int SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius - 1;
    status = 0;

    while (offsety >= offsetx) {
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2 * offsetx) {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

int SDL_RenderFillCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius - 1;
    status = 0;

    while (offsety >= offsetx) {

        status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
            x + offsety, y + offsetx);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
            x + offsetx, y + offsety);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
            x + offsetx, y - offsety);
        status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
            x + offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2 * offsetx) {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

struct Clock {
    Uint64 start = SDL_GetPerformanceCounter();

    float getElapsedTime()
    {
        Uint64 stop = SDL_GetPerformanceCounter();
        float secondsElapsed = (stop - start) / (float)SDL_GetPerformanceFrequency();
        return secondsElapsed * 1000;
    }

    float restart()
    {
        Uint64 stop = SDL_GetPerformanceCounter();
        float secondsElapsed = (stop - start) / (float)SDL_GetPerformanceFrequency();
        start = SDL_GetPerformanceCounter();
        return secondsElapsed * 1000;
    }
};

SDL_bool SDL_FRectEmpty(const SDL_FRect* r)
{
    return ((!r) || (r->w <= 0) || (r->h <= 0)) ? SDL_TRUE : SDL_FALSE;
}

SDL_bool SDL_IntersectFRect(const SDL_FRect* A, const SDL_FRect* B, SDL_FRect* result)
{
    int Amin, Amax, Bmin, Bmax;

    if (!A) {
        SDL_InvalidParamError("A");
        return SDL_FALSE;
    }

    if (!B) {
        SDL_InvalidParamError("B");
        return SDL_FALSE;
    }

    if (!result) {
        SDL_InvalidParamError("result");
        return SDL_FALSE;
    }

    /* Special cases for empty rects */
    if (SDL_FRectEmpty(A) || SDL_FRectEmpty(B)) {
        result->w = 0;
        result->h = 0;
        return SDL_FALSE;
    }

    /* Horizontal intersection */
    Amin = A->x;
    Amax = Amin + A->w;
    Bmin = B->x;
    Bmax = Bmin + B->w;
    if (Bmin > Amin)
        Amin = Bmin;
    result->x = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result->w = Amax - Amin;

    /* Vertical intersection */
    Amin = A->y;
    Amax = Amin + A->h;
    Bmin = B->y;
    Bmax = Bmin + B->h;
    if (Bmin > Amin)
        Amin = Bmin;
    result->y = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result->h = Amax - Amin;

    return (SDL_FRectEmpty(result) == SDL_TRUE) ? SDL_FALSE : SDL_TRUE;
}

SDL_bool SDL_HasIntersectionF(const SDL_FRect* A, const SDL_FRect* B)
{
    int Amin, Amax, Bmin, Bmax;

    if (!A) {
        SDL_InvalidParamError("A");
        return SDL_FALSE;
    }

    if (!B) {
        SDL_InvalidParamError("B");
        return SDL_FALSE;
    }

    /* Special cases for empty rects */
    if (SDL_FRectEmpty(A) || SDL_FRectEmpty(B)) {
        return SDL_FALSE;
    }

    /* Horizontal intersection */
    Amin = A->x;
    Amax = Amin + A->w;
    Bmin = B->x;
    Bmax = Bmin + B->w;
    if (Bmin > Amin)
        Amin = Bmin;
    if (Bmax < Amax)
        Amax = Bmax;
    if (Amax <= Amin)
        return SDL_FALSE;

    /* Vertical intersection */
    Amin = A->y;
    Amax = Amin + A->h;
    Bmin = B->y;
    Bmax = Bmin + B->h;
    if (Bmin > Amin)
        Amin = Bmin;
    if (Bmax < Amax)
        Amax = Bmax;
    if (Amax <= Amin)
        return SDL_FALSE;

    return SDL_TRUE;
}

int eventWatch(void* userdata, SDL_Event* event)
{
    // WARNING: Be very careful of what you do in the function, as it may run in a different thread
    if (event->type == SDL_APP_TERMINATING || event->type == SDL_APP_WILLENTERBACKGROUND) {
    }
    return 0;
}

enum class Direction {
    Left,
    Right,
};

struct Leaf {
    SDL_FRect r{};
    Direction direction = Direction::Right;
    SDL_FPoint offsetP{};
};

enum class State {
    Gameplay,
    Shop,
};

struct Shop {
    SDL_FRect treeR{};
    Text moreTreesText;
    Text moreTreesPriceText;
    SDL_FRect moreTreesBuyR;
    SDL_FRect leafR{};
    Text moreLeafsText;
    Text moreLeafsPriceText;
    SDL_FRect moreLeafsBuyR;
    SDL_FRect backArrowR{};
};

std::string prefPath;
SDL_Texture* leafT;
SDL_Texture* treeT;
SDL_Texture* tree1T;
SDL_Texture* tree2T;
SDL_Texture* tree3T;
SDL_Texture* shopT;
SDL_Texture* backArrowT;
SDL_Texture* buyT;
int currentTreeIndex = 0;
SDL_FRect treeR;
SDL_FRect tree2R;
SDL_FRect tree3R;
std::vector<Leaf> leafs;
Clock leafClock;
Clock globalClock;
Clock treeAnimationClock;
Text scoreText;
SDL_FRect shopR;
Shop shop;
State state = State::Gameplay;
int trees = 1;
int leafSpawnDelayInMs = LEAF_INIT_SPAWN_DELAY_IN_MS;
Clock leafRotClock;
int leafRotDelayInMs = LEAF_ROT_INIT_DELAY_IN_MS;

void saveData()
{
    pugi::xml_document doc;
    pugi::xml_node rootNode = doc.append_child("root");
    pugi::xml_node scoreNode = rootNode.append_child("score");
    scoreNode.append_child(pugi::node_pcdata).set_value(scoreText.text.c_str());
    pugi::xml_node treesNode = rootNode.append_child("trees");
    treesNode.append_child(pugi::node_pcdata).set_value(std::to_string(trees).c_str());
    pugi::xml_node leafSpawnDelayInMsNode = rootNode.append_child("leafSpawnDelayInMs");
    leafSpawnDelayInMsNode.append_child(pugi::node_pcdata).set_value(std::to_string(leafSpawnDelayInMs).c_str());
    doc.save_file((prefPath + "data.xml").c_str());
}

void readData()
{
    pugi::xml_document doc;
    doc.load_file((prefPath + "data.xml").c_str());
    pugi::xml_node rootNode = doc.child("root");
    scoreText.setText(renderer, robotoF, rootNode.child("score").text().as_int());
    trees = rootNode.child("trees").text().as_int(1);
    leafSpawnDelayInMs = rootNode.child("leafSpawnDelayInMs").text().as_int(LEAF_INIT_SPAWN_DELAY_IN_MS);
}

void mainLoop()
{
    float deltaTime = globalClock.restart();
    SDL_FRect windowR;
    windowR.w = windowWidth;
    windowR.h = windowHeight;
    windowR.x = 0;
    windowR.y = 0;
    if (state == State::Gameplay) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
                // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
            }
            if (event.type == SDL_KEYDOWN) {
                keys[event.key.keysym.scancode] = true;
            }
            if (event.type == SDL_KEYUP) {
                keys[event.key.keysym.scancode] = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                buttons[event.button.button] = true;
                for (int i = 0; i < leafs.size(); ++i) {
                    leafs[i].r.x += leafs[i].offsetP.x;
                    leafs[i].r.y += leafs[i].offsetP.y;
                    if (SDL_PointInFRect(&mousePos, &leafs[i].r)) {
                        leafs.erase(leafs.begin() + i--);
                        scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) + 1);
                    }
                    else {
                        leafs[i].r.x -= leafs[i].offsetP.x;
                        leafs[i].r.y -= leafs[i].offsetP.y;
                    }
                }
                if (SDL_PointInFRect(&mousePos, &shopR)) {
                    state = State::Shop;
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                buttons[event.button.button] = false;
            }
            if (event.type == SDL_MOUSEMOTION) {
                float scaleX, scaleY;
                SDL_RenderGetScale(renderer, &scaleX, &scaleY);
                mousePos.x = event.motion.x / scaleX;
                mousePos.y = event.motion.y / scaleY;
                realMousePos.x = event.motion.x;
                realMousePos.y = event.motion.y;
            }
        }
        if (leafClock.getElapsedTime() > leafSpawnDelayInMs) {
            leafs.push_back(Leaf());
            leafs.back().r.w = 32;
            leafs.back().r.h = 32;
            leafs.back().r.x = 0;
            leafs.back().r.y = 0;
            leafs.back().direction = (Direction)random(0, 1);
            leafs.back().offsetP.x = 100;
            leafs.back().offsetP.y = 230;
            if (trees > 1) {
                leafs.push_back(Leaf());
                leafs.back().r.w = 32;
                leafs.back().r.h = 32;
                leafs.back().r.x = 0;
                leafs.back().r.y = 0;
                leafs.back().direction = (Direction)random(0, 1);
                leafs.back().offsetP.x = 50;
                leafs.back().offsetP.y = 230;
            }
            if (trees > 2) {
                leafs.push_back(Leaf());
                leafs.back().r.w = 32;
                leafs.back().r.h = 32;
                leafs.back().r.x = 0;
                leafs.back().r.y = 0;
                leafs.back().direction = (Direction)random(0, 1);
                leafs.back().offsetP.x = 150;
                leafs.back().offsetP.y = 230;
            }
            leafClock.restart();
        }
        for (int i = 0; i < leafs.size(); ++i) {
            if (leafs[i].direction == Direction::Right) {
                leafs[i].r.x += 0.01 * deltaTime;
            }
            else if (leafs[i].direction == Direction::Left) {
                leafs[i].r.x += -0.01 * deltaTime;
            }
            leafs[i].r.y = std::pow(leafs[i].r.x, 2) / 28;
        }
        if (leafRotClock.getElapsedTime() > leafRotDelayInMs) {
            if (std::stoi(scoreText.text) > 0) {
                scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) - 1);
            }
            leafRotClock.restart();
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        if (currentTreeIndex == 0) {
            SDL_RenderCopyF(renderer, treeT, 0, &treeR);
        }
        else if (currentTreeIndex == 1) {
            SDL_RenderCopyF(renderer, tree1T, 0, &treeR);
        }
        else if (currentTreeIndex == 2) {
            SDL_RenderCopyF(renderer, tree2T, 0, &treeR);
        }
        else if (currentTreeIndex == 3) {
            SDL_RenderCopyF(renderer, tree3T, 0, &treeR);
        }
        if (treeAnimationClock.getElapsedTime() > 1000) {
            if (++currentTreeIndex >= 4) {
                currentTreeIndex = 0;
            }
            treeAnimationClock.restart();
        }
        if (trees > 1) {
            if (currentTreeIndex == 0) {
                SDL_RenderCopyF(renderer, treeT, 0, &tree2R);
            }
            else if (currentTreeIndex == 1) {
                SDL_RenderCopyF(renderer, tree1T, 0, &tree2R);
            }
            else if (currentTreeIndex == 2) {
                SDL_RenderCopyF(renderer, tree2T, 0, &tree2R);
            }
            else if (currentTreeIndex == 3) {
                SDL_RenderCopyF(renderer, tree3T, 0, &tree2R);
            }
        }
        if (trees > 2) {
            if (currentTreeIndex == 0) {
                SDL_RenderCopyF(renderer, treeT, 0, &tree3R);
            }
            else if (currentTreeIndex == 1) {
                SDL_RenderCopyF(renderer, tree1T, 0, &tree3R);
            }
            else if (currentTreeIndex == 2) {
                SDL_RenderCopyF(renderer, tree2T, 0, &tree3R);
            }
            else if (currentTreeIndex == 3) {
                SDL_RenderCopyF(renderer, tree3T, 0, &tree3R);
            }
        }
        for (int i = 0; i < leafs.size(); ++i) {
            leafs[i].r.x += leafs[i].offsetP.x;
            leafs[i].r.y += leafs[i].offsetP.y;
            SDL_RenderCopyF(renderer, leafT, 0, &leafs[i].r);
            leafs[i].r.x -= leafs[i].offsetP.x;
            leafs[i].r.y -= leafs[i].offsetP.y;
        }
        scoreText.draw(renderer);
        SDL_RenderCopyF(renderer, shopT, 0, &shopR);
        SDL_RenderPresent(renderer);
    }
    else if (state == State::Shop) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
                // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
            }
            if (event.type == SDL_KEYDOWN) {
                keys[event.key.keysym.scancode] = true;
            }
            if (event.type == SDL_KEYUP) {
                keys[event.key.keysym.scancode] = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                buttons[event.button.button] = true;
                if (SDL_PointInFRect(&mousePos, &shop.backArrowR)) {
                    state = State::Gameplay;
                }
                if (SDL_PointInFRect(&mousePos, &shop.moreTreesBuyR) && trees != 3) {
                    if (std::stoi(scoreText.text) >= std::stoi(shop.moreTreesPriceText.text)) {
                        scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) - std::stoi(shop.moreTreesPriceText.text));
                        ++trees;
                        if (trees == 2) {
                            shop.moreTreesPriceText.setText(renderer, robotoF, 200);
                        }
                    }
                }
                if (SDL_PointInFRect(&mousePos, &shop.moreLeafsBuyR) && leafSpawnDelayInMs > 100) {
                    if (std::stoi(scoreText.text) >= std::stoi(shop.moreLeafsPriceText.text)) {
                        scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) - std::stoi(shop.moreLeafsPriceText.text));
                        if (leafSpawnDelayInMs > 100) {
                            leafSpawnDelayInMs -= 100;
                        }
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                buttons[event.button.button] = false;
            }
            if (event.type == SDL_MOUSEMOTION) {
                float scaleX, scaleY;
                SDL_RenderGetScale(renderer, &scaleX, &scaleY);
                mousePos.x = event.motion.x / scaleX;
                mousePos.y = event.motion.y / scaleY;
                realMousePos.x = event.motion.x;
                realMousePos.y = event.motion.y;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        if (trees != 3) {
            SDL_RenderCopyF(renderer, treeT, 0, &shop.treeR);
            shop.moreTreesText.draw(renderer);
            shop.moreTreesPriceText.draw(renderer);
            SDL_RenderCopyF(renderer, buyT, 0, &shop.moreTreesBuyR);
        }
        if (leafSpawnDelayInMs > 100) {
            SDL_RenderCopyF(renderer, leafT, 0, &shop.leafR);
            shop.moreLeafsText.draw(renderer);
            shop.moreLeafsPriceText.draw(renderer);
            SDL_RenderCopyF(renderer, buyT, 0, &shop.moreLeafsBuyR);
        }
        SDL_RenderCopyF(renderer, backArrowT, 0, &shop.backArrowR);
        SDL_RenderPresent(renderer);
    }
    saveData();
}

int main(int argc, char* argv[])
{
    std::srand(std::time(0));
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    SDL_LogSetOutputFunction(logOutputCallback, 0);
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    window = SDL_CreateWindow("AutumnLeafCollector", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    robotoF = TTF_OpenFont("res/roboto.ttf", 72);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_RenderSetScale(renderer, w / (float)windowWidth, h / (float)windowHeight);
    SDL_AddEventWatch(eventWatch, 0);
    prefPath = SDL_GetPrefPath("NextCodeApps", "AutumnLeafCollector");
    leafT = IMG_LoadTexture(renderer, "res/leaves.png");
    treeT = IMG_LoadTexture(renderer, "res/tree.png");
    tree1T = IMG_LoadTexture(renderer, "res/tree1.png");
    tree2T = IMG_LoadTexture(renderer, "res/tree2.png");
    tree3T = IMG_LoadTexture(renderer, "res/tree3.png");
    shopT = IMG_LoadTexture(renderer, "res/shop.png");
    backArrowT = IMG_LoadTexture(renderer, "res/backArrow.png");
    buyT = IMG_LoadTexture(renderer, "res/buy.png");
    treeR.w = 32;
    treeR.h = 32;
    treeR.x = windowWidth / 2 - treeR.w / 2;
    treeR.y = windowHeight - treeR.h - 50;
    tree2R.w = 32;
    tree2R.h = 32;
    tree2R.x = treeR.x - tree2R.w - 20;
    tree2R.y = windowHeight - treeR.h - 50;
    tree3R.w = 32;
    tree3R.h = 32;
    tree3R.x = treeR.x + treeR.w + 20;
    tree3R.y = windowHeight - treeR.h - 50;
    scoreText.setText(renderer, robotoF, 0);
    scoreText.dstR.w = 50;
    scoreText.dstR.h = 30;
    scoreText.dstR.x = windowWidth / 2 - scoreText.dstR.w / 2;
    scoreText.dstR.y = 0;
    shopR.w = 64;
    shopR.h = 64;
    shopR.x = windowWidth - shopR.w;
    shopR.y = 5;
    shop.backArrowR.w = 32;
    shop.backArrowR.h = 32;
    shop.backArrowR.x = 5;
    shop.backArrowR.y = 5;
    shop.treeR.w = 32;
    shop.treeR.h = 32;
    shop.treeR.x = shop.backArrowR.x + shop.backArrowR.w + 5;
    shop.treeR.y = 15;
    shop.moreTreesText.setText(renderer, robotoF, "More trees");
    shop.moreTreesText.dstR.w = 70;
    shop.moreTreesText.dstR.h = 35;
    shop.moreTreesText.dstR.x = shop.treeR.x;
    shop.moreTreesText.dstR.y = shop.treeR.y + shop.treeR.h;
    shop.moreTreesPriceText.setText(renderer, robotoF, 100);
    shop.moreTreesPriceText.dstR.w = 70;
    shop.moreTreesPriceText.dstR.h = 35;
    shop.moreTreesPriceText.dstR.x = shop.moreTreesText.dstR.x;
    shop.moreTreesPriceText.dstR.y = shop.moreTreesText.dstR.y + shop.moreTreesText.dstR.h;
    shop.moreTreesBuyR.w = 45;
    shop.moreTreesBuyR.h = 32;
    shop.moreTreesBuyR.x = shop.moreTreesPriceText.dstR.x;
    shop.moreTreesBuyR.y = shop.moreTreesPriceText.dstR.y + shop.moreTreesPriceText.dstR.h;
    shop.leafR.w = 32;
    shop.leafR.h = 32;
    shop.leafR.x = shop.moreTreesText.dstR.x + shop.moreTreesText.dstR.w + 15;
    shop.leafR.y = 15;
    shop.moreLeafsText.setText(renderer, robotoF, "More leafs");
    shop.moreLeafsText.dstR.w = 70;
    shop.moreLeafsText.dstR.h = 35;
    shop.moreLeafsText.dstR.x = shop.leafR.x;
    shop.moreLeafsText.dstR.y = shop.leafR.y + shop.leafR.h;
    shop.moreLeafsPriceText.setText(renderer, robotoF, 100);
    shop.moreLeafsPriceText.dstR.w = 70;
    shop.moreLeafsPriceText.dstR.h = 35;
    shop.moreLeafsPriceText.dstR.x = shop.moreLeafsText.dstR.x;
    shop.moreLeafsPriceText.dstR.y = shop.moreLeafsText.dstR.y + shop.moreLeafsText.dstR.h;
    shop.moreLeafsBuyR.w = 45;
    shop.moreLeafsBuyR.h = 32;
    shop.moreLeafsBuyR.x = shop.moreLeafsPriceText.dstR.x;
    shop.moreLeafsBuyR.y = shop.moreLeafsPriceText.dstR.y + shop.moreLeafsPriceText.dstR.h;
    readData();
    leafClock.restart();
    globalClock.restart();
    treeAnimationClock.restart();
    leafRotClock.restart();
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoop, 0, 1);
#else
    while (running) {
        mainLoop();
    }
#endif
    // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
    return 0;
}