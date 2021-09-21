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
#include <array>
#ifdef __ANDROID__
#include "vendor/PUGIXML/src/pugixml.hpp"
#include <android/log.h> //__android_log_print(ANDROID_LOG_VERBOSE, "AutumnSurvival", "Example number log: %d", number);
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

#define ROT_INIT_DELAY_IN_MS 5000
#define GRAPE_INIT_SPAWN_DELAY_IN_MS 10000
#define APPLE_INIT_SPAWN_DELAY_IN_MS 5000
#define BANANA_INIT_SPAWN_DELAY_IN_MS 3000
#define CARROT_INIT_SPAWN_DELAY_IN_MS 5000
#define POTATO_INIT_SPAWN_DELAY_IN_MS 7000
#define PUMPKIN_INIT_SPAWN_DELAY_IN_MS 3000
#define SUNSET_HOUR_BEGIN 7
#define SUNSET_HOUR_END 17
#define MAX_ENERGY_INIT 10
#define INIT_PLAYER_SPEED 0.1
#define MAX_TREES 3
#define SCREEN_PADDING 50
#define EPSILON 0.001
#define BROWN_COLOR 140, 56, 4, 255
#define WARNING_COLOR 255, 0, 0, 255
#define PART_SPEED 0.1
#define HUNGER_DECREASE_DELAY_IN_MS 1000
#define HUNGER_INIT 100

#define BUTTON_SELECTED   \
    {                     \
        242, 182, 61, 255 \
    }
#define BUTTON_UNSELECTED  \
    {                      \
        209, 180, 140, 255 \
    }
#define BUTTON_HIGHLIGHTED \
    {                      \
        212, 110, 51, 255  \
    }
#define LETTER_WIDTH 25
#define PAUSE_NUM_OPTIONS 3
#define PAUSE_MENU_BUTTON_PADDING 15
#define MAINMENU_NUM_OPTIONS 4
#define MAINMENU_BUTTON_PADDING 15
#define MAX_SICKNESS_SCORE 10
#define ENERGY_SICK_FACTOR 2
#define HUNGER_SICK_FACTOR 3

int windowWidth = 800;
int windowHeight = 600;
SDL_Point mousePos;
SDL_Point realMousePos;
bool keys[SDL_NUM_SCANCODES];
bool buttons[SDL_BUTTON_X2 + 1];
SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* robotoF;
bool running = true;

std::string prefPath;
SDL_Texture* leavesT;
SDL_Texture* treeT;
SDL_Texture* tree1T;
SDL_Texture* tree2T;
SDL_Texture* tree3T;
SDL_Texture* shopT;
SDL_Texture* backArrowT;
SDL_Texture* buyT;
SDL_Texture* appleWithSprayerT;
SDL_Texture* vineT;
SDL_Texture* grapeT;
SDL_Texture* appleT;
SDL_Texture* appleTreeT;
SDL_Texture* bananaT;
SDL_Texture* bananaTreeT;
SDL_Texture* carrotT;
SDL_Texture* soilT;
SDL_Texture* mutedT;
SDL_Texture* soundT;
SDL_Texture* leafT;
SDL_Texture* sunT;
SDL_Texture* moonT;
SDL_Texture* energyT;
SDL_Texture* rotT;
SDL_Texture* moreEnergyT;
SDL_Texture* houseflyT;
SDL_Texture* wormT;
SDL_Texture* potatoT;
SDL_Texture* pumpkinT;
SDL_Texture* pumpkinTreeT;
SDL_Texture* tradeT;
SDL_Texture* playerT;
SDL_Texture* houseT;
SDL_Texture* collectT;
SDL_Texture* drawT;
SDL_Texture* openT;
SDL_Texture* xT;
SDL_Texture* bedT;
SDL_Texture* lightT;
SDL_Texture* bgLayerT;
SDL_Texture* lightLayerT;
SDL_Texture* resultLayerT;
SDL_Texture* currentPartT;
SDL_Texture* doorT;
SDL_Texture* chestT;
SDL_Texture* windowT;
SDL_Texture* hungerT;
SDL_Texture* carrotSoilT;
SDL_Texture* potatoSoilT;
SDL_Texture* sickT;
SDL_Texture* sellT;
SDL_Texture* circleT;
SDL_Texture* redCircleT;
Mix_Music* josephKosmaM;
Mix_Music* antonioVivaldiM;
Mix_Chunk* doorS;
Mix_Chunk* pickupS;
Mix_Chunk* powerupS;
Mix_Chunk* sleepS;

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
    float Amin, Amax, Bmin, Bmax;

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

enum class EntityType {
    Leaf,
    Apple,
    Grape,
    Banana,
    Carrot,
    Potato,
    Pumpkin
};

enum class Food {
    Empty = -1,
    Apple,
    Carrot,
    Grape,
    Potato,
    Pumpkin,
    Banana,
    NumFood
};

struct Entity {
    SDL_FRect r{};
    Direction direction = Direction::Right;
    SDL_FPoint offsetP{};
    EntityType entityType = EntityType::Leaf;
};

struct Player {
    SDL_FRect r{};
    int dx = 0;
    int dy = 0;
};

SDL_Texture* GetFoodTexture(Food type)
{
    switch (type) {
    case Food::Apple:
        return appleT;
    case Food::Banana:
        return bananaT;
    case Food::Carrot:
        return carrotT;
    case Food::Grape:
        return grapeT;
    case Food::Potato:
        return potatoT;
    case Food::Pumpkin:
        return pumpkinT;
    default:
        return NULL;
    }
}

struct Plot {
    SDL_FRect r;
    Food food{};
    int numProduce = 1;
    std::vector<SDL_FRect> producesR;

    Plot()
    {
        r.w = 200;
        r.h = 130;
        SDL_FRect a{};
        a.w = 64;
        a.h = 64;
        producesR.push_back(a);
    }

    void setFood(Food type)
    {
        food = type;
    }

    SDL_Texture* getPlotTexture()
    {
        switch (food) {
        case Food::Apple:
            return appleTreeT;
        case Food::Banana:
            return bananaTreeT;
        case Food::Carrot:
            return carrotSoilT;
        case Food::Grape:
            return vineT;
        case Food::Potato:
            return potatoSoilT;
        case Food::Pumpkin:
            return pumpkinTreeT;
        default:
            return appleTreeT;
        }
    }

    void renderPlot()
    {
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 200);
        SDL_RenderFillRectF(renderer, &r);
        for (int i = 0; i < numProduce; ++i) {

            producesR[i].x = r.x + (r.w / (numProduce + 1) * (i + 1)) - producesR[i].w / 2.0f;
            producesR[i].y = r.y + r.h / 2.0f - producesR[i].h / 2.0f;

            SDL_RenderCopyF(renderer, getPlotTexture(), 0, &producesR[i]);
        }
    }

    bool isIntersecting(const SDL_FRect A)
    {
        for (int i = 0; i < producesR.size(); ++i) {
            if (SDL_HasIntersectionF(&A, &producesR[i])) {
                return true;
            }
        }

        return false;
    }
};

struct Interactable {
    SDL_FRect dstR;
    std::string actionText;
    float textLength;

    void setActionText(const std::string text)
    {
        actionText = text;
        textLength = LETTER_WIDTH * text.length();
    }
};

enum class State {
    Main,
    Outside,
    Shop,
    Intro,
    Home,
    Minigame,
    Gameover,
    Storage,
    Paused,
    Credits,
    Controls
};

struct SellItem {
    SDL_FRect imageR{};
    Text countText;
    SDL_FRect sellR{};
    Text gainText;
    SDL_Texture* foodT = 0;
};

struct Shop {
    SDL_FRect lessRotR{};
    Text lessRotText;
    Text lessRotPriceText;
    SDL_FRect lessRotBuyR{};

    SDL_FRect moreEnergyR{};
    Text moreEnergyText;
    Text moreEnergyPriceText;
    SDL_FRect moreEnergyBuyR{};

    SDL_FRect backArrowR{};

    std::vector<SellItem> sellItems;
};

enum class Music {
    JosephKosma,
    AntonioVivaldi,
};

enum class Motion {
    Left,
    Right,
};

struct Intro {
    Clock introClock;
    SDL_FRect leafR{};
    Motion leafMotion = Motion::Right;
    int leafAngle = 0;
};

struct Part {
    SDL_FRect dstR{};
};

struct Tile {
    SDL_FRect dstR{};
    SDL_Rect srcR{};
    std::string source;
    int id = 0;
};

SDL_Texture* getT(SDL_Renderer* renderer, std::string name)
{
    static std::unordered_map<std::string, SDL_Texture*> map;
    auto it = map.find(name);
    if (it == map.end()) {
        SDL_Texture* t = IMG_LoadTexture(renderer, ("res/" + name).c_str());
        map.insert({ name, t });
        return t;
    }
    else {
        return it->second;
    }
}

struct StorageUI {
    SDL_FRect container{};
    SDL_FRect imgContainer{};
    Text amountText;
    int amount = 0;
    bool isChest;
    Food foodType;
    bool selected = false;

    void init(SDL_FRect r, Food foodType, bool isChest = true)
    {
        container = r;

        imgContainer.w = container.w * 0.9f;
        imgContainer.h = container.h * 0.9f;
        imgContainer.x = container.x + container.w / 2.0f - imgContainer.w / 2.0f;
        imgContainer.y = container.y + container.h / 2.0f - imgContainer.h / 2.0f;

        this->amountText.dstR.w = container.w * 0.25f;
        this->amountText.dstR.h = container.h * 0.25f;
        this->amountText.dstR.x = container.x + container.w - this->amountText.dstR.w;
        this->amountText.dstR.y = container.y + container.h - this->amountText.dstR.h;

        this->foodType = foodType;
        this->isChest = isChest;
    }

    void draw(SDL_Renderer* renderer)
    {

        if (selected) {
            SDL_SetRenderDrawColor(renderer, 212, 110, 51, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRectF(renderer, &container);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRectF(renderer, &container);
        }
        if (foodType != Food::Empty) {
            SDL_RenderCopyF(renderer, GetFoodTexture(foodType), 0, &imgContainer);
            std::string textAmount = "X" + std::to_string(amount);
            this->amountText.setText(renderer, robotoF, textAmount);
            this->amountText.draw(renderer);
        }
    }

    std::string getFoodName()
    {
        switch (foodType) {
        case Food::Apple:
            return "Apple";
        case Food::Banana:
            return "Banana";
        case Food::Carrot:
            return "Carrot";
        case Food::Grape:
            return "Grape";
        case Food::Potato:
            return "Potato";
        case Food::Pumpkin:
            return "Pumpkin";
        default:
            return "";
        }
    }
};

enum class MenuOption {
    Play = 1,
    Resume,
    Restart,
    Controls,
    Highscores,
    Main,
    Credits,
    Quit
};

struct MenuButton {
    MenuOption menuType;
    Text buttonText;
    std::string label;
    bool selected;

    void calculateButtonPosition(
        const int index,
        const int numButtons,
        const float width,
        const float height,
        float paddingVertical)
    {

        buttonText.dstR.x = width / 2.0f - buttonText.dstR.w / 2.0f;
        buttonText.dstR.y = height / 2.0f - numButtons * (buttonText.dstR.h / 2.0f + paddingVertical) + (index + 0.5f) * (buttonText.dstR.h + paddingVertical);
    }
};

void HandleMenuOption(MenuOption option, State& gameState, State& pausedState, Intro& intro, bool& gameRunning)
{
    switch (option) {
    case MenuOption::Play:
        gameState = State::Outside;
        break;
    case MenuOption::Credits:
        gameState = State::Credits;
        break;
    case MenuOption::Resume:
        gameState = pausedState;
        break;
    case MenuOption::Controls:
        break;
    case MenuOption::Main:
        gameState = State::Main;
        break;
    case MenuOption::Quit:
        gameRunning = false;
        break;
    }
}

enum class PlayerDirection {
    Left,
    Right,
    Up,
    Down,
};

void muteMusicAndSounds()
{
    Mix_VolumeMusic(0);
    Mix_Volume(-1, 0);
}

void unmuteMusicAndSounds()
{
    Mix_VolumeMusic(128);
    Mix_Volume(-1, 128);
}

void saveData(Text scoreText, int rotDelayInMs, bool isMuted, int maxEnergy, std::array<Food, 2>& foods, std::vector<StorageUI>& storage, Text& hungerText)
{
#ifdef __EMSCRIPTEN__
    EM_ASM({
        localStorage.setItem("score", $0);
    },
        std::stoi(scoreText.text));
    EM_ASM({
        localStorage.setItem("rotDelayInMs", $0);
    },
        rotDelayInMs);
    EM_ASM({
        localStorage.setItem("isMuted", $0);
    },
        isMuted);
    EM_ASM({
        localStorage.setItem("maxEnergy", $0);
    },
        maxEnergy);
    EM_ASM({
        localStorage.setItem("inventory", $0);
    },
        (int)foods[0]);
    EM_ASM({
        localStorage.setItem("inventory2", $0);
    },
        (int)foods[1]);
    for (int i = 0; i < storage.size(); ++i) {
        EM_ASM({
            localStorage.setItem("storage" + $1, $0);
        },
            storage[i].amount, i);
    }
    EM_ASM({
        localStorage.setItem("hunger", $0);
    },
        std::stoi(hungerText.text));
#else
    pugi::xml_document doc;
    pugi::xml_node rootNode = doc.append_child("root");
    pugi::xml_node scoreNode = rootNode.append_child("score");
    scoreNode.append_child(pugi::node_pcdata).set_value(scoreText.text.c_str());
    pugi::xml_node rotDelayInMsNode = rootNode.append_child("rotDelayInMs");
    rotDelayInMsNode.append_child(pugi::node_pcdata).set_value(std::to_string(rotDelayInMs).c_str());
    pugi::xml_node isMutedNode = rootNode.append_child("isMuted");
    isMutedNode.append_child(pugi::node_pcdata).set_value(std::to_string(isMuted).c_str());
    pugi::xml_node maxEnergyNode = rootNode.append_child("maxEnergy");
    maxEnergyNode.append_child(pugi::node_pcdata).set_value(std::to_string(maxEnergy).c_str());
    pugi::xml_node inventoryNode = rootNode.append_child("inventory");
    inventoryNode.append_child(pugi::node_pcdata).set_value(std::to_string((int)foods[0]).c_str());
    pugi::xml_node inventory2Node = rootNode.append_child("inventory2");
    inventory2Node.append_child(pugi::node_pcdata).set_value(std::to_string((int)foods[1]).c_str());
    for (int i = 0; i < storage.size(); ++i) {
        pugi::xml_node storageNode = rootNode.append_child(("storage" + std::to_string(i)).c_str());
        storageNode.append_child(pugi::node_pcdata).set_value(std::to_string(storage[i].amount).c_str());
    }
    pugi::xml_node hungerNode = rootNode.append_child("hunger");
    hungerNode.append_child(pugi::node_pcdata).set_value(hungerText.text.c_str());
    doc.save_file((prefPath + "data.xml").c_str());
#endif
}

void readData(Text& scoreText, int& rotDelayInMs, bool& isMuted, int& maxEnergy, Text& energyText, std::array<Food, 2>& foods, std::vector<StorageUI>& storage, Text& hungerText)
{
#ifdef __EMSCRIPTEN__
    scoreText.setText(renderer, robotoF,
        EM_ASM_INT({
            var s = localStorage.getItem("score");
            if (s) {
                return s;
            }
            else {
                return $0;
            }
        },
            0),
        {});
    rotDelayInMs = EM_ASM_INT({
        var s = localStorage.getItem("leafRotDelayInMs");
        if (s) {
            return s;
        }
        else {
            return $0;
        }
    },
        ROT_INIT_DELAY_IN_MS);
    isMuted = EM_ASM_INT({
        var s = localStorage.getItem("isMuted");
        if (s) {
            return s;
        }
        else {
            return $0;
        }
    },
        0);
    if (isMuted) {
        muteMusicAndSounds();
    }
    maxEnergy = EM_ASM_INT({
        var s = localStorage.getItem("maxEnergy");
        if (s) {
            return s;
        }
        else {
            return $0;
        }
    },
        MAX_ENERGY_INIT);
    foods[0] = (Food)EM_ASM_INT({
        var s = localStorage.getItem("inventory");
        if (s) {
            return s;
        }
        else {
            return $0;
        }
    },
        (int)Food::Empty);
    foods[1] = (Food)EM_ASM_INT({
        var s = localStorage.getItem("inventory2");
        if (s) {
            return s;
        }
        else {
            return $0;
        }
    },
        (int)Food::Empty);
    for (int i = 0; i < storage.size(); ++i) {
        storage[i].amount = EM_ASM_INT({
            var s = localStorage.getItem("storage" + $1);
            if (s) {
                return s;
            }
            else {
                return $0;
            }
        },
            0, i);
    }
    hungerText.setText(renderer, robotoF, EM_ASM_INT({
        var s = localStorage.getItem("hunger");
        if (s) {
            return s;
        }
        else {
            return $0;
        }
    },
                                              HUNGER_INIT));
#else
    pugi::xml_document doc;
    doc.load_file((prefPath + "data.xml").c_str());
    pugi::xml_node rootNode = doc.child("root");
    scoreText.setText(renderer, robotoF, rootNode.child("score").text().as_int());
    rotDelayInMs = rootNode.child("leafRotDelayInMs").text().as_int(ROT_INIT_DELAY_IN_MS);
    isMuted = rootNode.child("isMuted").text().as_bool();
    if (isMuted) {
        muteMusicAndSounds();
    }
    maxEnergy = rootNode.child("maxEnergy").text().as_int(MAX_ENERGY_INIT);
    energyText.setText(renderer, robotoF, maxEnergy);
    foods[0] = (Food)rootNode.child("inventory").text().as_int((int)Food::Empty);
    foods[1] = (Food)rootNode.child("inventory2").text().as_int((int)Food::Empty);
    for (int i = 0; i < storage.size(); ++i) {
        storage[i].amount = rootNode.child(("storage" + std::to_string(i)).c_str()).text().as_int();
    }
    hungerText.setText(renderer, robotoF, rootNode.child("hunger").text().as_int(HUNGER_INIT));
#endif
}

float clamp(float n, float lower, float upper)
{
    return std::max(lower, std::min(n, upper));
}

float lerp(float a, float b, float t)
{
    return (a + t * (b - a));
}

void RenderScreen(std::vector<Plot> plots)
{
    for (auto& plot : plots) {
        plot.renderPlot();
    }
}

void DrawInventory(SDL_FRect inventorySlotR, SDL_FRect inventorySlot2R, std::array<Food, 2> foods, SDL_FRect inventorySlotXR, SDL_FRect inventorySlotX2R)
{
    SDL_SetRenderDrawColor(renderer, BROWN_COLOR);
    SDL_RenderFillRectF(renderer, &inventorySlotR);
    SDL_RenderFillRectF(renderer, &inventorySlot2R);
    if (foods[0] == Food::Apple) {
        SDL_RenderCopyF(renderer, appleT, 0, &inventorySlotR);
    }
    else if (foods[0] == Food::Potato) {
        SDL_RenderCopyF(renderer, potatoT, 0, &inventorySlotR);
    }
    else if (foods[0] == Food::Banana) {
        SDL_RenderCopyF(renderer, bananaT, 0, &inventorySlotR);
    }
    else if (foods[0] == Food::Carrot) {
        SDL_RenderCopyF(renderer, carrotT, 0, &inventorySlotR);
    }
    else if (foods[0] == Food::Grape) {
        SDL_RenderCopyF(renderer, grapeT, 0, &inventorySlotR);
    }
    else if (foods[0] == Food::Pumpkin) {
        SDL_RenderCopyF(renderer, pumpkinT, 0, &inventorySlotR);
    }
    if (foods[1] == Food::Apple) {
        SDL_RenderCopyF(renderer, appleT, 0, &inventorySlot2R);
    }
    else if (foods[1] == Food::Potato) {
        SDL_RenderCopyF(renderer, potatoT, 0, &inventorySlot2R);
    }
    else if (foods[1] == Food::Banana) {
        SDL_RenderCopyF(renderer, bananaT, 0, &inventorySlot2R);
    }
    else if (foods[1] == Food::Carrot) {
        SDL_RenderCopyF(renderer, carrotT, 0, &inventorySlot2R);
    }
    else if (foods[1] == Food::Grape) {
        SDL_RenderCopyF(renderer, grapeT, 0, &inventorySlot2R);
    }
    else if (foods[1] == Food::Pumpkin) {
        SDL_RenderCopyF(renderer, pumpkinT, 0, &inventorySlot2R);
    }
    if (foods[0] != Food::Empty) {
        SDL_RenderCopyF(renderer, xT, 0, &inventorySlotXR);
    }
    if (foods[1] != Food::Empty) {
        SDL_RenderCopyF(renderer, xT, 0, &inventorySlotX2R);
    }
}

void RenderUI(Text scoreText,
    bool isMuted,
    SDL_FRect soundBtnR,
    Text hourText,
    Text energyText,
    int hour,
    SDL_FRect sunR,
    SDL_FRect energyR,
    PlayerDirection playerDirection,
    std::vector<SDL_Rect> playerAnimationFrames,
    int playerAnimationFrame,
    Player player,
    bool isMoving,
    Clock& playerAnimationClock,
    Text hungerText,
    Text sickText,
    int sickLevel,
    SDL_FRect inventorySlotR,
    SDL_FRect inventorySlot2R,
    std::array<Food, 2> foods,
    SDL_FRect inventorySlotXR,
    SDL_FRect inventorySlotX2R)
{
    scoreText.draw(renderer);
    if (isMuted) {
        SDL_RenderCopyF(renderer, mutedT, 0, &soundBtnR);
    }
    else {
        SDL_RenderCopyF(renderer, soundT, 0, &soundBtnR);
    }
    hourText.draw(renderer);
    if (hour >= 7 && hour <= 17) {
        SDL_RenderCopyF(renderer, sunT, 0, &sunR);
    }
    else {
        SDL_RenderCopyF(renderer, moonT, 0, &sunR);
    }
    energyText.draw(renderer);
    SDL_RenderCopyF(renderer, energyT, 0, &energyR);
    DrawInventory(inventorySlotR, inventorySlot2R, foods, inventorySlotXR, inventorySlotX2R);
    if (playerDirection == PlayerDirection::Left) {
        SDL_RenderCopyExF(renderer, playerT, &playerAnimationFrames[playerAnimationFrame], &player.r, 0, 0, SDL_FLIP_HORIZONTAL);
    }
    else if (playerDirection == PlayerDirection::Right) {
        SDL_RenderCopyExF(renderer, playerT, &playerAnimationFrames[playerAnimationFrame], &player.r, 0, 0, SDL_FLIP_NONE);
    }
    else if (playerDirection == PlayerDirection::Up) {
        SDL_RenderCopyExF(renderer, playerT, &playerAnimationFrames[playerAnimationFrame], &player.r, 270, 0, SDL_FLIP_NONE);
    }
    else if (playerDirection == PlayerDirection::Down) {
        SDL_RenderCopyExF(renderer, playerT, &playerAnimationFrames[playerAnimationFrame], &player.r, 90, 0, SDL_FLIP_NONE);
    }
    if (isMoving) {
        if (playerAnimationClock.getElapsedTime() > 500) {
            ++playerAnimationFrame;
            playerAnimationClock.restart();
        }
    }
    else {
        playerAnimationFrame = 0;
    }
    if (playerAnimationFrame > 7) {
        playerAnimationFrame = 0;
    }
    hungerText.draw(renderer);
    SDL_FRect hungerR;
    hungerR.w = 32;
    hungerR.h = 32;
    hungerR.x = hungerText.dstR.x - hungerR.w;
    hungerR.y = hungerText.dstR.y + hungerText.dstR.h / 2 - hungerR.h / 2;
    SDL_RenderCopyF(renderer, hungerT, 0, &hungerR);

    if (sickLevel > 0) {
        sickText.setText(renderer, robotoF, "x" + std::to_string(sickLevel));
        sickText.draw(renderer);
        SDL_FRect sickR;
        sickR.w = 32;
        sickR.h = 32;
        sickR.x = sickText.dstR.x - sickR.w;
        sickR.y = sickText.dstR.y + sickText.dstR.h / 2.0f - sickR.h / 2.0f;
        SDL_RenderCopyF(renderer, sickT, 0, &sickR);
    }
}

int ClosestNumber(int total, int size)
{
    int quotient = total / size;
    int closest = quotient * size;

    int nextClosest = (total * size) > 0 ? (size * (quotient + 1)) : (size * (quotient - 1));
    if (abs(total - closest) < abs(total - nextClosest)) {
        return closest;
    }

    return nextClosest;
}

void RenderInstructions(Text& controlsText, std::string controlsWords, Text& keyControlsText, std::string keyControlsWords)
{
    controlsText.setText(renderer, robotoF, controlsWords);
    controlsText.dstR.w = controlsText.text.length() * LETTER_WIDTH;
    keyControlsText.setText(renderer, robotoF, keyControlsWords, { 255, 0, 0 });
    keyControlsText.dstR.w = keyControlsText.text.length() * LETTER_WIDTH;
    keyControlsText.dstR.x = windowWidth / 2.0f - (controlsText.dstR.w + keyControlsText.dstR.w) / 2.0f;
    controlsText.dstR.x = keyControlsText.dstR.x + keyControlsText.dstR.w;

    controlsText.draw(renderer);
    keyControlsText.draw(renderer);

    controlsText.dstR.y -= controlsText.dstR.h;
    keyControlsText.dstR.y = controlsText.dstR.y;
}

void HomeInit(SDL_FRect& tile,
    SDL_FRect& homeGround,
    SDL_FRect& homeWall,
    SDL_FRect& homeSeparator,
    SDL_FRect& windowR,
    Interactable& doorI,
    Interactable& shopI,
    Player& player,
    Interactable& bedI,
    Interactable& chestI,
    Text& actionText)
{
    tile.w = 32;
    tile.h = 32;
    homeGround.w = ClosestNumber(windowWidth * 0.8f, tile.w);
    homeGround.h = ClosestNumber(windowHeight * 0.8f, tile.h);
    homeGround.x = windowWidth / 2.0f - homeGround.w / 2.0f;
    homeGround.y = windowHeight / 2.0f - homeGround.h / 2.0f;
    homeWall = homeGround;
    homeWall.h = tile.h * 4;
    homeSeparator = homeWall;
    homeSeparator.h = tile.h / 3.0f;
    homeSeparator.y += homeWall.h - homeSeparator.h;
    homeGround.h -= homeWall.h;
    homeGround.y = windowHeight / 2.0f - homeGround.h / 2.0f;

    windowR.w = 128;
    windowR.h = tile.h * 2.0;
    windowR.x = windowWidth / 2.0f - windowR.w / 2.0f;
    windowR.y = homeWall.y + homeWall.h / 2.0f - windowR.h / 2.0f;

    doorI.setActionText("Go back to Farm");
    doorI.dstR.w = player.r.h + 20;
    doorI.dstR.h = doorI.dstR.w;
    doorI.dstR.x = windowWidth / 2.0f - doorI.dstR.w / 2.0f;
    doorI.dstR.y = homeGround.y + homeGround.h - doorI.dstR.h;

    shopI.setActionText("Shop");
    shopI.dstR.w = 96;
    shopI.dstR.h = 96;
    shopI.dstR.x = homeGround.x + homeGround.w - shopI.dstR.w;
    shopI.dstR.y = homeGround.y + homeGround.h / 2.0f - shopI.dstR.h / 2.0f;

    bedI.setActionText("Sleep");
    bedI.dstR.w = 128;
    bedI.dstR.h = 128;
    bedI.dstR.x = homeGround.x;
    bedI.dstR.y = homeGround.y;

    chestI.setActionText("View Inventory");
    chestI.dstR.w = 64;
    chestI.dstR.h = 64;
    chestI.dstR.x = homeGround.w / 2.0f - chestI.dstR.w / 2.0f;
    chestI.dstR.y = homeGround.h / 2.0f - chestI.dstR.h / 2.0f;

    actionText.setText(renderer, robotoF, "");
    actionText.dstR.h = 50;
    actionText.dstR.y = windowHeight - actionText.dstR.h - SCREEN_PADDING;
}

void RenderHome(SDL_FRect homeGround,
    SDL_FRect tile,
    SDL_FRect homeWall,
    SDL_FRect homeSeparator,
    int hour,
    SDL_FRect windowR,
    Interactable& bedI,
    Interactable& doorI,
    Interactable& shopI,
    Interactable& chestI,
    Text& actionText,
    Interactable& currentAction,
    bool& shouldShowInfoText,
    Text& infoTextAboutNotEnoughEnergy,
    bool& shouldShowWhenCanSleepAndGoShop,
    Text& canSleepAndGoShopText,
    Text scoreText,
    bool isMuted,
    SDL_FRect soundBtnR,
    Text hourText,
    Text energyText,
    SDL_FRect sunR,
    SDL_FRect energyR,
    PlayerDirection playerDirection,
    std::vector<SDL_Rect> playerAnimationFrames,
    int playerAnimationFrame,
    Player player,
    bool isMoving,
    Clock& playerAnimationClock,
    Text hungerText,
    Text sickText,
    int sickLevel,
    SDL_FRect inventorySlotR,
    SDL_FRect inventorySlot2R,
    std::array<Food, 2> foods,
    SDL_FRect inventorySlotXR,
    SDL_FRect inventorySlotX2R,
    Text& controlsText,
    Text& keyControlsText)
{
    // Create home map
    SDL_SetRenderDrawColor(renderer, 209, 180, 140, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRectF(renderer, &homeGround);

    float xPos = homeGround.x + homeGround.w - tile.w;
    int rowTile = 0;
    int colTile = 0;
    float yPos = homeGround.y + homeGround.h - tile.h;
    bool tiled = false;
    SDL_SetRenderDrawColor(renderer, 255, 244, 224, SDL_ALPHA_OPAQUE);
    while (!tiled) {
        bool toTile = false;
        if (colTile % 2 == 0) {
            if (rowTile % 2 != 0) {
                toTile = true;
            }
        }
        else {
            if (rowTile % 2 == 0) {
                toTile = true;
            }
        }

        if (toTile) {
            tile.x = xPos;
            tile.y = yPos;
            SDL_RenderFillRectF(renderer, &tile);
        }

        if (++rowTile == (homeGround.w / tile.w)) {
            if (++colTile == (homeGround.h / tile.h)) {
                tiled = true;
            }
            else {
                rowTile = 0;
                yPos -= tile.h;
                xPos = homeGround.x + homeGround.w - tile.w;
            }
        }
        else {
            xPos -= tile.w;
        }
    }
    SDL_SetRenderDrawColor(renderer, 143, 204, 203, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRectF(renderer, &homeWall);
    SDL_SetRenderDrawColor(renderer, 44, 27, 46, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRectF(renderer, &homeSeparator);

    if (hour >= 7 && hour <= 17) {
        SDL_SetRenderDrawColor(renderer, 184, 222, 236, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 7, 11, 52, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderFillRectF(renderer, &windowR);
    SDL_RenderCopyF(renderer, windowT, 0, &windowR);

    //Initialize interactables.
    SDL_RenderCopyF(renderer, bedT, 0, &bedI.dstR);
    SDL_RenderCopyF(renderer, doorT, 0, &doorI.dstR);
    SDL_RenderCopyF(renderer, shopT, 0, &shopI.dstR);
    SDL_RenderCopyF(renderer, chestT, 0, &chestI.dstR);

    if (currentAction.actionText != "") {
        RenderInstructions(controlsText, currentAction.actionText, keyControlsText, "SPACE: ");
    }

    RenderUI(scoreText, isMuted, soundBtnR, hourText, energyText, hour, sunR, energyR, playerDirection, playerAnimationFrames, playerAnimationFrame, player, isMoving, playerAnimationClock, hungerText, sickText, sickLevel, inventorySlotR, inventorySlot2R, foods, inventorySlotXR, inventorySlotX2R);
    if (shouldShowInfoText) {
        infoTextAboutNotEnoughEnergy.draw(renderer);
    }
    if (shouldShowWhenCanSleepAndGoShop) {
        canSleepAndGoShopText.draw(renderer);
    }
}

void StorageInit(SDL_FRect& container,
    Text& inventoryTitleText,
    Text& titleText,
    Text& hungerText,
    std::vector<StorageUI>& storage,
    std::vector<SDL_FRect>& storageInventory)
{
    container.w = windowWidth * 0.8f;
    container.h = windowHeight * 0.8f;
    container.x = windowWidth / 2.0f - container.w / 2.0f;
    container.y = windowHeight / 2.0f - container.h / 2.0f;

    hungerText.dstR.h = 50;
    hungerText.dstR.x = windowWidth / 2.0f - hungerText.dstR.w / 2.0f;
    hungerText.dstR.y = container.y - hungerText.dstR.h;

    titleText.setText(renderer, robotoF, "Storage", { 255, 0, 0 });
    titleText.dstR.w = clamp(titleText.text.length() * LETTER_WIDTH, 0, container.w * 0.8f);
    titleText.dstR.h = 50;
    titleText.dstR.x = windowWidth / 2.0f - titleText.dstR.w / 2.0f;
    titleText.dstR.y = container.y + 10;

    inventoryTitleText.setText(renderer, robotoF, "Inventory", { 255, 0, 0 });
    inventoryTitleText.dstR.w = clamp(inventoryTitleText.text.length() * LETTER_WIDTH, 0, container.w * 0.8f);
    inventoryTitleText.dstR.h = 50;
    inventoryTitleText.dstR.x = windowWidth / 2.0f - inventoryTitleText.dstR.w / 2.0f;

    int rows = 2;
    int cols = 3;
    SDL_FRect tempPos{};
    tempPos.w = (container.w - (cols + 1) * 25) / cols;
    tempPos.h = (container.h - (rows + 4) * 10 - inventoryTitleText.dstR.h - titleText.dstR.h) / (rows + 1);
    tempPos.x = container.x + 25;
    tempPos.y = titleText.dstR.y + titleText.dstR.h + 10;
    for (int i = 0; i < static_cast<int>(Food::NumFood); i++) {
        StorageUI item;
        SDL_FRect pos = tempPos;
        pos.x = tempPos.x + (tempPos.w + 25) * (i % cols);
        pos.y = tempPos.y + (tempPos.h + 10) * (i % rows);
        item.init(pos, static_cast<Food>(i));
        storage.push_back(item);
    }

    inventoryTitleText.dstR.y = tempPos.y + ((tempPos.h + 10) * rows);
    tempPos.x = container.x + container.w / 2.0f - 5 - tempPos.w;
    tempPos.y = inventoryTitleText.dstR.y + 10 + inventoryTitleText.dstR.h;
    for (int i = 0; i < 2; i++) {
        SDL_FRect pos = tempPos;
        pos.x = tempPos.x + (tempPos.w + 25) * (i % 2);
        storageInventory.push_back(pos);
    }
}

void RenderStorage(SDL_FRect& container,
    Text& hoverText,
    Text& titleText,
    Text& hungerText,
    std::string hungerAmount,
    std::vector<StorageUI>& storage,
    std::vector<SDL_FRect>& storageInventory,
    std::array<Food, 2>& foods,
    std::array<bool, 2>& foodSelected)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRectF(renderer, &container);
    titleText.draw(renderer);
    hoverText.draw(renderer);

    hungerText.setText(renderer, robotoF, "Hunger: " + hungerAmount);
    hungerText.dstR.w = hungerText.text.length() * LETTER_WIDTH;
    hungerText.dstR.x = windowWidth / 2.0f - hungerText.dstR.w / 2.0f;
    hungerText.draw(renderer);

    for (auto& s : storage) {
        s.draw(renderer);
    }
    for (int i = 0; i < storageInventory.size(); i++) {
        if (foodSelected[i]) {
            SDL_SetRenderDrawColor(renderer, 212, 110, 51, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRectF(renderer, &storageInventory[i]);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRectF(renderer, &storageInventory[i]);
        }
        if (foods[i] != Food::Empty) {
            SDL_FRect imgContainer;
            imgContainer.w = storageInventory[i].w * 0.9f;
            imgContainer.h = storageInventory[i].h * 0.9f;
            imgContainer.x = storageInventory[i].x + storageInventory[i].w / 2.0f - imgContainer.w / 2.0f;
            imgContainer.y = storageInventory[i].y + storageInventory[i].h / 2.0f - imgContainer.h / 2.0f;
            SDL_RenderCopyF(renderer, GetFoodTexture(foods[i]), 0, &imgContainer);
        }
    }
}

void MenuInit(SDL_FRect& container,
    Text& titleText,
    std::string titleString,
    MenuButton options[],
    const int numOptions,
    const int buttonPadding,
    const std::string labels[],
    const MenuOption menuTypes[])
{
    // Setup background and title
    container.w = windowWidth;
    container.h = windowHeight;
    container.x = windowWidth / 2.0f - container.w / 2.0f;
    container.y = 0;

    titleText.dstR.w = container.w - SCREEN_PADDING * 2;
    titleText.dstR.h = 100;
    titleText.dstR.x = windowWidth / 2.0f - titleText.dstR.w / 2.0f;
    titleText.dstR.y = SCREEN_PADDING;
    titleText.setText(renderer, robotoF, titleString, { 255, 0, 0 });

    // Setup buttons
    for (int i = 0; i < numOptions; ++i) {
        options[i].label = labels[i];
        options[i].menuType = menuTypes[i];
        options[i].selected = false;
        options[i].buttonText.dstR.w = strlen(options[i].label.c_str()) * LETTER_WIDTH;
        options[i].buttonText.dstR.h = 50;
        options[i].calculateButtonPosition(i, numOptions, windowWidth, windowHeight, buttonPadding);
        options[i].buttonText.dstR.y += titleText.dstR.h;
        options[i].buttonText.setText(renderer, robotoF, options[i].label, BUTTON_UNSELECTED);
    }
}

void RenderMenu(SDL_FRect& container,
    Text& titleText,
    MenuButton options[],
    const int numOptions)
{
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 200);
    SDL_RenderFillRectF(renderer, &container);
    titleText.draw(renderer);

    for (int i = 0; i < numOptions; ++i) {
        if (options[i].selected) {
            options[i].buttonText.setText(renderer, robotoF, options[i].label, BUTTON_SELECTED);
        }
        else {
            options[i].buttonText.setText(renderer, robotoF, options[i].label, BUTTON_UNSELECTED);
        }

        options[i].buttonText.draw(renderer);
    }
}

void CreditsInit(Text& titleText,
    Text& authorsText,
    std::vector<Text>& authors,
    Text& externalGraphicsText,
    std::vector<Text>& egAuthorsTexts,
    MenuButton& backButton,
    SDL_FRect& backRect)
{
    authorsText.dstR.w = std::string("Game authors:").length() * LETTER_WIDTH;
    authorsText.dstR.h = 50;
    authorsText.dstR.x = windowWidth / 2 - authorsText.dstR.w / 2;
    authorsText.dstR.y = 0;
    authorsText.setText(renderer, robotoF, "Game authors:", { 255, 0, 0 });

    authors.push_back(Text());
    authors.back().setText(renderer, robotoF, "Huberti");
    authors.back().dstR.w = authors.back().text.length() * LETTER_WIDTH;
    authors.back().dstR.h = 50;
    authors.back().dstR.x = windowWidth / 2 - authors.back().dstR.w / 2;
    authors.back().dstR.y = authorsText.dstR.y + authorsText.dstR.h;
    authors.push_back(authors.back());
    authors.back().setText(renderer, robotoF, "ajyang2000");
    authors.push_back(authors.back());
    authors.back().setText(renderer, robotoF, "Altimerra");
    for (int i = 1; i < authors.size(); i++) {
        authors[i].dstR.w = authors[i].text.length() * LETTER_WIDTH;
        authors[i].dstR.x = windowWidth / 2 - authors[i].dstR.w / 2;
        authors[i].dstR.y = authors[i - 1].dstR.y + authors[i - 1].dstR.h;
    }

    externalGraphicsText.setText(renderer, robotoF, "External graphics:", { 0, 255, 0 });
    externalGraphicsText.dstR.w = externalGraphicsText.text.length() * LETTER_WIDTH;
    externalGraphicsText.dstR.h = 50;
    externalGraphicsText.dstR.x = windowWidth / 2 - externalGraphicsText.dstR.w / 2;
    externalGraphicsText.dstR.y = authors.back().dstR.y + authors.back().dstR.h + 50;

    egAuthorsTexts.push_back(Text());
    egAuthorsTexts.back().setText(renderer, robotoF, "Antonio Vivaldi - Music");
    egAuthorsTexts.back().dstR.w = egAuthorsTexts.back().text.length() * (LETTER_WIDTH / 50.0f * 25);
    egAuthorsTexts.back().dstR.h = 25;
    egAuthorsTexts.back().dstR.x = windowWidth / 3.0f - egAuthorsTexts.back().dstR.w / 2;
    egAuthorsTexts.back().dstR.y = externalGraphicsText.dstR.y + externalGraphicsText.dstR.h;
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "Becris");
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "Eucalyp");
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "Freepik");
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "Icongeek26");
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "Joseph Kosma - Music");
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "marcelofg55");
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "Pixel Perfect");
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "Nikita Golubev");
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "smalllikeart");
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "Smashicons");
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "TRBRY");
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "ultimatearm");
    egAuthorsTexts.push_back(egAuthorsTexts.back());
    egAuthorsTexts.back().setText(renderer, robotoF, "Vectors Market");

    for (int i = 1; i < egAuthorsTexts.size(); i++) {
        egAuthorsTexts[i].dstR.w = egAuthorsTexts[i].text.length() * (LETTER_WIDTH / 50.0f * 20);
        egAuthorsTexts[i].dstR.x = (windowWidth / 3 * (i % 2 + 1)) - egAuthorsTexts[i].dstR.w / 2;
        if (i == 1) {
            egAuthorsTexts[i].dstR.y = egAuthorsTexts[i - 1].dstR.y;
        }
        else {
            egAuthorsTexts[i].dstR.y = egAuthorsTexts[i - 2].dstR.y + egAuthorsTexts[i - 2].dstR.h;
        }
    }

    backButton.label = "Back to Main Menu";
    backButton.menuType = MenuOption::Main;
    backButton.selected = false;
    backButton.buttonText.setText(renderer, robotoF, backButton.label, BUTTON_UNSELECTED);
    backButton.buttonText.dstR.w = backButton.buttonText.text.length() * LETTER_WIDTH;
    backButton.buttonText.dstR.h = 50;

    backRect.w = backButton.buttonText.dstR.w * 1.05f;
    backRect.h = backButton.buttonText.dstR.h * 1.05f;
    backRect.x = windowWidth / 2.0f - backRect.w / 2.0f;
    backRect.y = windowHeight - SCREEN_PADDING - backRect.h;

    backButton.buttonText.dstR.x = backRect.x + backRect.w / 2.0f - backButton.buttonText.dstR.w / 2.0f;
    backButton.buttonText.dstR.y = backRect.y + backRect.h / 2.0f - backButton.buttonText.dstR.h / 2.0f;
}

void RenderCredits(Text& titleText,
    Text& authorsText,
    std::vector<Text>& authors,
    Text& externalGraphicsText,
    std::vector<Text>& egAuthorsTexts,
    MenuButton& backButton,
    SDL_FRect& backRect)
{
    authorsText.draw(renderer);
    for (auto& text : authors) {
        text.draw(renderer);
    }
    externalGraphicsText.draw(renderer);
    for (auto& text : egAuthorsTexts) {
        text.draw(renderer);
    }
    SDL_SetRenderDrawColor(renderer, 44, 27, 46, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRectF(renderer, &backRect);

    if (backButton.selected) {
        backButton.buttonText.setText(renderer, robotoF, backButton.label, BUTTON_SELECTED);
    }
    else {
        backButton.buttonText.setText(renderer, robotoF, backButton.label, BUTTON_UNSELECTED);
    }

    backButton.buttonText.draw(renderer);
}

void moveTimeByOneHour(Clock& timeClock, int& hour, Text& hourText, int& sickLevel, bool& alreadySick)
{
    if (timeClock.getElapsedTime() > 1000) {
        ++hour;
        if (hour == 24) {
            sickLevel--;
            alreadySick = false;
            hour = 0;
        }
        std::string s = std::to_string(hour);
        if (hour > 12) {
            s = std::to_string(hour - 12);
        }
        std::string h = s + ((hour <= 12) ? "am" : "pm");
        hourText.setText(renderer, robotoF, h);
        timeClock.restart();
    }
}

void loadMap(std::string path, int& mapWidth, int& mapHeight, std::vector<Tile>& tiles, std::vector<Plot>& plots, SDL_FRect houseR)
{
    pugi::xml_document doc;
    std::size_t length;
    char* buffer = (char*)SDL_LoadFile(path.c_str(), &length);
    doc.load_buffer(buffer, length);
    SDL_free(buffer);
    pugi::xml_node mapNode = doc.child("map");
    mapWidth = mapNode.attribute("width").as_int() * mapNode.attribute("tilewidth").as_int();
    mapHeight = mapNode.attribute("height").as_int() * mapNode.attribute("tileheight").as_int();
    auto layersIt = mapNode.children("layer");
    for (pugi::xml_node& layer : layersIt) {
        std::vector<int> data;
        {
            std::string csv = layer.child("data").text().as_string();
            std::stringstream ss(csv);
            std::string value;
            while (std::getline(ss, value, ',')) {
                data.push_back(std::stoi(value));
            }
        }
        for (int y = 0; y < mapNode.attribute("height").as_int(); ++y) {
            for (int x = 0; x < mapNode.attribute("width").as_int(); ++x) {
                Tile t;
                t.dstR.w = mapNode.attribute("tilewidth").as_int();
                t.dstR.h = mapNode.attribute("tileheight").as_int();
                t.dstR.x = x * t.dstR.w;
                t.dstR.y = y * t.dstR.h;
                int h = mapNode.attribute("height").as_int();
                int id = data[y * mapNode.attribute("width").as_int() + x];
                t.id = id;
                if (id != 0) {
                    pugi::xml_node tilesetNode;
                    auto tilesetsIt = mapNode.children("tileset");
                    for (pugi::xml_node& tileset : tilesetsIt) {
                        int firstgid = tileset.attribute("firstgid").as_int();
                        int tileCount = tileset.attribute("tilecount").as_int();
                        int lastGid = firstgid + tileCount - 1;
                        if (id >= firstgid && id <= lastGid) {
                            tilesetNode = tileset;
                            break;
                        }
                    }
                    t.srcR.w = tilesetNode.attribute("tilewidth").as_int();
                    t.srcR.h = tilesetNode.attribute("tileheight").as_int();
                    t.srcR.x = (id - tilesetNode.attribute("firstgid").as_int()) % tilesetNode.attribute("columns").as_int() * t.srcR.w;
                    t.srcR.y = (id - tilesetNode.attribute("firstgid").as_int()) / tilesetNode.attribute("columns").as_int() * t.srcR.h;
                    t.source = tilesetNode.child("image").attribute("source").as_string();
                    tiles.push_back(t);
                }
            }
        }
    }
    std::random_device rd{};
    auto rng = std::default_random_engine{ rd() };
shuffleBegin:
    plots.clear();
    std::shuffle(std::begin(tiles), std::end(tiles), rng);
    std::vector<int> trees;
    // NOTE: Assume that there are no empty tiles
    for (int y = 0; y < mapNode.attribute("height").as_int(); ++y) {
        for (int x = 0; x < mapNode.attribute("width").as_int(); ++x) {
            int index = x + (y * mapNode.attribute("width").as_int());
            // NOTE: Some of them shouldn't be placed together (trees). Randomize, if they are too close randomize once again
            if (tiles[index].source != "sand_tile.png") {
                trees.push_back(index);
            }
        }
    }
    for (int y = 0; y < mapNode.attribute("height").as_int(); ++y) {
        for (int x = 0; x < mapNode.attribute("width").as_int(); ++x) {
            int index = x + (y * mapNode.attribute("width").as_int());
            tiles[index].dstR.x = x * tiles[index].dstR.w;
            tiles[index].dstR.y = y * tiles[index].dstR.h;

            int h = mapNode.attribute("height").as_int();
            pugi::xml_node tilesetNode;
            auto tilesetsIt = mapNode.children("tileset");
            for (pugi::xml_node& tileset : tilesetsIt) {
                int firstgid = tileset.attribute("firstgid").as_int();
                int tileCount = tileset.attribute("tilecount").as_int();
                int lastGid = firstgid + tileCount - 1;
                if (tiles[index].id >= firstgid && tiles[index].id <= lastGid) {
                    tilesetNode = tileset;
                    break;
                }
            }
            tiles[index].srcR.w = tilesetNode.attribute("tilewidth").as_int();
            tiles[index].srcR.h = tilesetNode.attribute("tileheight").as_int();
            tiles[index].srcR.x = (tiles[index].id - tilesetNode.attribute("firstgid").as_int()) % tilesetNode.attribute("columns").as_int() * tiles[index].srcR.w;
            tiles[index].srcR.y = (tiles[index].id - tilesetNode.attribute("firstgid").as_int()) / tilesetNode.attribute("columns").as_int() * tiles[index].srcR.h;
            tiles[index].source = tilesetNode.child("image").attribute("source").as_string();

            if (tiles[index].id != 1) {
                plots.push_back(Plot());
                plots.back().r = tiles[index].dstR;
                if (tiles[index].id == 2) {
                    plots.back().food = Food::Apple;
                }
                else if (tiles[index].id == 3) {
                    plots.back().food = Food::Banana;
                }
                else if (tiles[index].id == 4) {
                    plots.back().food = Food::Pumpkin;
                }
                else if (tiles[index].id == 5) {
                    plots.back().food = Food::Grape;
                }
                else if (tiles[index].id == 6) {
                    plots.back().food = Food::Potato;
                }
                else if (tiles[index].id == 7) {
                    plots.back().food = Food::Carrot;
                }
            }
        }
    }
    for (int i = 0; i < trees.size(); ++i) {
        for (int j = i + 1; j < trees.size(); ++j) {
            SDL_FRect r1 = tiles[trees[i]].dstR;
            SDL_FRect r2 = tiles[trees[j]].dstR;
            r1.x -= 50;
            r1.y -= 50;
            r1.w += 100;
            r1.h += 100;

            r2.x -= 50;
            r2.y -= 50;
            r2.w += 100;
            r2.h += 100;

            if (SDL_HasIntersectionF(&r1, &r2)) {
                goto shuffleBegin;
            }
            if (r1.x < 0 || r1.y < 0 || r2.x < 0 || r2.y < 0
                || r1.x + r1.w > windowWidth || r1.y + r1.h > windowHeight || r2.x + r2.w > windowWidth || r2.y + r2.h > windowHeight) {
                goto shuffleBegin;
            }
            if (SDL_HasIntersectionF(&r1, &houseR) || SDL_HasIntersectionF(&r2, &houseR)) {
                goto shuffleBegin;
            }
        }
    }
}

void addBonusForFood(Food& food, Text& energyText, float& playerSpeed, Clock& increasedPlayerClockSpeed)
{
    if (food == Food::Apple || food == Food::Banana || food == Food::Grape) {
        energyText.setText(renderer, robotoF, std::stoi(energyText.text) + 5);
    }
    else if (food == Food::Carrot || food == Food::Pumpkin || food == Food::Potato) {
        playerSpeed = INIT_PLAYER_SPEED * 2;
        increasedPlayerClockSpeed.restart();
    }
}

void UpdateSickFactor(Food food, std::vector<int>& foodSicknessScore, int& sickLevel, bool alreadySick)
{
    int index = static_cast<int>(food);
    if (index < 0 || index >= foodSicknessScore.size()) {
        return;
    }
    for (int i = 0; i < foodSicknessScore.size(); i++) {
        if (i == index) {
            foodSicknessScore[i] += 3;
        }
        else {
            foodSicknessScore[i] == 0 ? 0 : foodSicknessScore[i]--;
        }
    }

    if (!alreadySick) {
        int possibility = rand() % MAX_SICKNESS_SCORE + 1;
        int threshold = foodSicknessScore[index];

        if (possibility < threshold) {
            alreadySick = true;
            sickLevel += (threshold - possibility) / 3 + 1;
        }
    }
}

Food eat(SDL_FRect& inventorySlotR, SDL_FRect& inventorySlot2R, Text& energyText, float& playerSpeed, Clock& increasedPlayerSpeedClock, std::array<Food, 2>& foods, Text& hungerText)
{
    Food eaten = Food::Empty;
    if (SDL_PointInFRect(&mousePos, &inventorySlotR)) {
        if (foods[0] != Food::Empty) {
            addBonusForFood(foods[0], energyText, playerSpeed, increasedPlayerSpeedClock);
            eaten = foods[0];
            foods[0] = Food::Empty;
            hungerText.setText(renderer, robotoF, std::stoi(hungerText.text) + 50 > 100 ? 100 : std::stoi(hungerText.text) + 50);
        }
    }
    if (SDL_PointInFRect(&mousePos, &inventorySlot2R)) {
        if (foods[1] != Food::Empty) {
            addBonusForFood(foods[1], energyText, playerSpeed, increasedPlayerSpeedClock);
            eaten = foods[1];
            foods[1] = Food::Empty;
            hungerText.setText(renderer, robotoF, std::stoi(hungerText.text) + 50 > 100 ? 100 : std::stoi(hungerText.text) + 50);
        }
    }

    return eaten;
}

Food eat(std::vector<StorageUI>& storage, Text& energyText, float& playerSpeed, Clock& increasedPlayerSpeedClock, std::vector<SDL_FRect>& inventory, std::array<Food, 2>& foods, Text& hungerText)
{
    for (auto& storageItem : storage) {
        if (SDL_PointInFRect(&mousePos, &storageItem.container)) {
            if (storageItem.amount > 0) {
                addBonusForFood(storageItem.foodType, energyText, playerSpeed, increasedPlayerSpeedClock);
                storageItem.amount--;
                hungerText.setText(renderer, robotoF, std::stoi(hungerText.text) + 50 > 100 ? 100 : std::stoi(hungerText.text) + 50);

                return storageItem.foodType;
            }
        }
    }

    for (int i = 0; i < inventory.size(); i++) {
        if (SDL_PointInFRect(&mousePos, &inventory[i])) {
            if (foods[i] != Food::Empty) {
                Food eaten = foods[i];
                addBonusForFood(foods[i], energyText, playerSpeed, increasedPlayerSpeedClock);
                foods[i] = Food::Empty;
                hungerText.setText(renderer, robotoF, std::stoi(hungerText.text) + 50 > 100 ? 100 : std::stoi(hungerText.text) + 50);

                return eaten;
            }
        }
    }

    return Food::Empty;
}

void onSpaceInOutsideState(int& hour, bool& canCollect, std::array<Food, 2>& foods, State& state, std::vector<Plot>& plots, Player& player, std::vector<Part>& parts)
{
    if (hour >= 7 && hour <= 17) {
        canCollect = true;
        if (foods[0] == Food::Empty || foods[1] == Food::Empty) {
            int index = foods[0] == Food::Empty ? 0 : 1;
            for (auto& plot : plots) {
                if (plot.isIntersecting(player.r)) {
                    state = State::Minigame;
                    if (plot.food == Food::Apple) {
                        currentPartT = appleT;
                    }
                    else if (plot.food == Food::Banana) {
                        currentPartT = bananaT;
                    }
                    else if (plot.food == Food::Carrot) {
                        currentPartT = carrotT;
                    }
                    else if (plot.food == Food::Grape) {
                        currentPartT = grapeT;
                    }
                    else if (plot.food == Food::Potato) {
                        currentPartT = potatoT;
                    }
                    else if (plot.food == Food::Pumpkin) {
                        currentPartT = pumpkinT;
                    }
                    for (int i = 0; i < 10; ++i) {
                        parts.push_back(Part());
                        parts.back().dstR.w = 32;
                        parts.back().dstR.h = 32;
                        parts.back().dstR.x = random(0, windowWidth - parts.back().dstR.w);
                        parts.back().dstR.y = -parts.back().dstR.h;
                    }
                }
            }
        }
    }
    else {
        canCollect = false;
    }
}

void onSpaceInHomeState(Player& player, int& hour, State& state, bool& shouldShowWhenCanSleepAndGoShop,
    std::array<Food, 2> foods, Shop& shop, std::vector<StorageUI>& storage, Interactable& doorI, bool& shouldGoHome,
    bool& canCollect, bool& shouldShowInfoTextAboutNotEnoughEnergy, SDL_FRect& houseR, Interactable& shopI, Interactable& bedI,
    bool& alreadySick, int& sickLevel, Text& hourText, Text& energyText, int& maxEnergy, Interactable& chestI)
{
    if (SDL_HasIntersectionF(&player.r, &shopI.dstR)) {
        if (hour >= 0 && hour < 7 || hour > 17) {
            state = State::Shop;
            shouldShowWhenCanSleepAndGoShop = false;
            Mix_PlayChannel(-1, doorS, 0);
            /*
                                        NOTE: Order in storage
                                        0 Apple,
                                        1 Carrot,
                                        2 Grape,
                                        3 Potato,
                                        4 Pumpkin,
                                        5 Banana,
                                        Order in shop
                                        0 banana
                                        1 apple
                                        2 grape
                                        3 pumpkin 
                                        4 potato
                                        5 carrot
                                    */
            shop.sellItems[0].countText.setText(renderer, robotoF, "x" + std::to_string(storage[5].amount));
            shop.sellItems[1].countText.setText(renderer, robotoF, "x" + std::to_string(storage[0].amount));
            shop.sellItems[2].countText.setText(renderer, robotoF, "x" + std::to_string(storage[2].amount));
            shop.sellItems[3].countText.setText(renderer, robotoF, "x" + std::to_string(storage[4].amount));
            shop.sellItems[4].countText.setText(renderer, robotoF, "x" + std::to_string(storage[3].amount));
            shop.sellItems[5].countText.setText(renderer, robotoF, "x" + std::to_string(storage[1].amount));
            if (foods[0] == Food::Apple) {
                if (foods[1] == Food::Apple) {
                    shop.sellItems[1].countText.setText(renderer, robotoF, "x" + std::to_string(storage[0].amount + 2));
                }
                else {
                    shop.sellItems[1].countText.setText(renderer, robotoF, "x" + std::to_string(storage[0].amount + 1));
                }
            }
            else if (foods[0] == Food::Banana) {
                if (foods[1] == Food::Banana) {
                    shop.sellItems[0].countText.setText(renderer, robotoF, "x" + std::to_string(storage[5].amount + 2));
                }
                else {
                    shop.sellItems[0].countText.setText(renderer, robotoF, "x" + std::to_string(storage[5].amount + 1));
                }
            }
            else if (foods[0] == Food::Carrot) {
                if (foods[1] == Food::Carrot) {
                    shop.sellItems[5].countText.setText(renderer, robotoF, "x" + std::to_string(storage[1].amount + 2));
                }
                else {
                    shop.sellItems[5].countText.setText(renderer, robotoF, "x" + std::to_string(storage[1].amount + 1));
                }
            }
            else if (foods[0] == Food::Grape) {
                if (foods[1] == Food::Grape) {
                    shop.sellItems[2].countText.setText(renderer, robotoF, "x" + std::to_string(storage[2].amount + 2));
                }
                else {
                    shop.sellItems[2].countText.setText(renderer, robotoF, "x" + std::to_string(storage[2].amount + 1));
                }
            }
            else if (foods[0] == Food::Potato) {
                if (foods[1] == Food::Potato) {
                    shop.sellItems[4].countText.setText(renderer, robotoF, "x" + std::to_string(storage[3].amount + 2));
                }
                else {
                    shop.sellItems[4].countText.setText(renderer, robotoF, "x" + std::to_string(storage[3].amount + 1));
                }
            }
            else if (foods[0] == Food::Pumpkin) {
                if (foods[1] == Food::Pumpkin) {
                    shop.sellItems[3].countText.setText(renderer, robotoF, "x" + std::to_string(storage[4].amount + 2));
                }
                else {
                    shop.sellItems[3].countText.setText(renderer, robotoF, "x" + std::to_string(storage[4].amount + 1));
                }
            }
            if (foods[0] != foods[1]) {
                if (foods[1] == Food::Apple) {
                    shop.sellItems[1].countText.setText(renderer, robotoF, "x" + std::to_string(storage[0].amount + 1));
                }
                else if (foods[1] == Food::Banana) {
                    shop.sellItems[0].countText.setText(renderer, robotoF, "x" + std::to_string(storage[5].amount + 1));
                }
                else if (foods[1] == Food::Carrot) {
                    shop.sellItems[5].countText.setText(renderer, robotoF, "x" + std::to_string(storage[1].amount + 1));
                }
                else if (foods[1] == Food::Grape) {
                    shop.sellItems[2].countText.setText(renderer, robotoF, "x" + std::to_string(storage[2].amount + 1));
                }
                else if (foods[1] == Food::Potato) {
                    shop.sellItems[4].countText.setText(renderer, robotoF, "x" + std::to_string(storage[3].amount + 1));
                }
                else if (foods[1] == Food::Pumpkin) {
                    shop.sellItems[3].countText.setText(renderer, robotoF, "x" + std::to_string(storage[4].amount + 1));
                }
            }
        }
        else {
            shouldShowWhenCanSleepAndGoShop = true;
        }
    }
    else if (SDL_HasIntersectionF(&player.r, &doorI.dstR)) {
        if (!shouldGoHome) {
            state = State::Outside;
            player.r.x = houseR.x + houseR.w + 5;
            player.r.y = houseR.y + houseR.h / 2 - player.r.h / 2;
            canCollect = true;
            shouldShowWhenCanSleepAndGoShop = false;
            Mix_PlayChannel(-1, doorS, 0);
        }
        else {
            shouldShowInfoTextAboutNotEnoughEnergy = true;
        }
    }
    else if (SDL_HasIntersectionF(&player.r, &bedI.dstR)) {
        if (hour >= 0 && hour < 7 || hour > 17) {
            shouldShowWhenCanSleepAndGoShop = false;
            hour = 7;
            alreadySick = false;
            if (sickLevel > 0) {
                sickLevel--;
            }
            hourText.setText(renderer, robotoF, std::to_string(hour) + "am");
            energyText.setText(renderer, robotoF, maxEnergy > std::stoi(energyText.text) ? maxEnergy : std::stoi(energyText.text));
            shouldGoHome = false;
            shouldShowInfoTextAboutNotEnoughEnergy = false;
            Mix_PlayChannel(-1, sleepS, 0);
        }
        else {
            shouldShowWhenCanSleepAndGoShop = true;
        }
    }
    else if (SDL_HasIntersectionF(&player.r, &chestI.dstR)) {
        state = State::Storage;
    }
}

int main(int argc, char* argv[])
{
    std::srand(std::time(0));
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
    SDL_LogSetOutputFunction(logOutputCallback, 0);
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    SDL_GetMouseState(&mousePos.x, &mousePos.y);
    window = SDL_CreateWindow("AutumnSurvival", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    robotoF = TTF_OpenFont("res/roboto.ttf", 72);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_RenderSetScale(renderer, w / (float)windowWidth, h / (float)windowHeight);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_AddEventWatch(eventWatch, 0);
    prefPath = SDL_GetPrefPath("NextCodeApps", "AutumnSurvival");
    bool shouldShowIntro = true;
gameBegin:
    leavesT = IMG_LoadTexture(renderer, "res/leaves.png");
    treeT = IMG_LoadTexture(renderer, "res/tree.png");
    tree1T = IMG_LoadTexture(renderer, "res/tree1.png");
    tree2T = IMG_LoadTexture(renderer, "res/tree2.png");
    tree3T = IMG_LoadTexture(renderer, "res/tree3.png");
    shopT = IMG_LoadTexture(renderer, "res/shop.png");
    backArrowT = IMG_LoadTexture(renderer, "res/backArrow.png");
    buyT = IMG_LoadTexture(renderer, "res/buy.png");
    appleWithSprayerT = IMG_LoadTexture(renderer, "res/appleWithSprayer.png");
    vineT = IMG_LoadTexture(renderer, "res/vine.png");
    grapeT = IMG_LoadTexture(renderer, "res/grape.png");
    appleT = IMG_LoadTexture(renderer, "res/apple.png");
    appleTreeT = IMG_LoadTexture(renderer, "res/appleTree.png");
    bananaT = IMG_LoadTexture(renderer, "res/banana.png");
    carrotT = IMG_LoadTexture(renderer, "res/carrot.png");
    soilT = IMG_LoadTexture(renderer, "res/soil.png");
    bananaTreeT = IMG_LoadTexture(renderer, "res/bananaTree.png");
    mutedT = IMG_LoadTexture(renderer, "res/muted.png");
    soundT = IMG_LoadTexture(renderer, "res/sound.png");
    leafT = IMG_LoadTexture(renderer, "res/leaf.png");
    sunT = IMG_LoadTexture(renderer, "res/sun.png");
    moonT = IMG_LoadTexture(renderer, "res/moon.png");
    energyT = IMG_LoadTexture(renderer, "res/energy.png");
    rotT = IMG_LoadTexture(renderer, "res/rot.png");
    moreEnergyT = IMG_LoadTexture(renderer, "res/moreEnergy.png");
    houseflyT = IMG_LoadTexture(renderer, "res/housefly.png");
    wormT = IMG_LoadTexture(renderer, "res/worm.png");
    potatoT = IMG_LoadTexture(renderer, "res/potato.png");
    pumpkinT = IMG_LoadTexture(renderer, "res/pumpkin.png");
    pumpkinTreeT = IMG_LoadTexture(renderer, "res/pumpkinTree.png");
    tradeT = IMG_LoadTexture(renderer, "res/trade.png");
    playerT = IMG_LoadTexture(renderer, "res/player.png");
    houseT = IMG_LoadTexture(renderer, "res/house.png");
    collectT = IMG_LoadTexture(renderer, "res/collect.png");
    openT = IMG_LoadTexture(renderer, "res/open.png");
    xT = IMG_LoadTexture(renderer, "res/x.png");
    bedT = IMG_LoadTexture(renderer, "res/bed.png");
    lightT = IMG_LoadTexture(renderer, "res/light.png");
    doorT = IMG_LoadTexture(renderer, "res/door.png");
    chestT = IMG_LoadTexture(renderer, "res/chest.png");
    windowT = IMG_LoadTexture(renderer, "res/window.png");
    hungerT = IMG_LoadTexture(renderer, "res/hunger.png");
    carrotSoilT = IMG_LoadTexture(renderer, "res/carrotSoil.png");
    potatoSoilT = IMG_LoadTexture(renderer, "res/potatoSoil.png");
    sickT = IMG_LoadTexture(renderer, "res/sick.png");
    sellT = IMG_LoadTexture(renderer, "res/sell.png");
    circleT = IMG_LoadTexture(renderer, "res/circle.png");
    redCircleT = IMG_LoadTexture(renderer, "res/redCircle.png");
    josephKosmaM = Mix_LoadMUS("res/autumnLeavesJosephKosma.mp3");
    antonioVivaldiM = Mix_LoadMUS("res/jesienAntonioVivaldi.mp3");
    doorS = Mix_LoadWAV("res/door.wav");
    pickupS = Mix_LoadWAV("res/pickup.wav");
    powerupS = Mix_LoadWAV("res/powerup.wav");
    sleepS = Mix_LoadWAV("res/sleep.wav");
    Mix_PlayMusic(josephKosmaM, 1);

    SDL_FRect grapeTreeR{};
    SDL_FRect appleTreeR{};
    SDL_FRect bananaTreeR{};
    SDL_FRect carrotTreeR{};
    SDL_FRect potatoTreeR{};
    SDL_FRect pumpkinTreeR{};
    SDL_FRect soundBtnR{};
    std::vector<Entity> entities;
    Clock leafClock;
    Clock globalClock;
    Clock grapeClock;
    Clock appleClock;
    Clock bananaClock;
    Clock carrotClock;
    Clock potatoClock;
    Clock pumpkinClock;
    Clock timeClock;
    Clock tradeClock;
    Text scoreText;
    Shop shop;
    shop.sellItems.push_back(SellItem());
    shop.sellItems.back().imageR.w = 72;
    shop.sellItems.back().imageR.h = 72;
    shop.sellItems.back().imageR.x = 5;
    shop.sellItems.back().imageR.y = windowHeight - shop.sellItems.back().imageR.h - 100;
    shop.sellItems.back().countText.dstR.w = 50;
    shop.sellItems.back().countText.dstR.h = 30;
    shop.sellItems.back().countText.dstR.x = shop.sellItems.back().imageR.x + shop.sellItems.back().imageR.w + 5;
    shop.sellItems.back().countText.dstR.y = shop.sellItems.back().imageR.y + shop.sellItems.back().imageR.h / 2;
    shop.sellItems.back().countText.setText(renderer, robotoF, "x0");
    shop.sellItems.back().sellR.w = shop.sellItems.back().imageR.w;
    shop.sellItems.back().sellR.h = 40;
    shop.sellItems.back().sellR.x = shop.sellItems.back().imageR.x;
    shop.sellItems.back().sellR.y = shop.sellItems.back().imageR.y + shop.sellItems.back().imageR.h;
    shop.sellItems.back().gainText.setText(renderer, robotoF, "5");
    shop.sellItems.back().gainText.dstR.w = 50;
    shop.sellItems.back().gainText.dstR.h = 40;
    shop.sellItems.back().gainText.dstR.x = shop.sellItems.back().sellR.x + shop.sellItems.back().sellR.w / 2 - shop.sellItems.back().gainText.dstR.w / 2;
    shop.sellItems.back().gainText.dstR.y = shop.sellItems.back().sellR.y + shop.sellItems.back().sellR.h;
    shop.sellItems.back().foodT = bananaT;
    shop.sellItems.push_back(SellItem());
    shop.sellItems.back().imageR.w = 72;
    shop.sellItems.back().imageR.h = 72;
    shop.sellItems.back().imageR.x = shop.sellItems[shop.sellItems.size() - 2].countText.dstR.x + shop.sellItems[shop.sellItems.size() - 2].countText.dstR.w + 5;
    shop.sellItems.back().imageR.y = windowHeight - shop.sellItems.back().imageR.h - 100;
    shop.sellItems.back().countText.dstR.w = 50;
    shop.sellItems.back().countText.dstR.h = 30;
    shop.sellItems.back().countText.dstR.x = shop.sellItems.back().imageR.x + shop.sellItems.back().imageR.w + 5;
    shop.sellItems.back().countText.dstR.y = shop.sellItems.back().imageR.y + shop.sellItems.back().imageR.h / 2;
    shop.sellItems.back().countText.setText(renderer, robotoF, "x0");
    shop.sellItems.back().sellR.w = shop.sellItems.back().imageR.w;
    shop.sellItems.back().sellR.h = 40;
    shop.sellItems.back().sellR.x = shop.sellItems.back().imageR.x;
    shop.sellItems.back().sellR.y = shop.sellItems.back().imageR.y + shop.sellItems.back().imageR.h;
    shop.sellItems.back().gainText.setText(renderer, robotoF, "3");
    shop.sellItems.back().gainText.dstR.w = 50;
    shop.sellItems.back().gainText.dstR.h = 40;
    shop.sellItems.back().gainText.dstR.x = shop.sellItems.back().sellR.x + shop.sellItems.back().sellR.w / 2 - shop.sellItems.back().gainText.dstR.w / 2;
    shop.sellItems.back().gainText.dstR.y = shop.sellItems.back().sellR.y + shop.sellItems.back().sellR.h;
    shop.sellItems.back().foodT = appleT;
    shop.sellItems.push_back(SellItem());
    shop.sellItems.back().imageR.w = 72;
    shop.sellItems.back().imageR.h = 72;
    shop.sellItems.back().imageR.x = shop.sellItems[shop.sellItems.size() - 2].countText.dstR.x + shop.sellItems[shop.sellItems.size() - 2].countText.dstR.w + 5;
    shop.sellItems.back().imageR.y = windowHeight - shop.sellItems.back().imageR.h - 100;
    shop.sellItems.back().countText.dstR.w = 50;
    shop.sellItems.back().countText.dstR.h = 30;
    shop.sellItems.back().countText.dstR.x = shop.sellItems.back().imageR.x + shop.sellItems.back().imageR.w + 5;
    shop.sellItems.back().countText.dstR.y = shop.sellItems.back().imageR.y + shop.sellItems.back().imageR.h / 2;
    shop.sellItems.back().countText.setText(renderer, robotoF, "x0");
    shop.sellItems.back().sellR.w = shop.sellItems.back().imageR.w;
    shop.sellItems.back().sellR.h = 40;
    shop.sellItems.back().sellR.x = shop.sellItems.back().imageR.x;
    shop.sellItems.back().sellR.y = shop.sellItems.back().imageR.y + shop.sellItems.back().imageR.h;
    shop.sellItems.back().gainText.setText(renderer, robotoF, "7");
    shop.sellItems.back().gainText.dstR.w = 50;
    shop.sellItems.back().gainText.dstR.h = 40;
    shop.sellItems.back().gainText.dstR.x = shop.sellItems.back().sellR.x + shop.sellItems.back().sellR.w / 2 - shop.sellItems.back().gainText.dstR.w / 2;
    shop.sellItems.back().gainText.dstR.y = shop.sellItems.back().sellR.y + shop.sellItems.back().sellR.h;
    shop.sellItems.back().foodT = grapeT;
    shop.sellItems.push_back(SellItem());
    shop.sellItems.back().imageR.w = 72;
    shop.sellItems.back().imageR.h = 72;
    shop.sellItems.back().imageR.x = shop.sellItems[shop.sellItems.size() - 2].countText.dstR.x + shop.sellItems[shop.sellItems.size() - 2].countText.dstR.w + 5;
    shop.sellItems.back().imageR.y = windowHeight - shop.sellItems.back().imageR.h - 100;
    shop.sellItems.back().countText.dstR.w = 50;
    shop.sellItems.back().countText.dstR.h = 30;
    shop.sellItems.back().countText.dstR.x = shop.sellItems.back().imageR.x + shop.sellItems.back().imageR.w + 5;
    shop.sellItems.back().countText.dstR.y = shop.sellItems.back().imageR.y + shop.sellItems.back().imageR.h / 2;
    shop.sellItems.back().countText.setText(renderer, robotoF, "x0");
    shop.sellItems.back().sellR.w = shop.sellItems.back().imageR.w;
    shop.sellItems.back().sellR.h = 40;
    shop.sellItems.back().sellR.x = shop.sellItems.back().imageR.x;
    shop.sellItems.back().sellR.y = shop.sellItems.back().imageR.y + shop.sellItems.back().imageR.h;
    shop.sellItems.back().gainText.setText(renderer, robotoF, "10");
    shop.sellItems.back().gainText.dstR.w = 50;
    shop.sellItems.back().gainText.dstR.h = 40;
    shop.sellItems.back().gainText.dstR.x = shop.sellItems.back().sellR.x + shop.sellItems.back().sellR.w / 2 - shop.sellItems.back().gainText.dstR.w / 2;
    shop.sellItems.back().gainText.dstR.y = shop.sellItems.back().sellR.y + shop.sellItems.back().sellR.h;
    shop.sellItems.back().foodT = pumpkinT;
    shop.sellItems.push_back(SellItem());
    shop.sellItems.back().imageR.w = 72;
    shop.sellItems.back().imageR.h = 72;
    shop.sellItems.back().imageR.x = shop.sellItems[shop.sellItems.size() - 2].countText.dstR.x + shop.sellItems[shop.sellItems.size() - 2].countText.dstR.w + 5;
    shop.sellItems.back().imageR.y = windowHeight - shop.sellItems.back().imageR.h - 100;
    shop.sellItems.back().countText.dstR.w = 50;
    shop.sellItems.back().countText.dstR.h = 30;
    shop.sellItems.back().countText.dstR.x = shop.sellItems.back().imageR.x + shop.sellItems.back().imageR.w + 5;
    shop.sellItems.back().countText.dstR.y = shop.sellItems.back().imageR.y + shop.sellItems.back().imageR.h / 2;
    shop.sellItems.back().countText.setText(renderer, robotoF, "x0");
    shop.sellItems.back().sellR.w = shop.sellItems.back().imageR.w;
    shop.sellItems.back().sellR.h = 40;
    shop.sellItems.back().sellR.x = shop.sellItems.back().imageR.x;
    shop.sellItems.back().sellR.y = shop.sellItems.back().imageR.y + shop.sellItems.back().imageR.h;
    shop.sellItems.back().gainText.setText(renderer, robotoF, "5");
    shop.sellItems.back().gainText.dstR.w = 50;
    shop.sellItems.back().gainText.dstR.h = 40;
    shop.sellItems.back().gainText.dstR.x = shop.sellItems.back().sellR.x + shop.sellItems.back().sellR.w / 2 - shop.sellItems.back().gainText.dstR.w / 2;
    shop.sellItems.back().gainText.dstR.y = shop.sellItems.back().sellR.y + shop.sellItems.back().sellR.h;
    shop.sellItems.back().foodT = potatoT;
    shop.sellItems.push_back(SellItem());
    shop.sellItems.back().imageR.w = 72;
    shop.sellItems.back().imageR.h = 72;
    shop.sellItems.back().imageR.x = shop.sellItems[shop.sellItems.size() - 2].countText.dstR.x + shop.sellItems[shop.sellItems.size() - 2].countText.dstR.w + 5;
    shop.sellItems.back().imageR.y = windowHeight - shop.sellItems.back().imageR.h - 100;
    shop.sellItems.back().countText.dstR.w = 50;
    shop.sellItems.back().countText.dstR.h = 30;
    shop.sellItems.back().countText.dstR.x = shop.sellItems.back().imageR.x + shop.sellItems.back().imageR.w + 5;
    shop.sellItems.back().countText.dstR.y = shop.sellItems.back().imageR.y + shop.sellItems.back().imageR.h / 2;
    shop.sellItems.back().countText.setText(renderer, robotoF, "x0");
    shop.sellItems.back().sellR.w = shop.sellItems.back().imageR.w;
    shop.sellItems.back().sellR.h = 40;
    shop.sellItems.back().sellR.x = shop.sellItems.back().imageR.x;
    shop.sellItems.back().sellR.y = shop.sellItems.back().imageR.y + shop.sellItems.back().imageR.h;
    shop.sellItems.back().gainText.setText(renderer, robotoF, "7");
    shop.sellItems.back().gainText.dstR.w = 50;
    shop.sellItems.back().gainText.dstR.h = 40;
    shop.sellItems.back().gainText.dstR.x = shop.sellItems.back().sellR.x + shop.sellItems.back().sellR.w / 2 - shop.sellItems.back().gainText.dstR.w / 2;
    shop.sellItems.back().gainText.dstR.y = shop.sellItems.back().sellR.y + shop.sellItems.back().sellR.h;
    shop.sellItems.back().foodT = carrotT;
    State state;
    if (shouldShowIntro) {
        state = State::Intro;
    }
    else {
        state = State::Outside;
    }
    State pausedState = state;
    Clock rotClock;
    int rotDelayInMs = ROT_INIT_DELAY_IN_MS;
    Music currentMusic = Music::JosephKosma;
    int grapeSpawnDelayInMs = GRAPE_INIT_SPAWN_DELAY_IN_MS;
    int appleSpawnDelayInMs = APPLE_INIT_SPAWN_DELAY_IN_MS;
    int bananaSpawnDelayInMs = BANANA_INIT_SPAWN_DELAY_IN_MS;
    int carrotSpawnDelayInMs = CARROT_INIT_SPAWN_DELAY_IN_MS;
    int potatoSpawnDelayInMs = POTATO_INIT_SPAWN_DELAY_IN_MS;
    int pumpkinSpawnDelayInMs = PUMPKIN_INIT_SPAWN_DELAY_IN_MS;
    bool isMuted = false;
    Intro intro;
    Text hourText;
    int hour = 7;
    SDL_FRect sunR{};
    SDL_FRect energyR{};
    Text energyText;
    bool shouldShowRotImage = false;
    SDL_FRect rotR{};
    int maxEnergy = MAX_ENERGY_INIT;
    SDL_FRect houseflyR{};
    bool houseflyGoingRight = true;
    std::vector<SDL_FRect> tradeRects;
    Player player{};
    SDL_FRect houseR{};
    bool isMoving = false;
    Clock energyClock;
    bool shouldGoHome = false;
    SDL_FRect collectR{};
    bool isCollecting = false;
    SDL_FRect inventorySlotR{};
    SDL_FRect inventorySlot2R{};
    SDL_FRect inventorySlotXR{};
    SDL_FRect inventorySlotX2R{};
    std::array<Food, 2> foods;
    std::array<bool, 2> foodSelected;
    Interactable doorI{};
    Interactable shopI{};
    Interactable bedI{};
    Interactable chestI{};
    bool exitedHome = false;
    Text infoTextAboutNotEnoughEnergy;
    bool shouldShowInfoTextAboutNotEnoughEnergy = false;
    Text cantCollectText;
    bool canCollect = true;
    Text canSleepAndGoShopText;
    bool shouldShowWhenCanSleepAndGoShop = false;
    std::vector<Part> parts;
    SDL_FRect tile{};
    SDL_FRect homeGround{};
    SDL_FRect homeWall{};
    SDL_FRect homeSeparator{};
    SDL_FRect windowR{};
    Text actionText;
    Interactable currentAction{};
    int mapWidth = 0;
    int mapHeight = 0;
    std::vector<Tile> tiles;
    std::vector<Plot> plots;
    std::vector<SDL_Rect> playerAnimationFrames;
    int playerAnimationFrame = 0;
    Clock playerAnimationClock;
    PlayerDirection playerDirection = PlayerDirection::Right;
    SDL_FRect doorR{};
    Text hungerText;
    Clock hungerClock;
    Text playAgainText;
    Text gameOverText;
    SDL_FRect storageContainer{};
    Text storageTitleText;
    Text storageHoverText;
    Text storageHungerText;
    std::vector<StorageUI> storage;
    std::vector<SDL_FRect> storageInvenPlaceholder;
    float playerSpeed = INIT_PLAYER_SPEED;
    Clock increasedPlayerSpeedClock;
    MenuButton pauseOptions[PAUSE_NUM_OPTIONS];
    const std::string pauseLabels[PAUSE_NUM_OPTIONS] = {
        "Resume",
        "Return to Main Menu",
        "Quit"
    };
    const MenuOption pauseMenuTypes[PAUSE_NUM_OPTIONS] = {
        MenuOption::Resume,
        MenuOption::Main,
        MenuOption::Quit
    };
    Text pauseTitleText;
    SDL_FRect pauseContainer;
    MenuButton mainOptions[MAINMENU_NUM_OPTIONS];
    const std::string mainLabels[MAINMENU_NUM_OPTIONS] = {
        "Play",
        "Controls",
        "Credits",
        "Exit"
    };
    const MenuOption mainMenuTypes[MAINMENU_NUM_OPTIONS] = {
        MenuOption::Play,
        MenuOption::Controls,
        MenuOption::Credits,
        MenuOption::Quit
    };
    Text mainTitleText;
    SDL_FRect mainContainer;
    bool pauseKeyHeld = false;
    Text creditsTitleText;
    Text authorsText;
    std::vector<Text> authors;
    Text externalGraphicsText;
    std::vector<Text> egAuthorsTexts;
    MenuButton backButton;
    SDL_FRect backRect;
    SDL_FRect xBtnR;
    xBtnR.w = 32;
    xBtnR.h = 32;
    xBtnR.x = 15;
    xBtnR.y = 15;
    std::vector<int> foodSicknessScore(static_cast<int>(Food::NumFood), 0);
    int sickLevel = 0;
    bool alreadySick = false;
    Text sickText;
    Text controlsText;
    Text keyControlsText;
    SDL_FRect leftR;
    leftR.w = 64;
    leftR.h = 64;
    leftR.x = windowWidth - leftR.w * 4;
    leftR.y = windowHeight - leftR.h - 45;
    SDL_FRect rightR;
    rightR.w = 64;
    rightR.h = 64;
    rightR.x = windowWidth - rightR.w * 2;
    rightR.y = windowHeight - rightR.h - 45;
    SDL_FRect upR;
    upR.w = 64;
    upR.h = 64;
    upR.x = windowWidth - upR.w * 3;
    upR.y = windowHeight - upR.h * 2 - 45;
    SDL_FRect downR;
    downR.w = 64;
    downR.h = 64;
    downR.x = windowWidth - downR.w * 3;
    downR.y = windowHeight - downR.h - 45;
    SDL_FRect actionR;
    actionR.w = 64;
    actionR.h = 64;
    actionR.x = upR.x - actionR.w - 15;
    actionR.y = upR.y - actionR.h - 15;

    soundBtnR.w = 48;
    soundBtnR.h = 48;
    soundBtnR.x = windowWidth - soundBtnR.w - 20;
    soundBtnR.y = 20;
    scoreText.setText(renderer, robotoF, 0);
    scoreText.dstR.w = 50;
    scoreText.dstR.h = 30;
    scoreText.dstR.x = windowWidth / 2.0f - scoreText.dstR.w / 2.0f;
    scoreText.dstR.y = SCREEN_PADDING / 2.0f;
    shop.backArrowR.w = 32;
    shop.backArrowR.h = 32;
    shop.backArrowR.x = 5;
    shop.backArrowR.y = 5;
    shop.lessRotR.w = 32;
    shop.lessRotR.h = 32;
    shop.lessRotR.x = shop.backArrowR.x + shop.backArrowR.w + 5;
    shop.lessRotR.y = shop.lessRotPriceText.dstR.y + shop.lessRotPriceText.dstR.h + 5;
    shop.lessRotText.setText(renderer, robotoF, "Less rot");
    shop.lessRotText.dstR.w = 65;
    shop.lessRotText.dstR.h = 35;
    shop.lessRotText.dstR.x = shop.lessRotR.x;
    shop.lessRotText.dstR.y = shop.lessRotR.y + shop.lessRotR.h;
    shop.lessRotPriceText.setText(renderer, robotoF, 100);
    shop.lessRotPriceText.dstR.w = 70;
    shop.lessRotPriceText.dstR.h = 35;
    shop.lessRotPriceText.dstR.x = shop.lessRotText.dstR.x;
    shop.lessRotPriceText.dstR.y = shop.lessRotText.dstR.y + shop.lessRotText.dstR.h;
    shop.lessRotBuyR.w = 45;
    shop.lessRotBuyR.h = 32;
    shop.lessRotBuyR.x = shop.lessRotPriceText.dstR.x;
    shop.lessRotBuyR.y = shop.lessRotPriceText.dstR.y + shop.lessRotPriceText.dstR.h;
    shop.moreEnergyR.w = 32;
    shop.moreEnergyR.h = 32;
    shop.moreEnergyR.x = shop.lessRotR.x + shop.lessRotR.w + 50;
    shop.moreEnergyR.y = shop.moreEnergyPriceText.dstR.y + shop.moreEnergyPriceText.dstR.h + 5;
    shop.moreEnergyText.setText(renderer, robotoF, "More energy");
    shop.moreEnergyText.dstR.w = 85;
    shop.moreEnergyText.dstR.h = 35;
    shop.moreEnergyText.dstR.x = shop.moreEnergyR.x;
    shop.moreEnergyText.dstR.y = shop.moreEnergyR.y + shop.moreEnergyR.h;
    shop.moreEnergyPriceText.setText(renderer, robotoF, 100);
    shop.moreEnergyPriceText.dstR.w = 70;
    shop.moreEnergyPriceText.dstR.h = 35;
    shop.moreEnergyPriceText.dstR.x = shop.moreEnergyText.dstR.x;
    shop.moreEnergyPriceText.dstR.y = shop.moreEnergyText.dstR.y + shop.moreEnergyText.dstR.h;
    shop.moreEnergyBuyR.w = 45;
    shop.moreEnergyBuyR.h = 32;
    shop.moreEnergyBuyR.x = shop.moreEnergyPriceText.dstR.x;
    shop.moreEnergyBuyR.y = shop.moreEnergyPriceText.dstR.y + shop.moreEnergyPriceText.dstR.h;
    intro.leafR.w = 64;
    intro.leafR.h = 64;
    intro.leafR.x = windowWidth / 2 - intro.leafR.w / 2;
    intro.leafR.y = windowHeight / 2 - intro.leafR.h / 2;
    hourText.setText(renderer, robotoF, "7 am");
    hourText.dstR.w = 100;
    hourText.dstR.h = 35;
    hourText.dstR.x = windowWidth - hourText.dstR.w - 20;
    hourText.dstR.y = soundBtnR.y + soundBtnR.h;
    sunR.w = 32;
    sunR.h = 32;
    sunR.x = hourText.dstR.x - sunR.w;
    sunR.y = hourText.dstR.y;
    energyText.setText(renderer, robotoF, MAX_ENERGY_INIT);
    energyText.dstR.w = 30;
    energyText.dstR.h = 20;
    energyText.dstR.x = 0;
    energyText.dstR.y = 0;
    energyR.w = 25;
    energyR.h = 15;
    energyR.x = energyText.dstR.x + energyText.dstR.w;
    energyR.y = energyText.dstR.y + energyText.dstR.h / 2 - energyR.h / 2;
    rotR.w = 64;
    rotR.h = 64;
    rotR.x = appleTreeR.x + appleTreeR.w / 2 - rotR.w / 2;
    rotR.y = appleTreeR.y - rotR.h;
    houseflyR.w = 32;
    houseflyR.h = 32;
    houseflyR.x = rotR.x;
    houseflyR.y = rotR.y + rotR.h - houseflyR.h;
    player.r.w = 64;
    player.r.h = 64;
    player.r.x = houseR.x + houseR.w + 5;
    player.r.y = houseR.y;
    collectR.w = 144;
    collectR.h = 104;
    collectR.x = windowWidth / 2 - collectR.w / 2;
    collectR.y = windowHeight - collectR.h;
    inventorySlotR.w = 64;
    inventorySlotR.h = 64;
    inventorySlotR.x = windowWidth - inventorySlotR.w * 2 - 10;
    inventorySlotR.y = windowHeight - inventorySlotR.h;
    inventorySlot2R.w = 64;
    inventorySlot2R.h = 64;
    inventorySlot2R.x = windowWidth - inventorySlot2R.w;
    inventorySlot2R.y = windowHeight - inventorySlot2R.h;
    inventorySlotXR.w = 16;
    inventorySlotXR.h = 16;
    inventorySlotXR.x = inventorySlotR.x - inventorySlotXR.w / 2;
    inventorySlotXR.y = inventorySlotR.y - inventorySlotXR.h;
    inventorySlotX2R = inventorySlotXR;
    inventorySlotX2R.x = inventorySlot2R.x - inventorySlotX2R.w / 2;
    infoTextAboutNotEnoughEnergy.setText(renderer, robotoF, "You don't have enough energy to leave", { WARNING_COLOR });
    infoTextAboutNotEnoughEnergy.dstR.w = 400;
    infoTextAboutNotEnoughEnergy.dstR.h = 40;
    infoTextAboutNotEnoughEnergy.dstR.x = windowWidth / 2 - infoTextAboutNotEnoughEnergy.dstR.w / 2;
    infoTextAboutNotEnoughEnergy.dstR.y = windowHeight / 2.0f - infoTextAboutNotEnoughEnergy.dstR.h / 2.0f;
    cantCollectText.setText(renderer, robotoF, "You can't collect in the night");
    cantCollectText.dstR.w = 180;
    cantCollectText.dstR.h = 40;
    cantCollectText.dstR.x = windowWidth / 2 - cantCollectText.dstR.w / 2;
    cantCollectText.dstR.y = 250;
    canSleepAndGoShopText.setText(renderer, robotoF, "You can sleep and go shop only in the night", { WARNING_COLOR });
    canSleepAndGoShopText.dstR.w = clamp(canSleepAndGoShopText.text.length() * LETTER_WIDTH, 10, windowWidth * 0.9f);
    canSleepAndGoShopText.dstR.h = 40;
    canSleepAndGoShopText.dstR.x = windowWidth / 2 - canSleepAndGoShopText.dstR.w / 2;
    canSleepAndGoShopText.dstR.y = windowHeight / 2.0f - canSleepAndGoShopText.dstR.h / 2.0f;
    bgLayerT = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, windowWidth, windowHeight);
    lightLayerT = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, windowWidth, windowHeight);
    resultLayerT = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, windowWidth, windowHeight);
    houseR.w = 64;
    houseR.h = 64;
    houseR.x = windowWidth / 2 - houseR.w / 2;
    houseR.y = windowHeight / 2 - houseR.h / 2;
    for (int i = 0; i < 8; ++i) {
        playerAnimationFrames.push_back({ i * 32, 0, 32, 32 });
    }
    doorR.w = 32;
    doorR.h = 32;
    doorR.x = 0;
    doorR.y = 0;
    hungerText.setText(renderer, robotoF, HUNGER_INIT);
    hungerText.dstR.w = 100;
    hungerText.dstR.h = 40;
    hungerText.dstR.x = hourText.dstR.x;
    hungerText.dstR.y = hourText.dstR.y + hourText.dstR.h;
    sickText.setText(renderer, robotoF, "100");
    sickText.dstR.w = 100;
    sickText.dstR.h = 40;
    sickText.dstR.x = hourText.dstR.x;
    sickText.dstR.y = hungerText.dstR.y + hungerText.dstR.h;
    gameOverText.setText(renderer, robotoF, "Game over");
    gameOverText.dstR.w = 400;
    gameOverText.dstR.h = 100;
    gameOverText.dstR.x = windowWidth / 2 - gameOverText.dstR.w / 2;
    gameOverText.dstR.y = windowHeight / 2 - gameOverText.dstR.h / 2;
    playAgainText.setText(renderer, robotoF, "Play again");
    playAgainText.dstR.w = 100;
    playAgainText.dstR.h = 40;
    playAgainText.dstR.x = windowWidth / 2 - playAgainText.dstR.w / 2;
    playAgainText.dstR.y = gameOverText.dstR.y + gameOverText.dstR.h;
    controlsText.dstR.h = 50;
    controlsText.dstR.y = windowHeight - controlsText.dstR.h;
    keyControlsText.dstR = controlsText.dstR;
    loadMap("res/map.tmx", mapWidth, mapHeight, tiles, plots, houseR);
    HomeInit(tile, homeGround, homeWall, homeSeparator, windowR, doorI, shopI, player, bedI, chestI, actionText);
    StorageInit(storageContainer, storageHoverText, storageTitleText, storageHungerText, storage, storageInvenPlaceholder);
    MenuInit(pauseContainer, pauseTitleText, "Paused", pauseOptions, PAUSE_NUM_OPTIONS, PAUSE_MENU_BUTTON_PADDING, pauseLabels, pauseMenuTypes);
    MenuInit(mainContainer, mainTitleText, "Autumn Survival", mainOptions, MAINMENU_NUM_OPTIONS, MAINMENU_BUTTON_PADDING, mainLabels, mainMenuTypes);
    CreditsInit(creditsTitleText, authorsText, authors, externalGraphicsText, egAuthorsTexts, backButton, backRect);
    for (auto& food : foods) {
        food = Food::Empty;
    }
    for (auto& item : foodSelected) {
        item = false;
    }
    leafClock.restart();
    globalClock.restart();
    rotClock.restart();
    grapeClock.restart();
    appleClock.restart();
    bananaClock.restart();
    intro.introClock.restart();
    timeClock.restart();
    potatoClock.restart();
    pumpkinClock.restart();
    playerAnimationClock.restart();
    hungerClock.restart();
    increasedPlayerSpeedClock.restart();
    readData(scoreText, rotDelayInMs, isMuted, maxEnergy, energyText, foods, storage, hungerText);
    if (std::stoi(hungerText.text) <= 0) {
        hungerText.setText(renderer, robotoF, HUNGER_INIT);
    }
    while (running) {
        float deltaTime = globalClock.restart();
        SDL_FRect windowScreenR;
        windowScreenR.w = windowWidth;
        windowScreenR.h = windowHeight;
        windowScreenR.x = 0;
        windowScreenR.y = 0;
        if (!Mix_PlayingMusic()) {
            if (currentMusic == Music::JosephKosma) {
                currentMusic = Music::AntonioVivaldi;
                Mix_PlayMusic(antonioVivaldiM, 1);
            }
            else if (currentMusic == Music::AntonioVivaldi) {
                currentMusic = Music::JosephKosma;
                Mix_PlayMusic(josephKosmaM, 1);
            }
        }
        if (state == State::Main) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                    // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
                }
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    buttons[event.button.button] = true;
                    for (int i = 0; i < MAINMENU_NUM_OPTIONS; ++i) {
                        if (SDL_PointInFRect(&mousePos, &mainOptions[i].buttonText.dstR)) {
                            HandleMenuOption(mainOptions[i].menuType, state, pausedState, intro, running);
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

                    for (int i = 0; i < MAINMENU_NUM_OPTIONS; ++i) {
                        if (SDL_PointInFRect(&mousePos, &mainOptions[i].buttonText.dstR)) {
                            mainOptions[i].selected = true;
                        }
                        else {
                            mainOptions[i].selected = false;
                        }
                    }
                }
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            RenderMenu(mainContainer, mainTitleText, mainOptions, MAINMENU_NUM_OPTIONS);
            SDL_RenderPresent(renderer);
        }
        else if (state == State::Intro) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                    // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
                }
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
                }
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (!pauseKeyHeld) {
                            pausedState = state;
                            pauseKeyHeld = true;
                            state = State::Paused;
                        }
                    }
                    else {
                        keys[event.key.keysym.scancode] = true;
                    }
                }
                if (event.type == SDL_KEYUP) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (pauseKeyHeld) {
                            pauseKeyHeld = false;
                        }
                    }
                    else {
                        keys[event.key.keysym.scancode] = false;
                    }
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    buttons[event.button.button] = true;
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
            if (intro.introClock.getElapsedTime() > 1000) {
                state = State::Main;
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_RenderCopyExF(renderer, leafT, 0, &intro.leafR, intro.leafAngle, 0, SDL_FLIP_NONE);
            if (intro.leafMotion == Motion::Right) {
                ++intro.leafAngle;
                if (intro.leafAngle == 75) {
                    intro.leafMotion = Motion::Left;
                }
            }
            else if (intro.leafMotion == Motion::Left) {
                --intro.leafAngle;
                if (intro.leafAngle == -75) {
                    intro.leafMotion = Motion::Right;
                }
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(8);
        }
        else if (state == State::Outside) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                    // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
                }
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
                }
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (!pauseKeyHeld) {
                            pausedState = state;
                            pauseKeyHeld = true;
                            state = State::Paused;
                        }
                    }
                    else {
                        keys[event.key.keysym.scancode] = true;
                        if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                            onSpaceInOutsideState(hour, canCollect, foods, state, plots, player, parts);
                        }
                    }
                }
                if (event.type == SDL_KEYUP) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (pauseKeyHeld) {
                            pauseKeyHeld = false;
                        }
                    }
                    else {
                        keys[event.key.keysym.scancode] = false;
                    }
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    buttons[event.button.button] = true;
                    if (SDL_PointInFRect(&mousePos, &soundBtnR)) {
                        isMuted = !isMuted;
                        if (isMuted) {
                            muteMusicAndSounds();
                        }
                        else {
                            unmuteMusicAndSounds();
                        }
                    }
                    if (SDL_PointInFRect(&mousePos, &inventorySlotXR)) {
                        foods[0] = Food::Empty;
                    }
                    if (SDL_PointInFRect(&mousePos, &inventorySlotX2R)) {
                        foods[1] = Food::Empty;
                    }
                    if (event.button.button == SDL_BUTTON_RIGHT) {
                        Food eaten = eat(inventorySlotR, inventorySlot2R, energyText, playerSpeed, increasedPlayerSpeedClock, foods, hungerText);
                        UpdateSickFactor(eaten, foodSicknessScore, sickLevel, alreadySick);
                        if (std::stoi(energyText.text) > 0) {
                            shouldGoHome = false;
                        }
                    }
                    if (SDL_PointInFRect(&mousePos, &actionR)) {
                        onSpaceInOutsideState(hour, canCollect, foods, state, plots, player, parts);
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
            for (int i = 0; i < entities.size(); ++i) {
                if (entities[i].direction == Direction::Right) {
                    entities[i].r.x += 0.01 * deltaTime;
                }
                else if (entities[i].direction == Direction::Left) {
                    entities[i].r.x += -0.01 * deltaTime;
                }
                entities[i].r.y = std::pow(entities[i].r.x, 2) / 28;
            }
            if (rotClock.getElapsedTime() > rotDelayInMs) {
                if (std::stoi(scoreText.text) > 0) {
                    scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) - 1);
                }
                rotClock.restart();
            }
            shouldShowRotImage = rotClock.getElapsedTime() + 1000 > rotDelayInMs && std::stoi(scoreText.text) > 0;
            moveTimeByOneHour(timeClock, hour, hourText, sickLevel, alreadySick);
            if (tradeClock.getElapsedTime() > 1000) // TODO: Set it to 20000
            {
                tradeRects.push_back(SDL_FRect());
                tradeRects.back().w = windowWidth;
                tradeRects.back().h = windowHeight;
                tradeRects.back().x = 0;
                tradeRects.back().y = -windowHeight + 1;
                tradeClock.restart();
            }
            for (int i = 0; i < tradeRects.size(); ++i) {
                tradeRects[i].y += deltaTime;
                if (!SDL_HasIntersectionF(&tradeRects[i], &windowScreenR)) {
                    tradeRects.erase(tradeRects.begin() + i--);
                    // TODO: Mix prize for each vegetable/fruit (save them to file also)
                }
            }
            player.dx = 0;
            player.dy = 0;
            if (shouldGoHome) {
                bool nothing = true;
                if (player.r.x + player.r.w < houseR.x) {
                    player.r.x += playerSpeed * deltaTime;
                    nothing = false;
                }
                else if (player.r.x > houseR.x + houseR.w) {
                    player.r.x += -playerSpeed * deltaTime;
                    nothing = false;
                }
                if (player.r.y + player.r.h < houseR.y) {
                    player.r.y += playerSpeed * deltaTime;
                    nothing = false;
                }
                else if (player.r.y > houseR.y + houseR.h) {
                    player.r.y += -playerSpeed * deltaTime;
                    nothing = false;
                }
                if (nothing) {
                    state = State::Home;
                }
            }
            else {
                if (keys[SDL_SCANCODE_A] || buttons[SDL_BUTTON_LEFT] && SDL_PointInFRect(&mousePos, &leftR)) {
                    player.dx = -1;
                    if (!isMoving) {
                        energyClock.restart();
                    }
                    isMoving = true;
                    playerDirection = PlayerDirection::Left;
                }
                else if (keys[SDL_SCANCODE_D] || buttons[SDL_BUTTON_LEFT] && SDL_PointInFRect(&mousePos, &rightR)) {
                    player.dx = 1;
                    if (!isMoving) {
                        energyClock.restart();
                    }
                    isMoving = true;
                    playerDirection = PlayerDirection::Right;
                }
                if (keys[SDL_SCANCODE_W] || buttons[SDL_BUTTON_LEFT] && SDL_PointInFRect(&mousePos, &upR)) {
                    player.dy = -1;
                    if (!isMoving) {
                        energyClock.restart();
                    }
                    isMoving = true;
                    playerDirection = PlayerDirection::Up;
                }
                else if (keys[SDL_SCANCODE_S] || buttons[SDL_BUTTON_LEFT] && SDL_PointInFRect(&mousePos, &downR)) {
                    player.dy = 1;
                    if (!isMoving) {
                        energyClock.restart();
                    }
                    isMoving = true;
                    playerDirection = PlayerDirection::Down;
                }
            }
            player.r.x = clamp(player.r.x, 0, windowWidth - player.r.w);
            player.r.y = clamp(player.r.y, 0, windowHeight - player.r.h);
            if (!keys[SDL_SCANCODE_A] && !keys[SDL_SCANCODE_D]
                && !keys[SDL_SCANCODE_W] && !keys[SDL_SCANCODE_S] && (!buttons[SDL_BUTTON_LEFT] || buttons[SDL_BUTTON_LEFT] && !SDL_PointInFRect(&mousePos, &leftR) && !SDL_PointInFRect(&mousePos, &rightR) && !SDL_PointInFRect(&mousePos, &upR) && !SDL_PointInFRect(&mousePos, &downR))) {
                isMoving = false;
            }
            if (isMoving) {
                if (energyClock.getElapsedTime() > 1000) {
                    if (std::stoi(energyText.text) != 0) {
                        energyText.setText(renderer, robotoF, std::stoi(energyText.text) - 1);
                        if (sickLevel != 0) {
                            energyText.setText(renderer, robotoF, static_cast<int>(sickLevel * ENERGY_SICK_FACTOR) > 0 ? std::stoi(energyText.text) - static_cast<int>(sickLevel * ENERGY_SICK_FACTOR) : std::stoi(energyText.text) + static_cast<int>(sickLevel * ENERGY_SICK_FACTOR));
                            if (std::stoi(energyText.text) < 0) {
                                energyText.setText(renderer, robotoF, 0);
                            }
                        }
                    }
                    if (energyText.text == "0") {
                        shouldGoHome = true;
                    }
                    energyClock.restart();
                }
            }
            player.r.x += player.dx * deltaTime * playerSpeed;
            player.r.y += player.dy * deltaTime * playerSpeed;
            if (SDL_HasIntersectionF(&houseR, &player.r) && (keys[SDL_SCANCODE_SPACE] || buttons[SDL_BUTTON_LEFT] && SDL_PointInFRect(&mousePos, &actionR))) {
                if (!exitedHome) {
                    exitedHome = true;
                    player.r.x = doorI.dstR.x + doorI.dstR.w / 2.0f - player.r.w / 2.0f;
                    player.r.y = doorI.dstR.y + doorI.dstR.h / 2.0f - player.r.h / 2.0f;
                    state = State::Home;
                    Mix_PlayChannel(-1, doorS, 0);
                }
            }
            if (exitedHome) {
                exitedHome = false;
            }
            isCollecting = false;
            for (auto& plot : plots) {
                if (plot.isIntersecting(player.r)) {
                    isCollecting = true;
                    break;
                }
            }
            if (hungerClock.getElapsedTime() > HUNGER_DECREASE_DELAY_IN_MS) {
                if (std::stoi(hungerText.text) > 0) {
                    hungerText.setText(renderer, robotoF, std::stoi(hungerText.text) - 1);
                    if (std::stoi(hungerText.text) <= 0) {
                        state = State::Gameover;
                    }
                }
                hungerClock.restart();
            }
            if (increasedPlayerSpeedClock.getElapsedTime() > 1000) {
                playerSpeed = INIT_PLAYER_SPEED;
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderTarget(renderer, bgLayerT);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            for (Tile& t : tiles) {
                SDL_RenderCopyF(renderer, getT(renderer, t.source), &t.srcR, &t.dstR);
            }
            for (int i = 0; i < plots.size(); ++i) {
                plots[i].renderPlot();
            }
            if (shouldShowRotImage) {
                SDL_RenderCopyF(renderer, rotT, 0, &rotR);
                SDL_RenderCopyF(renderer, houseflyT, 0, &houseflyR);
                if (houseflyGoingRight) {
                    houseflyR.x += 0.1 * deltaTime;
                    if (houseflyR.x + houseflyR.w >= rotR.x + rotR.w) {
                        houseflyGoingRight = false;
                        houseflyR.y -= 20;
                    }
                }
                else {
                    houseflyR.x -= 0.05 * deltaTime;
                }
            }
            else {
                houseflyR.x = rotR.x;
                houseflyGoingRight = true;
                houseflyR.y = rotR.y + rotR.h - houseflyR.h;
            }
            SDL_RenderCopyF(renderer, houseT, 0, &houseR);

            SDL_SetRenderTarget(renderer, 0);

            SDL_SetRenderTarget(renderer, lightLayerT);
            SDL_SetTextureBlendMode(lightLayerT, SDL_BLENDMODE_MOD);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_FRect spotR = player.r;
            spotR.x -= 200;
            spotR.y -= 200;
            spotR.w += 400;
            spotR.h += 400;
            if (hour >= 7 && hour <= 17) {
                spotR.w = windowWidth + 400;
                spotR.h = windowHeight + 400;
                spotR.x = -200;
                spotR.y = -200;
            }
            SDL_RenderCopyF(renderer, lightT, 0, &spotR);
            SDL_SetRenderTarget(renderer, 0);

            SDL_SetRenderTarget(renderer, resultLayerT);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetTextureBlendMode(resultLayerT, SDL_BLENDMODE_BLEND);
            SDL_RenderCopy(renderer, bgLayerT, 0, 0);
            SDL_RenderCopy(renderer, lightLayerT, 0, 0);

            SDL_SetRenderTarget(renderer, 0);
            SDL_RenderCopy(renderer, resultLayerT, 0, 0);
            if (isCollecting) {
                //SDL_RenderCopyF(renderer, collectT, 0, &collectR);
                RenderInstructions(controlsText, "Collect", keyControlsText, "SPACE: ");
            }
            if (SDL_HasIntersectionF(&houseR, &player.r)) {
                RenderInstructions(controlsText, "Enter", keyControlsText, "SPACE: ");
            }
            if (SDL_PointInFRect(&mousePos, &inventorySlotXR) || SDL_PointInFRect(&mousePos, &inventorySlotX2R)) {
                RenderInstructions(controlsText, "Delete", keyControlsText, "LEFT-CLICK: ");
            }
            if (SDL_PointInFRect(&mousePos, &inventorySlotR) && foods[0] != Food::Empty) {
                RenderInstructions(controlsText, "Eat", keyControlsText, "RIGHT-CLICK: ");
            }
            if (SDL_PointInFRect(&mousePos, &inventorySlot2R) && foods[1] != Food::Empty) {
                RenderInstructions(controlsText, "Eat", keyControlsText, "RIGHT-CLICK: ");
            }
            DrawInventory(inventorySlotR, inventorySlot2R, foods, inventorySlotXR, inventorySlotX2R);
            RenderUI(scoreText, isMuted, soundBtnR, hourText, energyText, hour, sunR, energyR, playerDirection, playerAnimationFrames, playerAnimationFrame, player, isMoving, playerAnimationClock, hungerText, sickText, sickLevel, inventorySlotR, inventorySlot2R, foods, inventorySlotXR, inventorySlotX2R);
            if (!canCollect) {
                cantCollectText.draw(renderer);
            }
#ifdef __ANDROID__
            SDL_RenderCopyF(renderer, circleT, 0, &leftR);
            SDL_RenderCopyF(renderer, circleT, 0, &rightR);
            SDL_RenderCopyF(renderer, circleT, 0, &upR);
            SDL_RenderCopyF(renderer, circleT, 0, &downR);
            SDL_RenderCopyF(renderer, redCircleT, 0, &actionR);
#endif
            SDL_RenderPresent(renderer);
        }
        else if (state == State::Shop) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                    // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
                }
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
                }
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (!pauseKeyHeld) {
                            pausedState = state;
                            pauseKeyHeld = true;
                            state = State::Paused;
                        }
                    }
                    else {
                        keys[event.key.keysym.scancode] = true;
                    }
                }
                if (event.type == SDL_KEYUP) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (pauseKeyHeld) {
                            pauseKeyHeld = false;
                        }
                    }
                    else {
                        keys[event.key.keysym.scancode] = false;
                    }
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    buttons[event.button.button] = true;
                    if (SDL_PointInFRect(&mousePos, &shop.backArrowR)) {
                        state = State::Home;
                    }
                    if (SDL_PointInFRect(&mousePos, &shop.lessRotBuyR)) {
                        if (std::stoi(scoreText.text) >= std::stoi(shop.lessRotPriceText.text)) {
                            scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) - std::stoi(shop.lessRotPriceText.text));
                            rotDelayInMs += 1000;
                            Mix_PlayChannel(-1, powerupS, 0);
                        }
                    }
                    if (SDL_PointInFRect(&mousePos, &shop.moreEnergyBuyR)) {
                        if (std::stoi(scoreText.text) >= std::stoi(shop.moreEnergyPriceText.text)) {
                            scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) - std::stoi(shop.moreEnergyPriceText.text));
                            ++maxEnergy;
                            Mix_PlayChannel(-1, powerupS, 0);
                        }
                    }
                    for (int i = 0; i < shop.sellItems.size(); ++i) {
                        if (SDL_PointInFRect(&mousePos, &shop.sellItems[i].sellR)) {
                            std::string count = shop.sellItems[i].countText.text;
                            count.erase(0, 1);
                            /*
                                NOTE: Order in storage
                                0 Apple,
                                1 Carrot,
                                2 Grape,
                                3 Potato,
                                4 Pumpkin,
                                5 Banana,
                                Order in shop
                                0 banana
                                1 apple
                                2 grape
                                3 pumpkin 
                                4 potato
                                5 carrot
                            */
                            if (std::stoi(count) > 0) {
                                scoreText.setText(renderer, robotoF, std::stoi(scoreText.text) + std::stoi(shop.sellItems[i].gainText.text));
                                count = std::to_string(std::stoi(count) - 1);
                                shop.sellItems[i].countText.setText(renderer, robotoF, "x" + count);
                                if (i == 0) {
                                    if (storage[5].amount > 0) {
                                        --storage[5].amount;
                                    }
                                    else {
                                        if (foods[0] == Food::Banana) {
                                            foods[0] = Food::Empty;
                                        }
                                        else {
                                            foods[1] = Food::Empty;
                                        }
                                    }
                                }
                                else if (i == 1) {
                                    if (storage[0].amount > 0) {
                                        --storage[0].amount;
                                    }
                                    else {
                                        if (foods[0] == Food::Apple) {
                                            foods[0] = Food::Empty;
                                        }
                                        else {
                                            foods[1] = Food::Empty;
                                        }
                                    }
                                }
                                else if (i == 2) {
                                    if (storage[2].amount > 0) {
                                        --storage[2].amount;
                                    }
                                    else {
                                        if (foods[0] == Food::Grape) {
                                            foods[0] = Food::Empty;
                                        }
                                        else {
                                            foods[1] = Food::Empty;
                                        }
                                    }
                                }
                                else if (i == 3) {
                                    if (storage[4].amount > 0) {
                                        --storage[4].amount;
                                    }
                                    else {
                                        if (foods[0] == Food::Pumpkin) {
                                            foods[0] = Food::Empty;
                                        }
                                        else {
                                            foods[1] = Food::Empty;
                                        }
                                    }
                                }
                                else if (i == 4) {
                                    if (storage[3].amount > 0) {
                                        --storage[3].amount;
                                    }
                                    else {
                                        if (foods[0] == Food::Potato) {
                                            foods[0] = Food::Empty;
                                        }
                                        else {
                                            foods[1] = Food::Empty;
                                        }
                                    }
                                }
                                else if (i == 5) {
                                    if (storage[1].amount > 0) {
                                        --storage[1].amount;
                                    }
                                    else {
                                        if (foods[0] == Food::Carrot) {
                                            foods[0] = Food::Empty;
                                        }
                                        else {
                                            foods[1] = Food::Empty;
                                        }
                                    }
                                }
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
            SDL_RenderCopyF(renderer, appleWithSprayerT, 0, &shop.lessRotR);
            shop.lessRotText.draw(renderer);
            shop.lessRotPriceText.draw(renderer);
            SDL_RenderCopyF(renderer, buyT, 0, &shop.lessRotBuyR);
            SDL_RenderCopyF(renderer, moreEnergyT, 0, &shop.moreEnergyR);
            shop.moreEnergyText.draw(renderer);
            shop.moreEnergyPriceText.draw(renderer);
            SDL_RenderCopyF(renderer, buyT, 0, &shop.moreEnergyBuyR);
            SDL_RenderCopyF(renderer, backArrowT, 0, &shop.backArrowR);
            for (int i = 0; i < shop.sellItems.size(); ++i) {
                SDL_RenderCopyF(renderer, shop.sellItems[i].foodT, 0, &shop.sellItems[i].imageR);
                shop.sellItems[i].countText.draw(renderer);
                SDL_RenderCopyF(renderer, sellT, 0, &shop.sellItems[i].sellR);
                shop.sellItems[i].gainText.draw(renderer);
            }
            SDL_RenderPresent(renderer);
        }
        else if (state == State::Home) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                    // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
                }
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
                }
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (!pauseKeyHeld) {
                            pausedState = state;
                            pauseKeyHeld = true;
                            state = State::Paused;
                        }
                    }
                    else {
                        keys[event.key.keysym.scancode] = true;
                        shouldShowWhenCanSleepAndGoShop = false;
                        shouldShowInfoTextAboutNotEnoughEnergy = false;
                        if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                            onSpaceInHomeState(player, hour, state, shouldShowWhenCanSleepAndGoShop, foods, shop, storage, doorI, shouldGoHome,
                                canCollect, shouldShowInfoTextAboutNotEnoughEnergy, houseR, shopI, bedI, alreadySick, sickLevel, hourText, energyText, maxEnergy,
                                chestI);
                        }
                    }
                }
                if (event.type == SDL_KEYUP) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (pauseKeyHeld) {
                            pauseKeyHeld = false;
                        }
                    }
                    else {
                        keys[event.key.keysym.scancode] = false;
                    }
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    buttons[event.button.button] = true;

                    if (SDL_PointInFRect(&mousePos, &soundBtnR)) {
                        isMuted = !isMuted;
                        if (isMuted) {
                            muteMusicAndSounds();
                        }
                        else {
                            unmuteMusicAndSounds();
                        }
                    }
                    if (event.button.button == SDL_BUTTON_RIGHT) {
                        Food eaten = eat(inventorySlotR, inventorySlot2R, energyText, playerSpeed, increasedPlayerSpeedClock, foods, hungerText);
                        UpdateSickFactor(eaten, foodSicknessScore, sickLevel, alreadySick);
                        if (std::stoi(energyText.text) > 0) {
                            shouldShowInfoTextAboutNotEnoughEnergy = false;
                            shouldGoHome = false;
                        }
                    }
                    if (SDL_PointInFRect(&mousePos, &actionR)) {
                        onSpaceInHomeState(player, hour, state, shouldShowWhenCanSleepAndGoShop, foods, shop, storage, doorI, shouldGoHome,
                            canCollect, shouldShowInfoTextAboutNotEnoughEnergy, houseR, shopI, bedI, alreadySick, sickLevel, hourText, energyText, maxEnergy,
                            chestI);
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
            player.dx = 0;
            player.dy = 0;
            if (keys[SDL_SCANCODE_A] || buttons[SDL_BUTTON_LEFT] && SDL_PointInFRect(&mousePos, &leftR)) {
                player.dx = -1;
                playerDirection = PlayerDirection::Left;
            }
            else if (keys[SDL_SCANCODE_D] || buttons[SDL_BUTTON_LEFT] && SDL_PointInFRect(&mousePos, &rightR)) {
                player.dx = 1;
                playerDirection = PlayerDirection::Right;
            }
            if (keys[SDL_SCANCODE_W] || buttons[SDL_BUTTON_LEFT] && SDL_PointInFRect(&mousePos, &upR)) {
                player.dy = -1;
                playerDirection = PlayerDirection::Up;
            }
            else if (keys[SDL_SCANCODE_S] || buttons[SDL_BUTTON_LEFT] && SDL_PointInFRect(&mousePos, &downR)) {
                player.dy = 1;
                playerDirection = PlayerDirection::Down;
            }
            player.r.x += player.dx * deltaTime * playerSpeed;
            player.r.y += player.dy * deltaTime * playerSpeed;
            player.r.x = clamp(player.r.x, homeGround.x - 20, homeGround.x + homeGround.w - player.r.w + 20);
            player.r.y = clamp(player.r.y, homeGround.y + 10, homeGround.y + homeGround.h - player.r.h);
            if (SDL_HasIntersectionF(&player.r, &doorR)) {
                state = State::Outside;
            }
            std::vector<int> scoreGain;
            for (int i = 0; i < (int)(Food::NumFood); ++i) {
                scoreGain.push_back(random(1, 10));
            }
            moveTimeByOneHour(timeClock, hour, hourText, sickLevel, alreadySick);
            if (hungerClock.getElapsedTime() > HUNGER_DECREASE_DELAY_IN_MS) {
                if (std::stoi(hungerText.text) > 0) {
                    hungerText.setText(renderer, robotoF, std::stoi(hungerText.text) - 1);
                    if (sickLevel != 0) {
                        hungerText.setText(renderer, robotoF,
                            static_cast<int>(sickLevel * HUNGER_SICK_FACTOR) > 0 ? std::stoi(hungerText.text) - static_cast<int>(sickLevel * HUNGER_SICK_FACTOR) : std::stoi(hungerText.text) + static_cast<int>(sickLevel * HUNGER_SICK_FACTOR));
                        if (std::stoi(energyText.text) < 0) {
                            energyText.setText(renderer, robotoF, 0);
                        }
                    }
                    if (std::stoi(hungerText.text) <= 0) {
                        state = State::Gameover;
                    }
                }
                hungerClock.restart();
            }
            if (increasedPlayerSpeedClock.getElapsedTime() > 1000) {
                playerSpeed = INIT_PLAYER_SPEED;
            }
            if (SDL_HasIntersectionF(&player.r, &shopI.dstR)) {
                currentAction.setActionText(shopI.actionText);
            }
            else if (SDL_HasIntersectionF(&player.r, &doorI.dstR)) {
                currentAction.setActionText(doorI.actionText);
            }
            else if (SDL_HasIntersectionF(&player.r, &bedI.dstR)) {
                currentAction.setActionText(bedI.actionText);
            }
            else if (SDL_HasIntersectionF(&player.r, &chestI.dstR)) {
                currentAction.setActionText(chestI.actionText);
            }
            else {
                currentAction.setActionText("");
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            RenderHome(homeGround, tile, homeWall, homeSeparator, hour, windowR, bedI, doorI, shopI, chestI,
                actionText, currentAction, shouldShowInfoTextAboutNotEnoughEnergy, infoTextAboutNotEnoughEnergy, shouldShowWhenCanSleepAndGoShop, canSleepAndGoShopText, scoreText, isMuted,
                soundBtnR, hourText, energyText, sunR, energyR, playerDirection, playerAnimationFrames, playerAnimationFrame, player, isMoving,
                playerAnimationClock, hungerText, sickText, sickLevel, inventorySlotR, inventorySlot2R, foods, inventorySlotXR, inventorySlotX2R, controlsText, keyControlsText);
#ifdef __ANDROID__
            SDL_RenderCopyF(renderer, circleT, 0, &leftR);
            SDL_RenderCopyF(renderer, circleT, 0, &rightR);
            SDL_RenderCopyF(renderer, circleT, 0, &upR);
            SDL_RenderCopyF(renderer, circleT, 0, &downR);
            SDL_RenderCopyF(renderer, redCircleT, 0, &actionR);
#endif
            SDL_RenderPresent(renderer);
        }
        else if (state == State::Minigame) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                    // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
                }
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
                }
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (!pauseKeyHeld) {
                            pausedState = state;
                            pauseKeyHeld = true;
                            state = State::Paused;
                        }
                    }
                    else {
                        keys[event.key.keysym.scancode] = true;
                    }
                }
                if (event.type == SDL_KEYUP) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (pauseKeyHeld) {
                            pauseKeyHeld = false;
                        }
                    }
                    else {
                        keys[event.key.keysym.scancode] = false;
                    }
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    buttons[event.button.button] = true;
                    for (int i = 0; i < parts.size(); ++i) {
                        if (SDL_PointInFRect(&mousePos, &parts[i].dstR)) {
                            parts.erase(parts.begin() + i--);
                            if (parts.empty()) {
                                state = State::Outside;
                                int index = foods[0] == Food::Empty ? 0 : 1;
                                if (currentPartT == appleT) {
                                    foods[index] = Food::Apple;
                                }
                                else if (currentPartT == bananaT) {
                                    foods[index] = Food::Banana;
                                }
                                else if (currentPartT == grapeT) {
                                    foods[index] = Food::Grape;
                                }
                                else if (currentPartT == carrotT) {
                                    foods[index] = Food::Carrot;
                                }
                                else if (currentPartT == potatoT) {
                                    foods[index] = Food::Potato;
                                }
                                else if (currentPartT == pumpkinT) {
                                    foods[index] = Food::Pumpkin;
                                }
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
            for (int i = 0; i < parts.size(); ++i) {
                parts[i].dstR.y += PART_SPEED * deltaTime;
            }
            for (int i = 0; i < parts.size(); ++i) {
                if (parts[i].dstR.y + parts[i].dstR.h > windowHeight) {
                    state = State::Outside;
                    parts.clear();
                }
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            for (int i = 0; i < parts.size(); ++i) {
                SDL_RenderCopyF(renderer, currentPartT, 0, &parts[i].dstR);
            }
            SDL_RenderPresent(renderer);
        }
        else if (state == State::Gameover) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
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
                    if (SDL_PointInFRect(&mousePos, &playAgainText.dstR)) {
                        shouldShowIntro = false;
                        goto gameBegin;
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
                    if (SDL_PointInFRect(&mousePos, &playAgainText.dstR)) {
                        playAgainText.setText(renderer, robotoF, "Play again", { 255, 0, 0 });
                    }
                    else {
                        playAgainText.setText(renderer, robotoF, "Play again");
                    }
                }
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            gameOverText.draw(renderer);
            playAgainText.draw(renderer);
            SDL_RenderPresent(renderer);
        }
        else if (state == State::Storage) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                    // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
                }
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
                }
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (!pauseKeyHeld) {
                            pausedState = state;
                            pauseKeyHeld = true;
                            state = State::Paused;
                        }
                    }
                    else {
                        keys[event.key.keysym.scancode] = true;
                    }
                }
                if (event.type == SDL_KEYUP) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        if (pauseKeyHeld) {
                            pauseKeyHeld = false;
                        }
                    }
                    else {
                        keys[event.key.keysym.scancode] = false;
                    }
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    buttons[event.button.button] = true;
                    if (SDL_PointInFRect(&mousePos, &xBtnR)) {
                        state = State::Home;
                    }
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        for (int i = 0; i < storageInvenPlaceholder.size(); ++i) {
                            if (SDL_PointInFRect(&mousePos, &storageInvenPlaceholder[i])) {
                                for (int j = 0; j < storage.size(); ++j) {
                                    if (storage[j].foodType == foods[i]) {
                                        ++storage[j].amount;
                                        foods[i] = Food::Empty;
                                        break;
                                    }
                                }
                            }
                        }
                        for (int i = 0; i < storage.size(); ++i) {
                            if (SDL_PointInFRect(&mousePos, &storage[i].container)) {
                                if (storage[i].amount) {
                                    if (foods[0] == Food::Empty) {
                                        foods[0] = storage[i].foodType;
                                        --storage[i].amount;
                                    }
                                    else if (foods[1] == Food::Empty) {
                                        foods[1] = storage[i].foodType;
                                        --storage[i].amount;
                                    }
                                }
                                break;
                            }
                        }
                    }
                    else if (event.button.button == SDL_BUTTON_RIGHT) {
                        Food eaten = eat(storage, energyText, playerSpeed, increasedPlayerSpeedClock, storageInvenPlaceholder, foods, hungerText);
                        UpdateSickFactor(eaten, foodSicknessScore, sickLevel, alreadySick);
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

                    for (auto& item : storage) {
                        if (SDL_PointInFRect(&mousePos, &item.container)) {
                            item.selected = true;
                        }
                        else if (item.selected) {
                            item.selected = false;
                        }
                    }

                    for (int i = 0; i < storageInvenPlaceholder.size(); i++) {
                        if (foods[i] != Food::Empty) {
                            if (SDL_PointInFRect(&mousePos, &storageInvenPlaceholder[i])) {
                                foodSelected[i] = true;
                            }
                            else if (foodSelected[i]) {
                                foodSelected[i] = false;
                            }
                        }
                    }
                }
            }
            SDL_SetRenderDrawColor(renderer, 82, 71, 55, 0);
            SDL_RenderClear(renderer);
            RenderStorage(storageContainer, storageHoverText, storageTitleText, storageHungerText, hungerText.text, storage, storageInvenPlaceholder, foods, foodSelected);
            for (auto& item : storage) {
                if (item.selected && item.amount != 0) {
                    RenderInstructions(controlsText, "Take", keyControlsText, "LEFT-CLICK: ");
                    RenderInstructions(controlsText, "Eat", keyControlsText, "RIGHT-CLICK: ");
                    break;
                }
            }

            for (int i = 0; i < storageInvenPlaceholder.size(); i++) {
                if (foods[i] != Food::Empty) {
                    if (foodSelected[i]) {
                        RenderInstructions(controlsText, "Store", keyControlsText, "LEFT-CLICK: ");
                        RenderInstructions(controlsText, "Eat", keyControlsText, "RIGHT-CLICK: ");
                        break;
                    }
                }
            }
            SDL_RenderCopyF(renderer, xT, 0, &xBtnR);
            SDL_RenderPresent(renderer);
        }
        else if (state == State::Paused) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                    // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
                }
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
                }
                if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    if (!pauseKeyHeld) {
                        pauseKeyHeld = true;
                        state = pausedState;
                    }
                }
                if (event.type == SDL_KEYUP && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    if (pauseKeyHeld) {
                        pauseKeyHeld = false;
                    }
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    buttons[event.button.button] = true;
                    for (int i = 0; i < PAUSE_NUM_OPTIONS; ++i) {
                        if (SDL_PointInFRect(&mousePos, &pauseOptions[i].buttonText.dstR)) {
                            HandleMenuOption(pauseOptions[i].menuType, state, pausedState, intro, running);
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

                    for (int i = 0; i < PAUSE_NUM_OPTIONS; ++i) {
                        if (SDL_PointInFRect(&mousePos, &pauseOptions[i].buttonText.dstR)) {
                            pauseOptions[i].selected = true;
                        }
                        else {
                            pauseOptions[i].selected = false;
                        }
                    }
                }
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            RenderMenu(pauseContainer, pauseTitleText, pauseOptions, PAUSE_NUM_OPTIONS);
            SDL_RenderPresent(renderer);
        }
        else if (state == State::Credits) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                    // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
                }
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_RenderSetScale(renderer, event.window.data1 / (float)windowWidth, event.window.data2 / (float)windowHeight);
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    buttons[event.button.button] = true;
                    if (SDL_PointInFRect(&mousePos, &backButton.buttonText.dstR)) {
                        HandleMenuOption(backButton.menuType, state, pausedState, intro, running);
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

                    if (SDL_PointInFRect(&mousePos, &backButton.buttonText.dstR)) {
                        backButton.selected = true;
                    }
                    else {
                        backButton.selected = false;
                    }
                }
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            RenderCredits(creditsTitleText, authorsText, authors, externalGraphicsText, egAuthorsTexts, backButton, backRect);
            SDL_RenderPresent(renderer);
        }

        controlsText.dstR.y = windowHeight - controlsText.dstR.h;
        keyControlsText.dstR.y = controlsText.dstR.y;
        saveData(scoreText, rotDelayInMs, isMuted, maxEnergy, foods, storage, hungerText);
    }
    // TODO: On mobile remember to use eventWatch function (it doesn't reach this code when terminating)
    return 0;
}